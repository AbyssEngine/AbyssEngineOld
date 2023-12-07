module;

import Abyss.Common.FileProvider;
import Abyss.Streams.InputStream;
import Abyss.Common.MouseProvider;
import Abyss.Common.MouseState;
import Abyss.Common.RendererProvider;

#include <functional>
#include <string>
#include <unordered_map>

export module OD2.Common.ButtonDefManager;

import Abyss.UI.ButtonDef;
import Abyss.UI.Button;
import Abyss.Common.Logging;
import OD2.Common.FontManager;
import Abyss.UI.SpriteFont;
import Abyss.DataTypes.DC6;

namespace OD2::Common {

export class ButtonDefManager {
    std::unordered_map<std::string, Abyss::UI::ButtonDef> buttonDefs{};

    ButtonDefManager() = default;

  public:
    static auto getInstance() -> ButtonDefManager & {
        static ButtonDefManager instance;
        return instance;
    }

    auto getButtonDef(const std::string_view name) -> Abyss::UI::ButtonDef & {
        auto it = buttonDefs.find(std::string(name));
        if (it == buttonDefs.end())
            throw std::runtime_error("ButtonDef not found");

        return it->second;
    }

    auto addButtonDef(const Abyss::UI::ButtonDef &def) -> void {
        if (buttonDefs.find(def.name) != buttonDefs.end())
            throw std::runtime_error("ButtonDef already exists");

        buttonDefs.emplace(def.name, def);
    }

    auto removeButtonDef(const std::string_view name) -> void {
        auto it = buttonDefs.find(std::string(name));
        if (it == buttonDefs.end())
            throw std::runtime_error("ButtonDef not found");

        buttonDefs.erase(it);
    }

    auto clearButtonDefs() -> void { buttonDefs.clear(); }
};

} // namespace OD2::Common

export auto GetButtonDef(const std::string_view name) -> Abyss::UI::ButtonDef & { return OD2::Common::ButtonDefManager::getInstance().getButtonDef(name); };

export auto CreateButton(const std::string_view buttonDefName, const std::string_view text, std::function<void()> onClick)
    -> Abyss::UI::Button<Abyss::DataTypes::DC6> {
    const auto getFontName = [](const std::string_view path) -> std::string {
        const auto pos = path.find_last_of('/');
        if (pos == std::string_view::npos)
            return std::string(path);

        return std::string(path.substr(pos + 1));
    };

    const auto &buttonDef = GetButtonDef(buttonDefName);
    const auto fontName = getFontName(buttonDef.fontPath);
    const auto &font = GetFont(fontName);

    return Abyss::UI::Button<Abyss::DataTypes::DC6>(buttonDef, text, font, std::move(onClick));
};
