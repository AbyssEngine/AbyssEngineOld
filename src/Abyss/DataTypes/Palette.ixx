module;

#include <SDL2/SDL.h>
#include <string>
#include <vector>

export module Abyss.DataTypes.Palette;

import Abyss.Singletons;
import Abyss.Enums.MouseButton;

namespace Abyss::DataTypes {

export class PaletteEntry {
    uint8_t red;
    uint8_t green;
    uint8_t blue;

  public:
    PaletteEntry(const uint8_t red, const uint8_t green, const uint8_t blue) : red(red), green(green), blue(blue) {}

    [[nodiscard]] auto getRed() const -> uint8_t { return red; }

    [[nodiscard]] auto getGreen() const -> uint8_t { return green; }

    [[nodiscard]] auto getBlue() const -> uint8_t { return blue; }

    [[nodiscard]] auto getSdlColor() const -> SDL_Color { return {red, green, blue, 255}; }
};

export class Palette {
    std::vector<PaletteEntry> _entries{};
    std::string _name;

  public:
    Palette(const std::string_view path, const std::string_view name) : _name(std::string(name)) {
        const auto bytes = Singletons::getFileProvider().loadBytes(path);

        for (size_t i = 0; i < bytes.size(); i += 3) {
            addEntry({static_cast<uint8_t>(bytes.at(i)), static_cast<uint8_t>(bytes.at(i + 1)), static_cast<uint8_t>(bytes.at(i + 2))});
        }
    }

    [[nodiscard]] auto getName() const -> const std::string & { return _name; }

    auto setName(const std::string_view newName) -> void { _name = newName; }

    [[nodiscard]] auto getEntry(const size_t index) const -> PaletteEntry {
        if (index >= _entries.size())
            throw std::out_of_range("Palette::getEntry: index out of range");

        return _entries.at(index);
    }

    [[nodiscard]] auto getEntries() const -> const std::vector<PaletteEntry> & { return _entries; }

    [[nodiscard]] auto getEntryCount() const -> size_t { return _entries.size(); }

    [[nodiscard]] auto getSdlColor(const size_t index) const -> SDL_Color { return index == 0 ? SDL_Color{0, 0, 0, 0} : getEntry(index).getSdlColor(); }

    auto addEntry(const PaletteEntry entry) -> void { _entries.push_back(entry); }

    auto addEntries(const std::vector<PaletteEntry> &newEntries) -> void {
        for (const auto &entry : newEntries) {
            addEntry(entry);
        }
    }
};

} // namespace Abyss::DataTypes
