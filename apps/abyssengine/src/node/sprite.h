#ifndef ABYSS_SPRITE_H
#define ABYSS_SPRITE_H

#include "../common/blendmode.h"
#include "../common/rectangle.h"
#include "../engine/image.h"
#include "../systemio/interface.h"
#include "node.h"
#include <sol/sol.hpp>
#include <tuple>

namespace AbyssEngine {

class Sprite : public Node {
  public:
    explicit Sprite(Image &image) : _image(image) {}

    enum class ePlayMode { Unknown, Paused, Forwards, Backwards };
    void UpdateCallback(uint32_t ticks) final;
    void RenderCallback(int offsetX, int offsetY) final;
    void MouseEventCallback(const MouseEvent &event) final;
    std::tuple<uint32_t, uint32_t> GetCellSize();
    void SetCellSize(int x, int y);
    void SetLuaMouseButtonDownHandler(sol::protected_function mouseButtonDownHandler);
    void SetLuaMouseButtonUpHandler(sol::protected_function mouseButtonUpHandler);
    void SetLuaAnimationFinishedHandler(sol::protected_function animationFinishedHandler);
    void SetLoopAnimation(bool loop);
    bool GetLoopAnimation();
    std::string_view LuaGetBlendMode();
    void LuaSetBlendMode(std::string_view val);
    void SetIsBottomOrigin(bool val);
    [[nodiscard]] bool GetIsBottomOrigin() const;
    void LuaSetPlayMode(std::string_view mode);
    std::string_view LuaGetPlayMode();

    void SetCurrentFrameIndex(uint32_t frame);
    uint32_t GetCurrentFrameIndex() const;

    [[nodiscard]] std::string_view NodeType() const final;

  protected:
    void Animate(float time_elapsed);
    void AdvanceFrame();

    Image &_image;
    bool _bottomOrigin = false;
    uint32_t _cellSizeX = 1;
    uint32_t _cellSizeY = 1;
    ePlayMode _playMode = ePlayMode::Paused;
    bool _loopAnimation = true;
    float _lastFrameTime = 0;
    float _playLength = 1.f;
    uint32_t _currentFrame = 0;
    // TODO: unconst and wire
    const uint32_t _currentAnimation = 0;
    sol::protected_function _mouseButtonDownHandler;
    sol::protected_function _mouseButtonUpHandler;
    sol::protected_function _mouseEnterHandler;
    sol::protected_function _mouseLeaveHandler;
    sol::protected_function _mouseMoveHandler;
    sol::protected_function _onAnimationFinished;
    bool _mouseInSprite = false;
};

} // namespace AbyssEngine

#endif // ABYSS_SPRITE_H
