#pragma once

#include "Profile.h"
#include <nlohmann/json.hpp>
#include <string_view>

namespace OD2::Layouts {

class Layout {
    nlohmann::json _data;

  public:
    // Reads from /data/global/ui/layouts/{name} (name should include .json extension)
    explicit Layout(std::string_view name, const Profile &profile);
};

} // namespace OD2::Layouts
