module;

#include <vector>

import Abyss.Common.Button;
import Abyss.Common.Logging;

export module OD2.Common.ButtonDefManager;

namespace OD2::Common {

export class ButtonDefManager {
    std::vector<Abyss::Common::ButtonDef> buttonDefs;

    ButtonDefManager() {}

  public:
    static auto getInstance() -> ButtonDefManager & {
        static ButtonDefManager instance;
        return instance;
    }

    auto getButtonDefs() -> std::vector<Abyss::Common::ButtonDef> & { return buttonDefs; }

    auto getButtonDef(const std::string_view name) -> Abyss::Common::ButtonDef & {
        for (auto &buttonDef : buttonDefs) {
            if (buttonDef.name == name) {
                return buttonDef;
            }
        }
        throw std::runtime_error("ButtonDef not found");
    }

    auto addButtonDef(const Abyss::Common::ButtonDef &def) -> void {
        for (auto &existingButtonDef : buttonDefs) {
            if (existingButtonDef.name == def.name) {
                Abyss::Common::Log::warn("ButtonDef already exists, replacing...");
                existingButtonDef = def;
                return;
            }
        }
        buttonDefs.push_back(def);
    }

    auto removeButtonDef(const std::string_view name) -> void {
        for (auto it = buttonDefs.begin(); it != buttonDefs.end(); ++it) {
            if (it->name == name) {
                buttonDefs.erase(it);
                return;
            }
        }
        throw std::runtime_error("ButtonDef not found");
    }

    auto clearButtonDefs() -> void { buttonDefs.clear(); }
};
} // namespace OD2::Common
