#include "debugconsole.h"
#include "../engine/engine.h"
#include "config.h"
#include <absl/strings/str_join.h>

const int CONSOLE_MAX_LINES = 13;
const int CONSOLE_SLIDE_OUT_TICKS = 150;
const int CONSOLE_HEIGHT = 225;
const std::string CONSOLE_SYMBOL = ">"; // NOLINT(cert-err58-cpp)

AbyssEngine::DebugConsole::DebugConsole()
    : _consoleFont("/__ABYSS_CONSOLE_FONT", 13, AbyssEngine::ITtf::Hinting::Light), _consoleLabel(_consoleFont), _inputLabel(_consoleFont) {
    _consoleLabel.SetCaption("");
    _consoleLabel.SetColorMod(255, 255, 255);
    _consoleLabel.SetVisible(true);
    _consoleLabel.SetActive(true);
    _consoleLabel.SetAlignment(eAlignment::Start, eAlignment::End);
    _consoleLabel.SetPosition(5, CONSOLE_HEIGHT - 16);

    _inputLabel.SetCaption("> ");
    _inputLabel.SetColorMod(255, 255, 0);
    _inputLabel.SetVisible(true);
    _inputLabel.SetActive(true);
    _inputLabel.SetAlignment(eAlignment::Start, eAlignment::End);
    _inputLabel.SetPosition(5, CONSOLE_HEIGHT - 1);

    AppendChild(&_consoleLabel);
    AppendChild(&_inputLabel);
    SetVisible(true);
    SetActive(false);

    _consoleBackground = Engine::Get()->GetSystemIO().LoadPNG(Engine::Get()->GetLoader().Load("/__ABYSS_CONSOLE_BACKGROUND"));
}

AbyssEngine::DebugConsole::~DebugConsole() = default;

void AbyssEngine::DebugConsole::RenderCallback(int offsetX, int offsetY) {
    auto &io = Engine::Get()->GetSystemIO();
    auto originY = Y + offsetY;
    //io.DrawRect(0, originY, 800, CONSOLE_HEIGHT, 0, 0, 0);
    AbyssEngine::Rectangle srcRect = {0, 0, 800, 225};
    AbyssEngine::Rectangle destRect = {0, originY, 800, 225};
    _consoleBackground->Render(srcRect, destRect);
    io.DrawLine(0, originY + CONSOLE_HEIGHT, 800, originY + CONSOLE_HEIGHT, 255, 255, 255);
    Node::RenderCallback(offsetX, offsetY);
}

void AbyssEngine::DebugConsole::AddLine(const std::string &line) {
    while (_lines.size() >= CONSOLE_MAX_LINES)
        _lines.erase(_lines.begin());

    _lines.push_back(line);
    _consoleLabel.SetCaption(absl::StrJoin(_lines, "\n"));
}
void AbyssEngine::DebugConsole::KeyboardEventCallback(const AbyssEngine::KeyboardEvent &event) {
    auto &io = Engine::Get()->GetSystemIO();

    if (event.Pressed && event.Scancode == 40) {
        // The user pressed return
        std::string command = io.GetInputText();
        AddLine(CONSOLE_SYMBOL + command);
        std::string result = Engine::Get()->ExecuteCommand(command);
        if (!result.empty())
            SPDLOG_INFO("{}", result);
        io.ClearInputText();
        _inputLabel.SetCaption(CONSOLE_SYMBOL);
        return;
    }

    if (_canClose && !event.Pressed && event.Scancode == 53) {
        // Grave key
        SetActive(false);
        io.ClearInputText();
        _inputLabel.SetCaption(CONSOLE_SYMBOL);
        _canClose = false;
        _upTicks = 0;
        return;
    }

    if (!_canClose && !io.IsKeyPressed(53)) {
        _canClose = true;
    }

    _inputLabel.SetCaption(CONSOLE_SYMBOL + io.GetInputText());

    Node::KeyboardEventCallback(event);
}

void AbyssEngine::DebugConsole::UpdateCallback(uint32_t ticks) {
    _upTicks += ticks;
    if (_upTicks >= CONSOLE_SLIDE_OUT_TICKS) {
        _upTicks = CONSOLE_SLIDE_OUT_TICKS;
        Y = 0;
        Node::UpdateCallback(ticks);
        return;
    }

    Y = -CONSOLE_HEIGHT + (int)(std::pow(((float)_upTicks / (float)CONSOLE_SLIDE_OUT_TICKS), 2.0f) * (float)CONSOLE_HEIGHT);

    Node::UpdateCallback(ticks);
}
