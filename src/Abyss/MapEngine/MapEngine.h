#pragma once

#include "Abyss/DataTypes/DS1.h"
#include "Abyss/DataTypes/DT1.h"

#include <vector>
#include <string>
#include <SDL2/SDL.h>

namespace Abyss::MapEngine {

class MapEngine {
    const int _width{};
    const int _height{};
    std::vector<DataTypes::DT1> _dt1s;
    std::vector<DataTypes::DS1> _ds1s;
    SDL_Point _cameraPosition{-320, -260};

    struct {
        std::vector<DataTypes::TileMap> floor{};
        std::vector<DataTypes::TileMap> wall{};
        std::vector<DataTypes::TileMap> shadow{};
        std::vector<DataTypes::TileMap> substitution{};
    } _layers;

public:
    MapEngine(int width, int height, std::vector<DataTypes::DT1> dt1s, std::vector<DataTypes::DS1> ds1s);
    void stampDs1(uint32_t ds1Index, int originX, int originY);
    void render() const;
    void setCameraPosition(int x, int y);
    void getCameraPosition(int &x, int &y) const;
    void getMapSize(int &width, int &height) const;
};
} // namespace Abyss::MapEngine
