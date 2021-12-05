#ifndef ABYSS_ENGINE_H
#define ABYSS_ENGINE_H

#include "../node/sprite.h"
#include "../node/video.h"
#include "../scripting/scripthost.h"
#include "../systemio/interface.h"
#include "libabyss/inifile.h"
#include "libabyss/palette.h"
#include "loader.h"
#include <absl/container/node_hash_map.h>
#include <exception>
#include <filesystem>
#include <mutex>
#include <thread>
#include <string>

namespace AbyssEngine {

/// @brief Engine class
/// @details This class is the main class of the engine. It is responsible for
///          loading all the resources and initializing the engine.
class Engine {
  public:
    Engine(LibAbyss::INIFile iniFile, std::unique_ptr<SystemIO> systemIo);
    ~Engine();

    /// Starts the engine
    void Run();

    /// Stops the engine
    void Stop();

    /// Returns the engine's script host
    void ScriptingThread();

    /// Adds a new palette to the engine
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

    /// Waits for video playback to complete
    void WaitForVideoToFinish();

    /// Plays a video
    /// \param stream The stream to play
    /// \param wait If true, the engine will wait for the video to finish before returning
    void PlayVideo(std::string_view name, LibAbyss::InputStream stream, bool wait);

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

    std::mutex& GetMutex();

  private:
    void RunMainLoop();
    void UpdateVideo(uint32_t tickDiff);
    void UpdateRootNode(uint32_t tickDiff);

    LibAbyss::INIFile _iniFile;
    Loader _loader;
    std::unique_ptr<AbyssEngine::SystemIO> _systemIO;
    std::mutex _mutex;
    std::mutex _videoMutex;
    absl::node_hash_map<std::string, LibAbyss::Palette> _palettes;
    std::unique_ptr<ScriptHost> _scriptHost;
    Node _rootNode;
    Node *_focusedNode = nullptr;
    bool _running = true;
    uint32_t _lastTicks = 0;
    std::unique_ptr<Video> _videoNode;
    bool _waitVideoPlayback = false;
    Sprite *_cursorSprite = nullptr;
    bool _showSystemCursor = false;
    int _cursorX = 0;
    int _cursorY = 0;
    eMouseButton _mouseButtonState;
    int _cursorOffsetX = 0;
    int _cursorOffsetY = 0;

    void RenderVideo();
    void RenderRootNode();
};

extern std::exception_ptr globalExceptionPtr;

} // namespace AbyssEngine

#endif // ABYSS_ENGINE_H
