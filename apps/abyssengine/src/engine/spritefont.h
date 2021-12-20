#ifndef ABYSS_SPRITEFONT_H
#define ABYSS_SPRITEFONT_H

#include "../common/alignment.h"
#include "../common/color.h"
#include "../common/rectangle.h"
#include "../systemio/interface.h"
#include "libabyss/formats/d2/dc6.h"
#include "libabyss/formats/d2/palette.h"
#include <spdlog/spdlog.h>
#include <string>
#include <type_traits>
#include "font.h"

namespace AbyssEngine {

class SpriteFont : public IFont {
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
    void GetMetrics(std::string_view text, int &width, int &height, int vertSpacing) const;
    void RenderText(int x, int y, std::string_view text, eBlendMode blendMode, RGB colorMod, eAlignment horizontalAlignment, int vertSpacing);

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
