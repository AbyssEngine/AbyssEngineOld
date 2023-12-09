#pragma once

#include "Abyss/Common/Logging.h"
#include "Abyss/Concepts/Drawable.h"
#include "Abyss/Concepts/FontRenderer.h"
#include "Abyss/Singletons.h"
#include <algorithm>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace Abyss::UI {

struct Glyph {
    uint16_t FrameIndex;
    uint8_t Width;
    uint8_t Height;
    int OffsetX;
    int OffsetY;
};

template <Concepts::Drawable T> class SpriteFont final : public Concepts::FontRenderer {
    T _drawable;
    std::unordered_map<int, Glyph> _glyphs;

    auto renderText(const std::string_view text, int &width, int &height) const -> std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> override {
        std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture{nullptr, SDL_DestroyTexture};
        width = 0;
        height = 0;

        for (const auto &c : text) {
            const auto &[glyphFrameIndex, glyphWidth, glyphHeight, glyphOffsetX, glyphOffsetY] = _glyphs.at(c >= _glyphs.size() ? '?' : c);
            int frameWidth, frameHeight;
            _drawable.getFrameSize(glyphFrameIndex, frameWidth, frameHeight);
            width += glyphWidth + glyphOffsetX;
            height = std::max(height, frameHeight);
        }

        const auto renderer = Singletons::getRendererProvider().getRenderer();
        texture.reset(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height));
        SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_MUL);
        const auto oldTarget = SDL_GetRenderTarget(renderer);
        SDL_SetRenderTarget(renderer, texture.get());

        int drawX = 0;
        for (const auto &c : text) {
            const auto &[glyphFrameIndex, glyphWidth, glyphHeight, glyphOffsetX, glyphOffsetY] = _glyphs.at(c >= _glyphs.size() ? '?' : c);
            int frameWidth, frameHeight;
            _drawable.getFrameSize(glyphFrameIndex, frameWidth, frameHeight);
            _drawable.draw(glyphFrameIndex, drawX, height);
            drawX += glyphWidth + glyphOffsetX;
        }

        SDL_SetRenderTarget(renderer, oldTarget);

        return std::move(texture);
    }

  public:
    explicit SpriteFont(const std::string_view path, const DataTypes::Palette &palette) : _drawable(std::string(path) + ".dc6") {
        Abyss::Common::Log::debug("Loading font {}...", path);

        _drawable.setPalette(palette);

        auto tableStream = Singletons::getFileProvider().loadFile(std::string(path) + ".tbl");
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
};

} // namespace Abyss::UI
