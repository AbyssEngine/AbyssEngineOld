#include "label.h"
#include <absl/strings/str_split.h>
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

void SpriteLabel::PrepareRender(int &width, int &height) { _font.GetMetrics(_caption, width, height, _vertSpacing); }
void TtfLabel::PrepareRender(int &width, int &height) {
    if (_textures.empty()) {
        int total_width = 0;
        int total_height = 0;
        _font.SetStyle(_style);
        for (std::string_view s : absl::StrSplit(_caption, '\n')) {
            Rectangle rect = {};
            if (s.empty()) {
                auto texture = _font.RenderText("x", rect.Width, rect.Height);
                _textures.push_back(nullptr);
            } else {
                auto texture = _font.RenderText(s, rect.Width, rect.Height);
                _textures.push_back(std::move(texture));
            }
            total_width = std::max(total_width, rect.Width);
            total_height += rect.Height + _vertSpacing;
            _rects.push_back(std::move(rect));
        }
        _total_height = total_height - _vertSpacing;
        _total_width = total_width;
    }
    width = _total_width;
    height = _total_height;
}

void SpriteLabel::DoRender(int x, int y) { _font.RenderText(x, y, _caption, _blendMode, _colorMod, _horizontalAlignment, _vertSpacing); }
void TtfLabel::DoRender(int x, int y) {
    for (int i = 0; i < (int)_textures.size(); ++i) {
        const Rectangle &rect = _rects[i];
        if (_textures[i] == nullptr) {
            y += rect.Height + _vertSpacing;
            continue;
        }
        const auto &texture = _textures[i];
        texture->SetBlendMode(_blendMode);
        texture->SetColorMod(_colorMod.Red, _colorMod.Green, _colorMod.Blue);
        AbyssEngine::Rectangle dst = {};
        dst.Y = y;
        switch (_horizontalAlignment) {
        case eAlignment::Start:
            dst.X = x;
            break;
        case eAlignment::Middle:
            dst.X = x + (_total_width - rect.Width) / 2;
            break;
        case eAlignment::End:
            dst.X = x + _total_width - rect.Width;
            break;
        default:
            break;
        }
        dst.Width = rect.Width;
        dst.Height = rect.Height;
        texture->Render(rect, dst);
        y += rect.Height + _vertSpacing;
    }
}
void TtfLabel::ClearCache() {
    _textures.clear();
    _rects.clear();
}

void Label::SetCaption(std::string_view value) {
    std::string str(value);
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    _caption = value;
    ClearCache();
}
std::string_view Label::GetCaption() const { return _caption; }
void Label::SetAlignment(eAlignment hAlign, eAlignment vAlign) {
    _horizontalAlignment = hAlign;
    _verticalAlignment = vAlign;
}
void Label::SetAlignmentStr(std::string_view hAlign, std::string_view vAlign) { SetAlignment(StringToAlignment(hAlign), StringToAlignment(vAlign)); }
void Label::SetColorMod(uint8_t red, uint8_t green, uint8_t blue) {
    _colorMod.Red = red;
    _colorMod.Green = green;
    _colorMod.Blue = blue;
}
void Label::LuaSetBlendMode(std::string_view mode) { _blendMode = StringToBlendMode(mode); }
std::string_view Label::LuaGetBlendMode() const { return BlendModeToString(_blendMode); }
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
void Label::SetVerticalSpacing(int value) {
    _vertSpacing = value;
    ClearCache();
}
int Label::GetVerticalSpacing() const { return _vertSpacing; }

} // namespace AbyssEngine
