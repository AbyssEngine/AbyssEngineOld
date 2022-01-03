#include "sdl2texture.h"
#include <stdexcept>
#include <spdlog/spdlog.h>

namespace AbyssEngine {

SDL2::SDL2Texture::SDL2Texture(SDL_Renderer *renderer, SDL_Texture *texture)
    : _renderer(renderer), _texture(texture), _width(0), _height(0), _textureFormat(ITexture::Format::Static) {}

SDL2::SDL2Texture::SDL2Texture(SDL_Renderer *renderer, ITexture::Format textureFormat, uint32_t width, uint32_t height)
    : _renderer(renderer), _texture(nullptr), _width(width), _height(height), _textureFormat(textureFormat) {

    switch (textureFormat) {

    case Format::Static:
        _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, (int)width, (int)height);
        break;
    case Format::BGRA:
        _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STATIC, (int)width, (int)height);
        break;
    case Format::YUV:
        _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, (int)width, (int)height);
        break;
    default:
        throw std::runtime_error("Unknown texture format specified.");
    }
}

SDL2::SDL2Texture::~SDL2Texture() {
    if (_texture != nullptr)
        SDL_DestroyTexture(_texture);
}

void SDL2::SDL2Texture::SetPixels(std::span<const uint32_t> pixels) {
    if (pixels.size() != (_width * _height))
        throw std::runtime_error("Attempted to set pixels on the texture, but the data was the incorrect size.");

    SDL_UpdateTexture(_texture, nullptr, pixels.data(), 4 * (int)_width);
}

void SDL2::SDL2Texture::SetPixels(std::span<const uint8_t> pixels) {
    SetPixels(pixels, 4 * (int)_width);
}

void SDL2::SDL2Texture::SetPixels(std::span<const uint8_t> pixels, int pitch) {
    if (pixels.size() != (pitch * _height))
        throw std::runtime_error("Attempted to set pixels on the texture, but the data was the incorrect size.");

    SDL_UpdateTexture(_texture, nullptr, pixels.data(), pitch);
}

void SDL2::SDL2Texture::Render(const Rectangle &sourceRect, const Rectangle &destRect) {
    if (sourceRect.Width <= 0 || sourceRect.Height <= 0)
        return;

    SDL_Rect sr{.x = sourceRect.X, .y = sourceRect.Y, .w = sourceRect.Width, .h = sourceRect.Height};
    SDL_Rect dr{.x = destRect.X, .y = destRect.Y, .w = destRect.Width, .h = destRect.Height};

    SDL_RenderCopy(_renderer, _texture, &sr, &dr);
}

void SDL2::SDL2Texture::SetYUVData(std::span<const uint8_t> yPlane, int yPitch, std::span<const uint8_t> uPlane, int uPitch,
                                   std::span<const uint8_t> vPlane, int vPitch) {
    if (_textureFormat != ITexture::Format::YUV)
        throw std::runtime_error("Cannot set YUV data on a non YUV texture.");

    if (SDL_UpdateYUVTexture(_texture, nullptr, yPlane.data(), yPitch, uPlane.data(), uPitch, vPlane.data(), vPitch) < 0) {
        throw std::runtime_error("Cannot set YUV data");
    }
}

void SDL2::SDL2Texture::SetBlendMode(eBlendMode blendMode) {
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

eBlendMode SDL2::SDL2Texture::GetBlendMode() { return _blendMode; }

void SDL2::SDL2Texture::SetColorMod(uint8_t red, uint8_t green, uint8_t blue) { SDL_SetTextureColorMod(_texture, red, green, blue); }

void SDL2::SDL2Texture::SaveAsBMP(const std::string &filePath) { SaveTexture(_renderer, _texture, filePath.c_str()); }
void SDL2::SDL2Texture::SaveTexture(SDL_Renderer *ren, SDL_Texture *tex, const char *filename) {
    SDL_Texture *ren_tex;
    SDL_Surface *surf;
    int st;
    int w;
    int h;
    int format;
    void *pixels;

    pixels = NULL;
    surf = NULL;
    ren_tex = NULL;
    format = SDL_PIXELFORMAT_RGBA32;

    /* Get information about texture we want to save */
    st = SDL_QueryTexture(tex, NULL, NULL, &w, &h);
    if (st != 0) {
        SDL_Log("Failed querying texture: %s\n", SDL_GetError());
        goto cleanup;
    }

    ren_tex = SDL_CreateTexture(ren, format, SDL_TEXTUREACCESS_TARGET, w, h);
    if (!ren_tex) {
        SDL_Log("Failed creating render texture: %s\n", SDL_GetError());
        goto cleanup;
    }

    /*
     * Initialize our canvas, then copy texture to a target whose pixel data we
     * can access
     */
    st = SDL_SetRenderTarget(ren, ren_tex);
    if (st != 0) {
        SDL_Log("Failed setting render target: %s\n", SDL_GetError());
        goto cleanup;
    }

    SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(ren);

    st = SDL_RenderCopy(ren, tex, NULL, NULL);
    if (st != 0) {
        SDL_Log("Failed copying texture data: %s\n", SDL_GetError());
        goto cleanup;
    }

    /* Create buffer to hold texture data and load it */
    pixels = malloc(w * h * SDL_BYTESPERPIXEL(format));
    if (!pixels) {
        SDL_Log("Failed allocating memory\n");
        goto cleanup;
    }

    st = SDL_RenderReadPixels(ren, NULL, format, pixels, w * SDL_BYTESPERPIXEL(format));
    if (st != 0) {
        SDL_Log("Failed reading pixel data: %s\n", SDL_GetError());
        goto cleanup;
    }

    /* Copy pixel data over to surface */
    surf = SDL_CreateRGBSurfaceWithFormatFrom(pixels, w, h, SDL_BITSPERPIXEL(format), w * SDL_BYTESPERPIXEL(format), format);
    if (!surf) {
        SDL_Log("Failed creating new surface: %s\n", SDL_GetError());
        goto cleanup;
    }

    /* Save result to an image */
    st = SDL_SaveBMP(surf, filename);
    if (st != 0) {
        SDL_Log("Failed saving image: %s\n", SDL_GetError());
        goto cleanup;
    }

    SDL_Log("Saved texture as BMP to \"%s\"\n", filename);

cleanup:
    SDL_FreeSurface(surf);
    free(pixels);
    SDL_DestroyTexture(ren_tex);
}

} // namespace AbyssEngine
