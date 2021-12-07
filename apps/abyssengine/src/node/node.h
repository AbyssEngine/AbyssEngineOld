#ifndef ABYSS_NODE_H
#define ABYSS_NODE_H

#include "../common/events.h"
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <tuple>
#include <vector>

namespace AbyssEngine {

class Node {
  public:
    Node();
    explicit Node(std::string_view name);
    virtual ~Node(){};

    virtual void UpdateCallback(uint32_t ticks);
    virtual void RenderCallback(int offsetX, int offsetY);
    virtual void MouseEventCallback(const MouseEvent &event);
    virtual void Initialize();
    [[nodiscard]] virtual std::string_view NodeType() const { return "Node"; };

    void DoInitialize();
    void GetEffectiveLayout(int &x, int &y);
    void AppendChild(Node *childNode);
    void RemoveChild(Node *nodeRef);
    void RemoveAllChildren();
    void SetPosition(int x, int y);
    void ProcessQueuedActions();
    void SetActive(bool active);
    [[nodiscard]] bool GetActive() const;
    void SetVisible(bool visible);
    [[nodiscard]] bool GetVisible();
    std::tuple<int, int> GetPosition();

    int X = 0;
    int Y = 0;
    bool Visible = true;
    bool Active = true;

    Node *Parent = nullptr;
    std::vector<Node *> Children;

  private:

    std::queue<Node *> _addChildQueue;
    std::queue<Node *> _removeChildQueue;
    bool _removeAllChildren = false;
    std::string Name;
    mutable std::mutex _mutex;
    bool _initialized = false;
};

} // namespace AbyssEngine

#endif // ABYSS_NODE_H
