#include "ButtonDefManager.h"

#include <Abyss/Common/Logging.hpp>

namespace OD2::Common {

ButtonDefManager::ButtonDefManager() {}

auto ButtonDefManager::getInstance() -> ButtonDefManager & {
    static ButtonDefManager instance;
    return instance;
}

auto ButtonDefManager::getButtonDefs() -> std::vector<Abyss::Common::ButtonDef> & { return buttonDefs; }

auto ButtonDefManager::getButtonDef(const std::string_view name) -> Abyss::Common::ButtonDef & {
    for (auto &buttonDef : buttonDefs) {
        if (buttonDef.name == name) {
            return buttonDef;
        }
    }
    throw std::runtime_error("ButtonDef not found");
}

auto ButtonDefManager::addButtonDef(const Abyss::Common::ButtonDef &def) -> void {
    for (auto &existingButtonDef : buttonDefs) {
        if (existingButtonDef.name == def.name) {
            Abyss::Common::Log::warn("ButtonDef already exists, replacing...");
            existingButtonDef = def;
            return;
        }
    }
    buttonDefs.push_back(def);
}

auto ButtonDefManager::removeButtonDef(const std::string_view name) -> void {
    for (auto it = buttonDefs.begin(); it != buttonDefs.end(); ++it) {
        if (it->name == name) {
            buttonDefs.erase(it);
            return;
        }
    }
    throw std::runtime_error("ButtonDef not found");
}

auto ButtonDefManager::clearButtonDefs() -> void { buttonDefs.clear(); }

} // namespace OD2::Common
