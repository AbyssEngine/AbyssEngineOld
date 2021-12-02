#include "engine.h"
#include "../hostnotify/hostnotify.h"
#include "filesystemprovider.h"
#include <spdlog/spdlog.h>

AbyssEngine::Engine *engineGlobalInstance = nullptr;

AbyssEngine::Engine::Engine(LibAbyss::INIFile iniFile, std::unique_ptr<SystemIO> systemIo)
    : _iniFile(std::move(iniFile)), _systemIO(std::move(systemIo)), _loader(), _palettes(), _scriptHost(std::make_unique<ScriptHost>(this)),
      _rootNode(), _videoNode(), _videoMutex() {
    SPDLOG_TRACE("creating engine");
    engineGlobalInstance = this;
    _systemIO->SetFullscreen(_iniFile.GetValueBool("Video", "FullScreen"));
}

void AbyssEngine::Engine::Run() {
    SPDLOG_TRACE("running engine");
    _loader.AddProvider(std::make_unique<FileSystemProvider>(std::filesystem::current_path()));
    std::thread scriptingThread([this] { ScriptingThread(); });
    RunMainLoop();
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
    _running = false;
    if (_videoNode != nullptr)
        _videoNode->StopVideo();
    _videoMutex.unlock();
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

void AbyssEngine::Engine::RunMainLoop() {
    _running = true;

    while (_running) {

        if (!_systemIO->HandleInputEvents(_videoNode != nullptr ? *_videoNode : _rootNode)) {
            Stop();
            break;
        }

        if (!_running)
            break;

        _systemIO->GetCursorState(_cursorX, _cursorY, _mouseButtonState);

        const auto newTicks = _systemIO->GetTicks();
        const auto tickDiff = newTicks - _lastTicks;

        if (tickDiff == 0)
            continue;

        _lastTicks = newTicks;

        {
            std::lock_guard<std::mutex> guard(_mutex);

            _systemIO->RenderStart();

            if (_videoNode != nullptr) {
                _videoNode->UpdateCallback(tickDiff);
                if (!_videoNode->GetIsPlaying()) {
                    _videoNode = nullptr;
                    if (_waitVideoPlayback)
                        _videoMutex.unlock();

                    continue;
                }
                _videoNode->RenderCallback(0, 0);
            } else {
                _rootNode.UpdateCallback(tickDiff);
                _rootNode.RenderCallback(0, 0);

                if (_showSystemCursor && _cursorSprite != nullptr) {
                    _cursorSprite->X = _cursorX;
                    _cursorSprite->Y = _cursorY;
                    _cursorSprite->RenderCallback(_cursorOffsetX, _cursorOffsetY);
                }
            }

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
    _systemIO->GetCursorState(_cursorX, _cursorY, _mouseButtonState);
    x = _cursorX;
    y = _cursorY;
}

AbyssEngine::eMouseButton AbyssEngine::Engine::GetMouseButtonState() {
    _systemIO->GetCursorState(_cursorX, _cursorY, _mouseButtonState);
    return _mouseButtonState;
}

void AbyssEngine::Engine::ResetMouseButtonState() { _mouseButtonState = (eMouseButton)0; }

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
    if (!_running)
        return;

    _videoMutex.lock();
    _waitVideoPlayback = wait;

    _videoNode = std::make_unique<Video>(std::move(stream));
}
