#ifndef ABYSS_ZONE_H
#define ABYSS_ZONE_H

#include <libabyss/zone/zoneproviders.h>
#include <functional>
#include <libabyss/common/leveltype.h>
#include <libabyss/formats/d2/dt1.h>
#include <string>
#include <vector>
#include "libabyss/common/tiletype.h"

namespace LibAbyss {
class Zone {
  public:
    explicit Zone(ProvideDT1Handler provideDt1Handler);
    void ResetMap(const LevelType& levelType, int width, int height, uint64_t seed);
    void Stamp(const DS1& ds1, int x, int y);

    std::vector<DT1::Tile> Tiles;

    std::vector<DS1::Object> Objects = {};
    std::vector<DS1::SubstitutionGroup> SubstitutionGroups = {};
    std::vector<std::vector<int>> Floors = {};
    std::vector<std::vector<int>> Walls = {};
    std::vector<std::vector<int>> Shadows = {};
    std::vector<std::vector<int>> Substitutions = {};
    int StartTileX = 0;
    int StartTileY = 0;
    int WidthInTiles = 0;
    int HeightInTiles = 0;
    std::vector<std::string> DT1Files;

  private:
    void AddDT1File(std::string_view fileName);
    int GetTile(int style, int sequence, TileType type);
    ProvideDT1Handler _provideDT1Handler;
    uint64_t _seed = 0;
};
} // namespace LibAbyss

#endif // ABYSS_ZONE_H
