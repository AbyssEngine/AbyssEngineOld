#pragma once

#include "DT1.h"
#include "Abyss/Streams/StreamReader.h"

#include <cstdint>
#include <string>
#include <vector>

namespace Abyss::DataTypes {

enum class LayerStreamType {
    Wall1,
    Wall2,
    Wall3,
    Wall4,
    Orientation1,
    Orientation2,
    Orientation3,
    Orientation4,
    Floor1,
    Floor2,
    Shadow,
    Substitution
};

struct TileShadow {
    bool animated;
};

struct TileFloor {
    bool animated;
};

struct TileWall {
    TileType type;
};

struct TileSubstitution {
    uint32_t sub;
};

class Tile {
public:
    void decode(uint8_t prop1, uint8_t prop2, uint8_t prop3, uint8_t prop4);
    uint8_t prop1{};
    uint8_t prop2{};
    uint8_t prop3{};
    uint8_t prop4{};
    TileType type{};
    uint16_t mainIndex{};
    uint16_t subIndex{};

    union {
        TileShadow shadow;
        TileFloor floor;
        TileWall wall;
        TileSubstitution substitution;
    };

    const DT1 *dt1Ref{nullptr};
    uint32_t dt1Index{};
};

using TileMap = std::vector<Tile>;

class DS1 {
    void loadLayerStreams(Streams::StreamReader &sr);
    static void bindLayerTileReferences(std::vector<Tile> &tiles, const std::vector<DT1> &dt1s);
    [[nodiscard]] std::vector<LayerStreamType> getLayerStreamTypes() const;

public:
    explicit DS1(std::string_view path);
    void resize(int width, int height);

    void bindTileReferences(const std::vector<DT1> &dt1s);

    std::string name{};
    int32_t version{};
    int32_t width{};
    int32_t height{};
    uint32_t act{};
    uint32_t substitutionType{};
    std::vector<std::string> files{};

    struct {
        std::vector<TileMap> floor{};
        std::vector<TileMap> wall{};
        std::vector<TileMap> shadow{};
        std::vector<TileMap> substitution{};
    } layers;
};

} // namespace Abyss::DataTypes
