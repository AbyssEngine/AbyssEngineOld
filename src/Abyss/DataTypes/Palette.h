#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <cstdint>

namespace Abyss::DataTypes {

class PaletteEntry {
    uint8_t red;
    uint8_t green;
    uint8_t blue;

public:
    PaletteEntry(uint8_t red, uint8_t green, uint8_t blue);
    [[nodiscard]] auto getRed() const -> uint8_t;
    [[nodiscard]] auto getGreen() const -> uint8_t;
    [[nodiscard]] auto getBlue() const -> uint8_t;
    [[nodiscard]] auto getSdlColor() const -> SDL_Color;
};

class Palette {
    std::vector<PaletteEntry> _entries{};
    std::string _name;
    static uint8_t colorAdjust(uint8_t value);
    inline static float gamma{1.2f};
    inline static float brightness{1.5f};

public:
    // Loads from file.
    Palette(std::string_view path, std::string_view name);
    [[nodiscard]] const std::string &getName() const;
    auto setName(const std::string_view newName) -> void { _name = newName; }
    [[nodiscard]] PaletteEntry getEntry(size_t index) const;
    [[nodiscard]] const std::vector<PaletteEntry> &getEntries() const;
    [[nodiscard]] size_t getEntryCount() const;
    [[nodiscard]] SDL_Color getSdlColor(size_t index) const;
    void addEntry(PaletteEntry entry);
    void addEntries(const std::vector<PaletteEntry> &newEntries);
};

} // namespace Abyss::DataTypes
