#pragma once

#include "Abyss/DataTypes/Palette.h"

#include <SDL2/SDL.h>
#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>

namespace Abyss::DataTypes {

enum class DT1MaterialFlag : uint16_t {
    Other = 0x0001,
    Water = 0x0002,
    WoodObject = 0x0004,
    InsideStone = 0x0008,
    OutsideStone = 0x0010,
    Dirt = 0x0020,
    Sand = 0x0040,
    Wood = 0x0080,
    Lava = 0x0100,
    Snow = 0x0400,
};

enum class DT1SubtileFlag : uint8_t {
    BlockWalk = 0x01,
    BlockLightBlockLOS = 0x02,
    BlockJump = 0x04,
    BlockPlayerWalkNotMercWalk = 0x08,
    BlockUnknown = 0x10,
    BlockLightOnlyNotLOS = 0x20,
    BlockUnknown2 = 0x40,
    BlockUnknown3 = 0x80,
};

enum class DT1TileType : uint32_t {
    Floor = 0,
    LeftWall = 1,
    UpperWall = 2,
    UpperPartOfUpperLeftCorner = 3,
    LeftPartOfUpperLeftCorner = 4,
    UpperRightCorner = 5,
    LowerLeftCorner = 6,
    LowerRightCorner = 7,
    LeftWallWithdoor = 8,
    UpperWallWithDoor = 9,
    Special1 = 10,
    Special2 = 11,
    Shadow = 13,
    Object = 14,
    Roof = 15,
    LowerWall = 16 // > 15
};

struct DT1FileHeader {
    uint32_t versionMajor{};
    uint32_t versionMinor{};
    uint8_t xUnused[260]{};
    uint32_t numberOfTiles{};
    uint32_t pointerToTileHeaders{};
};

struct DT1TileHeader {
    uint32_t direction{};
    uint16_t roofHeight{};
    uint8_t soundIndex{};
    uint8_t animated{};
    int32_t height{};
    int32_t width{};
    uint8_t zzUnused[4]{};
    DT1TileType orientation{};
    uint32_t mainIndex{};
    uint32_t subIndex{};
    uint32_t rarityOrFrameIndex{};
    uint8_t zzUnknown[4]{};
    DT1SubtileFlag subtileFlags[25]{};
    uint8_t zzZeros[7]{};
    uint32_t blockHeaderPointer{};
    uint32_t blockDataLength{};
    uint32_t numberOfBlocks{};
    uint8_t zzZeros2[12]{};
};

struct DT1BlockHeader {
    int16_t posX;
    int16_t posY;
    int8_t zeros[2];
    uint8_t gridX;
    uint8_t gridY;
    uint16_t format;
    uint32_t dataLength;
    uint8_t zeros2[2];
    uint32_t encodedDataFileOffset;
};

struct DT1Tile {
    DT1TileHeader header{};
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture = {nullptr, SDL_DestroyTexture};
    int drawOffsetY{};
    int width{};
    int height{};
};

class DT1 {
  public:
    std::string name;
    std::vector<DT1Tile> tiles{};
    DT1(std::string_view path, const Palette &palette);
    void drawTile(int x, int y, int tileIndex);
};

} // namespace Abyss::DataTypes
