#include "button.h"
#include "../engine/engine.h"
#include "../hostnotify/hostnotify.h"
#include <absl/strings/ascii.h>
#include <absl/strings/str_cat.h>

#include <spdlog/spdlog.h>
#include <utility>

AbyssEngine::Button::Button(Sprite &sprite)
    : _sprite(sprite), _luaActivateCallback(), _luaPressedCallback() {
}

AbyssEngine::Button::~Button() = default;

void AbyssEngine::Button::UpdateCallback(uint32_t ticks) {
    auto engine = Engine::Get();

    auto mouse_state = engine->GetMouseButtonState();
    int mx;
    int my;
    int nx1 = 0;
    int ny1 = 0;

    engine->GetCursorPosition(mx, my);
    GetEffectiveLayout(nx1, ny1);

    const auto nx2 = nx1 + _fixedWidth;
    const auto ny2 = ny1 + _fixedHeight;

    const auto mouse_hovered = (mx >= nx1) && (mx < nx2) && (my >= ny1) && (my < ny2);
    const auto mouse_clicked = (int)(mouse_state & eMouseButton::Left) > 0;
    const auto focused_node = engine->GetFocusedNode();
    const bool this_is_focused = this == focused_node;
    const bool any_is_focused = focused_node != nullptr;

    if (this_is_focused) {
        if (!mouse_clicked && !mouse_hovered) {
            engine->SetFocusedNode(nullptr);
            _buttonState = eState::Normal;
            Node::UpdateCallback(ticks);
            return;
        }

        if (mouse_clicked && mouse_hovered) {
            _buttonState = eState::Pressed;
            Node::UpdateCallback(ticks);
            return;
        }

        if (!mouse_clicked && mouse_hovered) {
            engine->SetFocusedNode(nullptr);
            _buttonState = eState::Hover;

            if (_luaActivateCallback.valid()) {
                auto result = _luaActivateCallback();
                if (!result.valid()) {
                    sol::error err = result;
                    SPDLOG_ERROR(err.what());
                    AbyssEngine::HostNotify::Notify(eNotifyType::Fatal, "Script Error", err.what());
                    return;
                }
            }
            Node::UpdateCallback(ticks);
            return;
        }

        _buttonState = eState::Normal;

        Node::UpdateCallback(ticks);

        return;
    }

    if (!mouse_hovered && mouse_clicked) {
        _ignoreMouseActivation = true;
        Node::UpdateCallback(ticks);
        return;
    }

    if (_ignoreMouseActivation && !mouse_clicked) {
        _ignoreMouseActivation = false;
        Node::UpdateCallback(ticks);
        return;
    }

    if (!_ignoreMouseActivation && mouse_hovered && !mouse_clicked && !any_is_focused) {
        _buttonState = eState::Hover;
        Node::UpdateCallback(ticks);
        return;
    }

    if (!_ignoreMouseActivation && mouse_hovered && mouse_clicked && (!any_is_focused)) {
        _buttonState = eState::Pressed;
        engine->SetFocusedNode(this);
        Node::UpdateCallback(ticks);

        if (_luaPressedCallback.valid()) {
            auto result = _luaPressedCallback();
            if (!result.valid()) {
                sol::error err = result;
                SPDLOG_ERROR(err.what());
                AbyssEngine::HostNotify::Notify(eNotifyType::Fatal, "Script Error", err.what());
                return;
            }
        }

        return;
    }

    _buttonState = eState::Normal;

    Node::UpdateCallback(ticks);
}

void AbyssEngine::Button::RenderCallback(int offsetX, int offsetY) {
    if (!Visible || !Active)
        return;

    int frame_index = -1;
    int text_offset_x = 0;
    int text_offset_y = 0;

    switch (_buttonState) {
    case eState::Normal:
        frame_index = _checked ? _frameIndexCheckedNormal : _frameIndexNormal;
        break;
    case eState::Pressed:
        frame_index = _checked ? _frameIndexCheckedPressed : _frameIndexPressed;
        text_offset_x = _pressedOffsetX;
        text_offset_y = _pressedOffsetY;
        break;
    case eState::Hover:
        frame_index = _checked ? _frameIndexCheckedHover : _frameIndexHover;
        break;
    case eState::Disabled:
        frame_index = _frameIndexDisabled;
        break;
    case eState::Unknown:
        frame_index = -1;
        break;
    }

    if (frame_index < 0)
        frame_index = _frameIndexNormal;

    _sprite.SetCellSize(_xSegments, _ySegments);
    _sprite.Render(frame_index, X + offsetX, Y + offsetY);

    Node::RenderCallback(offsetX + text_offset_x, offsetY + text_offset_y);
}

void AbyssEngine::Button::MouseEventCallback(const AbyssEngine::MouseEvent &event) { Node::MouseEventCallback(event); }
void AbyssEngine::Button::SetSegments(int x, int y) {
    _xSegments = x;
    _ySegments = y;
}

void AbyssEngine::Button::SetSize(int x, int y) {
    _fixedWidth = x;
    _fixedHeight = y;
}

bool AbyssEngine::Button::GetChecked() const {
    return _checked;
}
void AbyssEngine::Button::SetChecked(bool b) {
    _checked = b;
}

void AbyssEngine::Button::SetPressedOffset(int x, int y) {
    _pressedOffsetX = x;
    _pressedOffsetY = y;
}

void AbyssEngine::Button::LuaSetFrameIndex(std::string_view frameType, int index) {
    std::string str = absl::AsciiStrToLower(frameType);

    if (str == "normal")
        _frameIndexNormal = index;
    else if (str == "pressed")
        _frameIndexPressed = index;
    else if (str == "hover")
        _frameIndexHover = index;
    else if (str == "disabled")
        _frameIndexDisabled = index;
    else if (str == "checkednormal")
        _frameIndexCheckedNormal = index;
    else if (str == "checkedhover")
        _frameIndexCheckedHover = index;
    else if (str == "checkedpressed")
        _frameIndexCheckedPressed = index;
    else
        throw std::runtime_error(absl::StrCat("Unknown button state: ", frameType));
}

void AbyssEngine::Button::LuaSetActivateCallback(sol::safe_function luaActivateCallback) { _luaActivateCallback = std::move(luaActivateCallback); }

void AbyssEngine::Button::Initialize() { _sprite.Initialize(); }

void AbyssEngine::Button::LuaSetPressCallback(sol::safe_function luaPressCallback) { _luaPressedCallback = std::move(luaPressCallback); }
