#ifndef ABYSS_SPRITE_H
#define ABYSS_SPRITE_H

#include "../common/rectangle.h"
#include "node.h"
#include "../systemio/interface.h"
#include "../common/blendmode.h"
#include <tuple>
#include <sol/sol.hpp>

namespace AbyssEngine {

class Sprite : public Node {
  public:
    Sprite() = default;
    explicit Sprite(std::string_view name) : Node(name) {}

    enum class ePlayMode { Unknown, Paused, Forwards, Backwards };
    struct FramePosition {
        Rectangle Rect;
        int OffsetX;
        int OffsetY;
    };

    void UpdateCallback(uint32_t ticks) final;
    void RenderCallback(int offsetX, int offsetY) final;
    void MouseEventCallback(const MouseEvent& event) final;
    void Render(uint32_t startFrameIdx, int offsetX, int offsetY);
    void Initialize() override;

    virtual void GetFrameOffset(uint32_t frame, int &offsetX, int &offsetY) = 0;
    virtual void GetFrameSize(uint32_t frame, uint32_t &width, uint32_t &height) = 0;
    virtual uint32_t GetNumberOfAnimations() = 0;
    virtual uint32_t GetFramesPerAnimation() = 0;

    std::tuple<uint32_t, uint32_t> GetCellSize();
    void SetCellSize(int x, int y);
    void SetLuaMouseButtonDownHandler(sol::protected_function mouseButtonDownHandler);
    void SetLuaMouseButtonUpHandler(sol::protected_function mouseButtonUpHandler);
    std::string_view LuaGetBlendMode();
    void LuaSetBlendMode(std::string_view val);
    void SetIsBottomOrigin(bool val);
    [[nodiscard]] bool GetIsBottomOrigin() const;
    void LuaSetPlayMode(std::string_view mode);
    std::string_view LuaGetPlayMode();

    [[nodiscard]] std::string_view NodeType() const final { return "Sprite Node"; }

    ;

  protected:
    virtual void RegenerateAtlas() = 0;

    void Animate(float time_elapsed);
    void AdvanceFrame();

    std::unique_ptr<ITexture> _atlas;
    std::vector<FramePosition> _framePositions;
    bool _bottomOrigin = false;
    uint32_t _cellSizeX = 1;
    uint32_t _cellSizeY = 1;
    ePlayMode _playMode = ePlayMode::Paused;
    bool _loopAnimation = true;
    float _lastFrameTime = 0;
    float _playLength = 1.f;
    uint32_t _currentFrame = 0;
    uint32_t _currentAnimation = 0;
    sol::protected_function _mouseButtonDownHandler;
    sol::protected_function _mouseButtonUpHandler;
    sol::protected_function _mouseEnterHandler;
    sol::protected_function _mouseLeaveHandler;
    sol::protected_function _mouseMoveHandler;
    eBlendMode _blendMode = eBlendMode::Blend;
    bool _mouseInSprite = false;
};

} // namespace AbyssEngine

#endif // ABYSS_SPRITE_H
