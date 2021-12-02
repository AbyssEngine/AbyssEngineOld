#include "engine.h"
#include "../hostnotify/hostnotify.h"
#include "filesystemprovider.h"
#include <spdlog/spdlog.h>

std::exception_ptr AbyssEngine::globalExceptionPtr = nullptr;
AbyssEngine::Engine *engineGlobalInstance = nullptr;

AbyssEngine::Engine::Engine(LibAbyss::INIFile iniFile, std::unique_ptr<SystemIO> systemIo)
    : _iniFile(std::move(iniFile)), _systemIO(std::move(systemIo)), _loader(), _palettes(), _scriptHost(std::make_unique<ScriptHost>(this)),
      _rootNode(), _videoNode(), _videoMutex(), _mouseButtonState((eMouseButton)0) {
    SPDLOG_TRACE("Creating engine");

    // Set up the global instance
    engineGlobalInstance = this;

    // Set the full screen mode based on the INI file
    _systemIO->SetFullscreen(_iniFile.GetValueBool("Video", "FullScreen"));
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

    _running = false;
    _videoNode = nullptr;
    _videoMutex.try_lock();
    _videoMutex.unlock();
}

void AbyssEngine::Engine::AddPalette(std::string_view paletteName, const LibAbyss::Palette &palette) {
    std::lock_guard<std::mutex> guard(_mutex);

    _palettes.emplace(paletteName, palette);
}

AbyssEngine::Engine *AbyssEngine::Engine::Get() { return engineGlobalInstance; }

const LibAbyss::Palette &AbyssEngine::Engine::GetPalette(std::string_view paletteName) {
    std::lock_guard<std::mutex> guard(_mutex);

    return _palettes.at(std::string(paletteName));
}

AbyssEngine::Node &AbyssEngine::Engine::GetRootNode() {
    std::lock_guard<std::mutex> guard(_mutex);

    return _rootNode;
}

AbyssEngine::Node *AbyssEngine::Engine::GetFocusedNode() {
    std::lock_guard<std::mutex> guard(_mutex);

    return _focusedNode;
}

void AbyssEngine::Engine::SetFocusedNode(AbyssEngine::Node *node) {
    std::lock_guard<std::mutex> guard(_mutex);

    _focusedNode = node;
}

void AbyssEngine::Engine::RunMainLoop() {
    _running = true;

    while (_running) {

        if (!_systemIO->HandleInputEvents(_videoNode != nullptr ? *_videoNode : _rootNode)) {
            Stop();
            break;
        }

        if (globalExceptionPtr) {
            std::rethrow_exception(globalExceptionPtr);
        }

        if (!_running)
            break;

        _systemIO->GetCursorState(_cursorX, _cursorY, _mouseButtonState);

        const auto newTicks = _systemIO->GetTicks();
        const auto tickDiff = newTicks - _lastTicks;

        if (tickDiff == 0)
            continue;

        _lastTicks = newTicks;

        _videoNode != nullptr ? UpdateVideo(tickDiff) : UpdateRootNode(tickDiff);

        {
            std::lock_guard<std::mutex> guard(_mutex);
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
}

void AbyssEngine::Engine::WaitForVideoToFinish() {
    if (!_waitVideoPlayback)
        return;

    while (true) {
        if (!_videoMutex.try_lock()) {
            _systemIO->Delay(50);
            if (!_running)
                break;
        } else {
            break;
        }
    }

    _videoMutex.unlock();
}

void AbyssEngine::Engine::PlayVideo(LibAbyss::InputStream stream, bool wait) {
    std::lock_guard<std::mutex> guard(_mutex);
    if (!_running)
        return;

    if (_videoNode != nullptr) {
        SPDLOG_WARN("Video already playing");
        return;
    }

    _videoMutex.lock();
    _waitVideoPlayback = wait;
    _videoNode = std::make_unique<Video>(std::move(stream));
}

LibAbyss::INIFile &AbyssEngine::Engine::GetIniFile() { return _iniFile; }

AbyssEngine::Loader &AbyssEngine::Engine::GetLoader() { return _loader; }

void AbyssEngine::Engine::UpdateVideo(uint32_t tickDiff) {
    _videoNode->UpdateCallback(tickDiff);

    if (!_videoNode->GetIsPlaying()) {
        _videoNode = nullptr;
        if (_waitVideoPlayback)
            _videoMutex.unlock();

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
