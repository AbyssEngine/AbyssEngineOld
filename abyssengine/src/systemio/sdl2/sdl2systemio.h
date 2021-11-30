#ifndef ABYSS_SDL2SYSTEMIO_H
#define ABYSS_SDL2SYSTEMIO_H

#include "../../common/ringbuffer.h"
#include "../../node/video.h"
#include "../interface.h"
#include <SDL2/SDL.h>
#include <mutex>
#include <string>
#include <functional>

namespace AbyssEngine::SDL2 {

class SDL2SystemIO : public SystemIO {
  public:
    SDL2SystemIO();
    ~SDL2SystemIO() override;

    std::string_view Name() final;
    void PauseAudio(bool pause) final;
    void SetFullscreen(bool fullscreen) final;
    void RunMainLoop(Node &rootNode) final;
    void Stop() final;
    std::unique_ptr<ITexture> CreateTexture(ITexture::Format textureFormat, uint32_t width, uint32_t height) final;
    void PushAudioData(std::span<uint8_t> data) final;
    void PlayVideo(LibAbyss::InputStream stream, bool wait) final;
    void WaitForVideoToFinish() final;
    void ResetAudio() override;

  private:
    void InitializeAudio();
    void FinalizeAudio() const;
    static void HandleAudioCallback(void *userData, Uint8 *stream, int length);
    void HandleAudio(uint8_t *stream, int length);
    void HandleSdlEvent(const SDL_Event &sdlEvent, Node &rootNode);

    std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>> _sdlWindow;
    std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer*)>> _sdlRenderer;

    bool _runMainLoop = true;
    bool _hasAudio = false;
    SDL_AudioSpec _audioSpec;
    SDL_AudioDeviceID _audioDeviceId = 0;
    RingBuffer _audioBuffer;
    std::mutex _videoMutex;
    bool _waitVideoPlayback = false;
    uint32_t _lastTicks = 0;

    std::unique_ptr<Video> _videoNode;
};

} // namespace AbyssEngine::SDL2

#endif // ABYSS_SDL2SYSTEMIO_H
