#include "engine.h"
#include "../hostnotify/hostnotify.h"
#include "filesystemprovider.h"
#include <cmath>
#include <spdlog/spdlog.h>

std::exception_ptr AbyssEngine::globalExceptionPtr = nullptr;
AbyssEngine::Engine *engineGlobalInstance = nullptr;

AbyssEngine::Engine::Engine(LibAbyss::INIFile iniFile, std::unique_ptr<SystemIO> systemIo)
    : _iniFile(std::move(iniFile)), _systemIO(std::move(systemIo)), _loader(), _palettes(), _scriptHost(std::make_unique<ScriptHost>(this)),
      _rootNode("__root"), _videoNode(), _mouseButtonState((eMouseButton)0), _zmqContex(), _runningMutex(), _videoMutex(),
      _zmqSocket(_zmqContex, zmq::socket_type::router) {
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

    // Start up the scripting thread
    std::thread scriptingThread([this] { ScriptingThread(); });

    // Run the main loop
    RunMainLoop();

    // Stop the engine
    Stop();

    // Wait for the scripting thread to finish
    if (scriptingThread.joinable())
        scriptingThread.join();
}

AbyssEngine::Engine::~Engine() { SPDLOG_TRACE("destroying engine"); }

void AbyssEngine::Engine::ScriptingThread() {
    SPDLOG_TRACE("Scripting thread started");

    try {
        _scriptHost->ExecuteFile("bootstrap.lua");
    } catch (...) {
        globalExceptionPtr = std::current_exception();
    }

    SPDLOG_TRACE("Scripting thread finished");
}

void AbyssEngine::Engine::Stop() {
    std::lock_guard<std::mutex> guard(_mutex);

    {
        std::lock_guard<std::mutex> runningGuard(_runningMutex);
        _running = false;
    }

    {
        std::lock_guard<std::mutex> videoGuard(_videoMutex);
        _videoNode = nullptr;
    }
}

void AbyssEngine::Engine::AddPalette(std::string_view paletteName, const LibAbyss::Palette &palette) {
    std::lock_guard<std::mutex> guard(_mutex);

    _palettes.emplace(paletteName, palette);
}

AbyssEngine::Engine *AbyssEngine::Engine::Get() { return engineGlobalInstance; }

const LibAbyss::Palette &AbyssEngine::Engine::GetPalette(std::string_view paletteName) {
    std::lock_guard<std::mutex> guard(_mutex);

    return _palettes.at(paletteName);
}

AbyssEngine::Node &AbyssEngine::Engine::GetRootNode() { return _rootNode; }

AbyssEngine::Node *AbyssEngine::Engine::GetFocusedNode() {
    std::lock_guard<std::mutex> guard(_mutex);

    return _focusedNode;
}

void AbyssEngine::Engine::SetFocusedNode(AbyssEngine::Node *node) {
    std::lock_guard<std::mutex> guard(_mutex);

    _focusedNode = node;
}

void AbyssEngine::Engine::RunMainLoop() {
    {
        std::lock_guard<std::mutex> guard(_runningMutex);
        _running = true;
    }
    _luaLastGc = _systemIO->GetTicks();

    while (true) {
        {
            std::lock_guard<std::mutex> runningGuard(_runningMutex);
            if (!_running)
                break;
        }

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

        {
            std::lock_guard<std::mutex> runningGuard(_runningMutex);
            if (!_running)
                break;
        }

        const auto newTicks = _systemIO->GetTicks();
        const auto tickDiff = newTicks - _lastTicks;

        if (tickDiff == 0)
            continue;

        _lastTicks = newTicks;

        bool doVideoNode;
        {
            std::lock_guard<std::mutex> videoGuard(_videoMutex);
            doVideoNode = _videoNode != nullptr;
        }
        doVideoNode ? UpdateVideo(tickDiff) : UpdateRootNode(tickDiff);

        _rootNode.DoInitialize();

        {
            std::lock_guard<std::mutex> guard(_mutex);

            if (_cursorSprite != nullptr)
                _cursorSprite->DoInitialize();

            _systemIO->GetCursorState(_cursorX, _cursorY, _mouseButtonState);

            _systemIO->RenderStart();
            _videoNode != nullptr ? RenderVideo() : RenderRootNode();
            _systemIO->RenderEnd();
        }
    }
}

void AbyssEngine::Engine::SetCursorSprite(Sprite *cursorSprite, int offsetX, int offsetY) {
    std::lock_guard<std::mutex> guard(_mutex);

    _cursorSprite = cursorSprite;
    _cursorOffsetX = offsetX;
    _cursorOffsetY = offsetY;
    _cursorSprite->X = _cursorX;
    _cursorSprite->Y = _cursorY;
}

void AbyssEngine::Engine::ShowSystemCursor(bool show) {
    std::lock_guard<std::mutex> guard(_mutex);

    _showSystemCursor = show;
}

void AbyssEngine::Engine::GetCursorPosition(int &x, int &y) {
    std::lock_guard<std::mutex> guard(_mutex);

    _systemIO->GetCursorState(_cursorX, _cursorY, _mouseButtonState);
    x = _cursorX;
    y = _cursorY;
}

AbyssEngine::eMouseButton AbyssEngine::Engine::GetMouseButtonState() {
    std::lock_guard<std::mutex> guard(_mutex);

    _systemIO->GetCursorState(_cursorX, _cursorY, _mouseButtonState);
    return _mouseButtonState;
}

void AbyssEngine::Engine::ResetMouseButtonState() {
    std::lock_guard<std::mutex> guard(_mutex);

    _mouseButtonState = (eMouseButton)0;
    _systemIO->ResetMouseButtonState();
}

void AbyssEngine::Engine::WaitForVideoToFinish() {
    if (!_waitVideoPlayback)
        return;

    while (true) {
        {
            std::lock_guard<std::mutex> guard(_videoMutex);

            if (_videoNode == nullptr || !_videoNode->GetIsPlaying()) {
                break;
            }
        }

        _systemIO->Delay(50);

        {
            std::lock_guard<std::mutex> guard(_runningMutex);
            if (!_running)
                break;
        }
    }
}

void AbyssEngine::Engine::PlayVideo(std::string_view name, LibAbyss::InputStream stream, bool wait) {
    std::lock_guard<std::mutex> guard(_mutex);

    {
        std::lock_guard<std::mutex> runningGuard(_runningMutex);
        if (!_running)
            return;
    }

    {
        std::lock_guard<std::mutex> videoGuard(_videoMutex);

        if (_videoNode != nullptr) {
            SPDLOG_WARN("Video already playing");
            return;
        }
    }

    _waitVideoPlayback = wait;
    {
        std::lock_guard<std::mutex> videoGuard(_videoMutex);
        _videoNode = std::make_unique<Video>(name, std::move(stream));
    }
}

LibAbyss::INIFile &AbyssEngine::Engine::GetIniFile() { return _iniFile; }

AbyssEngine::Loader &AbyssEngine::Engine::GetLoader() { return _loader; }

void AbyssEngine::Engine::UpdateVideo(uint32_t tickDiff) {
    std::lock_guard<std::mutex> videoGuard(_videoMutex);

    _videoNode->UpdateCallback(tickDiff);

    if (!_videoNode->GetIsPlaying()) {
        _videoNode = nullptr;

        return;
    }
}

void AbyssEngine::Engine::UpdateRootNode(uint32_t tickDiff) { _rootNode.UpdateCallback(tickDiff); }

void AbyssEngine::Engine::RenderVideo() {
    std::lock_guard<std::mutex> videoGuard(_videoMutex);
    _videoNode->RenderCallback(0, 0);
}

void AbyssEngine::Engine::RenderRootNode() {
    _rootNode.RenderCallback(0, 0);

    if (!_showSystemCursor || _cursorSprite == nullptr)
        return;

    _cursorSprite->X = _cursorX;
    _cursorSprite->Y = _cursorY;
    _cursorSprite->RenderCallback(_cursorOffsetX, _cursorOffsetY);
}

std::mutex &AbyssEngine::Engine::GetMutex() { return _mutex; }

AbyssEngine::SystemIO &AbyssEngine::Engine::GetSystemIO() { return *_systemIO; }

bool AbyssEngine::Engine::IsRunning() const {
    std::lock_guard<std::mutex> guard(_runningMutex);
    return _running;
}
AbyssEngine::Node &AbyssEngine::Engine::GetRootNodeOrVideo() {
    std::lock_guard<std::mutex> videoGuard(_videoMutex);
    return _videoNode != nullptr ? *_videoNode : _rootNode;
}
