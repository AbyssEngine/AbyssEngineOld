#include "Label.h"

namespace Abyss::UI {

void Label::setTextureColor() const {
    if (!_texture)
        return;

    if (_color.r != 255 || _color.g != 255 || _color.b != 255 || _color.a != 255) {
        SDL_SetTextureColorMod(_texture.get(), _color.r, _color.g, _color.b);
        SDL_SetTextureAlphaMod(_texture.get(), _color.a);
        SDL_SetTextureBlendMode(_texture.get(), SDL_BLENDMODE_BLEND);
    }
}

Label::Label(const Concepts::FontRenderer &fontRenderer) : _fontRenderer(&fontRenderer) {}

Label::Label(const Concepts::FontRenderer &fontRenderer, const SDL_Color color, const std::string_view text) : _fontRenderer(&fontRenderer) {
    setText(text);
    setColor(color);
}

void Label::setText(const std::string_view text) {
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

void Label::setColor(const SDL_Color color) {
    if (color.r == _color.r && color.g == _color.g && color.b == _color.b && color.a == _color.a)
        return;
    _color = color;
    setTextureColor();
}

std::string_view Label::getText() const { return _text; }

SDL_Color Label::getColor() const { return _color; }

int Label::getWidth() const { return _width; }

int Label::getHeight() const { return _height; }

void Label::getSize(int &width, int &height) const {
    width = _width;
    height = _height;
}

void Label::draw(const int x, const int y) const {
    if (!_texture || _text.empty())
        return;

    const auto renderer = Singletons::getRendererProvider().getRenderer();
    const SDL_Rect rect{x, y, _width, _height};
    SDL_RenderCopy(renderer, _texture.get(), nullptr, &rect);
}

void Label::drawCentered(const int x, const int y) const {
    if (!_texture || _text.empty())
        return;

    const auto renderer = Singletons::getRendererProvider().getRenderer();
    const SDL_Rect rect{x - _width / 2, y - _height / 2, _width, _height};
    SDL_RenderCopy(renderer, _texture.get(), nullptr, &rect);
}

} // namespace Abyss::UI
