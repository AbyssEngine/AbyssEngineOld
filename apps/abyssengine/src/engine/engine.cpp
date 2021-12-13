#include "engine.h"
#include "../hostnotify/hostnotify.h"
#include "filesystemprovider.h"
#include <cmath>
#include <spdlog/spdlog.h>

std::exception_ptr AbyssEngine::globalExceptionPtr = nullptr;
AbyssEngine::Engine *engineGlobalInstance = nullptr;

AbyssEngine::Engine::Engine(LibAbyss::INIFile iniFile, std::unique_ptr<SystemIO> systemIo)
    : _iniFile(std::move(iniFile)), _loader(), _systemIO(std::move(systemIo)), _palettes(), _scriptHost(std::make_unique<ScriptHost>(this)),
      _rootNode("__root"), _videoNode(), _mouseButtonState((eMouseButton)0) {
    SPDLOG_TRACE("Creating engine");

    // Set up the global instance
    engineGlobalInstance = this;

    // Set the full screen mode based on the INI file
    _systemIO->SetFullscreen(_iniFile.GetValueBool("Video", "FullScreen"));

    // Bind the ZMQ socket
    // _zmqSocket.bind("inproc://engine");

    _luaGcRateMsec = _iniFile.GetValueInt("System", "LuaGcRateMsecs");

    _systemIO->SetAudioLevel(eAudioIntent::Master, _iniFile.GetValueFloat("Audio", "MasterVolume"));
    _systemIO->SetAudioLevel(eAudioIntent::Video, _iniFile.GetValueFloat("Audio", "VideoVolume"));
    _systemIO->SetAudioLevel(eAudioIntent::SoundEffect, _iniFile.GetValueFloat("Audio", "SoundEffectsVolume"));
    _systemIO->SetAudioLevel(eAudioIntent::BackgroundMusic, _iniFile.GetValueFloat("Audio", "BackgroundMusicVolume"));
}

void AbyssEngine::Engine::Run() {
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

AbyssEngine::Engine::~Engine() { SPDLOG_TRACE("destroying engine"); }

void AbyssEngine::Engine::Stop() {
    _running = false;
    _videoNode = nullptr;
}

void AbyssEngine::Engine::AddPalette(std::string_view paletteName, const LibAbyss::Palette &palette) { _palettes.emplace(paletteName, palette); }

AbyssEngine::Engine *AbyssEngine::Engine::Get() { return engineGlobalInstance; }

const LibAbyss::Palette &AbyssEngine::Engine::GetPalette(std::string_view paletteName) { return _palettes.at(paletteName); }

AbyssEngine::Node &AbyssEngine::Engine::GetRootNode() { return _rootNode; }

AbyssEngine::Node *AbyssEngine::Engine::GetFocusedNode() { return _focusedNode; }

void AbyssEngine::Engine::SetFocusedNode(AbyssEngine::Node *node) { _focusedNode = node; }

void AbyssEngine::Engine::RunMainLoop() {
    _running = true;
    _luaLastGc = _systemIO->GetTicks();

    while (true) {
        if (!_running)
            break;

        if (_systemIO->GetTicks() - _luaLastGc > _luaGcRateMsec) {
            _luaLastGc = _systemIO->GetTicks();
            _scriptHost->GC();
        }

        if (!_systemIO->HandleInputEvents(GetRootNodeOrVideo())) {
            Stop();
            break;
        }

        if (globalExceptionPtr) {
            std::rethrow_exception(globalExceptionPtr);
        }

        if (!_running)
            break;

        const auto newTicks = _systemIO->GetTicks();
        const auto tickDiff = newTicks - _lastTicks;

        if (tickDiff == 0)
            continue;

        _lastTicks = newTicks;

        bool doVideoNode;
        doVideoNode = _videoNode != nullptr;
        doVideoNode ? UpdateVideo(tickDiff) : UpdateRootNode(tickDiff);

        _rootNode.DoInitialize();

        if (_cursorSprite != nullptr)
            _cursorSprite->DoInitialize();

        _systemIO->GetCursorState(_cursorX, _cursorY, _mouseButtonState);

        _systemIO->RenderStart();
        _videoNode != nullptr ? RenderVideo() : RenderRootNode();
        _systemIO->RenderEnd();
    }
}

void AbyssEngine::Engine::SetCursorSprite(Sprite *cursorSprite, int offsetX, int offsetY) {
    _cursorSprite = cursorSprite;
    _cursorOffsetX = offsetX;
    _cursorOffsetY = offsetY;
    _cursorSprite->X = _cursorX;
    _cursorSprite->Y = _cursorY;
}

void AbyssEngine::Engine::ShowSystemCursor(bool show) { _showSystemCursor = show; }

void AbyssEngine::Engine::GetCursorPosition(int &x, int &y) {
    _systemIO->GetCursorState(_cursorX, _cursorY, _mouseButtonState);
    x = _cursorX;
    y = _cursorY;
}

AbyssEngine::eMouseButton AbyssEngine::Engine::GetMouseButtonState() {
    _systemIO->GetCursorState(_cursorX, _cursorY, _mouseButtonState);
    return _mouseButtonState;
}

void AbyssEngine::Engine::ResetMouseButtonState() {
    _mouseButtonState = (eMouseButton)0;
    _systemIO->ResetMouseButtonState();
}

void AbyssEngine::Engine::PlayVideo(std::string_view name, LibAbyss::InputStream stream, std::optional<LibAbyss::InputStream> audio, const sol::safe_function& callback) {
    if (!_running)
        return;

    _videoNode = std::make_unique<Video>(name, std::move(stream), std::move(audio));
    _onVideoEndCallback = callback;
}

LibAbyss::INIFile &AbyssEngine::Engine::GetIniFile() { return _iniFile; }

AbyssEngine::Loader &AbyssEngine::Engine::GetLoader() { return _loader; }

void AbyssEngine::Engine::UpdateVideo(uint32_t tickDiff) {
    _videoNode->UpdateCallback(tickDiff);

    if (!_videoNode->GetIsPlaying()) {
        _videoNode = nullptr;
        if (_onVideoEndCallback.valid()) {
            auto result = _onVideoEndCallback();
            if (!result.valid()) {
                sol::error err = result;
                SPDLOG_ERROR(err.what());
                AbyssEngine::HostNotify::Notify(eNotifyType::Fatal, "Script Error", err.what());
                return;
            }
        }
        return;
    }
}

void AbyssEngine::Engine::UpdateRootNode(uint32_t tickDiff) { _rootNode.UpdateCallback(tickDiff); }

void AbyssEngine::Engine::RenderVideo() { _videoNode->RenderCallback(0, 0); }

void AbyssEngine::Engine::RenderRootNode() {
    _rootNode.RenderCallback(0, 0);

    if (!_showSystemCursor || _cursorSprite == nullptr)
        return;

    _cursorSprite->X = _cursorX;
    _cursorSprite->Y = _cursorY;
    _cursorSprite->RenderCallback(_cursorOffsetX, _cursorOffsetY);
}

AbyssEngine::SystemIO &AbyssEngine::Engine::GetSystemIO() { return *_systemIO; }

bool AbyssEngine::Engine::IsRunning() const { return _running; }

AbyssEngine::Node &AbyssEngine::Engine::GetRootNodeOrVideo() { return _videoNode != nullptr ? *_videoNode : _rootNode; }
