#ifndef ABYSS_TTFFONT_H
#define ABYSS_TTFFONT_H

#include "../common/alignment.h"
#include "../common/blendmode.h"
#include "../common/color.h"
#include "../systemio/interface.h"
#include "font.h"
#include <cairo/cairo.h>
#include <cairomm/enums.h>
#include <filesystem>
#include <memory>
#include <string_view>

namespace AbyssEngine {
class TtfFont : public IFont {
  public:
    explicit TtfFont(const std::filesystem::path &path, std::string_view name, int size, Cairo::HintStyle hint);
    std::unique_ptr<ITexture> RenderText(const std::string &text, int &width, int &height, eAlignment horizontalAlignment, int maxWidth);

  private:
    std::string _name;
    Cairo::HintStyle _hint;
};
} // namespace AbyssEngine

#endif // ABYSS_TTFFONT_H
