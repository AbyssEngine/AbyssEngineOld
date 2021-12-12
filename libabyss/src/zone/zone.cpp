#include <libabyss/zone/zone.h>

#include <utility>

LibAbyss::Zone::Zone(LibAbyss::ProvideDT1Handler provideDT1Handler) : _provideDT1Handler(std::move(provideDT1Handler)) {}

void LibAbyss::Zone::ResetMap(LibAbyss::LevelType levelType, int width, int height, uint64_t seed) {
    WidthInTiles = width;
    HeightInTiles = height;

    DT1Files = levelType.Files;
    Tiles.clear();

    for (auto& file : DT1Files) {
        auto dt1 = _provideDT1Handler(file);
        Tiles.insert(Tiles.end(), dt1.Tiles.begin(), dt1.Tiles.end());
    }
}
