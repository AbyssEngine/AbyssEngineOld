module;

import Abyss.Common.FileProvider;
import Abyss.Streams.InputStream;
import Abyss.Common.MouseProvider;
import Abyss.Common.MouseState;
import Abyss.Common.RendererProvider;

#include <string>
#include <vector>

export module OD2.Common.ButtonDefManager;

import Abyss.UI.ButtonDef;
import Abyss.Common.Logging;

namespace OD2::Common {

export class ButtonDefManager {
    std::vector<Abyss::UI::ButtonDef> buttonDefs{};

    ButtonDefManager() = default;

  public:
    static auto getInstance() -> ButtonDefManager & {
        static ButtonDefManager instance;
        return instance;
    }

    auto getButtonDefs() -> std::vector<Abyss::UI::ButtonDef> & { return buttonDefs; }

    auto getButtonDef(const std::string_view name) -> Abyss::UI::ButtonDef & {
        for (auto &buttonDef : buttonDefs) {
            if (buttonDef.name == name) {
                return buttonDef;
            }
        }
        throw std::runtime_error("ButtonDef not found");
    }

    auto addButtonDef(const Abyss::UI::ButtonDef &def) -> void {
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
