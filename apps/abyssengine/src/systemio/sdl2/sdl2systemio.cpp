#include "sdl2systemio.h"
#include "../../engine/engine.h"
#include "../../hostnotify/hostnotify.h"
#include "config.h"
#include "sdl2texture.h"
#include <cstdint>
#include <libabyss/common/pngloader.h>
#include <png.h>
#include <SDL.h>
#include <SDL_hints.h>
#include <SDL_stdinc.h>
#include <SDL_syswm.h>
#include <span>
#include <spdlog/spdlog.h>
#ifdef __APPLE__
#include "../../hostnotify/hostnotify_mac_shim.h"
#endif // __APPLE__

// /usr/include/X11/X.h:115:30 defines it...
#ifdef None
#undef None
#endif

namespace AbyssEngine {

SDL2::SDL2SystemIO::SDL2SystemIO() : SystemIO::SystemIO(), _audioSpec(), _mouseButtonState((eMouseButton)0) {
    SPDLOG_TRACE("Creating SDL2 System IO");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0)
        throw std::runtime_error(SDL_GetError());

    _sdlWindow = std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>>(SDL_CreateWindow(ABYSS_VERSION_STRING, SDL_WINDOWPOS_UNDEFINED,
                                                                                                 SDL_WINDOWPOS_UNDEFINED, 800, 600,
                                                                                                 SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI),
                                                                                [](SDL_Window *x) {
                                                                                    SDL_DestroyWindow(x);
                                                                                    SDL_Quit();
                                                                                });

    _screenWidth = 800;
    _screenHeight = 600;

    if (_sdlWindow == nullptr)
        throw std::runtime_error(SDL_GetError());

#ifdef _WIN32
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(_sdlWindow.get(), &wmInfo);
    HostNotify::Win32Handle = wmInfo.info.win.window;
#endif // _WIN32
#ifdef __APPLE__
    AbyssHostNotifyInitMac();

#endif // __APPLE__

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_SetHint(SDL_HINT_AUDIO_DEVICE_APP_NAME, "Abyss Engine");

    _sdlRenderer = std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer *)>>(
        SDL_CreateRenderer(_sdlWindow.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC), [](SDL_Renderer *x) {
            SDL_DestroyRenderer(x);
            SDL_Quit();
        });

    if (_sdlRenderer == nullptr)
        throw std::runtime_error(SDL_GetError());

    SDL_RendererInfo renderInfo;
    SDL_GetRendererInfo(_sdlRenderer.get(), &renderInfo);

    SPDLOG_INFO("Using {0} graphics rendering API", renderInfo.name);
    SPDLOG_INFO("Max texture size: {0}x{1}", renderInfo.max_texture_width, renderInfo.max_texture_height);

    SDL_version sdlVersion;
    SDL_GetVersion(&sdlVersion);

    SPDLOG_INFO("SDL Version: {0}.{1}.{2}", sdlVersion.major, sdlVersion.minor, sdlVersion.patch);

    SDL_RenderSetLogicalSize(_sdlRenderer.get(), 800, 600);
    SDL_ShowCursor(SDL_FALSE);

    InitializeAudio();

    PauseAudio(false);
}

SDL2::SDL2SystemIO::~SDL2SystemIO() {
    SPDLOG_TRACE("Destroying SDL2 System IO");

    FinalizeAudio();

#ifdef __APPLE__
    AbyssHostNotifyFinalizeMac();
#endif // __APPLE__
}

std::string_view SDL2::SDL2SystemIO::Name() { return "SDL2"; }

void SDL2::SDL2SystemIO::PauseAudio(bool pause) {
    if (!_hasAudio)
        return;

    SDL_PauseAudioDevice(_audioDeviceId, pause ? SDL_TRUE : SDL_FALSE);
}

void SDL2::SDL2SystemIO::SetFullscreen(bool fullscreen) { SDL_SetWindowFullscreen(_sdlWindow.get(), fullscreen ? SDL_TRUE : SDL_FALSE); }

bool SDL2::SDL2SystemIO::HandleSdlEvent(const SDL_Event &sdlEvent, Node &rootNode) {
    switch (sdlEvent.type) {
    case SDL_MOUSEMOTION: {
        _cursorX = sdlEvent.motion.x;
        _cursorY = sdlEvent.motion.y;

        rootNode.MouseEventCallback(MouseMoveEvent{.X = sdlEvent.motion.x, .Y = sdlEvent.motion.y});
    }
        return true;
    case SDL_MOUSEBUTTONDOWN: {
        eMouseButton button;
        switch (sdlEvent.button.button) {
        case SDL_BUTTON_LEFT:
            button = eMouseButton::Left;
            break;
        case SDL_BUTTON_MIDDLE:
            button = eMouseButton::Middle;
            break;
        case SDL_BUTTON_RIGHT:
            button = eMouseButton::Right;
            break;
        default:
            return true;
        }

        _mouseButtonState += button;
        rootNode.MouseEventCallback(MouseButtonEvent{.Button = button, .IsPressed = true});

        return true;
    }
    case SDL_MOUSEBUTTONUP: {
        eMouseButton button;
        switch (sdlEvent.button.button) {
        case SDL_BUTTON_LEFT:
            button = eMouseButton::Left;
            break;
        case SDL_BUTTON_MIDDLE:
            button = eMouseButton::Middle;
            break;
        case SDL_BUTTON_RIGHT:
            button = eMouseButton::Right;
            break;
        default:
            return true;
        }

        _mouseButtonState -= button;
        rootNode.MouseEventCallback(MouseButtonEvent{.Button = button, .IsPressed = false});
        return true;
    }
    case SDL_KEYDOWN: {
        if (sdlEvent.key.keysym.sym == SDLK_BACKSPACE) {
            if (_inputText.length() > 0)
                _inputText.pop_back();
            return true;
        }

        _pressedKeys[sdlEvent.key.keysym.scancode] = true;
        rootNode.KeyboardEventCallback(KeyboardEvent{.Scancode = (uint16_t)sdlEvent.key.keysym.scancode, .Pressed = true});
        return true;
    }
    case SDL_KEYUP: {
        _pressedKeys[sdlEvent.key.keysym.scancode] = false;
        rootNode.KeyboardEventCallback(KeyboardEvent{.Scancode = (uint16_t)sdlEvent.key.keysym.scancode, .Pressed = false});
        return true;
    }
    case SDL_TEXTINPUT: {
        if (sdlEvent.text.text[0] == '`')
            return true;

        _inputText += sdlEvent.text.text;
        return true;
    }
    case SDL_QUIT:
        Engine::Get()->Stop();
        return false;
    case SDL_WINDOWEVENT:
        switch (sdlEvent.window.event) {
        case SDL_WINDOWEVENT_RESIZED:
            _screenWidth = sdlEvent.window.data1;
            _screenHeight = sdlEvent.window.data2;
            return true;
        }
        return true;
    default:
        return true;
    }
}

std::unique_ptr<ITexture> SDL2::SDL2SystemIO::CreateTexture(ITexture::Format textureFormat, uint32_t width, uint32_t height) {
    return std::make_unique<SDL2Texture>(_sdlRenderer.get(), textureFormat, width, height);
}

void SDL2::SDL2SystemIO::InitializeAudio() {
    SDL_AudioSpec requestedAudioSpec{
        .freq = 44100,
        .format = AUDIO_S16LSB,
        .channels = 2,
        .samples = 1024,
        .callback = SDL2SystemIO::HandleAudioCallback,
        .userdata = this,
    };

    _audioDeviceId = SDL_OpenAudioDevice(nullptr, SDL_FALSE, &requestedAudioSpec, &_audioSpec, 0);
    _hasAudio = _audioDeviceId >= 0;

    if (!_hasAudio) {
        SPDLOG_WARN("{}", SDL_GetError());
        return;
    }

    const char* name = SDL_GetAudioDeviceName(_audioDeviceId, SDL_FALSE);
    if (name == nullptr) {
        name = "(unknown)";
    }
    SPDLOG_INFO("Using audio device {0} via {1}", name, SDL_GetCurrentAudioDriver());
}

void SDL2::SDL2SystemIO::HandleAudioCallback(void *userData, Uint8 *stream, int length) {
    auto *source = (SDL2SystemIO *)userData;
    source->HandleAudio(stream, length);
}

void SDL2::SDL2SystemIO::HandleAudio(uint8_t *stream, int length) {
    if (length & 1) {
        SPDLOG_WARN("Audio callback length is not even, dropping samples");
        return;
    }

    std::lock_guard<std::mutex> lock(_mutex);

    // Apply the master volume level
    for (auto i = 0; i < length; i += 2) {
        int32_t sample = 0;

        // Add in the background music
        // Note the background music plays at half the native sample rate
        if (_backgroundMusicStream) {
            sample += _backgroundMusicStream->GetSample() * _backgroundMusicAudioLevelActual;
        }

        if (_video) {
            sample += _video->GetAudioSample() * _videoAudioLevelActual;
        }

        // Add in the sound effects
        for (auto &effect : _soundEffects) {
            if (!effect->GetIsPlaying())
                continue;

            sample += effect->GetSample() * _soundEffectsAudioLevelActual;
        }

        // Apply the master audio volume level
        sample *= _masterAudioLevelActual;

        // Clamp the output
        if (sample > 32767)
            sample = 32767;
        else if (sample < -32768)
            sample = -32768;

        stream[i] = (uint8_t)(sample & 0xFF);
        stream[i + 1] = (uint8_t)((sample >> 8) & 0xFF);
    }
}

void SDL2::SDL2SystemIO::FinalizeAudio() const {
    if (!_hasAudio)
        return;

    SDL_PauseAudioDevice(_audioDeviceId, SDL_TRUE);
    SDL_CloseAudioDevice(_audioDeviceId);
}

bool SDL2::SDL2SystemIO::HandleInputEvents(Node &rootNode) {
    SDL_Event sdlEvent;

    while (SDL_PollEvent(&sdlEvent)) {
        if (!HandleSdlEvent(sdlEvent, rootNode))
            return false;
    }

    return true;
}

uint32_t SDL2::SDL2SystemIO::GetTicks() { return SDL_GetTicks(); }

void SDL2::SDL2SystemIO::RenderStart() {
    SDL_SetRenderDrawColor(_sdlRenderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(_sdlRenderer.get());
}

void SDL2::SDL2SystemIO::RenderEnd() { SDL_RenderPresent(_sdlRenderer.get()); }

void SDL2::SDL2SystemIO::Delay(uint32_t ms) { SDL_Delay(ms); }

void SDL2::SDL2SystemIO::GetCursorState(int &cursorX, int &cursorY, eMouseButton &buttonState) {
    cursorX = _cursorX;
    cursorY = _cursorY;
    buttonState = _mouseButtonState;
}

float SDL2::SDL2SystemIO::GetAudioLevel(eAudioIntent intent) {
    std::lock_guard<std::mutex> lock(_mutex);

    return _masterAudioLevel;
}

void SDL2::SDL2SystemIO::SetAudioLevel(eAudioIntent intent, float level) {
    std::lock_guard<std::mutex> lock(_mutex);

    if (level < 0.0f)
        level = 0.0f;

    if (level > 1.0f)
        level = 1.0f;

    const auto finalLevel = level;
    auto finalLevelActual = std::pow(level, 2.0f);

    if (finalLevelActual > 1.0f)
        finalLevelActual = 1.0f;
    else if (finalLevelActual < 0.0f)
        finalLevelActual = 0.0f;

    switch (intent) {
    case eAudioIntent::BackgroundMusic:
        _backgroundMusicAudioLevel = finalLevel;
        _backgroundMusicAudioLevelActual = finalLevelActual;
        break;
    case eAudioIntent::SoundEffect:
        _soundEffectsAudioLevel = finalLevel;
        _soundEffectsAudioLevelActual = finalLevelActual;
        break;
    case eAudioIntent::Video:
        _videoAudioLevel = finalLevel;
        _videoAudioLevelActual = finalLevelActual;
        break;
    case eAudioIntent::Master:
        _masterAudioLevel = finalLevel;
        _masterAudioLevelActual = finalLevelActual;
        break;
    }
}

void SDL2::SDL2SystemIO::ResetMouseButtonState() { _mouseButtonState = (eMouseButton)0; }

void SDL2::SDL2SystemIO::SetBackgroundMusic(std::unique_ptr<LibAbyss::AudioStream> stream) {
    std::lock_guard<std::mutex> lock(_mutex);
    _backgroundMusicStream = std::move(stream);
}
void SDL2::SDL2SystemIO::AddSoundEffect(SoundEffect *soundEffect) {
    std::lock_guard<std::mutex> lock(_mutex);
    _soundEffects.push_back(soundEffect);
}

void SDL2::SDL2SystemIO::RemoveSoundEffect(SoundEffect *soundEffect) {
    std::lock_guard<std::mutex> lock(_mutex);
    _soundEffects.erase(std::remove(_soundEffects.begin(), _soundEffects.end(), soundEffect), _soundEffects.end());
}

void SDL2::SDL2SystemIO::SetVideo(IAudio *video) {
    std::lock_guard<std::mutex> lock(_mutex);
    _video = video;
}

void SDL2::SDL2SystemIO::DrawLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b) {
    SDL_SetRenderDrawColor(_sdlRenderer.get(), r, g, b, 255);
    SDL_RenderDrawLine(_sdlRenderer.get(), x1, y1, x2, y2);
}

void SDL2::SDL2SystemIO::DrawRect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) {
    SDL_SetRenderDrawColor(_sdlRenderer.get(), r, g, b, 255);
    SDL_Rect destRect{x, y, w, h};
    SDL_RenderFillRect(_sdlRenderer.get(), &destRect);
}

bool SDL2::SDL2SystemIO::IsKeyPressed(uint16_t scancode) { return _pressedKeys[scancode]; }

std::string SDL2::SDL2SystemIO::GetInputText() { return _inputText; }

void SDL2::SDL2SystemIO::ClearInputText() { _inputText.clear(); }

void SDL2::SDL2SystemIO::ResetKeyState(uint16_t scancode) { _pressedKeys[scancode] = false; }

void SDL2::SDL2SystemIO::Sleep(uint32_t ticks) { SDL_Delay(ticks); }

void SDL2::SDL2SystemIO::GetScreenSize(int* width, int* height) {
    *width = _screenWidth;
    *height = _screenHeight;
}
void SDL2::SDL2SystemIO::SetWindowTitle(std::string_view title) {
    SDL_SetWindowTitle(_sdlWindow.get(), title.data());
}

} // namespace AbyssEngine
