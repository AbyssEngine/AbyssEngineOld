#ifndef ABYSS_ZONE_H
#define ABYSS_ZONE_H

#include <libabyss/zone/zoneproviders.h>
#include <functional>
#include <libabyss/common/leveltype.h>
#include <libabyss/formats/d2/dt1.h>
#include <string>
#include <vector>

namespace LibAbyss {
class Zone {
  public:
    explicit Zone(ProvideDT1Handler provideDt1Handler);
    void ResetMap(const LevelType& levelType, int width, int height, uint64_t seed);
    void Stamp(const DS1& ds1, int x, int y);

    std::vector<DT1::Tile> Tiles;

    std::vector<DS1::Object> Objects = {};
    std::vector<DS1::SubstitutionGroup> SubstitutionGroups = {};
    std::vector<DS1::Layer> Floors = {};
    std::vector<DS1::Layer> Walls = {};
    std::vector<DS1::Layer> Shadows = {};
    std::vector<DS1::Layer> Substitutions = {};
    int StartSubTileX = 0;
    int StartSubTileY = 0;
    int WidthInTiles;
    int HeightInTiles;
    std::vector<std::string> DT1Files;

  private:
    void AddDT1File(std::string_view fileName);
    ProvideDT1Handler _provideDT1Handler;
    uint64_t _seed;
};
} // namespace LibAbyss

#endif // ABYSS_ZONE_H
