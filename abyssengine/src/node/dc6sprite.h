#ifndef ABYSS_DC6SPRITE_H
#define ABYSS_DC6SPRITE_H

#include "libabyss/dc6.h"
#include "sprite.h"
#include "libabyss/palette.h"

namespace AbyssEngine {

class DC6Sprite : public Sprite {
  public:
    explicit DC6Sprite(std::string_view name, LibAbyss::InputStream &stream, const LibAbyss::Palette &palette);

    void GetFrameOffset(uint32_t frame, int &offsetX, int &offsetY) final;
    void GetFrameSize(uint32_t frame, uint32_t &width, uint32_t &height) final;
    uint32_t GetNumberOfAnimations() final;
    uint32_t GetFramesPerAnimation() final;

  protected:
    void RegenerateAtlas() final;

  private:
    LibAbyss::DC6 _dc6;
    const LibAbyss::Palette &_palette;
};

} // namespace AbyssEngine

#endif // ABYSS_DC6SPRITE_H
