#include "Palette.h"

#include "Abyss/Singletons.h"

namespace Abyss::DataTypes {

PaletteEntry::PaletteEntry(const uint8_t red, const uint8_t green, const uint8_t blue) : red(red), green(green), blue(blue) {}

auto PaletteEntry::getRed() const -> uint8_t { return red; }

auto PaletteEntry::getGreen() const -> uint8_t { return green; }

auto PaletteEntry::getBlue() const -> uint8_t { return blue; }

auto PaletteEntry::getSdlColor() const -> SDL_Color { return {red, green, blue, 255}; }

Palette::Palette(const std::string_view path, const std::string_view name) : _name(name) {
    const auto bytes = Singletons::getFileProvider().loadBytes(path);

    for (size_t i = 0; i < bytes.size(); i += 3) {
        addEntry({static_cast<uint8_t>(bytes.at(i)), static_cast<uint8_t>(bytes.at(i + 1)), static_cast<uint8_t>(bytes.at(i + 2))});
    }
}

const std::string &Palette::getName() const { return _name; }

PaletteEntry Palette::getEntry(const size_t index) const {
    if (index >= _entries.size())
        throw std::out_of_range("Palette::getEntry: index out of range");

    return _entries.at(index);
}

const std::vector<PaletteEntry> &Palette::getEntries() const { return _entries; }

size_t Palette::getEntryCount() const { return _entries.size(); }

SDL_Color Palette::getSdlColor(const size_t index) const { return index == 0 ? SDL_Color{0, 0, 0, 0} : getEntry(index).getSdlColor(); }

void Palette::addEntry(const PaletteEntry entry) { _entries.push_back(entry); }

void Palette::addEntries(const std::vector<PaletteEntry> &newEntries) {
    for (const auto &entry : newEntries) {
        addEntry(entry);
    }
}

} // namespace Abyss::DataTypes
