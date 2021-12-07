#include "node.h"
#include <spdlog/spdlog.h>

AbyssEngine::Node::Node() : _addChildQueue(), _removeChildQueue(), _mutex() {}

AbyssEngine::Node::Node(std::string_view name) : Name(name) {}

void AbyssEngine::Node::UpdateCallback(uint32_t ticks) {
    ProcessQueuedActions();

    for (auto &item : Children) {
        if (!item->Active)
            continue;

        item->UpdateCallback(ticks);
    }
}

void AbyssEngine::Node::RenderCallback(int offsetX, int offsetY) {
    for (auto &item : Children) {
        if (!item->Active || !item->Visible)
            continue;

        item->RenderCallback(X + offsetX, Y + offsetY);
    }
}

void AbyssEngine::Node::GetEffectiveLayout(int &x, int &y) {
    std::lock_guard<std::mutex> lock(_mutex);

    x += X;
    y += Y;

    if (Parent != nullptr) {
        Parent->GetEffectiveLayout(x, y);
    }
}

void AbyssEngine::Node::AppendChild(Node *childNode) {
    std::lock_guard<std::mutex> lock(_mutex);

    if (childNode == nullptr)
        throw std::runtime_error("Attempted to append null node!");

    _addChildQueue.push(childNode);
}

void AbyssEngine::Node::RemoveChild(Node *nodeRef) {
    std::lock_guard<std::mutex> lock(_mutex);

    if (nodeRef == nullptr)
        throw std::runtime_error("Attempted to remove null node!");

    _removeChildQueue.push(nodeRef);
}

void AbyssEngine::Node::RemoveAllChildren() {
    std::lock_guard<std::mutex> lock(_mutex);

    _removeAllChildren = true;
}

void AbyssEngine::Node::SetPosition(int x, int y) {
    std::lock_guard<std::mutex> lock(_mutex);

    X = x;
    Y = y;
}

std::tuple<int, int> AbyssEngine::Node::GetPosition() {
    std::lock_guard<std::mutex> lock(_mutex);

    return {X, Y};
}

void AbyssEngine::Node::SetActive(bool active) {
    std::lock_guard<std::mutex> lock(_mutex);

    Active = active;
}

bool AbyssEngine::Node::GetActive() const {
    std::lock_guard<std::mutex> lock(_mutex);

    return Active;
}

void AbyssEngine::Node::SetVisible(bool visible) {
    std::lock_guard<std::mutex> lock(_mutex);

    Visible = visible;
}

bool AbyssEngine::Node::GetVisible() {
    std::lock_guard<std::mutex> lock(_mutex);

    return Visible;
}

void AbyssEngine::Node::MouseEventCallback(const AbyssEngine::MouseEvent &event) {
    for (auto &item : Children) {
        if (!item->Active || !item->Visible)
            continue;

        if (item != this)
            item->MouseEventCallback(event);
    }
}

void AbyssEngine::Node::ProcessQueuedActions() {
    std::lock_guard<std::mutex> lock(_mutex);

    if (_removeAllChildren) {
        _removeAllChildren = false;
        Children.clear();
    }

    while (!_addChildQueue.empty()) {
        Children.push_back(_addChildQueue.front());
        _addChildQueue.pop();
    }

    while (!_removeChildQueue.empty()) {
        Children.erase(std::remove(Children.begin(), Children.end(), _removeChildQueue.front()), Children.end());
        _removeChildQueue.pop();
    }
}

void AbyssEngine::Node::DoInitialize() {
    if (!_initialized) {
        Initialize();
        _initialized = true;
    }

    for (auto &item : Children) {
        item->DoInitialize();
    }
}
void AbyssEngine::Node::Initialize() {}
