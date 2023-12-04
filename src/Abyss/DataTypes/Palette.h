#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <vector>

namespace Abyss::DataTypes {

class PaletteEntry {
    uint8_t red;
    uint8_t green;
    uint8_t blue;

  public:
    PaletteEntry(uint8_t red, uint8_t green, uint8_t blue) : red(red), green(green), blue(blue) {}
    [[nodiscard]] auto getRed() const -> uint8_t;
    [[nodiscard]] auto getGreen() const -> uint8_t;
    [[nodiscard]] auto getBlue() const -> uint8_t;
    [[nodiscard]] auto getSdlColor() const -> SDL_Color { return {red, green, blue, 255}; }
};

class Palette {
    std::vector<PaletteEntry> _entries;
    std::string _name;

  public:
    Palette(std::string_view path, std::string_view name);
    [[nodiscard]] auto getName() const -> const std::string &;
    auto setName(std::string newName) -> void;
    [[nodiscard]] auto getEntry(size_t index) const -> PaletteEntry;
    [[nodiscard]] auto getEntries() const -> const std::vector<PaletteEntry> &;
    [[nodiscard]] auto getEntryCount() const -> size_t;
    [[nodiscard]] auto getSdlColor(size_t index) const -> SDL_Color;
    auto addEntry(PaletteEntry entry) -> void;
    auto addEntries(const std::vector<PaletteEntry> &newEntries) -> void;
};

} // namespace Abyss::DataTypes
