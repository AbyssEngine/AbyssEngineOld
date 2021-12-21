#include "debugconsole.h"
#include "../engine/engine.h"
#include "config.h"
#include <absl/strings/str_join.h>

const int CONSOLE_MAX_LINES = 14;
const int CONSOLE_SLIDE_OUT_TICKS = 150;
const int CONSOLE_HEIGHT = 200;

AbyssEngine::DebugConsole::DebugConsole()
    : _consoleFont("/__ABYSS_CONSOLE_FONT", 8, AbyssEngine::ITtf::Hinting::None), _consoleLabel(_consoleFont), _inputLabel(_consoleFont) {
    _consoleLabel.SetCaption("");
    _consoleLabel.SetColorMod(255, 255, 255);
    _consoleLabel.SetVisible(true);
    _consoleLabel.SetActive(true);
    _consoleLabel.SetAlignment(eAlignment::Start, eAlignment::End);
    _consoleLabel.SetPosition(5, CONSOLE_HEIGHT - 16);

    _inputLabel.SetCaption("> ");
    _inputLabel.SetColorMod(0, 0, 0);
    _inputLabel.SetVisible(true);
    _inputLabel.SetActive(true);
    _inputLabel.SetAlignment(eAlignment::Start, eAlignment::End);
    _inputLabel.SetPosition(5, CONSOLE_HEIGHT - 1);

    AppendChild(&_consoleLabel);
    AppendChild(&_inputLabel);
    SetVisible(true);
    SetActive(false);
}

AbyssEngine::DebugConsole::~DebugConsole() = default;

void AbyssEngine::DebugConsole::RenderCallback(int offsetX, int offsetY) {
    auto &io = Engine::Get()->GetSystemIO();
    auto originY = Y + offsetY;
    io.DrawRect(0, originY, 800, CONSOLE_HEIGHT - 15, 0, 0, 0);
    io.DrawRect(0, originY + CONSOLE_HEIGHT - 15, 800, 15, 200, 200, 200);
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
        AddLine("> " + command);
        std::string result = Engine::Get()->ExecuteCommand(command);
        if (!result.empty())
            AddLine(result);
        io.ClearInputText();
        _inputLabel.SetCaption("> ");
        return;
    }

    if (_canClose && !event.Pressed && event.Scancode == 53) {
        // Grave key
        SetActive(false);
        io.ClearInputText();
        _inputLabel.SetCaption("> ");
        _canClose = false;
        _upTicks = 0;
        return;
    }

    if (!_canClose && !io.IsKeyPressed(53)) {
        _canClose = true;
    }

    _inputLabel.SetCaption("> " + io.GetInputText());

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

    Y = -CONSOLE_HEIGHT + (uint64_t)(std::pow(((float)_upTicks / (float)CONSOLE_SLIDE_OUT_TICKS), 2.0f) * (float)CONSOLE_HEIGHT);

    Node::UpdateCallback(ticks);
}
