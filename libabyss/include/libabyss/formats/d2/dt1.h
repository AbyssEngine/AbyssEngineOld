#ifndef ABYSS_DT1_H
#define ABYSS_DT1_H

#include "libabyss/streams/inputstream.h"
#include <cstdint>
#include <span>
#include <vector>

namespace LibAbyss {
class DT1 {
  public:
    enum class GeneralTileType { Floor, Special, Shadow, Roof, LowerWall, NormalWall };
    struct Tile {
        struct MaterialFlags {
            bool Other = false;
            bool Water = false;
            bool WoodObject = false;
            bool InsideStone = false;
            bool OutsideStone = false;
            bool Dirt = false;
            bool Sand = false;
            bool Wood = false;
            bool Lava = false;
            bool Snow = false;

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
            bool BlockWalk = false;
            bool BlockLOS = false;
            bool BlockJump = false;
            bool BlockPlayerWalk = false;
            bool Unknown1 = false;
            bool BlockLight = false;
            bool Unknown2 = false;
            bool Unknown3 = false;

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
            enum class eBlockFormat { RLE = 0, Isometric = 1 };

            int16_t X = 0;
            int16_t Y = 0;
            uint8_t GridX = 0;
            uint8_t GridY = 0;
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
        bool Animated;
        MaterialFlags Material;
        int32_t Height;
        int32_t Width;
        uint32_t Type;
        uint32_t MainIndex;
        uint32_t SubIndex;
        int32_t RarityFrameIndex;
        SubTileFlags SubTileFlags[25];
        int32_t _blockHeaderPointer;
        int32_t _blockHeaderSize;
        std::vector<Block> Blocks;
        int32_t YAdjust = 0;
        int AltTile = -1; // Used for the 'other half' tile thing
        bool InUse = false; // Used for tile caching

        [[nodiscard]] GeneralTileType GetTileType() const;

        void CalculateOffsets();
    };

    DT1() = default;
    explicit DT1(InputStream &stream);

    uint32_t VersionMajor = 0;
    uint32_t VersionMinor = 0;
    std::vector<Tile> Tiles;
};
} // namespace LibAbyss

#endif // ABYSS_DT1_H
