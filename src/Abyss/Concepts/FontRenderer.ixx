module;

#include <SDL2/SDL.h>
#include <memory>
#include <string_view>

export module Abyss.Concepts.FontRenderer;

namespace Abyss::Concepts {

export class FontRenderer {
  public:
    virtual ~FontRenderer() = default;
    virtual auto renderText(std::string_view text, int &width, int &height) const -> std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> = 0;
};

} // namespace Abyss::Concepts
