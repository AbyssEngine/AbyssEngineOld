#include "label.h"
#include "../engine/engine.h"
#include <absl/strings/str_format.h>
#include <absl/strings/str_replace.h>
#include <absl/strings/str_split.h>

namespace AbyssEngine {

SpriteLabel::SpriteLabel(SpriteFont &font) : _font(font) {}
TtfLabel::TtfLabel(TtfFont &font) : _font(font) {}

Label::~Label() = default;

void Label::RenderCallback(int offsetX, int offsetY) {
    if (!Visible || !Active)
        return;

    int posX = X + offsetX;
    int posY = Y + offsetY;

    int finalWidth;
    int finalHeight;
    PrepareRender(finalWidth, finalHeight);

    switch (_horizontalAlignment) {
    case eAlignment::Middle:
        posX -= (finalWidth / 2);
        break;
    case eAlignment::End:
        posX -= finalWidth;
        break;
    default:
        break;
    }

    switch (_verticalAlignment) {
    case eAlignment::Middle:
        posY -= (finalHeight / 2);
        break;
    case eAlignment::End:
        posY -= finalHeight;
        break;
    default:
        break;
    }

    // Engine::Get()->GetSystemIO().DrawRect(posX, posY, finalWidth, finalHeight, 255, 100, 100);
    DoRender(posX, posY);

    Node::RenderCallback(offsetX, offsetY);
}

void SpriteLabel::PrepareRender(int &width, int &height) { _font.GetMetrics(_caption, width, height); }
void TtfLabel::PrepareRender(int &width, int &height) {
    if (!_texture) {
        std::string markup = absl::StrFormat("<span color='#%02x%02x%02x'>%s</span>", _colorMod.Red, _colorMod.Green, _colorMod.Blue, _caption);
        _texture = _font.RenderText(markup, _rect.Width, _rect.Height, _horizontalAlignment, _maxWidth);
    }
    width = _rect.Width;
    height = _rect.Height;
}

void SpriteLabel::DoRender(int x, int y) { _font.RenderText(x, y, _caption, _colorMod, _horizontalAlignment); }
void TtfLabel::DoRender(int x, int y) {
    Rectangle dst = {};
    dst.X = x;
    dst.Y = y;
    dst.Width = _rect.Width;
    dst.Height = _rect.Height;
    _texture->Render(_rect, dst);
}
void TtfLabel::ClearCache() { _texture = {}; }

void Label::SetCaption(std::string_view value) {
    _caption = value;
    _caption = absl::StrReplaceAll(_caption, {{"\t", "    "}});
    _caption.erase(std::remove(_caption.begin(), _caption.end(), '\r'), _caption.end());
    ClearCache();
}
std::string_view Label::GetCaption() const { return _caption; }
void Label::SetAlignment(eAlignment hAlign, eAlignment vAlign) {
    _horizontalAlignment = hAlign;
    _verticalAlignment = vAlign;
    ClearCache();
}
void Label::SetAlignmentStr(std::string_view hAlign, std::string_view vAlign) { SetAlignment(StringToAlignment(hAlign), StringToAlignment(vAlign)); }
void Label::SetColorMod(uint8_t red, uint8_t green, uint8_t blue) {
    _colorMod.Red = red;
    _colorMod.Green = green;
    _colorMod.Blue = blue;
    ClearCache();
}
void Label::SetMaxWidth(int value) {
    _maxWidth = value;
    ClearCache();
}
int Label::GetMaxWidth() const { return _maxWidth; }

} // namespace AbyssEngine
