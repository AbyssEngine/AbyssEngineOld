#pragma once

#include <Abyss/Common/Button.hpp>
#include <vector>

namespace OD2::Common {

class ButtonDefManager {
    std::vector<Abyss::Common::ButtonDef> buttonDefs;

    ButtonDefManager();

  public:
    static auto getInstance() -> ButtonDefManager &;

    auto operator[](const std::string_view name) -> Abyss::Common::ButtonDef & { return getButtonDef(name); }
    auto getButtonDefs() -> std::vector<Abyss::Common::ButtonDef> &;
    auto getButtonDef(std::string_view name) -> Abyss::Common::ButtonDef &;
    auto addButtonDef(const Abyss::Common::ButtonDef &def) -> void;
    auto removeButtonDef(std::string_view name) -> void;
    auto clearButtonDefs() -> void;
};
} // namespace OD2::Common
