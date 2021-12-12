#ifndef ABYSS_BUTTON_H
#define ABYSS_BUTTON_H

#include "../engine/spritefont.h"
#include "node.h"
#include "sprite.h"

namespace AbyssEngine {

class Button : public Node {
  public:
    enum class eState { Normal, Pressed, Hover, Disabled, Unknown };

    Button(SpriteFont *spriteFont, Sprite *sprite);
    ~Button();

    void UpdateCallback(uint32_t ticks) override;
    void RenderCallback(int offsetX, int offsetY) override;
    void MouseEventCallback(const MouseEvent &event) override;
    void SetSegments(int x, int y);
    void SetSize(int x, int y);
    void SetCaption(std::string_view caption);
    std::string_view GetCaption() const;
    void SetTextOffset(int x, int y);
    void SetChecked(bool b);
    bool GetChecked() const;
    void SetLabelColor(uint8_t red, uint8_t green, uint8_t blue);
    std::string_view LuaGetLabelBlendMode() const;
    void LuaSetLabelBlendMode(std::string_view val);
    void LuaSetFrameIndex(std::string_view frameType, int index);
    void LuaSetActivateCallback(sol::safe_function luaActivateCallback);
    void LuaSetPressCallback(sol::safe_function luaPressCallback);

    [[nodiscard]] std::string_view NodeType() const final { return "Button Node"; }
    void Initialize() override;

  private:
    Sprite *_sprite;
    SpriteFont *_spriteFont;
    eState _buttonState = eState::Normal;
    std::string _caption;
    bool _checked = false;
    uint8_t _xSegments = 0;
    uint8_t _ySegments = 0;
    int _textOffsetX = 0;
    int _textOffsetY = 0;
    int _fixedWidth = 0;
    int _fixedHeight = 0;
    RGB _labelColor = {255, 255, 255};
    eBlendMode _labelBlend = eBlendMode::None;
    int _frameIndexNormal = 0;
    int _frameIndexPressed = -1;
    int _frameIndexHover = -1;
    int _frameIndexDisabled = -1;
    int _frameIndexCheckedNormal = -1;
    int _frameIndexCheckedHover = -1;
    int _frameIndexCheckedPressed = -1;
    int _curCaptionWidth = 0;
    int _curCaptionHeight = 0;
    bool _ignoreMouseActivation = false;
    sol::safe_function _luaActivateCallback;
    sol::safe_function _luaPressedCallback;
};
} // namespace AbyssEngine

#endif // ABYSS_BUTTON_H
