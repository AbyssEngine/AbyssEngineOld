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
    void ResetMap(LevelType levelType, int width, int height, uint64_t seed);

    std::vector<DT1::Tile> Tiles;
    int StartSubtileX;
    int StartSubtileY;
    int WidthInTiles;
    int HeightInTiles;
    std::vector<std::string> DT1Files;

  private:
    ProvideDT1Handler _provideDT1Handler;
    uint64_t _seed;
};
} // namespace LibAbyss

#endif // ABYSS_ZONE_H
