#pragma once

#include <nlohmann/json.hpp>
#include <string_view>

namespace Abyss::Layouts {

class Layout {
    nlohmann::json _data;
  public:
    // Reads from /data/global/ui/layouts/{name} (name should include .json extension)
    explicit Layout(std::string_view name);
};

} // namespace Abyss::Layouts
