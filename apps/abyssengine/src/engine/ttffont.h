#ifndef ABYSS_TTFFONT_H
#define ABYSS_TTFFONT_H

#include "../common/blendmode.h"
#include "../common/color.h"
#include "../systemio/interface.h"
#include "font.h"
#include <memory>
#include <string_view>
#include <filesystem>
#include <cairo/cairo.h>
#include <cairomm/enums.h>

namespace AbyssEngine {
class TtfFont : public IFont {
  public:
    explicit TtfFont(const std::filesystem::path& path, std::string_view name, int size, Cairo::HintStyle hint);
    std::unique_ptr<ITexture> RenderText(const std::string& text, int &width, int &height);

  private:
    std::string _name;
    Cairo::HintStyle _hint;
};
} // namespace AbyssEngine

#endif // ABYSS_TTFFONT_H
