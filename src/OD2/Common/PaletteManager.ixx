module;

#include <vector>

import Abyss.DataTypes.Palette;
import Abyss.Common.Logging;

export module OD2.Common.PaletteManager;

namespace OD2::Common {

export class PaletteManager {
    std::vector<Abyss::DataTypes::Palette> palettes;
    PaletteManager() = default;

  public:
    static auto getInstance() -> PaletteManager & {
        static PaletteManager instance;
        return instance;
    }

    auto getPalettes() -> std::vector<Abyss::DataTypes::Palette> & { return palettes; }

    auto getPalette(const std::string_view name) -> Abyss::DataTypes::Palette & {
        for (auto &palette : palettes) {
            if (palette.getName() == name) {
                return palette;
            }
        }
        throw std::runtime_error("Palette not found");
    }

    auto addPalette(const Abyss::DataTypes::Palette &palette) -> void {
        for (auto &existingPalette : palettes) {
            if (existingPalette.getName() == palette.getName()) {
                Abyss::Common::Log::warn("Palette already exists, replacing...");
                existingPalette = palette;
                return;
            }
        }
        palettes.push_back(palette);
    }

    auto removePalette(const std::string_view name) -> void {
        for (auto it = palettes.begin(); it != palettes.end(); ++it) {
            if (it->getName() == name) {
                palettes.erase(it);
                return;
            }
        }
        throw std::runtime_error("Palette not found");
    }

    auto clearPalettes() -> void { palettes.clear(); }
};
} // namespace OD2::Common
