#ifndef ABYSS_DEBUGCONSOLE_H
#define ABYSS_DEBUGCONSOLE_H

#include "node.h"
#include "label.h"
#include <vector>
#include <string>

namespace AbyssEngine {

class DebugConsole : public Node {
  public:
    explicit DebugConsole();
    ~DebugConsole() override;
    void RenderCallback(int offsetX, int offsetY) final;
    void KeyboardEventCallback(const KeyboardEvent &event) final;
    void AddLine(const std::string&);
    void UpdateCallback(uint32_t ticks) final;

  private:
    TtfFont _consoleFont;
    TtfLabel _consoleLabel;
    TtfLabel _inputLabel;
    std::deque<std::string> _lines;
    bool _canClose = false;
    uint64_t _upTicks = 0;
};

}

#endif // ABYSS_DEBUGCONSOLE_H
