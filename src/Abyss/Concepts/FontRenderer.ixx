module;

#include <SDL2/SDL.h>
#include <memory>
#include <string_view>

export module Abyss.Concepts.FontRenderer;

namespace Abyss::Concepts {

export template <typename T>
concept FontRenderer = requires(T t, std::string_view text, int &width, int &height) {
    { t.renderText(text, width, height) } -> std::same_as<std::unique_ptr<SDL_Texture>>;
};

} // namespace Abyss::Concepts
