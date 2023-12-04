#pragma once

#include <cstdint>

namespace Abyss::Common {

enum class MouseButton : uint8_t {
    Left = 0x01,
    Right = 0x02,
    Middle = 0x04,
};

class MouseState {
    MouseButton _button;
    int _x;
    int _y;
    bool _visible;

  public:
    MouseState();

    [[nodiscard]] auto isButtonPressed(MouseButton button) const -> bool;
    auto getPosition(int &x, int &y) const -> void;
    [[nodiscard]] auto isVisible() const -> bool;
    auto setButtonState(MouseButton button, bool pressed) -> void;
    auto setPosition(int x, int y) -> void;
    auto setVisible(bool visible) -> void;
};

} // namespace Abyss::Common
