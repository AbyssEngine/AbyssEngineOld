#include "libabyss/formats/d2/dt1.h"
#include "libabyss/streams/streamreader.h"

LibAbyss::DT1::DT1(LibAbyss::InputStream &stream) {
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
            block.Format = sr.ReadInt16();
            block.Length = sr.ReadInt32();
            stream.seekg(2, std::ios_base::cur);
            block._fileOffset = sr.ReadInt32();
        }

        for (auto &block : tile.Blocks) {
            stream.seekg(tile._blockHeaderPointer + block._fileOffset, std::ios_base::beg);
            block._encodedData.resize(block.Length);
            sr.ReadBytes(block._encodedData);
        }
    }
}

LibAbyss::DT1::Tile::Tile(InputStream &stream) : unknown(), Material(), SubTileFlags() {
    StreamReader sr(stream);

    Direction = sr.ReadInt32();
    RoofHeight = sr.ReadInt16();
    Material = sr.ReadUInt16();
    Height = sr.ReadInt32();
    Width = sr.ReadInt32();
    stream.seekg(4, std::ios_base::cur);
    Type = sr.ReadInt32();
    Style = sr.ReadInt32();
    Sequence = sr.ReadInt32();
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
