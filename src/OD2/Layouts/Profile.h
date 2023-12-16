#pragma once

#include <nlohmann/json.hpp>
#include <string_view>

namespace OD2::Layouts {

class Profile {
    nlohmann::json _data;

  public:
    // Reads from /data/global/ui/layouts/_profile{name}.json
    explicit Profile(std::string_view name);
    void resolveReferences(nlohmann::json &object) const;
};

} // namespace OD2::Layouts
