#include "AbyssEngine.h"
#include "Common/CommandLineOpts.h"
#include "FileSystem/Direct.h"
#include "FileSystem/CASC.h"
#include "FileSystem/MPQ.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <memory>
#include <ranges>

namespace Abyss {

AbyssEngine::AbyssEngine()
    : _running(true), _mouseOverGameWindow(false), _window(nullptr, SDL_DestroyWindow), _renderer(nullptr, SDL_DestroyRenderer),
      _renderTexture(nullptr, SDL_DestroyTexture), _currentScene(nullptr), _nextScene(nullptr), _renderRect(), _locale("latin"), _lang("eng") {

    Singletons::setFileProvider(this);
    Singletons::setRendererProvider(this);
    Singletons::setMouseProvider(this);
    Singletons::setSoundEffectProvider(this);

    Common::Log::Initialize();
    Common::Log::info("Abyss Engine");
    initializeSDL();
    initializeImGui();
    initializeAudio();
    updateRenderRect();

    setBackgroundMusicVolumeLevel(0.8f);
    setVideoVolumeLevel(0.8f);
    setMasterVolumeLevel(0.8f);
    setSoundEffectsVolumeLevel(1.0f);
}

AbyssEngine::~AbyssEngine() {
    Common::Log::info("Shutting down...");

    // NOTE: you MUST clear all SDL2 related resources before tearing down SDL2! ---
    _currentScene.reset(nullptr);
    _nextScene.reset(nullptr);
    _cursorImage = nullptr;
    // -----------------------------------------------------------------------------

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_Quit();

    Common::Log::info("Shutdown complete");
}

void AbyssEngine::render() const {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();

    SDL_SetRenderDrawColor(_renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(_renderer.get());

    ImGui::NewFrame();
    if (_currentScene != nullptr) {
        SDL_SetRenderTarget(_renderer.get(), _renderTexture.get());

        if (_videoStream != nullptr) {
            _videoStream->render();
        } else {
            _currentScene->render();

            if (_cursorImage != nullptr && _mouseState.isVisible() && _mouseOverGameWindow) {
                int mx, my;
                _mouseState.getPosition(mx, my);
                _cursorImage->draw(0, mx + 1, my + 2);
            }
        }

        SDL_SetRenderTarget(_renderer.get(), nullptr);
        SDL_RenderCopy(_renderer.get(), _renderTexture.get(), nullptr, &_renderRect);
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

    // Is the mouse over an ImGui window, or outside _renderRect? If so, show the cursor, otherwise hide it
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || !_mouseOverGameWindow) {
        SDL_ShowCursor(SDL_TRUE);
    } else {
        SDL_ShowCursor(SDL_FALSE);
    }

    SDL_RenderPresent(_renderer.get());
}

void AbyssEngine::processEvents(const std::chrono::duration<double> deltaTime) {
    const std::map<uint8_t, Enums::MouseButton> buttonMap = {
        {SDL_BUTTON_LEFT, Enums::MouseButton::Left}, {SDL_BUTTON_RIGHT, Enums::MouseButton::Right}, {SDL_BUTTON_MIDDLE, Enums::MouseButton::Middle}};
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        switch (event.type) {
        case SDL_QUIT:
            _running = false;
            return;
        case SDL_MOUSEMOTION: {
            const auto mx = (event.motion.x - _renderRect.x) * 800 / _renderRect.w;
            const auto my = (event.motion.y - _renderRect.y) * 600 / _renderRect.h;
            _mouseOverGameWindow = mx >= 0 && mx < 800 && my >= 0 && my < 600;

            _mouseState.setPosition(std::clamp(mx, 0, 799), std::clamp(my, 0, 599));
        } break;
        case SDL_MOUSEBUTTONDOWN:
            if (_videoStream != nullptr && event.button.button == SDL_BUTTON_LEFT) {
                _videoStream->stopVideo();
                break;
            }
            _mouseState.setButtonState(buttonMap.at(event.button.button), true);
            break;
        case SDL_MOUSEBUTTONUP:
            _mouseState.setButtonState(buttonMap.at(event.button.button), false);
            break;
        case SDL_KEYDOWN:
            // If alt/option+enter is pressed, toggle fullscreen
            if (event.key.keysym.sym == SDLK_RETURN && (event.key.keysym.mod & KMOD_ALT) != 0) {
                if (const auto flags = SDL_GetWindowFlags(_window.get()); (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) == 0) {
                    SDL_SetWindowFullscreen(_window.get(), SDL_WINDOW_FULLSCREEN_DESKTOP);
                } else {
                    SDL_SetWindowFullscreen(_window.get(), 0);
                }
                updateRenderRect();
            }
            break;
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                updateRenderRect();
            }

            break;
        default:
            break;
        }

        if (_videoStream == nullptr && _currentScene != nullptr)
            _currentScene->processEvent(event);
    }

    if (_videoStream != nullptr) {
        _videoStream->update(static_cast<uint32_t>(deltaTime.count() * 1000.0));
    } else if (_currentScene != nullptr)
        _currentScene->update(deltaTime);
}

void AbyssEngine::initializeSDL() {
#ifdef _WIN32
    putenv("SDL_AUDIODRIVER=DirectSound");
#endif
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw std::runtime_error("SDL_Init Error: " + std::string(SDL_GetError()));
    }

    _window.reset(SDL_CreateWindow("Abyss Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE));
    if (_window == nullptr) {
        throw std::runtime_error("SDL_CreateWindow Error: " + std::string(SDL_GetError()));
    }

    _renderer.reset(SDL_CreateRenderer(_window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
    if (_renderer == nullptr) {
        throw std::runtime_error("SDL_CreateRenderer Error: " + std::string(SDL_GetError()));
    }

    _renderTexture.reset(SDL_CreateTexture(_renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 800, 600));
}

void AbyssEngine::initializeImGui() const {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(_window.get(), _renderer.get());
    ImGui_ImplSDLRenderer2_Init(_renderer.get());
    io.MouseDrawCursor = false;
    ImGui::SetMouseCursor(ImGuiMouseCursor_None);
}

void AbyssEngine::updateRenderRect() {
    // update the game's render rect so that it scales up to fit the window, using vertical or horizontal letterboxing
    int w, h;
    SDL_GetWindowSize(_window.get(), &w, &h);

    const auto windowAspectRatio = static_cast<float>(w) / static_cast<float>(h);
    constexpr float gameAspectRatio = 800.0f / 600.0f;

    if (windowAspectRatio > gameAspectRatio) {
        _renderRect.w = static_cast<int>(static_cast<float>(h) * gameAspectRatio);
        _renderRect.h = h;
        _renderRect.x = (w - _renderRect.w) / 2;
        _renderRect.y = 0;
        return;
    }

    _renderRect.w = w;
    _renderRect.h = static_cast<int>(static_cast<float>(w) / gameAspectRatio);
    _renderRect.x = 0;
    _renderRect.y = (h - _renderRect.h) / 2;
}

void AbyssEngine::processSceneChange() {
    if (_nextScene == nullptr) {
        return;
    }

    _mouseState.setButtonState(Enums::MouseButton::Left, false);
    _mouseState.setButtonState(Enums::MouseButton::Right, false);
    _mouseState.setButtonState(Enums::MouseButton::Middle, false);

    _currentScene = std::move(_nextScene);
    _nextScene = nullptr;
}

void AbyssEngine::initializeAudio() {
    Common::Log::info("Initializing audio...");

    SDL_AudioSpec want{.freq = 44100,
                       .format = AUDIO_S16LSB,
                       .channels = 2,
                       .samples = 512,
                       .callback = [](void *userdata, Uint8 *stream, const int len) { static_cast<AbyssEngine *>(userdata)->fillAudioBuffer(stream, len); },
                       .userdata = this};
    SDL_AudioSpec have{};

    if (SDL_OpenAudio(&want, &have) != 0)
        throw std::runtime_error("Failed to open audio: " + std::string(SDL_GetError()));

    Common::Log::info("Using audio device: {}", SDL_GetAudioDeviceName(0, 0));

    SDL_PauseAudio(0);
}

// ReSharper disable once CppDFAUnreachableFunctionCall - fillAudioBuffer is called via SDL2 audio callback
void AbyssEngine::fillAudioBuffer(Uint8 *stream, const int len) const {
    if (len & 1) {
        Common::Log::warn("Audio buffer length is not even, dropping samples...");
        std::fill_n(stream, len, 0);
        return;
    }

    for (auto i = 0; i < len; i += 2) {
        int32_t sample = 0;

        if (_videoStream != nullptr)
            sample += static_cast<int32_t>(static_cast<float>(_videoStream->getAudioSample()) * _videoAudioLevelActual);
        else if (_backgroundMusic != nullptr)
            sample += static_cast<int32_t>(static_cast<float>(_backgroundMusic->getSample()) * _backgroundMusicAudioLevelActual);

        for (const auto soundEffect : _soundEffects) {
            if (!soundEffect->getIsPlaying())
                continue;
            sample += static_cast<int32_t>(static_cast<float>(soundEffect->getSample()) * _soundEffectsAudioLevelActual);
        }

        sample = static_cast<int32_t>(static_cast<float>(sample) * _masterAudioLevelActual);

        sample = std::clamp(sample, -32768, 32767);
        stream[i] = sample & 0xFF;
        stream[i + 1] = (sample >> 8) & 0xFF;
    }
}

AbyssEngine &AbyssEngine::getInstance() {
    static AbyssEngine instance;
    return instance;
}

void AbyssEngine::quit() { _running = false; }

bool AbyssEngine::processCommandLineArguments(const int argc, char **argv) {
    bool quitOnRun = false;
    Common::CommandLineOpts::process(argc, argv, quitOnRun, _configuration);

    return !quitOnRun;
}

void AbyssEngine::initializeFiles() {
    if (!_configuration.getDirectDir().empty()) {
        _fileProvider.addProvider(std::make_unique<FileSystem::Direct>(_configuration.getDirectDir()));
    }
    if (!_configuration.getCASCDir().empty()) {
        _fileProvider.addProvider(std::make_unique<FileSystem::CASC>(_configuration.getCASCDir()));
    }
    if (!_configuration.getMPQDir().empty()) {
        for (const auto &mpqFile : _configuration.getLoadOrder()) {
            _fileProvider.addProvider(std::make_unique<FileSystem::MPQ>(mpqFile));
        }
    }
}

void AbyssEngine::run() {
    SDL_ShowCursor(SDL_FALSE);
    auto lastTime = std::chrono::high_resolution_clock::now();
    while (_running) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        processEvents(deltaTime);
        render();
        processSceneChange();

        if (_videoStream != nullptr && !_videoStream->getIsPlaying()) {
            _videoStream.reset(nullptr);
        }
    }
}

void AbyssEngine::setScene(std::unique_ptr<Common::Scene> scene) {
    if (_nextScene != nullptr) {
        Common::Log::warn("Scene already set, replacing...");
    }
    _nextScene = std::move(scene);
}

Common::Configuration &AbyssEngine::getConfiguration() { return _configuration; }

void AbyssEngine::setBackgroundMusic(const std::string_view path) {
    _backgroundMusic = std::make_unique<Streams::AudioStream>(loadFile(path));
    _backgroundMusic->setLoop(true);
    _backgroundMusic->play();
}

void AbyssEngine::addCursorImage(const std::string_view name, const std::string_view path, const DataTypes::Palette &palette) {
    _cursors.emplace(std::string(name), new DataTypes::DC6(path));
    auto &cursorIcon = *_cursors[name.data()];
    cursorIcon.setPalette(palette);
    cursorIcon.setBlendMode(Enums::BlendMode::Blend);
}

FileSystem::InputStream AbyssEngine::loadFile(const std::string_view file_path) {
    std::string path(file_path);
    std::ranges::transform(path, path.begin(), [](const char c) { return std::tolower(c); });
    if (const size_t pos = path.find("{lang_font}"); pos != std::string::npos)
        path.replace(pos, std::string("{lang_font}").length(), _locale);
    if (const size_t pos = path.find("{lang}"); pos != std::string::npos)
        path.replace(pos, std::string("{lang}").length(), _lang);
    return _fileProvider.loadFile(path);
}

bool AbyssEngine::fileExists(const std::string_view file_path) {
    std::string path(file_path);
    std::ranges::transform(path, path.begin(), [](const char c) { return std::tolower(c); });
    if (const size_t pos = path.find("{lang_font}"); pos != std::string::npos)
        path.replace(pos, std::string("{lang_font}").length(), _locale);
    if (const size_t pos = path.find("{lang}"); pos != std::string::npos)
        path.replace(pos, std::string("{lang}").length(), _lang);
    return _fileProvider.fileExists(path);
}

void AbyssEngine::setCursorImage(const std::string_view cursorName) { _cursorImage = _cursors[cursorName.data()].get(); }

void AbyssEngine::setCursorLocked(const bool locked) { SDL_SetRelativeMouseMode(locked ? SDL_TRUE : SDL_FALSE); }

Common::MouseState &AbyssEngine::getMouseState() { return _mouseState; }

SDL_Renderer *AbyssEngine::getRenderer() { return _renderer.get(); }

void AbyssEngine::setWindowTitle(const std::string_view title) const { SDL_SetWindowTitle(_window.get(), title.data()); }

void AbyssEngine::playVideo(const std::string_view path) { _videoStream = std::make_unique<Streams::VideoStream>(loadFile(path), std::nullopt); }
void AbyssEngine::playVideoAndAudio(const std::string_view videoPath, const std::string_view audioPath) { _videoStream = std::make_unique<Streams::VideoStream>(loadFile(videoPath), loadFile(audioPath)); }

float AbyssEngine::getMasterVolumeLevel() const { return _masterAudioLevel; }

void AbyssEngine::setMasterVolumeLevel(const float level) {
    _masterAudioLevel = std::clamp(level, 0.0f, 1.0f);
    _masterAudioLevelActual = std::pow(_masterAudioLevel, 2.0f);
}

float AbyssEngine::getVideoVolumeLevel() const { return _videoAudioLevel; }

void AbyssEngine::setVideoVolumeLevel(const float level) {
    _videoAudioLevel = std::clamp(level, 0.0f, 1.0f);
    _videoAudioLevelActual = std::pow(_videoAudioLevel, 2.0f);
}

float AbyssEngine::getBackgroundMusicVolumeLevel() const { return _backgroundMusicAudioLevel; }

void AbyssEngine::setBackgroundMusicVolumeLevel(const float level) {
    _backgroundMusicAudioLevel = std::clamp(level, 0.0f, 1.0f);
    _backgroundMusicAudioLevelActual = std::pow(_backgroundMusicAudioLevel, 2.0f);
}

float AbyssEngine::getSoundEffectsVolumeLevel() const { return _soundEffectsAudioLevel; }

void AbyssEngine::setSoundEffectsVolumeLevel(const float level) {
    _soundEffectsAudioLevel = std::clamp(level, 0.0f, 1.0f);
    _soundEffectsAudioLevelActual = std::pow(_soundEffectsAudioLevel, 2.0f);
}

void AbyssEngine::addSoundEffect(Common::SoundEffectInterface *soundEffect) { _soundEffects.push_back(soundEffect); }

void AbyssEngine::removeSoundEffect(Common::SoundEffectInterface *soundEffect) {
    _soundEffects.erase(std::ranges::remove(_soundEffects, soundEffect).begin(), _soundEffects.end());
}

} // namespace Abyss
