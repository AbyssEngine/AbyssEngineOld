#include "DT1.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <vector>

#include "Abyss/AbyssEngine.h"

namespace Abyss::DataTypes {

DT1::DT1(const std::string_view path, const Palette &palette) {
    auto reader = AbyssEngine::getInstance().loadFile(path);

    const auto header = reader.readValue<DT1FileHeader>();
    std::vector<DT1TileHeader> tileHeaders(header.numberOfTiles);

    for (auto &tileHeader : tileHeaders)
        tileHeader = reader.readValue<DT1TileHeader>();

    for (auto &tileHeader : tileHeaders) {
        auto &currentTile = tiles.emplace_back();
        currentTile.header = tileHeader;

        std::vector<DT1BlockHeader> blockHeaders(tileHeader.numberOfBlocks);
        reader.seekg(tileHeader.blockHeaderPointer, std::ios_base::beg);
        for (auto &blockHeader : blockHeaders) {
            blockHeader = reader.readValue<DT1BlockHeader>();
        }

        currentTile.width = 160; // Not technically true, but works for us
        currentTile.height = 0;

        if (tileHeader.orientation == DT1TileType::Floor || tileHeader.orientation == DT1TileType::Roof) {
            currentTile.height = 80;
        } else {
            int minCellY = std::numeric_limits<int>::max();
            int maxCellY = std::numeric_limits<int>::min();

            for (const auto &blockHeader : blockHeaders) {
                minCellY = std::min(minCellY, static_cast<int>(blockHeader.posY));
                maxCellY = std::max(maxCellY, blockHeader.posY + 32);
            }

            currentTile.height = maxCellY - minCellY;

            if (minCellY < 0)
                currentTile.drawOffsetY = -minCellY;
        }

        currentTile.texture.reset(SDL_CreateTexture(AbyssEngine::getInstance().getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC,
                                                    currentTile.width, currentTile.height));
        SDL_SetTextureBlendMode(currentTile.texture.get(), SDL_BLENDMODE_BLEND);

        // grab a pointer to the texture data
        std::vector<uint32_t> pixels(currentTile.width * currentTile.height);
        const int pitch = currentTile.width * sizeof(uint32_t);

        for (const auto &blockHeader : blockHeaders) {
            std::vector<uint8_t> encodedData(blockHeader.dataLength);
            reader.seekg(blockHeader.encodedDataFileOffset + tileHeader.blockHeaderPointer, std::ios_base::beg);
            reader.read(reinterpret_cast<char *>(encodedData.data()), encodedData.size());

            if (blockHeader.format == 1) {
                // isometric floor
                constexpr int xJmp[15] = {14, 12, 10, 8, 6, 4, 2, 0, 2, 4, 6, 8, 10, 12, 14};
                constexpr int nmbPixels[15] = {4, 8, 12, 16, 20, 24, 28, 32, 28, 24, 20, 16, 12, 8, 4};

                if (encodedData.size() != 256)
                    throw std::runtime_error("Invalid encoded data size");

                int x;
                int y = 0;
                int n;
                int length = static_cast<int>(encodedData.size());
                int dataOffset = 0;
                while (length > 0) {
                    x = xJmp[y];
                    n = nmbPixels[y];
                    length -= n;
                    while (n--) {
                        const auto targetX = x + blockHeader.posX;
                        const auto targetY = y + currentTile.drawOffsetY + blockHeader.posY;
                        const auto index = encodedData.at(dataOffset++);
                        if (index == 0) {
                            pixels[targetY * currentTile.width + targetX] = 0;
                            x++;
                            continue;
                        }
                        const auto &paletteEntry = palette.getEntry(index);
                        const auto color = (paletteEntry.getRed() << 24) | (paletteEntry.getGreen() << 16) | (paletteEntry.getBlue() << 8) | 0xFF;
                        pixels[targetY * currentTile.width + targetX] = color;
                        x++;
                    }
                    y++;
                }
            } else {
                // RLE decoding
                // 1st byte is pixels to "jump", 2nd is number of "solid" pixels, followed by the pixel color indexes. when 1st and 2nd bytes are 0 and 0, next
                // line.
                int x = 0;
                int y = 0;
                int length = static_cast<int>(encodedData.size());
                int dataOffset = 0;
                while (length > 0) {
                    const auto toSkip = encodedData.at(dataOffset++);
                    auto toDraw = encodedData.at(dataOffset++);
                    length -= 2;
                    if (toSkip == 0 && toDraw == 0) {
                        x = 0;
                        y++;
                        continue;
                    }
                    x += toSkip;
                    length -= toDraw;
                    while (toDraw--) {
                        const auto targetX = x + blockHeader.posX;
                        const auto targetY = y + currentTile.drawOffsetY + blockHeader.posY;
                        const auto index = encodedData.at(dataOffset++);
                        if (index == 0) {
                            pixels[targetY * currentTile.width + targetX] = 0;
                            x++;
                            continue;
                        }
                        const auto &paletteEntry = palette.getEntry(index);
                        const auto color = (paletteEntry.getRed() << 24) | (paletteEntry.getGreen() << 16) | (paletteEntry.getBlue() << 8) | 0xFF;
                        pixels[targetY * currentTile.width + targetX] = color;
                        x++;
                    }
                }
            }
        }

        SDL_UpdateTexture(currentTile.texture.get(), nullptr, pixels.data(), pitch);
    }
}
void DT1::drawTile(const int x, const int y, const int tileIndex) {
    const auto &tile = tiles.at(tileIndex);
    const auto &renderer = AbyssEngine::getInstance().getRenderer();
    const SDL_Rect destRect = {.x = x, .y = y - tile.drawOffsetY, .w = tile.width, .h = tile.height};
    SDL_RenderCopy(renderer, tile.texture.get(), nullptr, &destRect);
}

} // namespace Abyss::DataTypes
