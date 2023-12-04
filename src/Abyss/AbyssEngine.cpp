#include "AbyssEngine.h"

#include "Common/CommandLineOpts.h"
#include "Extras/Imgui/imgui_impl_sdl2.h"
#include "Extras/Imgui/imgui_impl_sdlrenderer2.h"
#include <Abyss/Common/Logging.h>
#include <map>

namespace Abyss {

AbyssEngine::AbyssEngine()
    : running(true), mouseOverGameWindow(false), _window(nullptr, SDL_DestroyWindow), _renderer(nullptr, SDL_DestroyRenderer),
      _renderTexture(nullptr, SDL_DestroyTexture), _currentScene(nullptr), _nextScene(nullptr), _renderRect() {
    Common::Log::Initialize();
    Common::Log::info("Abyss Engine");
    initializeSDL();
    initializeImGui();
    initializeAudio();
    updateRenderRect();
}

AbyssEngine::~AbyssEngine() {
    Common::Log::info("Shutting down...");

    // NOTE: you MUST clear all SDL2 related resources before tearing down SDL2! ---
    _currentScene.reset(nullptr);
    _nextScene.reset(nullptr);
    _cursorImage.reset(nullptr);
    // -----------------------------------------------------------------------------

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_Quit();

    Common::Log::info("Shutdown complete");
}

auto AbyssEngine::render() const -> void {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();

    SDL_SetRenderDrawColor(_renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(_renderer.get());

    ImGui::NewFrame();
    if (_currentScene != nullptr) {
        SDL_SetRenderTarget(_renderer.get(), _renderTexture.get());

        _currentScene->render();

        if (_cursorImage != nullptr && _mouseState.isVisible() && mouseOverGameWindow) {
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
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || !mouseOverGameWindow) {
        SDL_ShowCursor(SDL_TRUE);
    } else {
        SDL_ShowCursor(SDL_FALSE);
    }

    SDL_RenderPresent(_renderer.get());
}

auto AbyssEngine::processEvents(const std::chrono::duration<double> deltaTime) -> void {
    const std::map<uint8_t, Common::MouseButton> buttonMap = {
        {SDL_BUTTON_LEFT, Common::MouseButton::Left}, {SDL_BUTTON_RIGHT, Common::MouseButton::Right}, {SDL_BUTTON_MIDDLE, Common::MouseButton::Middle}};
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        switch (event.type) {
        case SDL_QUIT:
            running = false;
            return;
        case SDL_MOUSEMOTION: {
            const auto mx = (event.motion.x - _renderRect.x) * 800 / _renderRect.w;
            const auto my = (event.motion.y - _renderRect.y) * 600 / _renderRect.h;
            mouseOverGameWindow = mx >= 0 && mx < 800 && my >= 0 && my < 600;

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

auto AbyssEngine::initializeSDL() -> void {
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

auto AbyssEngine::initializeImGui() const -> void {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(_window.get(), _renderer.get());
    ImGui_ImplSDLRenderer2_Init(_renderer.get());
    io.MouseDrawCursor = false;
    ImGui::SetMouseCursor(ImGuiMouseCursor_None);
}

auto AbyssEngine::updateRenderRect() -> void {
    // Update the game's render rect so that it scales up to fit the window, using vertical or horizontal letterboxing
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

auto AbyssEngine::processSceneChange() -> void {
    if (_nextScene == nullptr) {
        return;
    }
    _currentScene = std::move(_nextScene);
    _nextScene = nullptr;
}

auto AbyssEngine::getInstance() -> AbyssEngine & {
    static AbyssEngine instance;
    return instance;
}

auto AbyssEngine::processCommandLineArguments(const int argc, char **argv) -> bool {
    bool quitOnRun = false;
    Common::CommandLineOpts::process(argc, argv, quitOnRun, configuration);

    return !quitOnRun;
}

auto AbyssEngine::run() -> void {
    SDL_ShowCursor(SDL_FALSE);
    auto lastTime = std::chrono::high_resolution_clock::now();
    while (running) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        processEvents(deltaTime);
        render();
        processSceneChange();
    }
}

auto AbyssEngine::setScene(std::unique_ptr<Common::Scene> scene) -> void {
    if (_nextScene != nullptr) {
        Common::Log::warn("Scene already set, replacing...");
    }
    _nextScene = std::move(scene);
}

auto AbyssEngine::getRenderer() const -> SDL_Renderer * { return _renderer.get(); }

auto AbyssEngine::getConfiguration() -> Common::Configuration & { return configuration; }

auto AbyssEngine::loadFile(const std::string_view path) -> Streams::MPQStream {
    std::string lowercasePath;
    lowercasePath = path;
    std::ranges::transform(lowercasePath, lowercasePath.begin(), [](const unsigned char c) { return std::tolower(c); });

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
            for (const auto &mpqFile : configuration.getLoadOrder()) {
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

auto AbyssEngine::loadStream(const std::string_view path) -> Streams::InputStream {
    return Streams::InputStream(std::make_unique<Streams::MPQStream>(loadFile(path)));
}

auto AbyssEngine::loadString(const std::string_view path) -> std::string {
    const auto stream = loadStream(path);
    std::stringstream ss;
    ss << stream.rdbuf();
    return ss.str();
}

auto AbyssEngine::loadStringList(const std::string_view path) -> std::vector<std::string> {
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

auto AbyssEngine::loadBytes(const std::string_view path) -> std::vector<std::byte> {
    auto stream = loadStream(path);
    std::vector<std::byte> result;
    result.resize(stream.size());
    stream.read(std::bit_cast<char *>(result.data()), stream.size());
    return result;
}

auto AbyssEngine::setCursorLocked(const bool locked) -> void { SDL_SetRelativeMouseMode(locked ? SDL_TRUE : SDL_FALSE); }

auto AbyssEngine::initializeAudio() -> void {
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

auto AbyssEngine::fillAudioBuffer(Uint8 *stream, const int len) -> void {
    if (len & 1) {
        Common::Log::warn("Audio buffer length is not even, dropping samples...");
        std::fill_n(stream, len, 0);
        return;
    }

    for (auto i = 0; i < len; i += 2) {
        int32_t sample = 0;

        if (_backgroundMusic != nullptr) {
            sample += _backgroundMusic->GetSample();
        }

        sample = sample * 0.5f; // VOLUME

        stream[i] = sample & 0xFF;
        stream[i + 1] = (sample >> 8) & 0xFF;
    }

    //
}

auto AbyssEngine::setCursorImage(std::string_view path, const DataTypes::Palette &palette) -> void {
    _cursorImage = std::make_unique<DataTypes::DC6>(path);
    _cursorImage->setPalette(palette);
    _cursorImage->setBlendMode(Enums::BlendMode::Blend);
}

auto AbyssEngine::getMouseState() -> Common::MouseState & { return _mouseState; }

auto AbyssEngine::setBackgroundMusic(const std::string_view path) -> void {
    _backgroundMusic = std::make_unique<Streams::AudioStream>(loadStream(path));
    _backgroundMusic->SetLoop(true);
    _backgroundMusic->Play();
}

} // namespace Abyss
