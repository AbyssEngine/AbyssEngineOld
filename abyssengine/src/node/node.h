#ifndef ABYSS_NODE_H
#define ABYSS_NODE_H

#include "../common/events.h"
#include <memory>
#include <tuple>
#include <vector>

namespace AbyssEngine {

class Node {
  public:
    Node();
    virtual ~Node();

    virtual void UpdateCallback(uint32_t ticks);
    virtual void RenderCallback(int offsetX, int offsetY);
    virtual void MouseEventCallback(const MouseEvent &event);
    void GetEffectiveLayout(int &x, int &y);
    void AppendChild(Node *childNode);
    void RemoveChild(Node *nodeRef);
    void RemoveAllChildren();
    void SetPosition(int x, int y);
    void SetActive(bool active);
    [[nodiscard]] bool GetActive() const;
    void SetVisible(bool visible);
    [[nodiscard]] bool GetVisible() const;
    std::tuple<int, int> GetPosition();

    int X = 0;
    int Y = 0;
    bool Visible = true;
    bool Active = true;

    Node *Parent = nullptr;
    std::vector<Node *> Children;
};

} // namespace AbyssEngine

#endif // ABYSS_NODE_H
