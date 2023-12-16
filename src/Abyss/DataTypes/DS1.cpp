#include "DS1.h"

#include "Abyss/AbyssEngine.h"
#include "Abyss/Streams/StreamReader.h"

#include <stdexcept>

namespace Abyss::DataTypes {

constexpr uint32_t prop1Bitmask = 0x000000FF;
constexpr uint32_t prop1Offset = 0;
constexpr uint32_t prop1Length = 8;

constexpr uint32_t sequenceBitmask = 0x00003F00;
constexpr uint32_t sequenceOffset = 8;
constexpr uint32_t sequenceLength = 6;

constexpr uint32_t unknown1Bitmask = 0x000FC000;
constexpr uint32_t unknown1Offset = 14;
constexpr uint32_t unknown1Length = 6;

constexpr uint32_t styleBitmask = 0x03F00000;
constexpr uint32_t styleOffset = 20;
constexpr uint32_t styleLength = 6;

constexpr uint32_t unknown2Bitmask = 0x7C000000;
constexpr uint32_t unknown2Offset = 26;
constexpr uint32_t unknown2Length = 5;

constexpr uint32_t hiddenBitmask = 0x80000000;
constexpr uint32_t hiddenOffset = 31;
constexpr uint32_t hiddenLength = 1;

void Tile::decode(const uint8_t prop1, const uint8_t prop2, const uint8_t prop3, const uint8_t prop4) {
    mainIndex = (prop3 >> 4) + ((prop4 & 0x03) << 4);
    subIndex = prop2;
    this->prop1 = prop1;
    this->prop2 = prop2;
    this->prop3 = prop3;
    this->prop4 = prop4;

}

void DS1::loadLayerStreams(Streams::StreamReader &sr) {
    static const std::vector dirLookup = {0x00, 0x01, 0x02, 0x01, 0x02, 0x03, 0x03, 0x05, 0x05, 0x06,
                                          0x06, 0x07, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
                                          0x0F, 0x10, 0x11, 0x12, 0x14};

    for (const auto layerStreamTypes = getLayerStreamTypes(); const auto &layerStreamType : layerStreamTypes) {
        for (auto y = 0; y < height; y++) {
            for (auto x = 0; x < width; x++) {
                const auto prop1 = sr.readUInt8();
                const auto prop2 = sr.readUInt8();
                const auto prop3 = sr.readUInt8();
                const auto prop4 = sr.readUInt8();

                switch (layerStreamType) {
                case LayerStreamType::Wall1:
                case LayerStreamType::Wall2:
                case LayerStreamType::Wall3:
                case LayerStreamType::Wall4: {
                    const auto wallIndex = static_cast<int>(layerStreamType) - static_cast<int>(LayerStreamType::Wall1);
                    layers.wall[wallIndex][x + y * width].decode(prop1, prop2, prop3, prop4);
                }
                break;
                case LayerStreamType::Orientation1:
                case LayerStreamType::Orientation2:
                case LayerStreamType::Orientation3:
                case LayerStreamType::Orientation4: {
                    constexpr uint32_t wallTypeBitmask = 0x000000FF;
                    const auto wallIndex = static_cast<int>(layerStreamType) - static_cast<int>(LayerStreamType::Orientation1);
                    auto c = prop1;
                    if (version < 7 && c < dirLookup.size())
                        c = dirLookup[c];
                    auto &tile = layers.wall[wallIndex][x + y * width];
                    tile.type = static_cast<TileType>(c);
                    // Zero field
                }
                break;
                case LayerStreamType::Floor1:
                case LayerStreamType::Floor2: {
                    const auto floorIndex = static_cast<int>(layerStreamType) - static_cast<int>(LayerStreamType::Floor1);
                    layers.floor[floorIndex][x + y * width].decode(prop1, prop2, prop3, prop4);
                }
                break;
                case LayerStreamType::Shadow:
                    layers.shadow[0][x + y * width].decode(prop1, prop2, prop3, prop4);
                    break;
                case LayerStreamType::Substitution:
                    layers.substitution[0][x + y * width].decode(prop1, prop2, prop3, prop4);
                    break;
                }
            }
        }
    }
}

std::vector<LayerStreamType> DS1::getLayerStreamTypes() const {
    if (version < 4) {
        return {LayerStreamType::Wall1, LayerStreamType::Floor1, LayerStreamType::Orientation1, LayerStreamType::Substitution,
                LayerStreamType::Shadow};
    }

    const auto numWalls = layers.wall.size();
    const auto numOrientations = numWalls;
    const auto numFloors = layers.floor.size();
    const auto numShadows = layers.shadow.size();
    const auto numSubstitutions = layers.substitution.size();
    const auto numLayers = numWalls + numOrientations + numFloors + numShadows + numSubstitutions;

    std::vector<LayerStreamType> layerStreamTypes = {};
    layerStreamTypes.reserve(numLayers);

    for (int i = 0; i < numWalls; ++i) {
        layerStreamTypes.push_back(static_cast<LayerStreamType>(static_cast<int>(LayerStreamType::Wall1) + i));
        layerStreamTypes.push_back(static_cast<LayerStreamType>(static_cast<int>(LayerStreamType::Orientation1) + i));
    }

    for (int i = 0; i < numFloors; ++i)
        layerStreamTypes.push_back(static_cast<LayerStreamType>(static_cast<int>(LayerStreamType::Floor1) + i));

    for (int i = 0; i < numShadows; ++i)
        layerStreamTypes.push_back(static_cast<LayerStreamType>(static_cast<int>(LayerStreamType::Shadow) + i));

    if (numSubstitutions > 0)
        layerStreamTypes.push_back(LayerStreamType::Substitution);

    return layerStreamTypes;
}

DS1::DS1(const std::string_view path) {
    if (const auto lastSeparator = std::max(path.find_last_of('/'), path.find_last_of('\\')); lastSeparator != std::string_view::npos) {
        name = std::string(path.substr(lastSeparator + 1));
    } else {
        name = std::string(path);
    }

    auto file = AbyssEngine::getInstance().loadFile(path);
    Streams::StreamReader sr(file);

    version = sr.readInt32();
    if (version < 3) {
        throw std::runtime_error("DS1 version is too old: " + std::to_string(version));
    }

    width = sr.readInt32() + 1;
    height = sr.readInt32() + 1;
    act = version >= 8 ? sr.readInt32() : 0;
    substitutionType = version >= 10 ? sr.readInt32() : 0;

    const auto numberOfFiles = sr.readInt32();

    for (int i = 0; i < numberOfFiles; ++i) {
        files.push_back(sr.readString());
    }

    int numFloors = 1;
    constexpr int numShadows = 1;
    const int numSubstitutions = (substitutionType == 1 || substitutionType == 2) ? 1 : 0;
    int numWalls = 0;

    if (version >= 9 && version <= 13)
        sr.skip(1);

    if (version >= 4)
        numWalls = sr.readInt32();

    if (version >= 16)
        numFloors = sr.readInt32();

    layers.floor.resize(numFloors);
    layers.wall.resize(numWalls);
    layers.shadow.resize(numShadows);
    layers.substitution.resize(numSubstitutions);

    resize(width, height);
    loadLayerStreams(sr);

}

void DS1::resize(const int width, const int height) {
    const auto elements = width * height;
    for (auto &layer : layers.floor)
        layer.resize(elements);
    for (auto &layer : layers.wall)
        layer.resize(elements);
    for (auto &layer : layers.shadow)
        layer.resize(elements);
    for (auto &layer : layers.substitution)
        layer.resize(elements);
}

void DS1::bindTileReferences(const std::vector<DT1> &dt1s) {
    for (auto &layer : layers.floor)
        bindLayerTileReferences(layer, dt1s);

    for (auto &layer : layers.shadow)
        bindLayerTileReferences(layer, dt1s);

    for (auto &layer : layers.wall)
        bindLayerTileReferences(layer, dt1s);

    for (auto &layer : layers.substitution)
        bindLayerTileReferences(layer, dt1s);
}


void DS1::bindLayerTileReferences(std::vector<Tile> &tiles, const std::vector<DT1> &dt1s) {
    for (auto &tile : tiles) {
        if (tile.prop1 == 0)
            continue;
        auto foundTile = false;
        for (const auto &dt1 : dt1s) {
            if (foundTile)
                break;
            for (const auto &tileInfo : dt1.tiles) {
                if (tileInfo.header.orientation != tile.type)
                    continue;
                if (tileInfo.header.mainIndex != tile.mainIndex)
                    continue;
                if (tileInfo.header.subIndex != tile.subIndex)
                    continue;
                tile.dt1Ref = &dt1;
                tile.dt1Index = tileInfo.dt1Index;
                if (tile.type == TileType::RightPartOfNorthCornerWall) {
                    // Get tile with style of LeftPartOfNorthCornerWall
                    for (const auto &tileInfo2 : dt1.tiles) {
                        if (tileInfo2.header.orientation != TileType::LeftPartOfNorthCornerWall)
                            continue;
                        if (tileInfo2.header.mainIndex != tile.mainIndex)
                            continue;
                        if (tileInfo2.header.subIndex != tile.subIndex)
                            continue;
                        tile.dt1IndexAlt = tileInfo2.dt1Index;
                        break;
                    }
                }
                foundTile = true;
                break;
            }
        }
    }
}

} // namespace Abyss::DataTypes
