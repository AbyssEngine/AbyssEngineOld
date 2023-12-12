#pragma once

#include "Abyss/Concepts/Drawable.h"
#include "Abyss/Singletons.h"
#include "Abyss/Streams/SoundEffect.h"
#include "ButtonDef.h"
#include "Label.h"

#include <functional>

namespace Abyss::UI {

enum class ButtonState { Normal, Pressed, Disabled };

template <Concepts::Drawable T> class Button {
    T _drawable;
    ButtonDef _def;
    ButtonState _state;
    std::string _text;
    Label _label;
    std::function<void()> _onClick;
    bool _hasFocus{false};
    bool _canFocus{false};
    Streams::SoundEffect _clickSound;

    void handleMouse(const int x, const int y) {
        const auto mouseState = Singletons::getMouseProvider().getMouseState();
        int mouseX, mouseY;
        mouseState.getPosition(mouseX, mouseY);
        const auto leftMousePressed = mouseState.isButtonPressed(Enums::MouseButton::Left);

        mouseX -= x;
        mouseY -= y;
        const auto isMouseOver = mouseX >= _def.clickableRect.x && mouseX <= _def.clickableRect.x + _def.clickableRect.w && mouseY >= _def.clickableRect.y &&
                                 mouseY <= _def.clickableRect.y + _def.clickableRect.h;

        if (isMouseOver && !leftMousePressed && _hasFocus) {
            _onClick();
            _hasFocus = false;
            _state = ButtonState::Normal; // TODO: Disabled state
        }

        if (isMouseOver && leftMousePressed && !_hasFocus && _canFocus) {
            _hasFocus = true;
            _clickSound.play();
        }

        if (!_hasFocus)
            _canFocus = !leftMousePressed;

        if (!leftMousePressed) {
            _hasFocus = false;
            _canFocus = true;
        }

        _state = isMouseOver && leftMousePressed && _canFocus ? ButtonState::Pressed : ButtonState::Normal;
    }

  public:
    explicit Button(const ButtonDef &buttonDef, const std::string_view text, const Concepts::FontRenderer &fontRenderer, std::function<void()> onClick)
        : _drawable(buttonDef.resourceName), _def(buttonDef), _state(ButtonState::Normal), _text(text), _label(fontRenderer), _onClick(std::move(onClick)),
          _clickSound(std::make_unique<Streams::AudioStream>(Singletons::getFileProvider().loadFile(buttonDef.clickSound))) {
        _drawable.setPalette(_def.palette);
        _label.setText(_text);
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

        auto labelX = x + _def.clickableRect.x + (_def.clickableRect.w / 2) - (_label.getWidth() / 2);
        auto labelY = y + _def.clickableRect.y + (_def.clickableRect.h / 2) - (_label.getHeight() / 2);

        if (_state == ButtonState::Pressed) {
            labelX -= 2;
            labelY += 2;
        }

        _label.draw(labelX, labelY);
    }
};

} // namespace Abyss::UI
