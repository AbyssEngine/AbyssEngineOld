#include "debugconsole.h"
#include "../engine/engine.h"
#include "config.h"
#include <absl/strings/str_join.h>
//#include <glibmm.h>
#include <glib.h>

namespace AbyssEngine {
namespace {
const int CONSOLE_MAX_LINES = 14;
const int CONSOLE_SLIDE_OUT_TICKS = 150;
const int CONSOLE_HEIGHT = 225;
constexpr std::string_view CONSOLE_SYMBOL = "> ";
} // namespace

DebugConsole::DebugConsole()
    : _consoleFont(std::filesystem::path{"/abyss-embedded/Hack-Regular.ttf"}, 9, /*Cairo::HINT_STYLE_SLIGHT*/ CAIRO_HINT_STYLE_MEDIUM, CAIRO_ANTIALIAS_SUBPIXEL), _consoleLabel(_consoleFont), _inputLabel(_consoleFont) {
    _consoleLabel.SetCaption("");
    _consoleLabel.SetColorMod(0xAA, 0xAA, 0xAA);
    _consoleLabel.SetVisible(true);
    _consoleLabel.SetActive(true);
    _consoleLabel.SetAlignment(eAlignment::Start, eAlignment::End);
    _consoleLabel.SetPosition(5, CONSOLE_HEIGHT - 16);

    _inputLabel.SetCaption(CONSOLE_SYMBOL);
    _inputLabel.SetColorMod(0xAA, 0xAA, 0);
    _inputLabel.SetVisible(true);
    _inputLabel.SetActive(true);
    _inputLabel.SetAlignment(eAlignment::Start, eAlignment::End);
    _inputLabel.SetPosition(5, CONSOLE_HEIGHT - 1);

    AppendChild(&_consoleLabel);
    AppendChild(&_inputLabel);
    SetVisible(true);
    SetActive(false);
}

DebugConsole::~DebugConsole() = default;

void DebugConsole::RenderCallback(int offsetX, int offsetY) {
    auto &io = Engine::Get()->GetSystemIO();
    auto originY = Y + offsetY;
    io.DrawRect(0, originY, 800, CONSOLE_HEIGHT, 0, 0, 0);
    io.DrawLine(0, originY + CONSOLE_HEIGHT, 800, originY + CONSOLE_HEIGHT, 255, 255, 255);
    Node::RenderCallback(offsetX, offsetY);
}

void DebugConsole::AddLine(const std::string &line) {
    while (_lines.size() >= CONSOLE_MAX_LINES)
        _lines.pop_front();

    _lines.push_back(line);
//    _consoleLabel.SetCaption(std::string(Glib::Markup::escape_text(absl::StrJoin(_lines, "\n"))));
    std::string text = absl::StrJoin(_lines, "\n");
    char* result = g_markup_escape_text(text.data(), text.length());
    std::string escaped = result;
    free(result);
    _consoleLabel.SetCaption(escaped);
}
void DebugConsole::KeyboardEventCallback(const KeyboardEvent &event) {
    auto &io = Engine::Get()->GetSystemIO();

    if (event.Pressed && event.Scancode == 40) {
        // The user pressed return
        std::string command = io.GetInputText();
        AddLine(absl::StrCat(CONSOLE_SYMBOL, command));
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

    _inputLabel.SetCaption(absl::StrCat(CONSOLE_SYMBOL, io.GetInputText()));

    Node::KeyboardEventCallback(event);
}

void DebugConsole::UpdateCallback(uint32_t ticks) {
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
} // namespace AbyssEngine
