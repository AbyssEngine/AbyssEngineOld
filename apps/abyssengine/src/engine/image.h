#ifndef ABYSS_IMAGE_H
#define ABYSS_IMAGE_H

#include "../systemio/interface.h"
#include <stdint.h>

namespace AbyssEngine {

class Image {
  public:
    struct FramePosition {
        Rectangle Rect;
        int OffsetX;
        int OffsetY;
    };

    virtual ~Image() = default;
    void Render(uint32_t frameIdx, int cellSizeX, int cellSizeY, int posX, int posY);
    virtual uint32_t GetNumberOfAnimations() = 0;
    virtual uint32_t GetFramesPerAnimation() = 0;
    virtual void GetFrameSize(uint32_t startFrameIdx, int cellSizeX, uint32_t &width, uint32_t &height) = 0;

    void SetBlendMode(eBlendMode mode);
    eBlendMode GetBlendMode() const { return _blendMode; }

  protected:
    virtual void GetFrameOffset(uint32_t frameIdx, int &offsetX, int &offsetY) = 0;

    std::unique_ptr<ITexture> _atlas;
    std::vector<FramePosition> _framePositions;
    eBlendMode _blendMode = eBlendMode::Blend;
};

} // namespace AbyssEngine

#endif // ABYSS_IMAGE_H
