#ifndef ABYSS_ENGINE_H
#define ABYSS_ENGINE_H

#include "../node/sprite.h"
#include "../node/video.h"
#include "../scripting/scripthost.h"
#include "../systemio/interface.h"
#include "libabyss/inifile.h"
#include "libabyss/palette.h"
#include "loader.h"
#include <filesystem>
#include <mutex>
#include <thread>
#include <absl/container/node_hash_map.h>

namespace AbyssEngine {

class Engine {
  public:
    Engine(LibAbyss::INIFile iniFile, std::unique_ptr<SystemIO> systemIo);
    ~Engine();

    void Run();
    void Stop();
    void ScriptingThread();
    void AddPalette(std::string_view paletteName, const LibAbyss::Palette &palette);
    Node &GetRootNode();
    Node *GetFocusedNode();
    void SetFocusedNode(Node *node);
    void SetCursorSprite(Sprite *cursorSprite, int offsetX, int offsetY);
    void ShowSystemCursor(bool show);
    void GetCursorPosition(int &x, int &y);
    eMouseButton GetMouseButtonState();
    void ResetMouseButtonState();
    void WaitForVideoToFinish();
    void PlayVideo(LibAbyss::InputStream stream, bool wait);
    Loader &GetLoader() { return _loader; }
    LibAbyss::INIFile &GetIniFile() { return _iniFile; }
    SystemIO &GetSystemIO() { return *_systemIO; }

    static Engine *Get();

    [[nodiscard]] const LibAbyss::Palette &GetPalette(std::string_view paletteName) const;

  private:
    void RunMainLoop();

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
};
} // namespace AbyssEngine

#endif // ABYSS_ENGINE_H
