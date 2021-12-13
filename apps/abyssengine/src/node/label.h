#ifndef ABYSS_LABEL_H
#define ABYSS_LABEL_H

#include "../common/alignment.h"
#include "../common/blendmode.h"
#include "../common/color.h"
#include "node.h"
#include "../engine/spritefont.h"
#include "../engine/ttffont.h"
#include <memory>

namespace AbyssEngine {

class Label : public Node {
  public:
    ~Label() override;

    void SetCaption(std::string_view value);
    std::string_view GetCaption() const;
    void SetAlignment(eAlignment hAlign, eAlignment vAlign);
    void SetAlignmentStr(std::string_view hAlign, std::string_view vAlign);
    void SetColorMod(uint8_t red, uint8_t green, uint8_t blue);
    void LuaSetBlendMode(std::string_view mode);
    std::string_view LuaGetBlendMode() const;

    void SetBold(bool value);
    bool GetBold() const;

    void SetItalic(bool value);
    bool GetItalic() const;

    void SetUnderline(bool value);
    bool GetUnderline() const;

    void SetStrikethrough(bool value);
    bool GetStrikethrough() const;

    [[nodiscard]] std::string_view NodeType() const final { return "Label Node"; };

  protected:
    void RenderCallback(int offsetX, int offsetY) override;
    virtual void PrepareRender(int& width, int& height) = 0;
    virtual void DoRender(int x, int y) = 0;
    virtual void ClearCache() {}

    eAlignment _horizontalAlignment = eAlignment::Start;
    eAlignment _verticalAlignment = eAlignment::Start;
    eBlendMode _blendMode = eBlendMode::Blend;
    RGB _colorMod = {255, 255, 255};
    std::string _caption;
    int _labelOffsetX = 0;
    int _labelOffsetY = 0;
    ITtf::Style _style = (ITtf::Style)0;
};

class SpriteLabel : public Label {
  public:
    explicit SpriteLabel(SpriteFont& font);

  private:
    void PrepareRender(int& width, int& height) override;
    void DoRender(int x, int y) override;

    SpriteFont& _font;
};

class TtfLabel : public Label {
  public:
    explicit TtfLabel(TtfFont& font);

  private:
    void PrepareRender(int& width, int& height) override;
    void DoRender(int x, int y) override;
    void ClearCache() override;

    std::unique_ptr<ITexture> _texture;
    Rectangle _rect = {};
    TtfFont& _font;
};

} // namespace AbyssEngine

#endif // ABYSS_LABEL_H
