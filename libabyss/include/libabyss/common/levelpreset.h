#ifndef ABYSS_LEVELPRESET_H
#define ABYSS_LEVELPRESET_H

#include <string>
#include <vector>

namespace LibAbyss {
struct LevelPreset {
    std::vector<std::string> Files;
    std::string Name;
    int DefinitionID;
    int LevelID;
    int SizeX;
    int SizeY;
    int Pops;
    int PopPad;
    uint32_t DT1Mask;
    bool Populate;
    bool Logicals;
    bool Outdoors;
    bool Animate;
    bool KillEdge;
    bool FillBlanks;
    bool AutoMap;
    bool Scan;
    bool Beta;
    bool Expansion;
};
} // namespace LibAbyss

#endif // ABYSS_LEVELPRESET_H
