#ifndef ABYSS_D2RSPRITE_H
#define ABYSS_D2RSPRITE_H

#include "sprite.h"
#include <vector>

namespace AbyssEngine {

class D2RSprite : public Sprite {
  public:
    explicit D2RSprite(LibAbyss::InputStream &stream);

    void GetFrameOffset(uint32_t frame, int &offsetX, int &offsetY) final;
    void GetFrameSize(uint32_t frame, uint32_t &width, uint32_t &height) final;
    uint32_t GetNumberOfAnimations() final;
    uint32_t GetFramesPerAnimation() final;

  protected:
    void RegenerateAtlas() final;

  private:
    uint32_t _frames;
    uint32_t _width_total;
    uint32_t _height;
    std::vector<uint8_t> _data;
};

} // namespace AbyssEngine

#endif // ABYSS_D2RSPRITE_H
