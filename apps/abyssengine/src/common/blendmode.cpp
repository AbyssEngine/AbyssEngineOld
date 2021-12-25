#include "blendmode.h"
#include <absl/strings/ascii.h>
#include <absl/strings/str_cat.h>
#include <stdexcept>

namespace AbyssEngine {

std::string_view BlendModeToString(const eBlendMode &blendMode) {
    switch (blendMode) {
    case eBlendMode::None:
        return "none";
    case eBlendMode::Blend:
        return "blend";
    case eBlendMode::Add:
        return "additive";
    case eBlendMode::Mod:
        return "modulus";
    case eBlendMode::Mul:
        return "multiply";
    default:
        return "";
    }
}

eBlendMode StringToBlendMode(std::string_view str) {
    auto val = absl::AsciiStrToLower(str);

    if (val == "none")
        return eBlendMode::None;

    if (val == "blend")
        return eBlendMode::Blend;

    if (val == "additive")
        return eBlendMode::Add;

    if (val == "modulus")
        return eBlendMode::Mod;

    if (val == "multiply")
        return eBlendMode::Mul;

    if (val.empty())
        return eBlendMode::Unknown;

    throw std::runtime_error(absl::StrCat("Unknown Blend Mode: ", str));
}

} // namespace AbyssEngine
