#ifndef ABYSS_ZONESTAMP_H
#define ABYSS_ZONESTAMP_H

#include <string>
#include <vector>
#include "libabyss/formats/d2/dt1.h"
#include "libabyss/formats/d2/ds1.h"
#include "libabyss/common/leveltype.h"
#include "libabyss/common/levelpreset.h"

namespace LibAbyss {
class ZoneStamp {
  public:
    LevelType LevelType;
    LevelPreset LevelPreset;
    std::vector<DT1::Tile> Tiles;
    DS1 DS1;
};
} // namespace LibAbyss

#endif // ABYSS_ZONESTAMP_H
