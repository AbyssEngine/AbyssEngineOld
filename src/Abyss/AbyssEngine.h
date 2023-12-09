#pragma once

#include "Common/CommandLineOpts.h"
#include "Common/Configuration.h"
#include "Common/Logging.h"
#include "Common/MouseProvider.h"
#include "Common/RendererProvider.h"
#include "Common/Scene.h"
#include "Common/SoundEffectProvider.h"
#include "DataTypes/DC6.h"
#include "FileSystem/FileLoader.h"
#include "FileSystem/MPQ.h"
#include "Singletons.h"
#include "Streams/VideoStream.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include <SDL2/SDL.h>
#include <chrono>
#include <cmath>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Abyss {

class AbyssEngine final : public FileSystem::FileLoader, public Common::RendererProvider, public Common::MouseProvider, Common::SoundEffectProvider {
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

    AbyssEngine();
    ~AbyssEngine() override;
    void render() const;
    void processEvents(std::chrono::duration<double> deltaTime);
    void initializeSDL();
    void initializeImGui() const;
    void updateRenderRect();
    void processSceneChange();
    void initializeAudio();
    void fillAudioBuffer(Uint8 *stream, int len) const;

  public:
    [[nodiscard]] static AbyssEngine &getInstance();
    void quit();
    bool processCommandLineArguments(int argc, char **argv);
    void initializeFiles();
    void run();
    void setScene(std::unique_ptr<Common::Scene> scene);
    [[nodiscard]] Common::Configuration &getConfiguration();
    void setBackgroundMusic(std::string_view path);
    void addCursorImage(std::string_view name, std::string_view path, const DataTypes::Palette &palette);

    // FileProvider
    [[nodiscard]] FileSystem::InputStream loadFile(std::string_view file_path) override;
    [[nodiscard]] bool fileExists(std::string_view file_path) override;

    // MouseProvider
    void setCursorImage(std::string_view cursorName) override;
    void setCursorLocked(bool locked) override;
    [[nodiscard]] Common::MouseState &getMouseState() override;

    // RendererProvider
    [[nodiscard]] SDL_Renderer *getRenderer() override;
    void setWindowTitle(std::string_view title);
    void playVideo(std::string_view path);
    float getMasterVolumeLevel() const;
    void setMasterVolumeLevel(float level);
    float getVideoVolumeLevel() const;
    void setVideoVolumeLevel(float level);
    float getBackgroundMusicVolumeLevel() const;
    void setBackgroundMusicVolumeLevel(float level);
    float getSoundEffectsVolumeLevel() const;
    void setSoundEffectsVolumeLevel(float level);
    void addSoundEffect(Common::SoundEffectInterface *soundEffect) override;
    void removeSoundEffect(Common::SoundEffectInterface *soundEffect) override;
};

} // namespace Abyss
