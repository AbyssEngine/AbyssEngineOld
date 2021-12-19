#include "label.h"
#include <spdlog/spdlog.h>

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

    DoRender(posX, posY);

    Node::RenderCallback(offsetX, offsetY);
}

void SpriteLabel::PrepareRender(int& width, int& height) {
    _font.GetMetrics(_caption, width, height);
}
void TtfLabel::PrepareRender(int& width, int& height) {
    if (!_texture) {
        _font.SetStyle(_style);
        _texture = _font.RenderText(_caption, _rect.Width, _rect.Height);
    }
    width = _rect.Width;
    height = _rect.Height;
}

void SpriteLabel::DoRender(int x, int y) {
    _font.RenderText(x, y, _caption, _blendMode, _colorMod);
}
void TtfLabel::DoRender(int x, int y) {
    _texture->SetBlendMode(_blendMode);
    _texture->SetColorMod(_colorMod.Red, _colorMod.Green, _colorMod.Blue);
    AbyssEngine::Rectangle dst {};
    dst.X = x;
    dst.Y = y;
    dst.Width = _rect.Width;
    dst.Height = _rect.Height;
    _texture->Render(_rect, dst);
}
void TtfLabel::ClearCache() {
    _texture = nullptr;
}

void Label::SetCaption(std::string_view value) {
    _caption = value;
    ClearCache();
}
std::string_view Label::GetCaption() const { return _caption; }
void Label::SetAlignment(eAlignment hAlign, eAlignment vAlign) {
    _horizontalAlignment = hAlign;
    _verticalAlignment = vAlign;
}
void Label::SetAlignmentStr(std::string_view hAlign, std::string_view vAlign) {
    SetAlignment(StringToAlignment(hAlign), StringToAlignment(vAlign));
}
void Label::SetColorMod(uint8_t red, uint8_t green, uint8_t blue) {
    _colorMod.Red = red;
    _colorMod.Green = green;
    _colorMod.Blue = blue;
}
void Label::LuaSetBlendMode(std::string_view mode) {
    _blendMode = StringToBlendMode(mode);
}
std::string_view Label::LuaGetBlendMode() const {
    return BlendModeToString(_blendMode);
}
void Label::SetBold(bool value) {
    if (value) {
        _style += ITtf::Style::Bold;
    } else {
        _style -= ITtf::Style::Bold;
    }
    ClearCache();
}
bool Label::GetBold() const { return _style & ITtf::Style::Bold; }
void Label::SetItalic(bool value) {
    if (value) {
        _style += ITtf::Style::Italic;
    } else {
        _style -= ITtf::Style::Italic;
    }
    ClearCache();
}
bool Label::GetItalic() const { return _style & ITtf::Style::Italic; }
void Label::SetUnderline(bool value) {
    if (value) {
        _style += ITtf::Style::Underline;
    } else {
        _style -= ITtf::Style::Underline;
    }
    ClearCache();
}
bool Label::GetUnderline() const { return _style & ITtf::Style::Underline; }
void Label::SetStrikethrough(bool value) {
    if (value) {
        _style += ITtf::Style::Strikethrough;
    } else {
        _style -= ITtf::Style::Strikethrough;
    }
    ClearCache();
}
bool Label::GetStrikethrough() const { return _style & ITtf::Style::Strikethrough; }

} // namespace AbyssEngine
