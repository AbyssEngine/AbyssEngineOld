#include <absl/strings/ascii.h>
#include <libabyss/zone/zone.h>
#include <utility>
#include <spdlog/spdlog.h>

LibAbyss::Zone::Zone(LibAbyss::ProvideDT1Handler provideDT1Handler) : _provideDT1Handler(std::move(provideDT1Handler)) {}

void LibAbyss::Zone::ResetMap(const LibAbyss::LevelType &levelType, int width, int height, uint64_t seed) {
    WidthInTiles = width;
    HeightInTiles = height;

    Floors.clear();
    Walls.clear();
    Shadows.clear();
    Substitutions.clear();
    SubstitutionGroups.clear();
    Objects.clear();
    Tiles.clear();
    DT1Files.clear();

    // Go through each DT1 file and load the tiles
    for (const auto &file : levelType.Files)
        AddDT1File(file);
}
void LibAbyss::Zone::Stamp(const LibAbyss::DS1 &ds1, int x, int y) {

    // Load all of the DS1 files
    for (auto &file : ds1.Files)
        AddDT1File(file);

    for (int ty = 0; ty < ds1.Height; ty++) {
        for (int tx = 0; tx < ds1.Width; tx++) {

            // Load the floors
            for (int i = 0; i < ds1.Floors.size(); i++) {
                if (Floors.size() <= i) {
                    Floors.emplace_back();
                    Floors[i].resize(WidthInTiles * HeightInTiles);
                }

                auto &tile = ds1.Floors[i](tx, ty);

                Floors[i][(x + tx + ((y + ty) * WidthInTiles))] = GetTile(tile.Common.Style, tile.Common.Sequence, TileType::Floor);
            }

            // Load the walls
            for (int i = 0; i < ds1.Walls.size(); i++) {
                if (Walls.size() <= i) {
                    Walls.emplace_back();
                    Walls[i].resize(WidthInTiles * HeightInTiles);
                }

                auto &tile = ds1.Walls[i](tx, ty);

                if ((TileType)tile.Wall.Type == TileType::SpecialTile1 || (TileType)tile.Wall.Type == TileType::SpecialTile2) {
                    if (tile.Common.Style == 30) {
                        StartTileX = tx;
                        StartTileY = ty;
                    }

                    continue;
                }

                Walls[i][(x + tx + ((y + ty) * WidthInTiles))] = GetTile(tile.Common.Style, tile.Common.Sequence, (TileType)tile.Wall.Type);
            }

            // Load the shadows
            for (int i = 0; i < ds1.Shadows.size(); i++) {
                if (Shadows.size() <= i) {
                    Shadows.emplace_back();
                    Shadows[i].resize(WidthInTiles * HeightInTiles);
                }

                auto &tile = ds1.Shadows[i](tx, ty);

                Shadows[i][(x + tx + ((y + ty) * WidthInTiles))] = GetTile(tile.Common.Style, tile.Common.Sequence, TileType::Shadow);
            }

            // Load the substitution groups
            SubstitutionGroups.insert(SubstitutionGroups.end(), ds1.SubstitutionGroups.begin(), ds1.SubstitutionGroups.end());

            // Load the objects
            Objects.insert(Objects.end(), ds1.Objects.begin(), ds1.Objects.end());
        }
    }
}
void LibAbyss::Zone::AddDT1File(std::string_view fileName) {
    std::string file = absl::AsciiStrToLower(fileName);

    // If DT1File already has file, then do nothing
    if (std::find(DT1Files.begin(), DT1Files.end(), file) != DT1Files.end())
        return;

    // Add the file to the DT1Files
    DT1Files.push_back(file);

    // Load the tiles
    auto dt1 = _provideDT1Handler(fileName);
    Tiles.insert(Tiles.end(), dt1.Tiles.begin(), dt1.Tiles.end());
}

LibAbyss::DT1::Tile *LibAbyss::Zone::GetTile(int style, int sequence, TileType type) {
    for (auto &tile : Tiles) {
        if (tile.Style != style || tile.Sequence != sequence || tile.Type != (int32_t)type)
            continue;

        return &tile;
    }

    SPDLOG_WARN("Could not find tile with style {}, sequence {}, and type {}", style, sequence, (int32_t)type);
    return nullptr;
}
