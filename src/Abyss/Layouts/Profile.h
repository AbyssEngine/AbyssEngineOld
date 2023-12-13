#pragma once

#include <nlohmann/json.hpp>
#include <string_view>

namespace Abyss::Layouts {

class Profile {
    nlohmann::json _data;
  public:
    explicit Profile(std::string_view name);
    void resolveReferences(nlohmann::json& object);
};

} // namespace Abyss::Layouts
