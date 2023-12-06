module;

#include <SDL_render.h>

export module Abyss.Common.RendererProvider;

namespace Abyss::Common {

export class RendererProvider {
  public:
    virtual ~RendererProvider() = default;
    [[nodiscard]] virtual auto getRenderer() -> SDL_Renderer * = 0;
};

} // namespace Abyss::Common
