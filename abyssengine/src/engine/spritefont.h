#ifndef ABYSS_SPRITEFONT_H
#define ABYSS_SPRITEFONT_H

#include "../common/color.h"
#include "../common/rectangle.h"
#include "../systemio/interface.h"
#include "libabyss/dc6.h"
#include "libabyss/palette.h"
#include <spdlog/spdlog.h>
#include <string>
#include <type_traits>

namespace AbyssEngine {

class SpriteFont {
  public:
    struct Glyph {
        uint16_t FrameIndex;
        uint8_t Width;
        uint8_t Height;
    };

    struct FramePosition {
        Rectangle Rect;
        int OffsetX;
        int OffsetY;
    };

    SpriteFont(std::string_view filePath, std::string_view paletteName);
    ~SpriteFont() = default;
    void GetMetrics(std::string_view text, int &width, int &height);
    void RenderText(int x, int y, std::string_view text, eBlendMode blendMode, RGB colorMod);

  private:
    void RegenerateAtlas();
    std::unique_ptr<LibAbyss::DC6> _dc6;
    std::unique_ptr<ITexture> _atlas;
    std::vector<Glyph> _glyphs;
    std::vector<FramePosition> _frameRects;
    const LibAbyss::Palette &_palette;
};

} // namespace AbyssEngine

#endif // ABYSS_SPRITEFONT_H
