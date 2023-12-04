#include "PaletteManager.h"

namespace OD2::Common {

PaletteManager::PaletteManager() : palettes() {}

auto PaletteManager::getInstance() -> PaletteManager & {
    static PaletteManager instance;
    return instance;
}

auto PaletteManager::getPalettes() -> std::vector<Abyss::DataTypes::Palette> & { return palettes; }

auto PaletteManager::getPalette(std::string_view name) -> Abyss::DataTypes::Palette & {
    for (auto &palette : palettes) {
        if (palette.getName() == name) {
            return palette;
        }
    }
    throw std::runtime_error("Palette not found");
}

auto PaletteManager::addPalette(const Abyss::DataTypes::Palette &palette) -> void {
    for (auto &existingPalette : palettes) {
        if (existingPalette.getName() == palette.getName()) {
            Abyss::Common::Log::warn("Palette already exists, replacing...");
            existingPalette = palette;
            return;
        }
    }
    palettes.push_back(palette);
}

auto PaletteManager::removePalette(std::string_view name) -> void {
    for (auto it = palettes.begin(); it != palettes.end(); ++it) {
        if (it->getName() == name) {
            palettes.erase(it);
            return;
        }
    }
    throw std::runtime_error("Palette not found");
}

auto PaletteManager::clearPalettes() -> void { palettes.clear(); }

} // namespace OD2::Common
