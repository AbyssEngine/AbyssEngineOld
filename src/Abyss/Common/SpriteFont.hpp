#pragma once

#include <Abyss/Common/Drawable.hpp>
#include <Abyss/Singletons.hpp>
#include <cstdint>
#include <unordered_map>

namespace Abyss::Common {

struct Glyph {
    uint16_t FrameIndex;
    uint8_t Width;
    uint8_t Height;
    int OffsetX;
    int OffsetY;
};

template <Drawable T> class SpriteFont {
    T _drawable;
    std::unordered_map<int, Glyph> _glyphs;
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> _texture{nullptr, SDL_DestroyTexture};
    std::string _text;
    bool _dirty;
    int _width;
    int _height;

    auto regenerateTexture() -> void {
        _dirty = false;

        _width = 0;
        _height = 0;

        for (const auto &c : _text) {
            const auto &[glyphFrameIndex, glyphWidth, glyphHeight, glyphOffsetX, glyphOffsetY] = _glyphs[c];
            int frameWidth, frameHeight;
            _drawable.getFrameSize(glyphFrameIndex, frameWidth, frameHeight);
            _width += glyphWidth + glyphOffsetX;
            _height = std::max(_height, frameHeight);
        }

        const auto renderer = Singletons::getRendererProvider().getRenderer();
        _texture.reset(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, _width, _height));
        SDL_SetTextureBlendMode(_texture.get(), SDL_BLENDMODE_MUL);
        const auto oldTarget = SDL_GetRenderTarget(renderer);
        SDL_SetRenderTarget(renderer, _texture.get());

        int drawX = 0;
        for (const auto &c : _text) {
            const auto &[glyphFrameIndex, glyphWidth, glyphHeight, glyphOffsetX, glyphOffsetY] = _glyphs[c];
            int frameWidth, frameHeight;
            _drawable.getFrameSize(glyphFrameIndex, frameWidth, frameHeight);
            _drawable.draw(glyphFrameIndex, drawX, _height);
            drawX += glyphWidth + glyphOffsetX;
        }

        SDL_SetRenderTarget(renderer, oldTarget);
    }

  public:
    explicit SpriteFont(const std::string_view path, const DataTypes::Palette &palette)
        : _drawable(std::string(path) + ".dc6"), _dirty(false), _width(0), _height(0) {
        _drawable.setPalette(palette);

        auto tableStream = Singletons::getFileProvider().loadStream(std::string(path) + ".tbl");
        char signature[6] = {0};
        tableStream.read(signature, 5);
        if (std::string_view(signature) != "Woo!\x01")
            throw std::runtime_error("Invalid font file: " + std::string(path));

        tableStream.ignore(7); // skip unknown bytes

        while (!tableStream.eof()) {
            const auto code = tableStream.readValue<uint16_t>();
            auto &[glyphFrameIndex, glyphWidth, glyphHeight, glyphOffsetX, glyphOffsetY] = _glyphs[code];

            tableStream.ignore(1); // Skip a byte for some reason

            glyphWidth = tableStream.readValue<uint8_t>();
            glyphHeight = tableStream.readValue<uint8_t>();

            tableStream.ignore(3); // Skip 3 unknown bytes

            glyphFrameIndex = tableStream.readValue<uint16_t>();

            if (glyphFrameIndex >= _drawable.getFrameCount())
                throw std::runtime_error("Invalid font file: " + std::string(path));

            _drawable.getFrameOffset(glyphFrameIndex, glyphOffsetX, glyphOffsetY);

            tableStream.ignore(4); // More magic ignores, fun!
        }
    }

    auto draw(const int x, const int y) -> void {
        const auto renderer = Singletons::getRendererProvider().getRenderer();

        if (_texture == nullptr || _dirty)
            regenerateTexture();

        const SDL_Rect rect{x, y, _width, _height};
        SDL_RenderCopy(renderer, _texture.get(), nullptr, &rect);
    }

    auto setText(const std::string_view text) -> void {
        if (_text == text)
            return;

        _text = text;
        _dirty = true;
    }

    [[nodiscard]] auto getText() const -> const std::string & { return _text; }

    auto getSize(int &width, int &height) const -> void {
        width = _width;
        height = _height;
    }
};

} // namespace Abyss::Common
