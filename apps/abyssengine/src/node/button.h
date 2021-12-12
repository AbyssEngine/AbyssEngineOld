#ifndef ABYSS_BUTTON_H
#define ABYSS_BUTTON_H

#include "node.h"
#include "sprite.h"

namespace AbyssEngine {

class Button : public Node {
  public:
    enum class eState { Normal, Pressed, Hover, Disabled, Unknown };

    explicit Button(Sprite &sprite);
    ~Button();

    void UpdateCallback(uint32_t ticks) override;
    void RenderCallback(int offsetX, int offsetY) override;
    void MouseEventCallback(const MouseEvent &event) override;
    void SetSegments(int x, int y);
    void SetSize(int x, int y);
    void SetPressedOffset(int x, int y);
    void SetChecked(bool b);
    bool GetChecked() const;
    void LuaSetFrameIndex(std::string_view frameType, int index);
    void LuaSetActivateCallback(sol::safe_function luaActivateCallback);
    void LuaSetPressCallback(sol::safe_function luaPressCallback);

    [[nodiscard]] std::string_view NodeType() const final { return "Button Node"; }
    void Initialize() override;

  private:
    Sprite &_sprite;
    eState _buttonState = eState::Normal;
    bool _checked = false;
    uint8_t _xSegments = 0;
    uint8_t _ySegments = 0;
    int _pressedOffsetX = 0;
    int _pressedOffsetY = 0;
    int _fixedWidth = 0;
    int _fixedHeight = 0;
    int _frameIndexNormal = 0;
    int _frameIndexPressed = -1;
    int _frameIndexHover = -1;
    int _frameIndexDisabled = -1;
    int _frameIndexCheckedNormal = -1;
    int _frameIndexCheckedHover = -1;
    int _frameIndexCheckedPressed = -1;
    bool _ignoreMouseActivation = false;
    sol::safe_function _luaActivateCallback;
    sol::safe_function _luaPressedCallback;
};
} // namespace AbyssEngine

#endif // ABYSS_BUTTON_H
