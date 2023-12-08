module;

#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <SDL2/SDL.h>
#include <chrono>
#include <map>
#include <memory>
#include <string>

export module Abyss.AbyssEngine;

import Abyss.Common.Configuration;
import Abyss.Common.FileProvider;
import Abyss.Common.MouseProvider;
import Abyss.Common.RendererProvider;
import Abyss.Common.Scene;
import Abyss.Singletons;
import Abyss.Streams.AudioStream;
import Abyss.Common.Logging;
import Abyss.DataTypes.DC6;
import Abyss.Common.MouseState;
import Abyss.MPQ.File;
import Abyss.Streams.MPQStream;
import Abyss.Enums.MouseButton;
import Abyss.Enums.BlendMode;
import Abyss.DataTypes.Palette;
import Abyss.Streams.InputStream;
import Abyss.Common.CommandLineOpts;

namespace Abyss {

export class AbyssEngine final : public Common::FileProvider, public Common::RendererProvider, public Common::MouseProvider {
    bool _running;
    bool _mouseOverGameWindow;
    Common::Configuration _configuration;
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> _window;
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> _renderer;
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> _renderTexture;
    std::unique_ptr<Common::Scene> _currentScene;
    std::unique_ptr<Common::Scene> _nextScene;
    std::map<std::string, std::unique_ptr<DataTypes::DC6>> _cursors;
    std::map<std::string, std::shared_ptr<MPQ::File>> _mapResourceMpqFileMap;
    std::vector<std::shared_ptr<MPQ::File>> _mpqFileCache;
    DataTypes::DC6 *_cursorImage{};
    SDL_Rect _renderRect;
    Common::MouseState _mouseState;
    std::unique_ptr<Streams::AudioStream> _backgroundMusic;
    std::string _locale;

    AbyssEngine()
        : _running(true), _mouseOverGameWindow(false), _window(nullptr, SDL_DestroyWindow), _renderer(nullptr, SDL_DestroyRenderer),
          _renderTexture(nullptr, SDL_DestroyTexture), _currentScene(nullptr), _nextScene(nullptr), _renderRect(), _locale("latin") {

        Singletons::setFileProvider(this);
        Singletons::setRendererProvider(this);
        Singletons::setMouseProvider(this);

        Common::Log::Initialize();
        Common::Log::info("Abyss Engine");
        initializeSDL();
        initializeImGui();
        initializeAudio();
        updateRenderRect();
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

            _currentScene->render();

            if (_cursorImage != nullptr && _mouseState.isVisible() && _mouseOverGameWindow) {
                int mx, my;
                _mouseState.getPosition(mx, my);
                _cursorImage->draw(0, mx + 1, my + 2);
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

            if (_currentScene != nullptr) {
                _currentScene->processEvent(event);
            }
        }

        if (_currentScene != nullptr) {
            _currentScene->update(deltaTime);
        }
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

        SDL_AudioSpec want{};
        SDL_AudioSpec have{};

        want.freq = 44100;
        want.format = AUDIO_S16LSB;
        want.channels = 2;
        want.samples = 1024;
        // set callback to a lambda
        want.callback = [](void *userdata, Uint8 *stream, const int len) { static_cast<AbyssEngine *>(userdata)->fillAudioBuffer(stream, len); };
        want.userdata = this;

        if (SDL_OpenAudio(&want, &have) != 0)
            throw std::runtime_error("Failed to open audio: " + std::string(SDL_GetError()));
        /*
        if (have.format != want.format)
            throw std::runtime_error("Failed to open audio: Could not initialize with the proper format.");

        if (have.channels != want.channels)
            throw std::runtime_error("Failed to open audio: Could not initialize with the proper number of channels.");

        if (have.freq != want.freq)
            throw std::runtime_error("Failed to open audio: Could not initialize with the proper frequency.");

        if (have.samples != want.samples)
            throw std::runtime_error("Failed to open audio: Could not initialize with the proper sample size.");*/

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

            if (_backgroundMusic != nullptr) {
                sample += _backgroundMusic->getSample();
            }

            sample = sample * 0.5f; // VOLUME

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
        _backgroundMusic = std::make_unique<Streams::AudioStream>(loadStream(path));
        _backgroundMusic->setLoop(true);
        _backgroundMusic->play();
    }

    auto addCursorImage(const std::string_view name, const std::string_view path, const DataTypes::Palette &palette) -> void {
        _cursors.emplace(std::string(name), new DataTypes::DC6(path));
        auto &cursorIcon = *_cursors[name.data()];
        cursorIcon.setPalette(palette);
        cursorIcon.setBlendMode(Enums::BlendMode::Blend);
    }

    [[nodiscard]] auto loadFile(const std::string_view path) -> Streams::MPQStream {
        std::string lowercasePath;
        lowercasePath = path;
        std::ranges::transform(lowercasePath, lowercasePath.begin(), [](const unsigned char c) { return std::tolower(c); });
        if (const size_t pos = lowercasePath.find("{lang_font}"); pos != std::string::npos)
            lowercasePath.replace(pos, std::string("{lang_font}").length(), _locale);

        if (const auto mpqFileMapExists = _mapResourceMpqFileMap.contains(lowercasePath); !mpqFileMapExists) {
            bool found = false;
            for (const auto &mpqFile : _mpqFileCache) {
                if (mpqFile->hasFile(lowercasePath)) {
                    _mapResourceMpqFileMap[lowercasePath] = mpqFile;
                    found = true;
                    break;
                }
            }

            if (!found) {
                for (const auto &mpqFile : _configuration.getLoadOrder()) {
                    if (const auto file = std::make_shared<MPQ::File>(mpqFile); file->hasFile(lowercasePath)) {
                        Common::Log::info("Loaded MPQ file {}", mpqFile.string());
                        _mapResourceMpqFileMap[lowercasePath] = file;
                        _mpqFileCache.push_back(file);
                        break;
                    }
                }
            }

            if (!_mapResourceMpqFileMap.contains(lowercasePath)) {
                throw std::runtime_error("File not found in MPQ files: " + lowercasePath);
            }
        }

        return {_mapResourceMpqFileMap.at(lowercasePath), lowercasePath};
    }

    // MouseProvider
    auto setCursorImage(const std::string_view cursorName) -> void override { _cursorImage = _cursors[cursorName.data()].get(); }

    auto setCursorLocked(const bool locked) -> void override { SDL_SetRelativeMouseMode(locked ? SDL_TRUE : SDL_FALSE); }

    [[nodiscard]] auto getMouseState() -> Common::MouseState & override { return _mouseState; }

    // RendererProvider
    [[nodiscard]] auto getRenderer() -> SDL_Renderer * override { return _renderer.get(); }

    // FileProvider
    [[nodiscard]] auto loadStream(const std::string_view path) -> Streams::InputStream override {
        return Streams::InputStream(std::make_unique<Streams::MPQStream>(loadFile(path)));
    }

    [[nodiscard]] auto loadString(const std::string_view path) -> std::string override {
        const auto stream = loadStream(path);
        std::stringstream ss;
        ss << stream.rdbuf();
        return ss.str();
    }

    [[nodiscard]] auto loadStringList(const std::string_view path) -> std::vector<std::string> override {
        auto stream = loadStream(path);
        std::vector<std::string> result;
        std::string line;
        while (std::getline(stream, line)) {
            if (line.empty()) {
                continue;
            }
            result.push_back(line);
        }
        return result;
    }

    [[nodiscard]] auto loadBytes(const std::string_view path) -> std::vector<std::byte> override {
        auto stream = loadStream(path);
        std::vector<std::byte> result;
        result.resize(stream.size());
        stream.read(std::bit_cast<char *>(result.data()), stream.size());
        return result;
    }

    auto setWindowTitle(const std::string_view title) -> void { SDL_SetWindowTitle(_window.get(), title.data()); }
};

} // namespace Abyss
