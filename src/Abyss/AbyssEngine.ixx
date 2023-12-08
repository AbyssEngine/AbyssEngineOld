module;

#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <SDL2/SDL.h>
#include <chrono>
#include <cmath>
#include <map>
#include <memory>
#include <string>
#include <vector>

export module Abyss.AbyssEngine;

import Abyss.Common.Configuration;
import Abyss.FileSystem.FileLoader;
import Abyss.FileSystem.MPQ;
import Abyss.Common.MouseProvider;
import Abyss.Common.RendererProvider;
import Abyss.Common.Scene;
import Abyss.Singletons;
import Abyss.Streams.AudioStream;
import Abyss.Streams.VideoStream;
import Abyss.DataTypes.DC6;
import Abyss.Common.Logging;
import Abyss.DataTypes.DC6;
import Abyss.Common.MouseState;
import Abyss.Enums.MouseButton;
import Abyss.Enums.BlendMode;
import Abyss.DataTypes.Palette;
import Abyss.FileSystem.InputStream;
import Abyss.Common.CommandLineOpts;
import Abyss.Common.SoundEffectProvider;

namespace Abyss {

export class AbyssEngine final : public FileSystem::FileLoader, public Common::RendererProvider, public Common::MouseProvider, Common::SoundEffectProvider {
    bool _running;
    bool _mouseOverGameWindow;
    Common::Configuration _configuration;
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> _window;
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> _renderer;
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> _renderTexture;
    std::unique_ptr<Common::Scene> _currentScene;
    std::unique_ptr<Common::Scene> _nextScene;
    std::unique_ptr<Streams::VideoStream> _videoStream;
    std::map<std::string, std::unique_ptr<DataTypes::DC6>> _cursors;
    std::vector<Common::SoundEffectInterface *> _soundEffects;
    DataTypes::DC6 *_cursorImage{};
    SDL_Rect _renderRect;
    Common::MouseState _mouseState;
    std::unique_ptr<Streams::AudioStream> _backgroundMusic;
    std::string _locale;
    std::string _lang;
    FileSystem::MultiFileLoader _fileProvider;
    float _masterAudioLevel = 1.0f;
    float _masterAudioLevelActual = 0.5f;
    float _videoAudioLevel = 1.0f;
    float _videoAudioLevelActual = 1.0f;
    float _backgroundMusicAudioLevel = 1.0f;
    float _backgroundMusicAudioLevelActual = 1.0f;
    float _soundEffectsAudioLevel = 1.0f;
    float _soundEffectsAudioLevelActual = 1.0f;

    AbyssEngine()
        : _running(true), _mouseOverGameWindow(false), _window(nullptr, SDL_DestroyWindow), _renderer(nullptr, SDL_DestroyRenderer),
          _renderTexture(nullptr, SDL_DestroyTexture), _currentScene(nullptr), _nextScene(nullptr), _videoStream(), _renderRect(), _locale("latin"),
          _lang("eng") {

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

    ~AbyssEngine() override {
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

    auto render() const -> void {
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

    auto processEvents(const std::chrono::duration<double> deltaTime) -> void {
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
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    updateRenderRect();
                }

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

    auto initializeSDL() -> void {
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

    auto initializeImGui() const -> void {
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

    auto updateRenderRect() -> void {
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

    auto processSceneChange() -> void {
        if (_nextScene == nullptr) {
            return;
        }

        _mouseState.setButtonState(Enums::MouseButton::Left, false);
        _mouseState.setButtonState(Enums::MouseButton::Right, false);
        _mouseState.setButtonState(Enums::MouseButton::Middle, false);

        _currentScene = std::move(_nextScene);
        _nextScene = nullptr;
    }

    auto initializeAudio() -> void {
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

    auto fillAudioBuffer(Uint8 *stream, const int len) -> void {
        if (len & 1) {
            Common::Log::warn("Audio buffer length is not even, dropping samples...");
            std::fill_n(stream, len, 0);
            return;
        }

        for (auto i = 0; i < len; i += 2) {
            int32_t sample = 0;

            if (_videoStream != nullptr)
                sample += _videoStream->getAudioSample() * _videoAudioLevelActual;
            else if (_backgroundMusic != nullptr)
                sample += _backgroundMusic->getSample() * _backgroundMusicAudioLevelActual;

            for (auto soundEffect : _soundEffects) {
                if (!soundEffect->getIsPlaying())
                    continue;
                sample += soundEffect->getSample() * _soundEffectsAudioLevelActual;
            }

            sample *= _masterAudioLevelActual;

            sample = std::clamp(sample, -32768, 32767);
            stream[i] = sample & 0xFF;
            stream[i + 1] = (sample >> 8) & 0xFF;
        }
    }

  public:
    [[nodiscard]] static auto getInstance() -> AbyssEngine & {
        static AbyssEngine instance;
        return instance;
    }

    auto quit() -> void { _running = false; }

    auto processCommandLineArguments(const int argc, char **argv) -> bool {
        bool quitOnRun = false;
        Common::CommandLineOpts::process(argc, argv, quitOnRun, _configuration);

        return !quitOnRun;
    }

    void initializeFiles() {
        for (const auto &mpqFile : _configuration.getLoadOrder()) {
            _fileProvider.addProvider(std::make_unique<FileSystem::MPQ>(mpqFile));
        }
    }

    auto run() -> void {
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

    auto setScene(std::unique_ptr<Common::Scene> scene) -> void {
        if (_nextScene != nullptr) {
            Common::Log::warn("Scene already set, replacing...");
        }
        _nextScene = std::move(scene);
    }

    [[nodiscard]] auto getConfiguration() -> Common::Configuration & { return _configuration; }

    auto setBackgroundMusic(const std::string_view path) -> void {
        _backgroundMusic = std::make_unique<Streams::AudioStream>(loadFile(path));
        _backgroundMusic->setLoop(true);
        _backgroundMusic->play();
    }

    auto addCursorImage(const std::string_view name, const std::string_view path, const DataTypes::Palette &palette) -> void {
        _cursors.emplace(std::string(name), new DataTypes::DC6(path));
        auto &cursorIcon = *_cursors[name.data()];
        cursorIcon.setPalette(palette);
        cursorIcon.setBlendMode(Enums::BlendMode::Blend);
    }

    // FileProvider
    [[nodiscard]] auto loadFile(std::string_view file_path) -> FileSystem::InputStream override {
        std::string path(file_path);
        std::ranges::transform(path, path.begin(), [](const char c) { return std::tolower(c); });
        if (const size_t pos = path.find("{lang_font}"); pos != std::string::npos)
            path.replace(pos, std::string("{lang_font}").length(), _locale);
        if (const size_t pos = path.find("{lang}"); pos != std::string::npos)
            path.replace(pos, std::string("{lang}").length(), _lang);
        return _fileProvider.loadFile(path);
    }

    [[nodiscard]] auto fileExists(std::string_view file_path) -> bool override {
        std::string path(file_path);
        std::ranges::transform(path, path.begin(), [](const char c) { return std::tolower(c); });
        if (const size_t pos = path.find("{lang_font}"); pos != std::string::npos)
            path.replace(pos, std::string("{lang_font}").length(), _locale);
        if (const size_t pos = path.find("{lang}"); pos != std::string::npos)
            path.replace(pos, std::string("{lang}").length(), _lang);
        return _fileProvider.fileExists(path);
    }

    // MouseProvider
    auto setCursorImage(const std::string_view cursorName) -> void override { _cursorImage = _cursors[cursorName.data()].get(); }

    auto setCursorLocked(const bool locked) -> void override { SDL_SetRelativeMouseMode(locked ? SDL_TRUE : SDL_FALSE); }

    [[nodiscard]] auto getMouseState() -> Common::MouseState & override { return _mouseState; }

    // RendererProvider
    [[nodiscard]] auto getRenderer() -> SDL_Renderer * override { return _renderer.get(); }

    auto setWindowTitle(const std::string_view title) -> void { SDL_SetWindowTitle(_window.get(), title.data()); }

    auto playVideo(const std::string_view path) -> void { _videoStream = std::make_unique<Streams::VideoStream>(std::move(loadFile(path)), std::nullopt); }

    auto getMasterVolumeLevel() const -> float { return _masterAudioLevel; }

    auto setMasterVolumeLevel(const float level) -> void {
        _masterAudioLevel = std::clamp(level, 0.0f, 1.0f);
        _masterAudioLevelActual = std::pow(_masterAudioLevel, 2.0f);
    }

    auto getVideoVolumeLevel() const -> float { return _videoAudioLevel; }

    auto setVideoVolumeLevel(const float level) -> void {
        _videoAudioLevel = std::clamp(level, 0.0f, 1.0f);
        _videoAudioLevelActual = std::pow(_videoAudioLevel, 2.0f);
    }

    auto getBackgroundMusicVolumeLevel() const -> float { return _backgroundMusicAudioLevel; }

    auto setBackgroundMusicVolumeLevel(const float level) -> void {
        _backgroundMusicAudioLevel = std::clamp(level, 0.0f, 1.0f);
        _backgroundMusicAudioLevelActual = std::pow(_backgroundMusicAudioLevel, 2.0f);
    }

    auto getSoundEffectsVolumeLevel() const -> float { return _soundEffectsAudioLevel; }

    auto setSoundEffectsVolumeLevel(const float level) -> void {
        _soundEffectsAudioLevel = std::clamp(level, 0.0f, 1.0f);
        _soundEffectsAudioLevelActual = std::pow(_soundEffectsAudioLevel, 2.0f);
    }

    auto addSoundEffect(Common::SoundEffectInterface *soundEffect) -> void override { _soundEffects.push_back(soundEffect); }

    auto removeSoundEffect(Common::SoundEffectInterface *soundEffect) -> void override {
        _soundEffects.erase(std::remove(_soundEffects.begin(), _soundEffects.end(), soundEffect), _soundEffects.end());
    }
};

} // namespace Abyss
