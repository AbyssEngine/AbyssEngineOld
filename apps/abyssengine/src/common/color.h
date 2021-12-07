#ifndef ABYSS_COLOR_H
#define ABYSS_COLOR_H

#include <cstdint>
namespace AbyssEngine {
struct RGB {
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
};

struct RGBA {
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
    uint8_t Alpha;
};
} // namespace AbyssEngine

#endif // ABYSS_COLOR_H
