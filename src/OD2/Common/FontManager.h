#pragma once

#include "Abyss/Concepts/FontRenderer.h"

#include <absl/container/flat_hash_map.h>
#include <memory>
#include <ranges>
#include <string>

namespace OD2::Common {

class FontManager {
  absl::flat_hash_map<std::string, std::unique_ptr<Abyss::Concepts::FontRenderer>> _fonts{};
    FontManager() = default;

  public:
    static FontManager &getInstance() {
        static FontManager instance;
        return instance;
    }

    Abyss::Concepts::FontRenderer &getFont(const std::string_view name) {
        const auto it = _fonts.find(name);
        if (it == _fonts.end())
            throw std::runtime_error("Font not found");

        return *it->second;
    }

    void addFont(const std::string_view name, std::unique_ptr<Abyss::Concepts::FontRenderer> renderer) {
        if (_fonts.contains(name))
            throw std::runtime_error("Font already exists");

        _fonts.emplace(name, std::move(renderer));
    }

    void removeFont(const std::string_view name) {
        const auto it = _fonts.find(name);
        if (it == _fonts.end())
            throw std::runtime_error("Font not found");

        _fonts.erase(it);
    }

    void clearFonts() { _fonts.clear(); }
};

inline Abyss::Concepts::FontRenderer &GetFont(const std::string_view name) { return FontManager::getInstance().getFont(name); }

} // namespace OD2::Common
