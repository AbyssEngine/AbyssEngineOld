#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Abyss::DataTypes {

enum class LayerStreamType { Wall1, Wall2, Wall3, Wall4, Orientation1, Orientation2, Orientation3, Orientation4, Floor1, Floor2, Shadow, Substitution };

struct Tile {};

using TileMap = std::vector<Tile>;

class DS1 {
  public:
    explicit DS1(std::string_view path);
    void resize(int width, int height);

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
