#include "sdl2systemio.h"
#include "../../hostnotify/hostnotify.h"
#include "config.h"
#include "sdl2texture.h"
#include <cstdint>
#include <SDL.h>
#include <SDL_hints.h>
#include <SDL_stdinc.h>
#include <SDL_syswm.h>
#include <span>
#include <spdlog/spdlog.h>
#ifdef __APPLE__
#include "../../hostnotify/hostnotify_mac_shim.h"
#include "../../engine/engine.h"
#endif // __APPLE__

namespace {
const int AudioBufferSize = 1024 * 128;
}

AbyssEngine::SDL2::SDL2SystemIO::SDL2SystemIO()
    : AbyssEngine::SystemIO::SystemIO(), _audioBuffer(AudioBufferSize), _audioSpec(), _mutex(), _mouseButtonState((eMouseButton)0) {
    SPDLOG_TRACE("Creating SDL2 System IO");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0)
        throw std::runtime_error(SDL_GetError());

    _sdlWindow = std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>>(
        SDL_CreateWindow(ABYSS_VERSION_STRING, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_RESIZABLE), [](SDL_Window *x) {
            SDL_DestroyWindow(x);
            SDL_Quit();
        });

    if (_sdlWindow == nullptr)
        throw std::runtime_error(SDL_GetError());

#ifdef _WIN32
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(_sdlWindow.get(), &wmInfo);
    AbyssEngine::HostNotify::Win32Handle = wmInfo.info.win.window;
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

AbyssEngine::SDL2::SDL2SystemIO::~SDL2SystemIO() {
    SPDLOG_TRACE("Destroying SDL2 System IO");

    FinalizeAudio();

#ifdef __APPLE__
    AbyssHostNotifyFinalizeMac();
#endif // __APPLE__
}

std::string_view AbyssEngine::SDL2::SDL2SystemIO::Name() { return "SDL2"; }

void AbyssEngine::SDL2::SDL2SystemIO::PauseAudio(bool pause) {
    std::lock_guard<std::mutex> lock(_mutex);

    if (!_hasAudio)
        return;

    SDL_PauseAudioDevice(_audioDeviceId, pause ? SDL_TRUE : SDL_FALSE);
}

void AbyssEngine::SDL2::SDL2SystemIO::SetFullscreen(bool fullscreen) {
    std::lock_guard<std::mutex> lock(_mutex);

    SDL_SetWindowFullscreen(_sdlWindow.get(), fullscreen ? SDL_TRUE : SDL_FALSE);
}

bool AbyssEngine::SDL2::SDL2SystemIO::HandleSdlEvent(const SDL_Event &sdlEvent, Node &rootNode) {
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
    case SDL_QUIT:
        return false;
    default:
        return true;
    }
}

std::unique_ptr<AbyssEngine::ITexture> AbyssEngine::SDL2::SDL2SystemIO::CreateTexture(ITexture::Format textureFormat, uint32_t width,
                                                                                      uint32_t height) {
    std::lock_guard<std::mutex> lock(_mutex);

    return std::make_unique<SDL2Texture>(_sdlRenderer.get(), textureFormat, width, height);
}
void AbyssEngine::SDL2::SDL2SystemIO::InitializeAudio() {
    std::lock_guard<std::mutex> lock(_mutex);

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
        SPDLOG_WARN(SDL_GetError());
        return;
    }

    SPDLOG_INFO("Using audio device {0} via {1}", SDL_GetAudioDeviceName(_audioDeviceId, SDL_FALSE), SDL_GetCurrentAudioDriver());
}

void AbyssEngine::SDL2::SDL2SystemIO::HandleAudioCallback(void *userData, Uint8 *stream, int length) {
    auto *source = (SDL2SystemIO *)userData;
    source->HandleAudio(stream, length);
}

void AbyssEngine::SDL2::SDL2SystemIO::HandleAudio(uint8_t *stream, int length) {
    std::lock_guard<std::mutex> lock(_mutex);

    if (length & 1) {
        SPDLOG_WARN("Audio callback length is not even, dropping samples");
        return;
    }

    _audioBuffer.ReadData(std::span(stream, length));
}

void AbyssEngine::SDL2::SDL2SystemIO::FinalizeAudio() {
    std::lock_guard<std::mutex> lock(_mutex);

    if (!_hasAudio)
        return;

    SDL_PauseAudioDevice(_audioDeviceId, SDL_TRUE);
    SDL_CloseAudioDevice(_audioDeviceId);
}
void AbyssEngine::SDL2::SDL2SystemIO::PushAudioData(std::span<const uint8_t> data) { _audioBuffer.PushData(data); }

void AbyssEngine::SDL2::SDL2SystemIO::ResetAudio() { _audioBuffer.Reset(); }

bool AbyssEngine::SDL2::SDL2SystemIO::HandleInputEvents(Node &rootNode) {
    SDL_Event sdlEvent;

    while (SDL_PollEvent(&sdlEvent)) {
        if (!HandleSdlEvent(sdlEvent, rootNode))
            return false;
    }

    return true;
}

uint32_t AbyssEngine::SDL2::SDL2SystemIO::GetTicks() { return SDL_GetTicks(); }

void AbyssEngine::SDL2::SDL2SystemIO::RenderStart() { SDL_RenderClear(_sdlRenderer.get()); }

void AbyssEngine::SDL2::SDL2SystemIO::RenderEnd() { SDL_RenderPresent(_sdlRenderer.get()); }

void AbyssEngine::SDL2::SDL2SystemIO::Delay(uint32_t ms) { SDL_Delay(ms); }

void AbyssEngine::SDL2::SDL2SystemIO::GetCursorState(int &cursorX, int &cursorY, eMouseButton &buttonState) {
    std::lock_guard<std::mutex> lock(_mutex);

    cursorX = _cursorX;
    cursorY = _cursorY;
    buttonState = _mouseButtonState;
}

float AbyssEngine::SDL2::SDL2SystemIO::GetMasterAudioLevel() { return _masterAudioLevel; }

void AbyssEngine::SDL2::SDL2SystemIO::SetMasterAudioLevel(float level) { _masterAudioLevel = level; }
