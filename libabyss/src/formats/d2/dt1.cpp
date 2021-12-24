#include "libabyss/formats/d2/dt1.h"
#include "libabyss/streams/streamreader.h"

namespace LibAbyss {

DT1::DT1(InputStream &stream) {
    StreamReader sr(stream);

    VersionMajor = sr.ReadInt32();
    VersionMinor = sr.ReadInt32();

    if (VersionMajor != 7 || VersionMinor != 6)
        throw std::runtime_error("DT1 version not supported");

    stream.seekg(260, std::ios_base::cur);

    auto numberOfTiles = sr.ReadInt32();
    auto bodyPosition = sr.ReadInt32();

    stream.seekg(bodyPosition, std::ios_base::beg);

    for (int i = 0; i < numberOfTiles; i++)
        Tiles.emplace_back(stream);

    for (auto &tile : Tiles) {
        stream.seekg(tile._blockHeaderPointer, std::ios_base::beg);
        for (auto &block : tile.Blocks) {
            block.X = sr.ReadInt16();
            block.Y = sr.ReadInt16();
            stream.seekg(2, std::ios_base::cur);
            block.GridX = sr.ReadByte();
            block.GridY = sr.ReadByte();
            block.Format = sr.ReadInt16() == 1 ? Tile::Block::eBlockFormat::Isometric : Tile::Block::eBlockFormat::RLE;
            block.Length = sr.ReadInt32();
            stream.seekg(2, std::ios_base::cur);
            block._fileOffset = sr.ReadInt32();
        }

        for (auto &block : tile.Blocks) {
            stream.seekg(tile._blockHeaderPointer + block._fileOffset, std::ios_base::beg);

            block.EncodedBytes.resize(block.Length);
            sr.ReadBytes(block.EncodedBytes);
        }

        tile.CalculateOffsets();
    }
}

DT1::Tile::Tile(InputStream &stream) : unknown(), Material(), SubTileFlags() {
    StreamReader sr(stream);

    Direction = sr.ReadInt32();
    RoofHeight = sr.ReadInt16();
    Material = sr.ReadUInt8();
    Animated = sr.ReadUInt8() == 1;
    Height = sr.ReadInt32();
    Width = sr.ReadInt32();
    stream.seekg(4, std::ios_base::cur);
    Type = sr.ReadUInt32();
    MainIndex = sr.ReadUInt32();
    SubIndex = sr.ReadUInt32();
    RarityFrameIndex = sr.ReadInt32();
    stream.seekg(4, std::ios_base::cur);
    for (auto &subTileFlag : SubTileFlags)
        subTileFlag = sr.ReadByte();
    stream.seekg(7, std::ios_base::cur);
    _blockHeaderPointer = sr.ReadInt32();
    _blockHeaderSize = sr.ReadInt32();
    auto numBlocks = sr.ReadInt32();
    Blocks.resize(numBlocks);
    stream.seekg(12, std::ios_base::cur);
}
void DT1::Tile::CalculateOffsets() {
    YAdjust = 0;

    if (Blocks.empty() || Width == 0 || Height == 0)
        return;

    int32_t minY = INT32_MAX;
    int32_t maxY = INT32_MIN;

    for (const auto &block : Blocks) {
        if (block.Y < minY)
            minY = block.Y;

        if (block.Y + 32 > maxY)
            maxY = block.Y + 32;
    }

    switch (GetTileType()) {
    case GeneralTileType::Floor:
        YAdjust = 0;
        break;
    case GeneralTileType::Roof:
        YAdjust = -RoofHeight;
        break;
    case GeneralTileType::LowerWall:
        YAdjust = -96 + 80;
        break;
    case GeneralTileType::Shadow:
    case GeneralTileType::Special:
    case GeneralTileType::NormalWall:
        YAdjust = Height + 80;
        break;
    default:
        YAdjust = 0;
    }
}
DT1::GeneralTileType DT1::Tile::GetTileType() const {
    switch (Type) {
    case 0:
        return GeneralTileType::Floor;
    case 10:
    case 11:
        return GeneralTileType::Special;
    case 13:
        return GeneralTileType::Shadow;
    case 15:
        return GeneralTileType::Roof;
    }

    return (Type < 15) ? GeneralTileType::NormalWall : GeneralTileType::LowerWall;
}

} // namespace LibAbyss
