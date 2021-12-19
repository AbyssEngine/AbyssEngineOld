#ifndef ABYSS_INPUTLISTENER_H
#define ABYSS_INPUTLISTENER_H

#include "node.h"
namespace AbyssEngine {

class InputListener : public Node {
  public:
    void MouseEventCallback(const MouseEvent &event) override;

    void LuaSetMouseButtonCallback(sol::safe_function func);
    void LuaSetMouseMoveCallback(sol::safe_function func);

  private:
    sol::safe_function _luaMouseButtonCallback;
    sol::safe_function _luaMouseMoveCallback;
};

}

#endif // ABYSS_INPUTLISTENER_H
