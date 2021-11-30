#include "engine.h"
#include "../hostnotify/hostnotify.h"
#include "filesystemprovider.h"
#include <spdlog/spdlog.h>

AbyssEngine::Engine *engineGlobalInstance = nullptr;

AbyssEngine::Engine::Engine(Common::INIFile iniFile, std::unique_ptr<SystemIO> systemIo)
    : _iniFile(std::move(iniFile)), _systemIO(std::move(systemIo)), _loader(), _palettes(), _scriptHost(std::make_unique<ScriptHost>(this)),
      _rootNode() {
    SPDLOG_TRACE("creating engine");
    engineGlobalInstance = this;
    _systemIO->SetFullscreen(_iniFile.GetValueBool("Video", "FullScreen"));
}

void AbyssEngine::Engine::Run() {
    SPDLOG_TRACE("running engine");
    _loader.AddProvider(std::make_unique<FileSystemProvider>(std::filesystem::current_path()));
    std::thread scriptingThread([this] { ScriptingThread(); });
    // ScriptingThread();
    _systemIO->RunMainLoop(_rootNode);
    scriptingThread.join();
}

AbyssEngine::Engine::~Engine() { SPDLOG_TRACE("destroying engine"); }

void AbyssEngine::Engine::ScriptingThread() {
    // SPDLOG_TRACE("Scripting thread started");
    try {
        _scriptHost->ExecuteFile("bootstrap.lua");
    } catch (std::exception &ex) {
        SPDLOG_ERROR("Lua Error:\n{0}", ex.what());
        AbyssEngine::HostNotify::Notify(AbyssEngine::eNotifyType::Fatal, "Script Error", ex.what());
        Stop();
    }

    // SPDLOG_TRACE("Scripting thread finished");
}

void AbyssEngine::Engine::Stop() {
    _systemIO->Stop();
}

void AbyssEngine::Engine::AddPalette(std::string_view paletteName, const LibAbyss::Palette &palette) {
    std::lock_guard<std::mutex> guard(_mutex);
    _palettes.emplace(paletteName, palette);
}

AbyssEngine::Engine *AbyssEngine::Engine::Get() { return engineGlobalInstance; }

const LibAbyss::Palette &AbyssEngine::Engine::GetPalette(std::string_view paletteName) const { return _palettes.at(std::string(paletteName)); }

AbyssEngine::Node &AbyssEngine::Engine::GetRootNode() { return _rootNode; }

AbyssEngine::Node *AbyssEngine::Engine::GetFocusedNode() { return _focusedNode; }

void AbyssEngine::Engine::SetFocusedNode(AbyssEngine::Node *node) { _focusedNode = node; }
