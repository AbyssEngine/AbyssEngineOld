#pragma once

#include <Abyss/DataTypes/Palette.h>
#include <vector>

namespace OD2::Common {

class PaletteManager {
    std::vector<Abyss::DataTypes::Palette> palettes;
    PaletteManager();

  public:
    static auto getInstance() -> PaletteManager &;
    auto operator[](const std::string_view name) -> Abyss::DataTypes::Palette & { return getPalette(name); }
    auto getPalettes() -> std::vector<Abyss::DataTypes::Palette> &;
    auto getPalette(std::string_view name) -> Abyss::DataTypes::Palette &;
    auto addPalette(const Abyss::DataTypes::Palette &palette) -> void;
    auto removePalette(std::string_view name) -> void;
    auto clearPalettes() -> void;
};
} // namespace OD2::Common
