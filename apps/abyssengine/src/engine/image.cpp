#include "image.h"
#include <absl/strings/str_cat.h>
#include <stdexcept>

namespace AbyssEngine {

void Image::Render(uint32_t frameIdx, int cellSizeX, int cellSizeY, int posX, int posY) {
    const auto totalFrames = GetFramesPerAnimation();

    if (_framePositions.empty())
        return;

    const auto startX = posX;

    for (auto cellOffsetY = 0; cellOffsetY < cellSizeY; cellOffsetY++) {
        auto lastHeight = 0;

        for (auto cellOffsetX = 0; cellOffsetX < cellSizeX; cellOffsetX++) {
            const auto cellIndex = frameIdx + (cellOffsetX + (cellOffsetY * cellSizeX));
            if (cellIndex >= totalFrames)
                throw std::runtime_error(absl::StrCat("No such frame in image: ", cellIndex, ", it has only ",
                            totalFrames, " frames"));
            const auto& framePos = _framePositions.at((/*currentAnimation*/0 * totalFrames) + cellIndex);

            auto destRect = framePos.Rect;
            destRect.X = framePos.OffsetX + posX;
            destRect.Y = framePos.OffsetY + posY;

            _atlas->SetBlendMode(_blendMode);
            _atlas->Render(framePos.Rect, destRect);

            posX += destRect.Width;
            lastHeight = destRect.Height;
        }

        posX = startX;
        posY += lastHeight;
    }
}

void Image::SetBlendMode(eBlendMode mode) {
    _blendMode = mode;
}
void Image::SetColorMod(uint8_t r, uint8_t g, uint8_t b) {
    _modR = r;
    _modG = g;
    _modB = b;
    _atlas->SetColorMod(r, g, b);
}
std::tuple<uint32_t, uint32_t> Image::LuaGetFrameSize(uint32_t startFrameIdx, int cellSizeX) {
    uint32_t width, height;
    GetFrameSize(startFrameIdx, cellSizeX, width, height);
    return {width, height};
}

} // namespace AbyssEngine
