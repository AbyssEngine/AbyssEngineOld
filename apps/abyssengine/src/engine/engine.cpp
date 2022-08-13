#include "engine.h"
#include "../hostnotify/hostnotify.h"
#include "filesystemprovider.h"
#include <memory>
#include <span>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace AbyssEngine {

Engine *engineGlobalInstance = nullptr;

Engine::Engine(LibAbyss::INIFile iniFile, std::unique_ptr<SystemIO> systemIo)
    : _iniFile(std::move(iniFile)), _loader(), _systemIO(std::move(systemIo)), _palettes(), _scriptHost(std::make_unique<ScriptHost>(this)),
      _rootNode("__root"), _mouseButtonState((eMouseButton)0) {

    SPDLOG_TRACE("Creating engine");

    // Set up the global instance
    engineGlobalInstance = this;
    _rootNode.SetLuaTable(_scriptHost->CreateTable());

    // Set the full screen mode based on the INI file
    _systemIO->SetFullscreen(_iniFile.GetValueBool("Video", "FullScreen"));

    // Bind the ZMQ socket
    // _zmqSocket.bind("inproc://engine");

    _luaGcRateMsec = _iniFile.GetValueInt("System", "LuaGcRateMsecs");

    _systemIO->SetAudioLevel(eAudioIntent::Master, _iniFile.GetValueFloat("Audio", "MasterVolume"));
    _systemIO->SetAudioLevel(eAudioIntent::Video, _iniFile.GetValueFloat("Audio", "VideoVolume"));
    _systemIO->SetAudioLevel(eAudioIntent::SoundEffect, _iniFile.GetValueFloat("Audio", "SoundEffectsVolume"));
    _systemIO->SetAudioLevel(eAudioIntent::BackgroundMusic, _iniFile.GetValueFloat("Audio", "BackgroundMusicVolume"));

    _maxFPS = _iniFile.GetValueInt("Video", "MaxFPS");
    _ticksPerFrame = _maxFPS > 0 ? 1000 / _maxFPS : 0;
}

void Engine::Run() {
    _loader.AddProvider(std::make_unique<EmbeddedFileProvider>());
    _debugConsoleNode.emplace();

    auto logger = std::shared_ptr<spdlog::logger>(
        new spdlog::logger("Logger", {std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
                                      std::make_shared<EngineLogger>([this](const std::string &line) { _debugConsoleNode->AddLine(line); })}));

    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);

    SPDLOG_TRACE("Running engine");

    // Add a filesystem provider to allow loading of files in the working directory
    _loader.AddProvider(std::make_unique<FileSystemProvider>(std::filesystem::current_path()));

    // Run the script
    _scriptHost->ExecuteFile("bootstrap.lua");

    // Run the main loop
    RunMainLoop();

    // Stop the engine
    Stop();
}

Engine::~Engine() {
    spdlog::set_default_logger(spdlog::stdout_color_mt("Exit"));
    SPDLOG_TRACE("destroying engine");
}

void Engine::Stop() {
    _running = false;
    _videoNode = nullptr;
}

void Engine::AddPalette(std::string_view paletteName, const LibAbyss::Palette &palette) { _palettes.emplace(paletteName, palette); }

Engine *Engine::Get() { return engineGlobalInstance; }

const LibAbyss::Palette &Engine::GetPalette(std::string_view paletteName) { return _palettes.at(paletteName); }

Node &Engine::GetRootNode() { return _rootNode; }

Node *Engine::GetFocusedNode() { return _focusedNode; }

void Engine::SetFocusedNode(Node *node) { _focusedNode = node; }

void Engine::RunMainLoop() {
    // Initialize the running flag so that the loop happens
    _running = true;

    // Set the initial time so that we can calculate the delta time for lua garbage collection
    _luaLastGc = _systemIO->GetTicks();

    unsigned int lastFrameTime = _systemIO->GetTicks();
    unsigned int gcStart = 0;
    unsigned int eventsStart = 0;
    unsigned int updateStart = 0;
    unsigned int renderStart = 0;
    unsigned int loopDone = 0;

    // Run the main loop
    while (_running) {
        // Limit max frame rate
        if (_ticksPerFrame > 0) {
            auto currentTime = _systemIO->GetTicks();
            if (currentTime - lastFrameTime < _ticksPerFrame) {
                _systemIO->Sleep(lastFrameTime + _ticksPerFrame - currentTime);
            }
            lastFrameTime = currentTime;
        }

        _systemIO->GetScreenSize(&_screenWidth, &_screenHeight);

        gcStart = _systemIO->GetTicks();
        ScriptGarbageCollect();

        eventsStart = _systemIO->GetTicks();
        if (!_systemIO->HandleInputEvents(GetInputReceiverNode()))
            Stop();

        updateStart = _systemIO->GetTicks();
        if (!UpdateTicks())
            continue;

        // Process updates
        if (_debugConsoleNode->Active)
            _debugConsoleNode->UpdateCallback(_tickDiff);
        else if (_systemIO->IsKeyPressed(53)) {
            _systemIO->ResetKeyState(53);
            // Grave pressed, activate terminal
            _debugConsoleNode->Active = true;
            _debugConsoleNode->UpdateCallback(_tickDiff);
        }

        (_videoNode != nullptr) ? UpdateVideo(_tickDiff) : UpdateRootNode(_tickDiff);

        // Run node initializations
        _rootNode.DoInitialize();
        if (_cursorSprite != nullptr)
            _cursorSprite->DoInitialize();

        // Update the cursor position
        _systemIO->GetCursorState(_cursorX, _cursorY, _mouseButtonState);

        // Render the video or root node
        renderStart = _systemIO->GetTicks();
        _systemIO->RenderStart();
        _videoNode != nullptr ? RenderVideo() : RenderRootNode();
        if (_debugConsoleNode->Active)
            _debugConsoleNode->RenderCallback(0, 0);
        loopDone = _systemIO->GetTicks();
        _systemIO->RenderEnd();
    }
}

void Engine::ScriptGarbageCollect() {
    if ((_systemIO->GetTicks() - _luaLastGc) <= _luaGcRateMsec)
        return;

    _luaLastGc = _systemIO->GetTicks();
    _scriptHost->GC();
}

void Engine::SetCursorSprite(Sprite *cursorSprite, int offsetX, int offsetY) {
    _cursorSprite = cursorSprite;

    _cursorOffsetX = offsetX;
    _cursorOffsetY = offsetY;

    _cursorSprite->X = _cursorX;
    _cursorSprite->Y = _cursorY;
}

void Engine::ShowSystemCursor(bool show) { _showSystemCursor = show; }

void Engine::GetCursorPosition(int &x, int &y) {
    _systemIO->GetCursorState(_cursorX, _cursorY, _mouseButtonState);

    x = _cursorX;
    y = _cursorY;
}

eMouseButton Engine::GetMouseButtonState() {
    _systemIO->GetCursorState(_cursorX, _cursorY, _mouseButtonState);

    return _mouseButtonState;
}

void Engine::ResetMouseButtonState() {
    _mouseButtonState = (eMouseButton)0;
    _systemIO->ResetMouseButtonState();
}

void Engine::PlayVideo(std::string_view name, LibAbyss::InputStream stream, std::optional<LibAbyss::InputStream> audio,
                       const sol::safe_function &callback) {
    if (!_running)
        return;

    _videoNode = std::make_unique<Video>(name, std::move(stream), std::move(audio));
    _onVideoEndCallback = callback;
}

LibAbyss::INIFile &Engine::GetIniFile() { return _iniFile; }

Loader &Engine::GetLoader() { return _loader; }

void Engine::UpdateVideo(uint32_t tickDiff) {
    _videoNode->UpdateCallback(tickDiff);

    if (_videoNode->GetIsPlaying())
        return;

    _videoNode = nullptr;

    if (!_onVideoEndCallback.valid())
        return;

    auto result = _onVideoEndCallback();

    if (result.valid())
        return;

    sol::error err = result;
    Engine::Get()->Panic(err.what());
}

void Engine::UpdateRootNode(uint32_t tickDiff) { _rootNode.UpdateCallback(tickDiff); }

void Engine::RenderVideo() { _videoNode->RenderCallback(0, 0); }

void Engine::RenderRootNode() {
    _rootNode.RenderCallback(0, 0);

    if (!_showSystemCursor || _cursorSprite == nullptr)
        return;

    _cursorSprite->X = _cursorX;
    _cursorSprite->Y = _cursorY;
    _cursorSprite->RenderCallback(_cursorOffsetX, _cursorOffsetY);
}

SystemIO &Engine::GetSystemIO() { return *_systemIO; }

bool Engine::IsRunning() const { return _running; }

Node &Engine::GetInputReceiverNode() {
    if (_debugConsoleNode->Active)
        return *_debugConsoleNode;

    return _videoNode != nullptr ? *_videoNode : _rootNode;
}

void Engine::Panic(std::string_view message) {
    spdlog::critical(message);
    _debugConsoleNode->Active = true;
    // HostNotify::Notify(eNotifyType::Fatal, "Engine Panic", (std::string)message);
    // Stop();
}

bool Engine::UpdateTicks() {
    const auto newTicks = _systemIO->GetTicks();
    const auto tickDiff = newTicks - _lastTicks;

    if (tickDiff == 0)
        return false;

    _lastTicks = newTicks;
    _tickDiff = tickDiff;

    return true;
}
std::string Engine::ExecuteCommand(const std::string &command) { return _scriptHost->ExecuteString(command); }

void Engine::GetScreenSize(int *width, int *height) { _systemIO->GetScreenSize(width, height); }
void Engine::SetWindowTitle(std::string_view title) { _systemIO->SetWindowTitle(title); }

} // namespace AbyssEngine
