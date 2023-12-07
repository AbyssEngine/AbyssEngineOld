module;

#include <SDL2/SDL.h>
#include <memory>
#include <string>

import Abyss.Enums.MouseButton;

export module Abyss.UI.Label;

import Abyss.Concepts.FontRenderer;
import Abyss.Singletons;

namespace Abyss::UI {

export class Label {
    const Concepts::FontRenderer *_fontRenderer;
    std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)> _texture = {nullptr, SDL_DestroyTexture};
    std::string _text = "";
    SDL_Color _color = {255, 255, 255, 255};
    int _width = 0;
    int _height = 0;

    auto setTextureColor() const -> void {
        if (!_texture)
            return;

        if (_color.r != 255 || _color.g != 255 || _color.b != 255 || _color.a != 255) {
            SDL_SetTextureColorMod(_texture.get(), _color.r, _color.g, _color.b);
            SDL_SetTextureAlphaMod(_texture.get(), _color.a);
            SDL_SetTextureBlendMode(_texture.get(), SDL_BLENDMODE_BLEND);
        }
    }

  public:
    explicit Label(const Concepts::FontRenderer &fontRenderer) : _fontRenderer(&fontRenderer) {}

    Label(const Concepts::FontRenderer &fontRenderer, const SDL_Color color, const std::string_view text) : _fontRenderer(&fontRenderer) {
        setText(text);
        setColor(color);
    }

    auto setText(const std::string_view text) -> void {
        if (text == _text)
            return;
        _text = text;

        if (_text.empty()) {
            _texture.reset();
            return;
        }

        _texture = _fontRenderer->renderText(_text, _width, _height);
        setTextureColor();
    }

    auto setColor(const SDL_Color color) -> void {
        if (color.r == _color.r && color.g == _color.g && color.b == _color.b && color.a == _color.a)
            return;
        _color = color;
        setTextureColor();
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

    auto drawCentered(const int x, const int y) const -> void {
        if (!_texture || _text.empty())
            return;

        const auto renderer = Singletons::getRendererProvider().getRenderer();
        const SDL_Rect rect{x - _width / 2, y - _height / 2, _width, _height};
        SDL_RenderCopy(renderer, _texture.get(), nullptr, &rect);
    }
};

} // namespace Abyss::UI
