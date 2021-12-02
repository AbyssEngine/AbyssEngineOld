#ifndef ABYSS_SDL2TEXTURE_H
#define ABYSS_SDL2TEXTURE_H

#include "../interface.h"
#include <SDL2/SDL.h>

namespace AbyssEngine::SDL2 {

class SDL2Texture : public ITexture {
  public:
    SDL2Texture(SDL_Renderer *renderer, ITexture::Format textureFormat, uint32_t width, uint32_t height);
    ~SDL2Texture() override;
    void SetPixels(std::span<const uint32_t> pixels) final;
    void SetPixels(std::span<const uint8_t> pixels) final;
    void Render(const AbyssEngine::Rectangle &sourceRect, const AbyssEngine::Rectangle &destRect) final;
    void SetYUVData(std::span<const uint8_t> yPlane, int yPitch, std::span<const uint8_t> uPlane, int uPitch, std::span<const uint8_t> vPlane, int vPitch) final;
    void SetBlendMode(eBlendMode blendMode) final;
    eBlendMode GetBlendMode() final;
    void SetColorMod(uint8_t red, uint8_t green, uint8_t blue) override;

  private:
    SDL_Renderer *_renderer;
    SDL_Texture *_texture;
    const uint32_t _width;
    const uint32_t _height;
    const ITexture::Format _textureFormat;
    eBlendMode _blendMode = eBlendMode::None;
};

} // namespace AbyssEngine::SDL2

#endif // ABYSS_SDL2TEXTURE_H
