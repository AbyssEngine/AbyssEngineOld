module;

#include <memory>
#include <string>
#include <unordered_map>

export module OD2.Common.FontManager;

import Abyss.Concepts.FontRenderer;
import Abyss.DataTypes.Palette;

namespace OD2::Common {

export class FontManager {
    std::unordered_map<std::string, std::unique_ptr<Abyss::Concepts::FontRenderer>> _fonts{};
    FontManager() = default;

  public:
    static auto getInstance() -> FontManager & {
        static FontManager instance;
        return instance;
    }

    auto getFont(const std::string_view name) -> Abyss::Concepts::FontRenderer & {
        auto it = _fonts.find(std::string(name));
        if (it == _fonts.end())
            throw std::runtime_error("Font not found");

        return *it->second;
    }

    auto addFont(const std::string_view name, std::unique_ptr<Abyss::Concepts::FontRenderer> renderer) -> void {
        if (_fonts.find(std::string(name)) != _fonts.end())
            throw std::runtime_error("Font already exists");

        _fonts.emplace(std::string(name), std::move(renderer));
    }

    auto removeFont(const std::string_view name) -> void {
        auto it = _fonts.find(std::string(name));
        if (it == _fonts.end())
            throw std::runtime_error("Font not found");

        _fonts.erase(it);
    }

    auto clearFonts() -> void { _fonts.clear(); }
};

} // namespace OD2::Common

export auto GetFont(const std::string_view name) -> Abyss::Concepts::FontRenderer & { return OD2::Common::FontManager::getInstance().getFont(name); }
