#ifndef ABYSS_ZONETILE_H
#define ABYSS_ZONETILE_H

#include <libabyss/formats/d2/ds1.h>
#include <vector>

namespace LibAbyss {
class ZoneTile {
    std::vector<DS1::Tile> Walls;
    std::vector<DS1::Tile> Orientations;
    std::vector<DS1::Tile> Floors;
    std::vector<DS1::Tile> Shadows;
    std::vector<DS1::Tile> Substitutions;
};
} // namespace LibAbyss

#endif // ABYSS_ZONETILE_H
