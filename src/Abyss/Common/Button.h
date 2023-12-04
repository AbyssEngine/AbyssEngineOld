#pragma once

#include <Abyss/AbyssEngine.h>
#include <Abyss/Common/Drawable.h>

namespace Abyss::Common {

struct ButtonDef {
    std::string name;
    std::string resourceName;
    DataTypes::Palette palette;
    std::string fontPath;
    SDL_Rect clickableRect;
    struct {
        uint8_t x;
        uint8_t y;
    } segments;
    struct {
        int8_t base;
        int8_t pressed;
        int8_t disabled;
    } frames;
};

enum class ButtonState { Normal, Pressed, Disabled };

template <Drawable T> class Button {
    T _drawable;
    ButtonDef _def;
    ButtonState _state;
    std::string _text;
    std::function<void()> _onClick;

    auto handleMouse(const int x, const int y) -> void {
        const auto mouseState = AbyssEngine::getInstance().getMouseState();
        int mouseX, mouseY;
        mouseState.getPosition(mouseX, mouseY);
        const auto leftMousePressed = mouseState.isButtonPressed(MouseButton::Left);
        mouseX -= x;
        mouseY -= y;
        const auto isMouseOver = mouseX >= _def.clickableRect.x && mouseX <= _def.clickableRect.x + _def.clickableRect.w && mouseY >= _def.clickableRect.y &&
                                 mouseY <= _def.clickableRect.y + _def.clickableRect.h;

        _state = isMouseOver && leftMousePressed ? ButtonState::Pressed : ButtonState::Normal;
    }

  public:
    explicit Button(ButtonDef buttonDef, const std::string_view text, std::function<void()> onClick)
        : _drawable(buttonDef.resourceName), _def(buttonDef), _state(ButtonState::Normal), _text(text), _onClick(std::move(onClick)) {
        _drawable.setPalette(_def.palette);
    }

    auto draw(const int x, const int y) -> void {
        handleMouse(x, y);

        auto posX = x;
        auto posY = y;
        auto frameIdx = -1;
        const int framesPerType = _def.segments.x * _def.segments.y;
        switch (_state) {
        case ButtonState::Normal:
            frameIdx = _def.frames.base * framesPerType;
            break;
        case ButtonState::Pressed:
            frameIdx = _def.frames.pressed * framesPerType;
            break;
        case ButtonState::Disabled:
            frameIdx = _def.frames.disabled * framesPerType;
            break;
        }

        for (int frameY = 0; frameY < _def.segments.y; frameY++) {
            int frameHeight;
            for (int frameX = 0; frameX < _def.segments.x; frameX++) {
                int frameWidth;
                _drawable.getFrameSize(frameIdx, frameWidth, frameHeight);
                _drawable.draw(frameIdx++, posX, posY + frameHeight);
                posX += frameWidth;
            }
            posX = x;
            posY += frameHeight;
        }
    }
};

} // namespace Abyss::Common
