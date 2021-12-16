#ifndef ABYSS_DC6SPRITE_H
#define ABYSS_DC6SPRITE_H

#include "../engine/image.h"
#include "libabyss/formats/d2/dc6.h"
#include "libabyss/formats/d2/palette.h"

namespace AbyssEngine {

class DC6Sprite : public Image {
  public:
    explicit DC6Sprite(LibAbyss::InputStream &stream, const LibAbyss::Palette &palette);

    void GetFrameOffset(uint32_t frame, int &offsetX, int &offsetY) final;
    void GetFrameSize(uint32_t startFrameIdx, int cellSizeX, uint32_t &width, uint32_t &height) final;
    uint32_t GetNumberOfAnimations() final;
    uint32_t GetFramesPerAnimation() final;

  private:
    void RegenerateAtlas();

    LibAbyss::DC6 _dc6;
    const LibAbyss::Palette &_palette;
};

} // namespace AbyssEngine

#endif // ABYSS_DC6SPRITE_H
