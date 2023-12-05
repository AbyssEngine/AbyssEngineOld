#pragma once

#include <Abyss/Common/Configuration.hpp>
#include <Abyss/Common/FileProvider.hpp>
#include <Abyss/Common/MouseProvider.hpp>
#include <Abyss/Common/Scene.hpp>
#include <Abyss/DataTypes/DC6.hpp>
#include <Abyss/MPQ/File.h>
#include <Abyss/Streams/AudioStream.h>
#include <Abyss/Streams/MPQStream.h>
#include <SDL2/SDL.h>
#include <map>
#include <string>

namespace Abyss {

class AbyssEngine final : public Common::FileProvider, public Common::RendererProvider, public Common::MouseProvider {
    bool running;
    bool mouseOverGameWindow;
    Common::Configuration configuration;
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> _window;
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> _renderer;
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> _renderTexture;
    std::unique_ptr<Common::Scene> _currentScene;
    std::unique_ptr<Common::Scene> _nextScene;
    std::map<std::string, std::unique_ptr<DataTypes::DC6>> _cursors;
    std::map<std::string, std::shared_ptr<MPQ::File>> _mapResourceMpqFileMap;
    std::vector<std::shared_ptr<MPQ::File>> _mpqFileCache;
    DataTypes::DC6 *_cursorImage;
    SDL_Rect _renderRect;
    Common::MouseState _mouseState;
    std::unique_ptr<Streams::AudioStream> _backgroundMusic;
    std::string _locale;

    AbyssEngine();
    ~AbyssEngine() override;
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
    [[nodiscard]] auto getConfiguration() -> Common::Configuration &;
    auto setBackgroundMusic(std::string_view path) -> void;
    auto addCursorImage(std::string_view name, std::string_view path, const DataTypes::Palette &palette) -> void;

    // MouseProvider
    auto setCursorImage(std::string_view cursorName) -> void override;
    auto setCursorLocked(bool locked) -> void override;
    [[nodiscard]] auto getMouseState() -> Common::MouseState & override;

    // RendererProvider
    [[nodiscard]] auto getRenderer() const -> SDL_Renderer *;

    // FileProvider
    [[nodiscard]] auto loadFile(std::string_view path) -> Streams::MPQStream override;
    [[nodiscard]] auto loadStream(std::string_view path) -> Streams::InputStream override;
    [[nodiscard]] auto loadString(std::string_view path) -> std::string override;
    [[nodiscard]] auto loadStringList(std::string_view path) -> std::vector<std::string> override;
    [[nodiscard]] auto loadBytes(std::string_view path) -> std::vector<std::byte> override;
    [[nodiscard]] auto getRenderer() -> SDL_Renderer * override { return _renderer.get(); }
};

} // namespace Abyss
