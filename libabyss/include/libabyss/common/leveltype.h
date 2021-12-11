#ifndef ABYSS_LEVELTYPE_H
#define ABYSS_LEVELTYPE_H

#include <string>
#include <vector>

namespace LibAbyss {
struct LevelType {
    std::vector<std::string> Files;
    std::string Name;
    int ID;
    int Act;
    bool Beta;
    bool Expansion;
};
} // namespace LibAbyss

#endif // ABYSS_LEVELTYPE_H
