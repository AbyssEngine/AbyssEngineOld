#include "MouseState.h"

namespace Abyss::Common {

MouseState::MouseState() : _x(0), _y(0), _visible(true) {}

[[nodiscard]] bool MouseState::isButtonPressed(Enums::MouseButton button) const { return (static_cast<uint8_t>(_button) & static_cast<uint8_t>(button)) != 0; }

void MouseState::getPosition(int &x, int &y) const {
    x = _x;
    y = _y;
}

[[nodiscard]] bool MouseState::isVisible() const { return _visible; }

void MouseState::setButtonState(Enums::MouseButton button, const bool pressed) {
    _button = static_cast<Enums::MouseButton>(
        (pressed ? static_cast<uint8_t>(_button) | static_cast<uint8_t>(button) : static_cast<uint8_t>(_button) & ~static_cast<uint8_t>(button)));
}

void MouseState::setPosition(const int x, const int y) {
    _x = x;
    _y = y;
}

void MouseState::setVisible(const bool visible) { _visible = visible; }

} // namespace Abyss::Common
