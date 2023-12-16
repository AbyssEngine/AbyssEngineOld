#include "MapEngine.h"

#include "Abyss/AbyssEngine.h"

#include <future>

namespace Abyss::MapEngine {
MapEngine::MapEngine(const int width, const int height, std::vector<DataTypes::DT1> dt1s, std::vector<DataTypes::DS1> ds1s)
    : _width(width), _height(height), _dt1s(std::move(dt1s)), _ds1s(std::move(ds1s)) {

    std::vector<std::future<void>> futures;

    futures.reserve(_ds1s.size());

    for (auto &ds1 : _ds1s)
        futures.emplace_back(std::async(std::launch::async, [&ds1, this]() { ds1.bindTileReferences(_dt1s); }));

    for (auto &future : futures)
        future.wait();

    auto maxFloors = 0;
    auto maxWalls = 0;
    auto maxShadows = 0;
    auto maxSubstitutions = 0;

    for (const auto &ds1 : _ds1s) {
        maxFloors = std::max(maxFloors, static_cast<int>(ds1.layers.floor.size()));
        maxWalls = std::max(maxWalls, static_cast<int>(ds1.layers.wall.size()));
        maxShadows = std::max(maxShadows, static_cast<int>(ds1.layers.shadow.size()));
        maxSubstitutions = std::max(maxSubstitutions, static_cast<int>(ds1.layers.substitution.size()));
    }

    _layers.floor.resize(maxFloors);
    _layers.wall.resize(maxWalls);
    _layers.shadow.resize(maxShadows);
    _layers.substitution.resize(maxSubstitutions);

    const int cellCount = _width * _height;
    for (auto &layer : _layers.floor)
        layer.resize(cellCount);
    for (auto &layer : _layers.wall)
        layer.resize(cellCount);
    for (auto &layer : _layers.shadow)
        layer.resize(cellCount);
    for (auto &layer : _layers.substitution)
        layer.resize(cellCount);
}

void MapEngine::stampDs1(const uint32_t ds1Index, const int originX, const int originY) {
    const auto &ds1 = _ds1s[ds1Index];
    const auto ds1Width = ds1.width;
    const auto ds1Height = ds1.height;

    // Block copy all layers of the specific DS1 to the specified location on the map
    for (auto y = 0; y < ds1Height; ++y) {
        for (auto x = 0; x < ds1Width; ++x) {
            // Floors
            for (auto layerIdx = 0; layerIdx < ds1.layers.floor.size(); ++layerIdx) {
                const auto &tile = ds1.layers.floor[layerIdx][y * ds1Width + x];
                const auto mapX = originX + x;
                const auto mapY = originY + y;
                if (mapX < 0 || mapX >= _width || mapY < 0 || mapY >= _height)
                    continue;
                _layers.floor[layerIdx][mapY * _width + mapX] = tile;
            }

            // Walls
            for (auto layerIdx = 0; layerIdx < ds1.layers.wall.size(); ++layerIdx) {
                const auto &tile = ds1.layers.wall[layerIdx][y * ds1Width + x];
                const auto mapX = originX + x;
                const auto mapY = originY + y;
                if (mapX < 0 || mapX >= _width || mapY < 0 || mapY >= _height)
                    continue;
                _layers.wall[layerIdx][mapY * _width + mapX] = tile;
            }

            // Shadows
            for (auto layerIdx = 0; layerIdx < ds1.layers.shadow.size(); ++layerIdx) {
                const auto &tile = ds1.layers.shadow[layerIdx][y * ds1Width + x];
                const auto mapX = originX + x;
                const auto mapY = originY + y;
                if (mapX < 0 || mapX >= _width || mapY < 0 || mapY >= _height)
                    continue;
                _layers.shadow[layerIdx][mapY * _width + mapX] = tile;
            }

            // Substitutions
            for (auto layerIdx = 0; layerIdx < ds1.layers.substitution.size(); ++layerIdx) {
                const auto &tile = ds1.layers.substitution[layerIdx][y * ds1Width + x];
                const auto mapX = originX + x;
                const auto mapY = originY + y;
                if (mapX < 0 || mapX >= _width || mapY < 0 || mapY >= _height)
                    continue;
                _layers.substitution[layerIdx][mapY * _width + mapX] = tile;
            }
        }
    }

}

void MapEngine::render() const {
    const auto &renderer = AbyssEngine::getInstance().getRenderer();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int tileY = 0; tileY < _height; ++tileY) {
        for (int tileX = 0; tileX < _width; ++tileX) {
            const auto posX = (tileX - tileY) * 80;
            const auto posY = (tileX + tileY) * 40;

            // Draw lower walls
            for (const auto &layer : _layers.wall) {
                if (const auto &tile = layer[tileX + tileY * _width];
                    tile.dt1Ref && tile.type >= DataTypes::TileType::LowerWallsEquivalentToLeftWall)
                    tile.dt1Ref->drawTile(posX - _cameraPosition.x, posY - _cameraPosition.y, tile.dt1Index);
            }

            // Draw floors
            for (const auto &layer : _layers.floor) {
                if (const auto &tile = layer[tileX + tileY * _width]; tile.dt1Ref)
                    tile.dt1Ref->drawTile(posX - _cameraPosition.x, posY - _cameraPosition.y, tile.dt1Index);
            }
            // Draw shadows
            for (const auto &layer : _layers.shadow) {
                if (const auto &tile = layer[tileX + tileY * _width]; tile.dt1Ref)
                    tile.dt1Ref->drawTile(posX - _cameraPosition.x, posY - _cameraPosition.y, tile.dt1Index);
            }
        }
    }

    for (int tileY = 0; tileY < _height; ++tileY) {
        for (int tileX = 0; tileX < _width; ++tileX) {
            const auto posX = (tileX - tileY) * 80;
            const auto posY = (tileX + tileY) * 40;

            // Draw upper
            for (const auto &layer : _layers.wall) {
                if (const auto &tile = layer[tileX + tileY * _width];
                    tile.dt1Ref && ((tile.type >= DataTypes::TileType::LeftWall && tile.type <=
                                     DataTypes::TileType::RightWallWithDoor) || (
                                        tile.type >= DataTypes::TileType::PillarsColumnsAndStandaloneObjects && tile.type <=
                                        DataTypes::TileType::Tree))) {
                    tile.dt1Ref->drawTile(posX - _cameraPosition.x, posY - _cameraPosition.y + 96, tile.dt1Index);

                    // Super special condition. This was fun to figure out :(
                    if (tile.type == DataTypes::TileType::RightPartOfNorthCornerWall)
                        tile.dt1Ref->drawTile(posX - _cameraPosition.x, posY - _cameraPosition.y + 96, tile.dt1IndexAlt);
                }

            }

        }
    }

    for (int tileY = 0; tileY < _height; ++tileY) {
        for (int tileX = 0; tileX < _width; ++tileX) {
            const auto posX = (tileX - tileY) * 80;
            const auto posY = (tileX + tileY) * 40;

            // Draw Roof
            for (const auto &layer : _layers.wall) {
                if (const auto &tile = layer[tileX + tileY * _width];
                    tile.dt1Ref && tile.type == DataTypes::TileType::Roof)
                    tile.dt1Ref->drawTile(posX - _cameraPosition.x, posY - _cameraPosition.y - 72, tile.dt1Index);
            }

        }
    }
}

void MapEngine::setCameraPosition(int x, int y) {
    _cameraPosition.x = x - 320;
    _cameraPosition.y = y - 260;
}

void MapEngine::getCameraPosition(int &x, int &y) const {
    x = _cameraPosition.x + 320;
    y = _cameraPosition.y + 260;
}

void MapEngine::getMapSize(int &width, int &height) const {
    width = _width;
    height = _height;
}


} // namespace Abyss::MapEngine
