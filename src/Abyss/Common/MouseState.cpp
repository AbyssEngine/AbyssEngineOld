#include "MouseState.h"

namespace Abyss::Common {

MouseState::MouseState() : _button(), _x(0), _y(0), _visible(true) {}

auto MouseState::isButtonPressed(MouseButton button) const -> bool { return (static_cast<uint8_t>(_button) & static_cast<uint8_t>(button)) != 0; }

auto MouseState::getPosition(int &x, int &y) const -> void {
    x = _x;
    y = _y;
}

auto MouseState::setButtonState(MouseButton button, const bool pressed) -> void {
    if (pressed) {
        _button = static_cast<MouseButton>(static_cast<uint8_t>(_button) | static_cast<uint8_t>(button));
    } else {
        _button = static_cast<MouseButton>(static_cast<uint8_t>(_button) & ~static_cast<uint8_t>(button));
    }
}

auto MouseState::setPosition(const int x, const int y) -> void {
    _x = x;
    _y = y;
}

auto MouseState::isVisible() const -> bool { return _visible; }

auto MouseState::setVisible(const bool visible) -> void { _visible = visible; }

} // namespace Abyss::Common
