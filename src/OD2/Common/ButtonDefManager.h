#pragma once
#include "Abyss/DataTypes/DC6.h"
#include "Abyss/UI/Button.h"
#include "Abyss/UI/ButtonDef.h"
#include "FontManager.h"

#include <functional>
#include <string>
#include <unordered_map>

namespace OD2::Common {

class ButtonDefManager {
    std::unordered_map<std::string, Abyss::UI::ButtonDef> buttonDefs{};

    ButtonDefManager() = default;

  public:
    static ButtonDefManager &getInstance() {
        static ButtonDefManager instance;
        return instance;
    }

    Abyss::UI::ButtonDef &getButtonDef(const std::string_view name) {
        auto it = buttonDefs.find(std::string(name));
        if (it == buttonDefs.end())
            throw std::runtime_error("ButtonDef not found");

        return it->second;
    }

    void addButtonDef(const Abyss::UI::ButtonDef &def) {
        if (buttonDefs.find(def.name) != buttonDefs.end())
            throw std::runtime_error("ButtonDef already exists");

        buttonDefs.emplace(def.name, def);
    }

    void removeButtonDef(const std::string_view name) {
        auto it = buttonDefs.find(std::string(name));
        if (it == buttonDefs.end())
            throw std::runtime_error("ButtonDef not found");

        buttonDefs.erase(it);
    }

    void clearButtonDefs() { buttonDefs.clear(); }
};

} // namespace OD2::Common

static Abyss::UI::ButtonDef &GetButtonDef(const std::string_view name) { return OD2::Common::ButtonDefManager::getInstance().getButtonDef(name); };

static Abyss::UI::Button<Abyss::DataTypes::DC6> CreateButton(const std::string_view buttonDefName, const std::string_view text, std::function<void()> onClick) {
    const auto &buttonDef = GetButtonDef(buttonDefName);
    const auto fontName = buttonDef.font;
    const auto &font = GetFont(fontName);

    return Abyss::UI::Button<Abyss::DataTypes::DC6>(buttonDef, text, font, std::move(onClick));
};
