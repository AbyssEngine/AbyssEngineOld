#ifndef ABYSS_ENGINE_H
#define ABYSS_ENGINE_H

#include "../common/inifile.h"
#include "../node/sprite.h"
#include "../scripting/scripthost.h"
#include "../systemio/interface.h"
#include "libabyss/palette.h"
#include "loader.h"
#include <filesystem>
#include <map>
#include <mutex>
#include <thread>

namespace AbyssEngine {

class Engine {
  public:
    Engine(Common::INIFile iniFile, std::unique_ptr<SystemIO> systemIo);
    ~Engine();

    void Run();
    void Stop();
    void ScriptingThread();
    void AddPalette(std::string_view paletteName, const LibAbyss::Palette &palette);
    Node &GetRootNode();
    Node *GetFocusedNode();
    void SetFocusedNode(Node *node);

    Loader &GetLoader() { return _loader; }
    Common::INIFile &GetIniFile() { return _iniFile; }
    SystemIO &GetSystemIO() { return *_systemIO; }

    static Engine *Get();

    [[nodiscard]] const LibAbyss::Palette &GetPalette(std::string_view paletteName) const;

  private:
    Common::INIFile _iniFile;
    Loader _loader;
    std::unique_ptr<AbyssEngine::SystemIO> _systemIO;
    std::mutex _mutex;
    std::map<std::string, LibAbyss::Palette> _palettes;
    std::unique_ptr<ScriptHost> _scriptHost;
    Node _rootNode;
    Node *_focusedNode = nullptr;
};
} // namespace AbyssEngine

#endif // ABYSS_ENGINE_H
