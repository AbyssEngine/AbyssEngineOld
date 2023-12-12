#pragma once

#include "Abyss/Concepts/FontRenderer.h"

#include <SDL2/SDL.h>
#include <memory>
#include <string>

namespace Abyss::UI {

class Label {
    const Concepts::FontRenderer *_fontRenderer;
    std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)> _texture = {nullptr, SDL_DestroyTexture};
    std::string _text;
    SDL_Color _color = {255, 255, 255, 255};
    int _width = 0;
    int _height = 0;

    void setTextureColor() const;

  public:
    explicit Label(const Concepts::FontRenderer &fontRenderer);
    Label(const Concepts::FontRenderer &fontRenderer, SDL_Color color, std::string_view text);
    void setText(std::string_view text);
    void setColor(SDL_Color color);
    [[nodiscard]] std::string_view getText() const;
    [[nodiscard]] SDL_Color getColor() const;
    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;
    void getSize(int &width, int &height) const;
    void draw(int x, int y) const;
    void drawCentered(int x, int y) const;
};

} // namespace Abyss::UI
