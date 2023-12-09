#pragma once

#include "Abyss/DataTypes/Palette.h"

#include <string>
#include <unordered_map>

namespace OD2::Common {

class PaletteManager {
    std::unordered_map<std::string, Abyss::DataTypes::Palette> _palettes;
    PaletteManager() = default;

  public:
    static PaletteManager &getInstance() {
        static PaletteManager instance;
        return instance;
    }

    Abyss::DataTypes::Palette &getPalette(const std::string_view name) {
        auto it = _palettes.find(std::string(name));
        if (it == _palettes.end())
            throw std::runtime_error("Palette not found");

        return it->second;
    }

    void addPalette(const Abyss::DataTypes::Palette &palette) {
        if (_palettes.find(palette.getName()) != _palettes.end())
            throw std::runtime_error("Palette already exists");

        _palettes.emplace(palette.getName(), palette);
    }

    void removePalette(const std::string_view name) {
        auto it = _palettes.find(std::string(name));
        if (it == _palettes.end())
            throw std::runtime_error("Palette not found");

        _palettes.erase(it);
    }

    void clearPalettes() { _palettes.clear(); }
};

inline Abyss::DataTypes::Palette &GetPalette(const std::string_view name) { return OD2::Common::PaletteManager::getInstance().getPalette(name); }

} // namespace OD2::Common
