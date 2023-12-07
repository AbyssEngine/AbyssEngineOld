module;

#include <string>
#include <unordered_map>

export module OD2.Common.PaletteManager;

import Abyss.DataTypes.Palette;
import Abyss.Common.Logging;

namespace OD2::Common {

export class PaletteManager {
    std::unordered_map<std::string, Abyss::DataTypes::Palette> _palettes;
    PaletteManager() = default;

  public:
    static auto getInstance() -> PaletteManager & {
        static PaletteManager instance;
        return instance;
    }

    auto getPalette(const std::string_view name) -> Abyss::DataTypes::Palette & {
        auto it = _palettes.find(std::string(name));
        if (it == _palettes.end())
            throw std::runtime_error("Palette not found");

        return it->second;
    }

    auto addPalette(const Abyss::DataTypes::Palette &palette) -> void {
        if (_palettes.find(palette.getName()) != _palettes.end())
            throw std::runtime_error("Palette already exists");

        _palettes.emplace(palette.getName(), palette);
    }

    auto removePalette(const std::string_view name) -> void {
        auto it = _palettes.find(std::string(name));
        if (it == _palettes.end())
            throw std::runtime_error("Palette not found");

        _palettes.erase(it);
    }

    auto clearPalettes() -> void { _palettes.clear(); }
};
} // namespace OD2::Common

export auto GetPalette(const std::string_view name) -> Abyss::DataTypes::Palette & { return OD2::Common::PaletteManager::getInstance().getPalette(name); }
