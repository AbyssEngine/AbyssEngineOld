#include "node.h"
#include "../engine/engine.h"
#include <spdlog/spdlog.h>

#include <utility>

AbyssEngine::Node::Node() : _addChildQueue(), _removeChildQueue() {}

AbyssEngine::Node::Node(std::string_view name) : Name(name) {}

void AbyssEngine::Node::UpdateCallback(uint32_t ticks) {
    ProcessQueuedActions();

    if (_onUpdateHandler.valid()) {
        auto result = _onUpdateHandler((float)ticks / 1000.f);

        if (!result.valid()) {
            sol::error err = result;
            Engine::Get()->Panic(err.what());
        }
    }

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

void AbyssEngine::Node::GetEffectiveLayout(int &x, int &y) const {
    x += X;
    y += Y;

    if (Parent != nullptr) {
        Parent->GetEffectiveLayout(x, y);
    }
}

void AbyssEngine::Node::AppendChild(Node *childNode) {
    if (childNode == nullptr)
        throw std::runtime_error("Attempted to append null node!");

    childNode->Parent = this;
    _addChildQueue.push(childNode);
}

void AbyssEngine::Node::RemoveChild(Node *nodeRef) {
    if (nodeRef == nullptr)
        throw std::runtime_error("Attempted to remove null node!");

    nodeRef->Parent = nullptr;
    _removeChildQueue.push(nodeRef);
}

void AbyssEngine::Node::RemoveAllChildren() { _removeAllChildren = true; }

void AbyssEngine::Node::SetPosition(int x, int y) {
    X = x;
    Y = y;
}

std::tuple<int, int> AbyssEngine::Node::GetPosition() const { return {X, Y}; }

void AbyssEngine::Node::SetActive(bool active) { Active = active; }

bool AbyssEngine::Node::GetActive() const { return Active; }

void AbyssEngine::Node::SetVisible(bool visible) { Visible = visible; }

bool AbyssEngine::Node::GetVisible() const { return Visible; }

void AbyssEngine::Node::MouseEventCallback(const AbyssEngine::MouseEvent &event) {
    for (auto &item : Children) {
        if (!item->Active || !item->Visible)
            continue;

        if (item != this)
            item->MouseEventCallback(event);
    }
}

void AbyssEngine::Node::ProcessQueuedActions() {
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

void AbyssEngine::Node::SetLuaOnUpdateHandler(sol::protected_function onUpdateHandler) {
    _onUpdateHandler = std::move(onUpdateHandler);
}

void AbyssEngine::Node::KeyboardEventCallback(const AbyssEngine::KeyboardEvent &event) {
    for (auto &item : Children) {
        if (!item->Active)
            continue;

        if (item != this)
            item->KeyboardEventCallback(event);
    }
}
