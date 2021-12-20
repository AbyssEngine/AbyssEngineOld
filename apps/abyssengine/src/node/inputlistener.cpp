#include "inputlistener.h"
#include "../common/overload.h"
#include "../engine/engine.h"
#include <utility>
#include <variant>

void AbyssEngine::InputListener::MouseEventCallback(const AbyssEngine::MouseEvent &event) {
    std::visit(Overload{[this](const MouseMoveEvent &evt) {
                            if (!this->_luaMouseMoveCallback.valid())
                                return;

                            auto result = this->_luaMouseMoveCallback(evt.X, evt.Y);

                            if (result.valid())
                                return;

                            sol::error err = result;
                            Engine::Get()->Panic(err.what());
                        },
                        [this](const MouseButtonEvent &evt) {
                            if (!this->_luaMouseButtonCallback.valid())
                                return;

                            auto result = this->_luaMouseButtonCallback(evt.Button, evt.IsPressed);

                            if (result.valid())
                                return;

                            sol::error err = result;
                            Engine::Get()->Panic(err.what());
                        }},
               event);

    Node::MouseEventCallback(event);
}

void AbyssEngine::InputListener::LuaSetMouseButtonCallback(sol::safe_function func) { this->_luaMouseButtonCallback = std::move(func); }

void AbyssEngine::InputListener::LuaSetMouseMoveCallback(sol::safe_function func) { this->_luaMouseMoveCallback = std::move(func); }

void AbyssEngine::InputListener::LuaSetKeyCallback(sol::safe_function func) { this->_luaKeyCallback = std::move(func); }
void AbyssEngine::InputListener::KeyboardEventCallback(const AbyssEngine::KeyboardEvent &event) {
    if (!this->_luaKeyCallback.valid())
        return;

    auto result = this->_luaKeyCallback(event.Scancode, event.Pressed);

    if (result.valid())
        return;

    sol::error err = result;
    Engine::Get()->Panic(err.what());
}
