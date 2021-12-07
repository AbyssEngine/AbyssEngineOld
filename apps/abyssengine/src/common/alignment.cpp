#include "alignment.h"
#include <absl/strings/ascii.h>
#include <absl/strings/str_cat.h>
#include <stdexcept>

std::string_view AbyssEngine::AlignmentToString(const AbyssEngine::eAlignment &alignment) {
    switch (alignment) {
    case AbyssEngine::eAlignment::Start:
        return "start";
    case AbyssEngine::eAlignment::Middle:
        return "middle";
    case AbyssEngine::eAlignment::End:
        return "end";
    default:
        return "";
    }
}

AbyssEngine::eAlignment AbyssEngine::StringToAlignment(std::string_view str) {
    auto val = absl::AsciiStrToLower(str);
    if (val == "start")
        return AbyssEngine::eAlignment::Start;
    if (val == "middle")
        return AbyssEngine::eAlignment::Middle;
    if (val == "end")
        return AbyssEngine::eAlignment::End;
    if (val.empty())
        return AbyssEngine::eAlignment::Unknown;

    throw std::runtime_error(absl::StrCat("Unknown Alignment: ", str));
}
