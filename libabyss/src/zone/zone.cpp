#include <absl/strings/ascii.h>
#include <libabyss/zone/zone.h>
#include <random>
#include <spdlog/spdlog.h>
#include <utility>

namespace LibAbyss {

Zone::Zone(ProvideDT1Handler provideDT1Handler) : _provideDT1Handler(std::move(provideDT1Handler)) {}

void Zone::ResetMap(const LevelType &levelType, uint32_t dt1Mask, int width, int height, uint64_t seed) {
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
    for (const auto &file : levelType.Files) {
        if (((dt1Mask & 1) != 0) && !file.ends_with("/0"))
            AddDT1File(file);
        dt1Mask >>= 1;
    }
}
void Zone::Stamp(const DS1 &ds1, int x, int y) {

    // Load all of the DS1 files
    for (auto &file : ds1.Files)
        if (!file.ends_with("/0"))
            AddDT1File(file);

    for (int ty = 0; ty < ds1.Height; ty++) {
        for (int tx = 0; tx < ds1.Width; tx++) {
            const int idx = x + tx + ((y + ty) * WidthInTiles);

            // Load the floors
            for (int i = 0; i < ds1.Floors.size(); i++) {
                if (Floors.size() <= i) {
                    Floors.emplace_back();
                    Floors[i].resize(WidthInTiles * HeightInTiles);
                }

                const auto &tile = ds1.Floors[i](tx, ty);

                if (tile.Common.Style == 30) {
                    Floors[i][idx] = -1;
                    continue;
                }

                if (tile.Common.Visible == 0) {
                    Floors[i][idx] = -1;
                    continue;
                }

                Floors[i][idx] = GetTile(tile.Common.Style, tile.Common.Sequence, 0);
            }

            // Load the walls
            for (int i = 0; i < ds1.Walls.size(); i++) {
                if (Walls.size() <= i) {
                    Walls.emplace_back();
                    Walls[i].resize(WidthInTiles * HeightInTiles);
                }

                const auto &tile = ds1.Walls[i](tx, ty);

                if ((TileType)tile.Wall.Type == TileType::SpecialTile1 || (TileType)tile.Wall.Type == TileType::SpecialTile2) {
                    if (tile.Common.Style == 30) {
                        StartTileX = tx;
                        StartTileY = ty;
                    }

                    Walls[i][idx] = -1;
                    continue;
                }

                if (tile.Common.Visible == 0) {
                    Walls[i][idx] = -1;
                    continue;
                }

                Walls[i][idx] = GetTile(tile.Common.Style, tile.Common.Sequence, tile.Wall.Type);
            }

            // Load the shadows
            for (int i = 0; i < ds1.Shadows.size(); i++) {
                if (Shadows.size() <= i) {
                    Shadows.emplace_back();
                    Shadows[i].resize(WidthInTiles * HeightInTiles);
                }

                const auto &tile = ds1.Shadows[i](tx, ty);

                if (tile.Common.Style == 30) {
                    Shadows[i][idx] = -1;
                    continue;
                }

                if (tile.Common.Visible == 0) {
                    Shadows[i][idx] = -1;
                    continue;
                }

                Shadows[i][idx] = GetTile(tile.Common.Style, tile.Common.Sequence, 13);
            }

            // Load the substitution groups
            SubstitutionGroups.insert(SubstitutionGroups.end(), ds1.SubstitutionGroups.begin(), ds1.SubstitutionGroups.end());

            // Load the objects
            Objects.insert(Objects.end(), ds1.Objects.begin(), ds1.Objects.end());
        }
    }
}
void Zone::AddDT1File(std::string_view fileName) {
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

static std::random_device randomDevice;
static std::mt19937 randomGenerator(randomDevice());

int Zone::GetTile(int style, int sequence, uint32_t type) {
    int firstTileIdx = -1;
    std::vector<int> tileIds;

    for (int i = 0; i < Tiles.size(); i++) {
        const auto &tile = Tiles[i];

        if ((tile.MainIndex != style) || (tile.SubIndex != sequence) || (tile.Type != type))
            continue;

        if (tile.Width == 0 || tile.Height == 0)
            continue;

        if (firstTileIdx == -1)
            firstTileIdx = i;

        for (int ri = 0; ri < tile.RarityFrameIndex; ri++)
            tileIds.push_back(i);
    }

    if (tileIds.empty()) {
        if (firstTileIdx >= 0) {
            if ((TileType)Tiles[firstTileIdx].Type == TileType::RightPartOfNorthCornerWall) {
                Tiles[firstTileIdx].AltTile = GetTile(style, sequence, (uint32_t)TileType::LeftPartOfNorthCornerWall);
            }
            Tiles[firstTileIdx].InUse = true;
            return firstTileIdx;
        }

        SPDLOG_WARN("Could not find tile with style {}, sequence {}, and type {}", style, sequence, type);
        return -1;
    }

    auto start = tileIds.begin();
    std::uniform_int_distribution<> dis(0, std::distance(start, tileIds.end()) - 1);
    std::advance(start, dis(randomGenerator));

    auto tileIdx = *start;
    if ((TileType)Tiles[tileIdx].Type == TileType::RightPartOfNorthCornerWall) {
        Tiles[tileIdx].AltTile = GetTile(style, sequence, (uint32_t)TileType::LeftPartOfNorthCornerWall);
    }

    Tiles[tileIdx].InUse = true;
    return tileIdx;
}
std::vector<DT1::Tile> Zone::GetTileInfo(int x, int y) {
    std::vector<DT1::Tile> result;
    const uint32_t idx = x + (y * WidthInTiles);

    for (const auto &floor : Floors)
        if (floor[idx] >= 0)
            result.push_back(Tiles[floor[idx]]);

    for (const auto &wall : Walls)
        if (wall[idx] >= 0)
            result.push_back(Tiles[wall[idx]]);

    return result;
}

} // namespace LibAbyss
