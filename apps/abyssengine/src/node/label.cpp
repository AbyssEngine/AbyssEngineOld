#include "label.h"
#include <spdlog/spdlog.h>
AbyssEngine::Label::Label(SpriteFont *spriteFont) : _spriteFont(spriteFont), _caption() {
    if (spriteFont == nullptr)
        throw std::runtime_error("SpriteFont is null");
}

AbyssEngine::Label::~Label() = default;

void AbyssEngine::Label::UpdateCallback(uint32_t ticks) { Node::UpdateCallback(ticks); }

void AbyssEngine::Label::RenderCallback(int offsetX, int offsetY) {
    if (!Visible || !Active)
        return;

    int posX = X + offsetX;
    int posY = Y + offsetY;

    int finalWidth;
    int finalHeight;
    _spriteFont->GetMetrics(_caption, finalWidth, finalHeight);

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

    _spriteFont->RenderText(posX, posY, _caption, _blendMode, _colorMod);

    Node::RenderCallback(X + offsetX, Y + offsetY);
}
void AbyssEngine::Label::MouseEventCallback(const AbyssEngine::MouseEvent &event) { Node::MouseEventCallback(event); }
void AbyssEngine::Label::SetCaption(std::string_view value) { _caption = value; }
std::string_view AbyssEngine::Label::GetCaption() { return _caption; }
void AbyssEngine::Label::SetAlignment(AbyssEngine::eAlignment hAlign, AbyssEngine::eAlignment vAlign) {
    _horizontalAlignment = hAlign;
    _verticalAlignment = vAlign;
}
void AbyssEngine::Label::SetAlignmentStr(std::string_view hAlign, std::string_view vAlign) {
    SetAlignment(StringToAlignment(hAlign), StringToAlignment(vAlign));
}
void AbyssEngine::Label::SetColorMod(uint8_t red, uint8_t green, uint8_t blue) {
    _colorMod.Red = red;
    _colorMod.Green = green;
    _colorMod.Blue = blue;
}
