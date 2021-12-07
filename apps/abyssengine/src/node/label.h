#ifndef ABYSS_LABEL_H
#define ABYSS_LABEL_H

#include "../common/alignment.h"
#include "../common/blendmode.h"
#include "../common/color.h"
#include "node.h"
#include "../engine/spritefont.h"
#include <memory>

namespace AbyssEngine {

class Label : public Node {
  public:
    explicit Label(SpriteFont* spriteFont);
    ~Label() override;

    void UpdateCallback(uint32_t ticks) override;
    void RenderCallback(int offsetX, int offsetY) override;
    void MouseEventCallback(const MouseEvent &event) override;
    void SetCaption(std::string_view value);
    std::string_view GetCaption();
    void SetAlignment(eAlignment hAlign, eAlignment vAlign);
    void SetAlignmentStr(std::string_view hAlign, std::string_view vAlign);
    void SetColorMod(uint8_t red, uint8_t green, uint8_t blue);
    [[nodiscard]] std::string_view NodeType() const final { return "Label Node"; };

  private:
    eAlignment _horizontalAlignment = eAlignment::Start;
    eAlignment _verticalAlignment = eAlignment::Start;
    eBlendMode _blendMode = eBlendMode::Blend;
    RGB _colorMod = {255, 255, 255};
    SpriteFont* _spriteFont;
    std::string _caption;
    int _labelOffsetX = 0;
    int _labelOffsetY = 0;
};

} // namespace AbyssEngine

#endif // ABYSS_LABEL_H
