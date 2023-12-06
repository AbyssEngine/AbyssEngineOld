module;

#include <cstdint>

export module Abyss.Common.MouseState;

import Abyss.Enums.MouseButton;

namespace Abyss::Common {

export class MouseState {
    Enums::MouseButton _button{};
    int _x;
    int _y;
    bool _visible;

  public:
    MouseState() : _x(0), _y(0), _visible(true) {}

    [[nodiscard]] auto isButtonPressed(Enums::MouseButton button) const -> bool { return (static_cast<uint8_t>(_button) & static_cast<uint8_t>(button)) != 0; }

    auto getPosition(int &x, int &y) const -> void {
        x = _x;
        y = _y;
    }

    [[nodiscard]] auto isVisible() const -> bool { return _visible; }

    auto setButtonState(Enums::MouseButton button, const bool pressed) -> void {
        _button = static_cast<Enums::MouseButton>(
            (pressed ? static_cast<uint8_t>(_button) | static_cast<uint8_t>(button) : static_cast<uint8_t>(_button) & ~static_cast<uint8_t>(button)));
    }

    auto setPosition(const int x, const int y) -> void {
        _x = x;
        _y = y;
    }

    auto setVisible(const bool visible) -> void { _visible = visible; }
};

} // namespace Abyss::Common
