#ifndef ABYSS_NODE_H
#define ABYSS_NODE_H

#include "../common/events.h"
#include <memory>
#include <queue>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <sol/sol.hpp>

namespace AbyssEngine {

class Node {
  public:
    Node();
    explicit Node(std::string_view name);
    virtual ~Node() {}

    virtual void UpdateCallback(uint32_t ticks);
    virtual void RenderCallback(int offsetX, int offsetY);
    virtual void MouseEventCallback(const MouseEvent &event);
    virtual void KeyboardEventCallback(const KeyboardEvent &event);
    virtual void Initialize();
    [[nodiscard]] virtual std::string_view NodeType() const { return "Node"; };

    void DoInitialize();
    void GetEffectiveLayout(int &x, int &y) const;
    void AppendChild(Node *childNode);
    void RemoveChild(Node *nodeRef);
    void RemoveAllChildren();
    std::vector<Node*> GetChildren() { return Children; }
    void SetPosition(int x, int y);
    void ProcessQueuedActions();
    void SetActive(bool active);
    [[nodiscard]] bool GetActive() const;
    void SetVisible(bool visible);
    [[nodiscard]] bool GetVisible() const;
    [[nodiscard]] std::tuple<int, int> GetPosition() const;
    void SetLuaTable(sol::table table) { _table = std::move(table); }
    [[nodiscard]] sol::table GetLuaTable() const { return _table; }
    void SetLuaOnUpdateHandler(sol::protected_function onUpdateHandler);

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
    bool _initialized = false;
    sol::table _table;

    sol::protected_function _onUpdateHandler;
};

} // namespace AbyssEngine

#endif // ABYSS_NODE_H
