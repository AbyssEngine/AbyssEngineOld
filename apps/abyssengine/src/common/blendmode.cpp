#include "blendmode.h"
#include <absl/strings/ascii.h>
#include <absl/strings/str_cat.h>
#include <stdexcept>

std::string_view AbyssEngine::BlendModeToString(const AbyssEngine::eBlendMode &blendMode) {
    switch (blendMode) {
    case AbyssEngine::eBlendMode::None:
        return "none";
    case AbyssEngine::eBlendMode::Blend:
        return "blend";
    case AbyssEngine::eBlendMode::Add:
        return "additive";
    case AbyssEngine::eBlendMode::Mod:
        return "modulus";
    case AbyssEngine::eBlendMode::Mul:
        return "multiply";
    default:
        return "";
    }
}

AbyssEngine::eBlendMode AbyssEngine::StringToBlendMode(std::string_view str) {
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