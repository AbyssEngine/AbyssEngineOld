#ifndef ABYSS_TTFFONT_H
#define ABYSS_TTFFONT_H

#include "../common/blendmode.h"
#include "../common/color.h"
#include "../systemio/interface.h"
#include "font.h"
#include <memory>
#include <string_view>

namespace AbyssEngine {
class TtfFont : public IFont {
  public:
    explicit TtfFont(std::string_view filePath, int size, ITtf::Hinting hinting);
    std::unique_ptr<ITexture> RenderText(std::string_view text, int &width, int &height);

  private:
    std::unique_ptr<ITtf> _ttf;
};
} // namespace AbyssEngine

#endif // ABYSS_TTFFONT_H
