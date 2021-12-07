#include "sdl2texture.h"
#include <stdexcept>

AbyssEngine::SDL2::SDL2Texture::SDL2Texture(SDL_Renderer *renderer, ITexture::Format textureFormat, uint32_t width, uint32_t height)
    : _texture(nullptr), _renderer(renderer), _width(width), _height(height), _textureFormat(textureFormat) {

    switch (textureFormat) {

    case Format::Static:
        _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, (int)width, (int)height);
        break;
    case Format::YUV:
        _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, (int)width, (int)height);
        break;
    default:
        throw std::runtime_error("Unknown texture format specified.");
    }
}

AbyssEngine::SDL2::SDL2Texture::~SDL2Texture() {
    if (_texture != nullptr)
        SDL_DestroyTexture(_texture);
}

void AbyssEngine::SDL2::SDL2Texture::SetPixels(std::span<const uint32_t> pixels) {
    if (pixels.size() != (_width * _height))
        throw std::runtime_error("Attempted to set pixels on the texture, but the data was the incorrect size.");

    SDL_UpdateTexture(_texture, nullptr, pixels.data(), 4 * (int)_width);
}

void AbyssEngine::SDL2::SDL2Texture::SetPixels(std::span<const uint8_t> pixels) {
    if (pixels.size() != (4 * _width * _height))
        throw std::runtime_error("Attempted to set pixels on the texture, but the data was the incorrect size.");

    SDL_UpdateTexture(_texture, nullptr, pixels.data(), 4 * (int)_width);
}

void AbyssEngine::SDL2::SDL2Texture::Render(const AbyssEngine::Rectangle &sourceRect, const AbyssEngine::Rectangle &destRect) {
    SDL_Rect sr{.x = sourceRect.X, .y = sourceRect.Y, .w = sourceRect.Width, .h = sourceRect.Height};
    SDL_Rect dr{.x = destRect.X, .y = destRect.Y, .w = destRect.Width, .h = destRect.Height};

    SDL_RenderCopy(_renderer, _texture, &sr, &dr);
}

void AbyssEngine::SDL2::SDL2Texture::SetYUVData(std::span<const uint8_t> yPlane, int yPitch, std::span<const uint8_t> uPlane, int uPitch,
                                                std::span<const uint8_t> vPlane, int vPitch) {
    if (_textureFormat != ITexture::Format::YUV)
        throw std::runtime_error("Cannot set YUV data on a non YUV texture.");

    if (SDL_UpdateYUVTexture(_texture, nullptr, yPlane.data(), yPitch, uPlane.data(), uPitch, vPlane.data(), vPitch) < 0) {
        throw std::runtime_error("Cannot set YUV data");
    }
}

void AbyssEngine::SDL2::SDL2Texture::SetBlendMode(AbyssEngine::eBlendMode blendMode) {
    _blendMode = blendMode;
    SDL_BlendMode m;
    switch (blendMode) {
    case eBlendMode::Unknown:
    case eBlendMode::None:
        m = SDL_BLENDMODE_NONE;
        break;
    case eBlendMode::Blend:
        m = SDL_BLENDMODE_BLEND;
        break;
    case eBlendMode::Add:
        m = SDL_BLENDMODE_ADD;
        break;
    case eBlendMode::Mod:
        m = SDL_BLENDMODE_MOD;
        break;
    case eBlendMode::Mul:
        m = SDL_BLENDMODE_MUL;
        break;
    }

    SDL_SetTextureBlendMode(_texture, m);
}

AbyssEngine::eBlendMode AbyssEngine::SDL2::SDL2Texture::GetBlendMode() { return _blendMode; }

void AbyssEngine::SDL2::SDL2Texture::SetColorMod(uint8_t red, uint8_t green, uint8_t blue) { SDL_SetTextureColorMod(_texture, red, green, blue); }
