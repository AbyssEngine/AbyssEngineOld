#include "dc6sprite.h"
#include "../engine/engine.h"

AbyssEngine::DC6Sprite::DC6Sprite(std::string_view name, LibAbyss::InputStream &stream, const LibAbyss::Palette &palette)
    : Sprite(name), _palette(palette), _dc6(stream) {
    if (_dc6.FramesPerDirection == 0 || _dc6.NumberOfDirections == 0)
        throw std::runtime_error("DC6 has no frames!");
}

void AbyssEngine::DC6Sprite::GetFrameOffset(uint32_t frame, int &offsetX, int &offsetY) {
    const auto &item = _dc6.Directions[_currentAnimation].Frames[frame];
    offsetX = item.OffsetX;
    offsetY = item.OffsetY;
}

void AbyssEngine::DC6Sprite::GetFrameSize(uint32_t frame, uint32_t &width, uint32_t &height) {
    width = 0;
    height = 0;

    for (auto i = 0; i < _cellSizeX; i++) {
        const auto &item = _dc6.Directions[_currentAnimation].Frames[frame + i];
        width += item.Width;
        height += item.Height;
    }
}

uint32_t AbyssEngine::DC6Sprite::GetNumberOfAnimations() { return _dc6.NumberOfDirections; }

uint32_t AbyssEngine::DC6Sprite::GetFramesPerAnimation() { return _dc6.FramesPerDirection; }

void AbyssEngine::DC6Sprite::RegenerateAtlas() {

    uint32_t atlasWidth = 0;
    uint32_t atlasHeight = 0;

    for (const auto &direction : _dc6.Directions) {
        uint32_t directionMaxWidth = 0;
        uint32_t directionMaxHeight = 0;

        for (const auto &frame : direction.Frames) {
            directionMaxWidth += frame.Width;
            directionMaxHeight = (directionMaxHeight < frame.Height) ? frame.Height : directionMaxHeight;
        }

        atlasWidth = (atlasWidth < directionMaxWidth) ? directionMaxWidth : atlasWidth;
        atlasHeight += directionMaxHeight;
    }

    if (atlasWidth == 0 || atlasHeight == 0)
        throw std::runtime_error("DC6 atlas has no calculated size!");

    _atlas = Engine::Get()->GetSystemIO().CreateTexture(ITexture::Format::Static, atlasWidth, atlasHeight);

    _framePositions.clear();
    _framePositions.resize(_dc6.NumberOfDirections * _dc6.FramesPerDirection);

    std::vector<uint32_t> buffer;
    buffer.resize(atlasWidth * atlasHeight);

    int startX = 0;
    int startY = 0;
    uint32_t framePosIndex = 0;

    for (const auto &direction : _dc6.Directions) {
        uint32_t directionMaxHeight = 0;

        for (const auto &frame : direction.Frames) {
            auto &framePosition = _framePositions[framePosIndex++];
            framePosition.Rect = {.X = startX, .Y = startY, .Width = (int)frame.Width, .Height = (int)frame.Height};
            framePosition.OffsetX = frame.OffsetX;
            framePosition.OffsetY = frame.OffsetY;

            for (auto y = 0; y < frame.Height; y++) {
                for (auto x = 0; x < frame.Width; x++) {
                    const auto &color = _palette.BasePalette[frame.IndexData[x + (y * frame.Width)]];
                    buffer[startX + x + ((startY + y) * atlasWidth)] = (uint32_t)color;
                }
            }

            directionMaxHeight = (directionMaxHeight < frame.Height) ? frame.Height : directionMaxHeight;
            startX += (int)frame.Width;
        }

        startX = 0;
        startY += (int)directionMaxHeight;
    }

    _atlas->SetPixels(buffer);
}
