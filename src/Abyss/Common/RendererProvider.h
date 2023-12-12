#pragma once

#include <SDL_render.h>

namespace Abyss::Common {

class RendererProvider {
  public:
    virtual ~RendererProvider() = default;
    [[nodiscard]] virtual auto getRenderer() -> SDL_Renderer * = 0;
};

} // namespace Abyss::Common
