#ifndef ABYSS_INPUTLISTENER_H
#define ABYSS_INPUTLISTENER_H

#include "node.h"
namespace AbyssEngine {

class InputListener : public Node {
  public:
    void MouseEventCallback(const MouseEvent &event) final;
    void KeyboardEventCallback(const KeyboardEvent &event) final;

    void LuaSetMouseButtonCallback(sol::safe_function func);
    void LuaSetMouseMoveCallback(sol::safe_function func);
    void LuaSetKeyCallback(sol::safe_function func);

  private:
    sol::safe_function _luaMouseButtonCallback;
    sol::safe_function _luaMouseMoveCallback;
    sol::safe_function _luaKeyCallback;
};

}

#endif // ABYSS_INPUTLISTENER_H
