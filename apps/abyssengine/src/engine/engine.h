#ifndef ABYSS_ENGINE_H
#define ABYSS_ENGINE_H

#include "../node/debugconsole.h"
#include "../node/sprite.h"
#include "../node/video.h"
#include "../systemio/interface.h"
#include "embeddedfileprovider.h"
#include "libabyss/formats/abyss/inifile.h"
#include "libabyss/formats/d2/palette.h"
#include "loader.h"
#include "scripthost.h"
#include <absl/container/node_hash_map.h>
#include <exception>
#include <filesystem>
#include <functional>
#include <optional>
#include <spdlog/sinks/sink.h>
#include <string>
#include <utility>

namespace AbyssEngine {

/// @brief Engine class
/// @details This class is the main class of the engine. It is responsible for
///          loading all the resources and initializing the engine.
class Engine {
  public:
    /// @brief Constructor
    /// @details Constructs the engine.
    /// \param iniFile The ini file to use.
    /// \param systemIo The system io to use.
    Engine(LibAbyss::INIFile iniFile, std::unique_ptr<SystemIO> systemIo);

    ~Engine();

    /// @brief Runs the engine
    void Run();

    /// @brief Stops the engine
    void Stop();

    /// @breif Adds the palette to the engine
    /// \param paletteName The name of the palette.
    /// \param palette The palette to add.
    void AddPalette(std::string_view paletteName, const LibAbyss::Palette &palette);

    /// Returns the root node
    /// \return The root node
    Node &GetRootNode();

    /// Gets the currently focused node (for UI)
    /// \return The currently focused node
    Node *GetFocusedNode();

    /// Sets the focused node
    void SetFocusedNode(Node *node);

    /// Sets the cursor sprite
    /// \param cursorSprite The cursor sprite
    /// \param offsetX The cursor offset X
    /// \param offsetY The cursor offset Y
    void SetCursorSprite(Sprite *cursorSprite, int offsetX, int offsetY);

    /// Shows the system cursor
    /// \param show True to show the cursor, false to hide it
    void ShowSystemCursor(bool show);

    /// Gets the current cursor position
    void GetCursorPosition(int &x, int &y);

    /// Gets the mouse button state
    /// \return The mouse button state
    eMouseButton GetMouseButtonState();

    /// Resets the mouse button state
    void ResetMouseButtonState();

    /// Plays a video
    /// \param name Internal name, will be assigned to the node
    /// \param stream The stream to play
    /// \param audio The audiostream to play, if it's separate from video
    /// \param wait If true, the engine will wait for the video to finish before returning
    void PlayVideo(std::string_view name, LibAbyss::InputStream stream, std::optional<LibAbyss::InputStream> audio,
                   const sol::safe_function &callback);

    /// Returns the resource loader
    /// \return s The resource loader
    Loader &GetLoader();

    /// Returns the INI configuration file
    /// \return s The INI configuration file
    LibAbyss::INIFile &GetIniFile();

    /// Returns the IO subsystem
    /// \return s The IO subsystem
    SystemIO &GetSystemIO();

    /// Returns a singleton instance of the engine
    static Engine *Get();

    /// Returns the specified palette
    ///  \param paletteName The name of the palette to return
    ///  \return The palette
    [[nodiscard]] const LibAbyss::Palette &GetPalette(std::string_view paletteName);

    /// Returns whether or not the engine is running
    /// \return True if the engine is running, false otherwise
    [[nodiscard]] bool IsRunning() const;

    /// Stops the engine with a panic message
    /// \param message The message to display
    void Panic(std::string_view message);

    std::string ExecuteCommand(std::string command);

  private:
    class EngineLogger : public spdlog::sinks::sink {
      public:
        explicit EngineLogger(std::function<void(std::string)> logCallback) : sink(), _logCallback(std::move(logCallback)) {}
        void log(const spdlog::details::log_msg &msg) override { _logCallback(std::string(msg.payload.data(), msg.payload.size())); }
        void flush() override {}
        void set_pattern(const std::string &pattern) override {}
        void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override {}

      private:
        std::function<void(std::string)> _logCallback;
    };

    void RunMainLoop();
    void UpdateVideo(uint32_t tickDiff);
    void UpdateRootNode(uint32_t tickDiff);
    void RenderVideo();
    void RenderRootNode();
    Node &GetInputReceiverNode();
    void ScriptGarbageCollect();

    LibAbyss::INIFile _iniFile;
    Loader _loader;
    std::shared_ptr<EngineLogger> _logger;
    std::unique_ptr<AbyssEngine::SystemIO> _systemIO;
    absl::node_hash_map<std::string, LibAbyss::Palette> _palettes;
    std::unique_ptr<ScriptHost> _scriptHost;
    Node _rootNode;
    Node *_focusedNode = nullptr;
    std::optional<DebugConsole> _debugConsoleNode;
    bool _running = true;
    uint32_t _lastTicks = 0;
    std::unique_ptr<Video> _videoNode;
    Sprite *_cursorSprite = nullptr;
    bool _showSystemCursor = false;
    int _cursorX = 0;
    int _cursorY = 0;
    uint32_t _tickDiff = 0;
    eMouseButton _mouseButtonState;
    int _cursorOffsetX = 0;
    int _cursorOffsetY = 0;
    uint32_t _luaGcRateMsec = 1024;
    uint32_t _luaLastGc = 0;
    unsigned int _maxFPS = 0;
    unsigned int _ticksPerFrame = 0;
    sol::safe_function _onVideoEndCallback;
    bool UpdateTicks();
};

} // namespace AbyssEngine

#endif // ABYSS_ENGINE_H
