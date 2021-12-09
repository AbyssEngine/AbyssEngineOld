#ifndef ABYSS_SDL2SYSTEMIO_H
#define ABYSS_SDL2SYSTEMIO_H

#include "../../node/video.h"
#include "../interface.h"
#include <functional>
#include <libabyss/ringbuffer.h>
#include <mutex>
#include <SDL2/SDL.h>
#include <string>
#include <vector>

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
    void PushAudioData(eAudioIntent intent, std::span<uint8_t> data) final;
    void ResetAudio() final;
    bool HandleInputEvents(Node &rootNode) final;
    uint32_t GetTicks() final;
    void RenderStart() final;
    void RenderEnd() final;
    void Delay(uint32_t ms) final;
    float GetAudioLevel(eAudioIntent intent) final;
    void SetAudioLevel(eAudioIntent intent, float level) final;
    void ResetMouseButtonState() final;
    void SetBackgroundMusic(std::unique_ptr<LibAbyss::AudioStream> stream) final;
    void AddSoundEffect(SoundEffect *soundEffect) final;
    void RemoveSoundEffect(SoundEffect *soundEffect) final;

  private:
    void InitializeAudio();
    void FinalizeAudio();
    static void HandleAudioCallback(void *userData, Uint8 *stream, int length);
    void HandleAudio(uint8_t *stream, int length);
    bool HandleSdlEvent(const SDL_Event &sdlEvent, Node &rootNode);

    std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>> _sdlWindow;
    std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer *)>> _sdlRenderer;
    std::unique_ptr<LibAbyss::AudioStream> _backgroundMusicStream;
    std::vector<SoundEffect*> _soundEffects;
    int _backgroundMusicSampleRate = 0;
    std::mutex _mutex;
    std::mutex _buttonStateMutex;
    bool _hasAudio = false;
    SDL_AudioSpec _audioSpec;
    SDL_AudioDeviceID _audioDeviceId = 0;
    LibAbyss::RingBuffer _audioBuffer;
    int _cursorX = 0;
    int _cursorY = 0;
    eMouseButton _mouseButtonState;
    float _masterAudioLevel = 1.0f;
    float _masterAudioLevelActual = 1.0f;
    float _videoAudioLevel = 1.0f;
    float _videoAudioLevelActual = 1.0f;
    float _backgroundMusicAudioLevel = 1.0f;
    float _backgroundMusicAudioLevelActual = 1.0f;
    float _soundEffectsAudioLevel = 1.0f;
    float _soundEffectsAudioLevelActual = 1.0f;
};

} // namespace AbyssEngine::SDL2

#endif // ABYSS_SDL2SYSTEMIO_H
