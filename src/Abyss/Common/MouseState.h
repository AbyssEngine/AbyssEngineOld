#pragma once

#include "Abyss/Enums/MouseButton.h"

namespace Abyss::Common {

class MouseState {
    Enums::MouseButton _button{};
    int _x;
    int _y;
    bool _visible;

  public:
    MouseState();
    [[nodiscard]] bool isButtonPressed(Enums::MouseButton button) const;
    void getPosition(int &x, int &y) const;
    [[nodiscard]] bool isVisible() const;
    void setButtonState(Enums::MouseButton button, bool pressed);
    void setPosition(int x, int y);
    void setVisible(bool visible);
};

} // namespace Abyss::Common
