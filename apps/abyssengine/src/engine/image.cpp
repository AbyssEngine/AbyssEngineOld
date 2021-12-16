#include "image.h"

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
            const auto framePos = _framePositions[(/*currentAnimation*/0 * totalFrames) + cellIndex];

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

} // namespace AbyssEngine
