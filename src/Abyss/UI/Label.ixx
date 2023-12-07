module;

#include <SDL2/SDL.h>
#include <memory>
#include <string>

export module Abyss.UI.Label;

import Abyss.Concepts.FontRenderer;
import Abyss.Singletons;

namespace Abyss::UI {

export class Label {
    const Concepts::FontRenderer *_fontRenderer;
    std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)> _texture;
    std::string _text;
    SDL_Color _color;
    int _width;
    int _height;

  public:
    explicit Label(const Concepts::FontRenderer &fontRenderer)
        : _fontRenderer(&fontRenderer), _texture(nullptr, SDL_DestroyTexture), _text(), _color{255, 255, 255, 255}, _width(0), _height(0) {}

    auto setText(const std::string_view text) -> void {
        if (text == _text)
            return;
        _text = text;

        if (_text.empty()) {
            _texture.reset();
            return;
        }

        _texture = _fontRenderer->renderText(_text, _color, _width, _height);
    }

    auto setColor(const SDL_Color color) -> void {
        if (color.r == _color.r && color.g == _color.g && color.b == _color.b && color.a == _color.a)
            return;
        _color = color;

        if (_text.empty())
            return;

        _texture = _fontRenderer->renderText(_text, _color, _width, _height);
    }

    [[nodiscard]] auto getText() const -> std::string_view { return _text; }

    [[nodiscard]] auto getColor() const -> SDL_Color { return _color; }

    [[nodiscard]] auto getWidth() const -> int { return _width; }

    [[nodiscard]] auto getHeight() const -> int { return _height; }

    auto getSize(int &width, int &height) const -> void {
        width = _width;
        height = _height;
    }

    auto draw(const int x, const int y) const -> void {
        if (!_texture || _text.empty())
            return;

        const auto renderer = Singletons::getRendererProvider().getRenderer();
        const SDL_Rect rect{x, y, _width, _height};
        SDL_RenderCopy(renderer, _texture.get(), nullptr, &rect);
    }
};

} // namespace Abyss::UI
