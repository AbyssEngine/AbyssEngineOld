#include "alignment.h"
#include <absl/strings/ascii.h>
#include <absl/strings/str_cat.h>
#include <stdexcept>

namespace AbyssEngine {

std::string_view AlignmentToString(const eAlignment &alignment) {
    switch (alignment) {
    case eAlignment::Start:
        return "start";
    case eAlignment::Middle:
        return "middle";
    case eAlignment::End:
        return "end";
    default:
        return "";
    }
}

eAlignment StringToAlignment(std::string_view str) {
    auto val = absl::AsciiStrToLower(str);
    if (val == "start")
        return eAlignment::Start;
    if (val == "middle")
        return eAlignment::Middle;
    if (val == "end")
        return eAlignment::End;
    if (val.empty())
        return eAlignment::Unknown;

    throw std::runtime_error(absl::StrCat("Unknown Alignment: ", str));
}

} // namespace AbyssEngine
