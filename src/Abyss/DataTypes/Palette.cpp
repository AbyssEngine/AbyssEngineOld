#include "Palette.h"

#include <Abyss/AbyssEngine.h>

namespace Abyss::DataTypes {

auto PaletteEntry::getRed() const -> uint8_t { return red; }

auto PaletteEntry::getGreen() const -> uint8_t { return green; }

auto PaletteEntry::getBlue() const -> uint8_t { return blue; }

Palette::Palette(std::string_view path, std::string_view name) : _name(std::string(name)) {
    auto &engine = Abyss::AbyssEngine::getInstance();
    const auto bytes = engine.loadBytes(path);

    for (size_t i = 0; i < bytes.size(); i += 3) {
        addEntry({static_cast<uint8_t>(bytes.at(i)), static_cast<uint8_t>(bytes.at(i + 1)), static_cast<uint8_t>(bytes.at(i + 2))});
    }
}

auto Palette::getName() const -> const std::string & { return _name; }

auto Palette::setName(std::string newName) -> void { _name = std::move(newName); }

auto Palette::getEntry(size_t index) const -> PaletteEntry {
    if (index >= _entries.size()) {
        throw std::out_of_range("Palette::getEntry: index out of range");
    }

    return _entries.at(index);
}

auto Palette::getEntries() const -> const std::vector<PaletteEntry> & { return _entries; }

auto Palette::getEntryCount() const -> size_t { return _entries.size(); }

auto Palette::getSdlColor(size_t index) const -> SDL_Color { return (index == 0) ? SDL_Color{0, 0, 0, 0} : getEntry(index).getSdlColor(); }

auto Palette::addEntry(PaletteEntry entry) -> void { _entries.push_back(entry); }

auto Palette::addEntries(const std::vector<PaletteEntry> &newEntries) -> void {
    for (const auto &entry : newEntries) {
        addEntry(entry);
    }
}

} // namespace Abyss::DataTypes
// namespace Abyss::DataTypes
