#pragma once

#include "Abyss/Streams/InputStream.h"
#include <Abyss/Common/CommandLineOpts.h>
#include <Abyss/Common/Configuration.h>
#include <Abyss/Common/MouseState.h>
#include <Abyss/Common/Scene.h>
#include <Abyss/DataTypes/DC6.h>
#include <Abyss/MPQ/File.h>
#include <Abyss/Streams/AudioStream.h>
#include <Abyss/Streams/MPQStream.h>
#include <SDL2/SDL.h>
#include <map>
#include <string>

namespace Abyss {

class AbyssEngine {
    bool running;
    bool mouseOverGameWindow;
    Common::Configuration configuration;
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> _window;
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> _renderer;
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> _renderTexture;
    std::unique_ptr<Common::Scene> _currentScene;
    std::unique_ptr<Common::Scene> _nextScene;
    std::map<std::string, std::shared_ptr<MPQ::File>> _mapResourceMpqFileMap;
    std::vector<std::shared_ptr<MPQ::File>> _mpqFileCache;
    std::unique_ptr<DataTypes::DC6> _cursorImage;
    SDL_Rect _renderRect;
    Common::MouseState _mouseState;
    std::unique_ptr<Streams::AudioStream> _backgroundMusic;

    AbyssEngine();
    ~AbyssEngine();
    auto render() const -> void;
    auto processEvents(std::chrono::duration<double> deltaTime) -> void;
    auto initializeSDL() -> void;
    auto initializeImGui() const -> void;
    auto updateRenderRect() -> void;
    auto processSceneChange() -> void;
    auto initializeAudio() -> void;
    auto fillAudioBuffer(Uint8 *stream, int len) -> void;

  public:
    [[nodiscard]] static auto getInstance() -> AbyssEngine &;
    auto processCommandLineArguments(int argc, char **argv) -> bool;
    auto run() -> void;
    auto setScene(std::unique_ptr<Common::Scene> scene) -> void;
    [[nodiscard]] auto getRenderer() const -> SDL_Renderer *;
    [[nodiscard]] auto getConfiguration() -> Common::Configuration &;
    auto setCursorImage(std::string_view path, const DataTypes::Palette &palette) -> void;
    auto setCursorLocked(bool locked) -> void;
    auto setBackgroundMusic(std::string_view path) -> void;
    [[nodiscard]] auto getMouseState() -> Common::MouseState &;
    [[nodiscard]] auto loadFile(std::string_view path) -> Streams::MPQStream;
    [[nodiscard]] auto loadStream(std::string_view path) -> Streams::InputStream;
    [[nodiscard]] auto loadString(std::string_view path) -> std::string;
    [[nodiscard]] auto loadStringList(std::string_view path) -> std::vector<std::string>;
    [[nodiscard]] auto loadBytes(std::string_view path) -> std::vector<std::byte>;
};

} // namespace Abyss
