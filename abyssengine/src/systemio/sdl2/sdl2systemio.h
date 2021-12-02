#ifndef ABYSS_SDL2SYSTEMIO_H
#define ABYSS_SDL2SYSTEMIO_H

#include "../../common/ringbuffer.h"
#include "../../node/video.h"
#include "../interface.h"
#include <SDL2/SDL.h>
#include <functional>
#include <mutex>
#include <string>

namespace AbyssEngine::SDL2 {

class SDL2SystemIO : public SystemIO {
  public:
    SDL2SystemIO();
    ~SDL2SystemIO() override;
    void GetCursorState(int &cursorX, int &cursorY, eMouseButton &buttonState) override;

    std::string_view Name() final;
    void PauseAudio(bool pause) final;
    void SetFullscreen(bool fullscreen) final;
    std::unique_ptr<ITexture> CreateTexture(ITexture::Format textureFormat, uint32_t width, uint32_t height) final;
    void PushAudioData(std::span<const uint8_t> data) final;
    void ResetAudio() final;
    bool HandleInputEvents(Node &rootNode) final;
    uint32_t GetTicks() final;
    void RenderStart() override;
    void RenderEnd() override;
    void Delay(uint32_t ms) override;

  private:
    void InitializeAudio();
    void FinalizeAudio() const;
    static void HandleAudioCallback(void *userData, Uint8 *stream, int length);
    void HandleAudio(uint8_t *stream, int length);
    bool HandleSdlEvent(const SDL_Event &sdlEvent, Node &rootNode);

    std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>> _sdlWindow;
    std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer *)>> _sdlRenderer;

    bool _hasAudio = false;
    SDL_AudioSpec _audioSpec;
    SDL_AudioDeviceID _audioDeviceId = 0;
    RingBuffer _audioBuffer;
    int _cursorX = 0;
    int _cursorY = 0;
    eMouseButton _mouseButtonState;
};

} // namespace AbyssEngine::SDL2

#endif // ABYSS_SDL2SYSTEMIO_H
