#pragma once

#include <nlohmann/json.hpp>
#include <string_view>

namespace Abyss::Common {

// Like normal nlohmann, but supports comments and trailing commas
nlohmann::json parseJson(std::string_view json);

} // namespace Abyss::Common
