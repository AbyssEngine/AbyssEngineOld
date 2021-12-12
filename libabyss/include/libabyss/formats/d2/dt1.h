#ifndef ABYSS_DT1_H
#define ABYSS_DT1_H

#include "libabyss/streams/inputstream.h"
#include <cstdint>
#include <vector>
#include <span>

namespace LibAbyss {
class DT1 {
  public:
    struct Tile {
        struct MaterialFlags {
            bool Other;
            bool Water;
            bool WoodObject;
            bool InsideStone;
            bool OutsideStone;
            bool Dirt;
            bool Sand;
            bool Wood;
            bool Lava;
            bool Snow;

            inline MaterialFlags &operator=(const uint16_t &flags) {
                Other = (flags & 0x01) != 0;
                Water = (flags & 0x02) != 0;
                WoodObject = (flags & 0x04) != 0;
                InsideStone = (flags & 0x08) != 0;
                OutsideStone = (flags & 0x10) != 0;
                Dirt = (flags & 0x20) != 0;
                Sand = (flags & 0x40) != 0;
                Wood = (flags & 0x80) != 0;
                Lava = (flags & 0x100) != 0;
                Snow = (flags & 0x200) != 0;
                return *this;
            }
        };

        struct SubTileFlags {
            bool BlockWalk;
            bool BlockLOS;
            bool BlockJump;
            bool BlockPlayerWalk;
            bool Unknown1;
            bool BlockLight;
            bool Unknown2;
            bool Unknown3;

            inline SubTileFlags &operator=(const uint16_t &flags) {
                BlockWalk = (flags & 0x01) != 0;
                BlockLOS = (flags & 0x02) != 0;
                BlockJump = (flags & 0x04) != 0;
                BlockPlayerWalk = (flags & 0x08) != 0;
                Unknown1 = (flags & 0x10) != 0;
                BlockLight = (flags & 0x20) != 0;
                Unknown2 = (flags & 0x40) != 0;
                Unknown3 = (flags & 0x80) != 0;
                return *this;
            }
        };

        struct Block {
            enum class eBlockFormat  { RLE = 0, Isometric = 1 };

            int16_t X;
            int16_t Y;
            uint8_t GridX;
            uint8_t GridY;
            eBlockFormat Format;
            std::vector<uint8_t> EncodedBytes;
            int32_t Length;
            int32_t _fileOffset;
        };

        Tile() = default;
        explicit Tile(InputStream &stream);
        uint8_t unknown[4];
        int32_t Direction;
        int16_t RoofHeight;
        MaterialFlags Material;
        int32_t Height;
        int32_t Width;
        int32_t Type;
        int32_t Style;
        int32_t Sequence;
        int32_t RarityFrameIndex;
        SubTileFlags SubTileFlags[25];
        int32_t _blockHeaderPointer;
        int32_t _blockHeaderSize;
        std::vector<Block> Blocks;
    };

    DT1() = default;
    explicit DT1(InputStream &stream);

    uint32_t VersionMajor = 0;
    uint32_t VersionMinor = 0;
    std::vector<Tile> Tiles;
};
} // namespace LibAbyss

#endif // ABYSS_DT1_H
