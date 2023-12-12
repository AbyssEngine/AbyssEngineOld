#pragma once

#include <SDL2/SDL.h>
#include <memory>
#include <string_view>

namespace Abyss::Concepts {

class FontRenderer {
  public:
    virtual ~FontRenderer() = default;
    virtual std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> renderText(std::string_view text, int &width, int &height) const = 0;
};

} // namespace Abyss::Concepts
