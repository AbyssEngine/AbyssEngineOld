#include "sprite.h"

#include "../common/overload.h"
#include <absl/strings/ascii.h>
#include <absl/strings/str_cat.h>
#include <utility>

void AbyssEngine::Sprite::UpdateCallback(uint32_t ticks) {
    if (!Active || !(_playMode == ePlayMode::Backwards || _playMode == ePlayMode::Forwards)) {
        Node::UpdateCallback(ticks);
        return;
    }

    Animate((float)ticks / 1000.f);

    Node::UpdateCallback(ticks);
}

void AbyssEngine::Sprite::Render(uint32_t startFrameIdx, int offsetX, int offsetY) {
    const auto totalFrames = GetFramesPerAnimation();
    // const auto totalAnimations = GetNumberOfAnimations();

    if (_framePositions.empty())
        return;

    uint32_t frameWidth;
    uint32_t frameHeight;
    GetFrameSize(_currentFrame, frameWidth, frameHeight);

    auto posX = X + offsetX;
    auto posY = Y + offsetY;

    if (_bottomOrigin)
        posY -= (int)frameHeight;

    const auto startX = posX;

    for (auto cellOffsetY = 0; cellOffsetY < _cellSizeY; cellOffsetY++) {
        auto lastHeight = 0;

        for (auto cellOffsetX = 0; cellOffsetX < _cellSizeX; cellOffsetX++) {
            const auto cellIndex = startFrameIdx + (cellOffsetX + (cellOffsetY * _cellSizeX));
            const auto framePos = _framePositions[(_currentAnimation * totalFrames) + cellIndex];

            Rectangle destRect = framePos.Rect;
            destRect.X = framePos.OffsetX + posX;
            destRect.Y = framePos.OffsetY + posY;

            _atlas->Render(framePos.Rect, destRect);

            posX += destRect.Width;
            lastHeight = destRect.Height;
        }

        posX = startX;
        posY += lastHeight;
    }
}

void AbyssEngine::Sprite::RenderCallback(int offsetX, int offsetY) {
    if (!Visible || !Active)
        return;

    Render(_currentFrame, offsetX, offsetY);

    Node::RenderCallback(X + offsetX, Y + offsetY);
}

void AbyssEngine::Sprite::MouseEventCallback(const AbyssEngine::MouseEvent &event) {
    std::visit(Overload{[this](const MouseMoveEvent &evt) {
                            const int mx = evt.X;
                            const int my = evt.Y;

                            const int sx = X;
                            const int sy = Y;
                            int sx2 = sx;
                            int sy2 = sy;
                            uint32_t fx;
                            uint32_t fy;
                            GetFrameSize(_currentFrame, fx, fy);
                            sx2 += (int)fx;
                            sy2 += (int)fy;

                            if ((mx < sx) || (mx >= sx2) || (my < sy) || (my >= sy2)) {
                                if (_mouseInSprite) {
                                    _mouseInSprite = false;
                                    if (_mouseLeaveHandler.valid()) {
                                        auto result = _mouseLeaveHandler();
                                        if (!result.valid()) {
                                            sol::error err = result;
                                            throw std::runtime_error(err.what());
                                        }
                                    }
                                }
                                return;
                            }

                            if (!_mouseInSprite) {
                                _mouseInSprite = true;
                                if (_mouseEnterHandler.valid()) {
                                    auto result = _mouseEnterHandler();
                                    if (!result.valid()) {
                                        sol::error err = result;
                                        throw std::runtime_error(err.what());
                                    }
                                }
                                return;
                            }

                            if (_mouseMoveHandler.valid()) {
                                auto result = _mouseMoveHandler();
                                if (!result.valid()) {
                                    sol::error err = result;
                                    throw std::runtime_error(err.what());
                                }
                            }
                        },
                        [this](const MouseButtonEvent &evt) {
                            if (!_mouseInSprite)
                                return;

                            if (evt.IsPressed && _mouseButtonDownHandler.valid()) {
                                auto result = _mouseButtonDownHandler();
                                if (!result.valid()) {
                                    sol::error err = result;
                                    throw std::runtime_error(err.what());
                                }

                            } else if (!evt.IsPressed && _mouseButtonUpHandler.valid()) {
                                auto result = _mouseButtonUpHandler();
                                if (!result.valid()) {
                                    sol::error err = result;
                                    throw std::runtime_error(err.what());
                                }
                            }
                        }},
               event);

    Node::MouseEventCallback(event);
}

void AbyssEngine::Sprite::Animate(float elapsed) {
    if (_playMode == ePlayMode::Unknown || _playMode == ePlayMode::Paused)
        return;

    const auto frameCount = GetFramesPerAnimation();
    const auto frameLength = _playLength / (float)frameCount;
    _lastFrameTime += elapsed;
    const auto framesAdvanced = (int)(_lastFrameTime / frameLength);
    _lastFrameTime -= (float)framesAdvanced * frameLength;

    for (auto i = 0; i < framesAdvanced; i++)
        AdvanceFrame();
}

void AbyssEngine::Sprite::AdvanceFrame() {
    if (_playMode == ePlayMode::Unknown || _playMode == ePlayMode::Paused)
        return;

    const auto startIndex = 0;
    const auto endIndex = GetFramesPerAnimation();

    switch (_playMode) {
    case ePlayMode::Forwards: {
        _currentFrame++;

        if (_currentFrame < endIndex)
            return;

        _currentFrame = _loopAnimation ? startIndex : endIndex - 1;
    }
        return;
    case ePlayMode::Backwards: {
        if (_currentFrame != 0) {
            _currentFrame--;
            return;
        }

        _currentFrame = _loopAnimation ? endIndex - 1 : startIndex;
    }
    default:
        throw std::runtime_error("Unimplemented play mode");
    }
}
std::tuple<uint32_t, uint32_t> AbyssEngine::Sprite::GetCellSize() { return {_cellSizeX, _cellSizeY}; }
void AbyssEngine::Sprite::SetCellSize(const int x, const int y) {
    _cellSizeX = x;
    _cellSizeY = y;
}
void AbyssEngine::Sprite::SetLuaMouseButtonDownHandler(sol::protected_function mouseButtonDownHandler) {
    _mouseButtonDownHandler = std::move(mouseButtonDownHandler);
}

void AbyssEngine::Sprite::SetLuaMouseButtonUpHandler(sol::protected_function mouseButtonUpHandler) {
    _mouseButtonUpHandler = std::move(mouseButtonUpHandler);
}

std::string_view AbyssEngine::Sprite::LuaGetBlendMode() { return BlendModeToString(_blendMode); }

void AbyssEngine::Sprite::LuaSetBlendMode(std::string_view val) {
    _blendMode = StringToBlendMode(val);
    if (_atlas != nullptr)
        _atlas->SetBlendMode(_blendMode);
}

void AbyssEngine::Sprite::SetIsBottomOrigin(bool val) { _bottomOrigin = val; }

bool AbyssEngine::Sprite::GetIsBottomOrigin() const { return _bottomOrigin; }

void AbyssEngine::Sprite::LuaSetPlayMode(std::string_view mode) {
    auto str = absl::AsciiStrToLower(mode);

    if (str == "paused")
        _playMode = ePlayMode::Paused;
    else if (str == "forwards")
        _playMode = ePlayMode::Forwards;
    else if (str == "backwards")
        _playMode = ePlayMode::Backwards;
    else if (str.empty())
        _playMode = ePlayMode::Unknown;
    else
        throw std::runtime_error(absl::StrCat("Unknown play mode: ", mode));
}
std::string_view AbyssEngine::Sprite::LuaGetPlayMode() {
    switch (_playMode) {

    case ePlayMode::Unknown:
        return "";
    case ePlayMode::Paused:
        return "paused";
    case ePlayMode::Forwards:
        return "forwards";
    case ePlayMode::Backwards:
        return "backwards";
    }
    return "";
}

void AbyssEngine::Sprite::Initialize() {
    RegenerateAtlas();
    _atlas->SetBlendMode(_blendMode);
}
