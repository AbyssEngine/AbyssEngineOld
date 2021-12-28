#include "libabyss/formats/abyss/inifile.h"
#include <absl/strings/str_format.h>
#include <absl/strings/str_split.h>
#include <algorithm>
#include <charconv>
#include <fstream>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace LibAbyss {

INIFile::INIFile(const std::filesystem::path &iniFilePath) {
    SPDLOG_TRACE("creating ini file from '{0}'", iniFilePath.string());

    std::ifstream input_file(iniFilePath);

    if (!input_file.is_open())
        throw std::runtime_error("File not found:" + iniFilePath.string());

    std::string fileString((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    auto lines = absl::StrSplit(fileString, "\n");

    std::string currentCategory;

    auto lineIdx = 0;
    for (const auto &lineSrc : lines) {
        lineIdx++;

        auto line = absl::StripAsciiWhitespace(lineSrc);

        if (line.empty())
            continue;

        if (line.front() == ';')
            continue;

        if (line.front() == '[') {
            if (line.back() != ']') {
                spdlog::error("error parsing INI file '{0}' on line {1}: Malformed Category", iniFilePath.string(), lineIdx);
                continue;
            }

            currentCategory = absl::AsciiStrToLower(line.substr(1, line.length() - 2));

            continue;
        }

        if (currentCategory.empty()) {
            spdlog::error("error parsing INI file '{0}' on line {1}: Values must be under a named category", iniFilePath.string(), lineIdx);
            continue;
        }

        std::vector<std::string_view> values = absl::StrSplit(line, absl::MaxSplits('=', 1));
        if (values.size() < 2) {
            spdlog::error("error parsing INI file '{0}' on line {1}: Values must be in the format name=value", iniFilePath.string(), lineIdx);
            continue;
        }

        if (values.size() == 1) {
            _values[currentCategory][absl::AsciiStrToLower(values[0])] = "";
            continue;
        }

        auto name = absl::AsciiStrToLower(values[0]);
        auto value = std::string(values[1]);
        _values[currentCategory][name] = value;
    }
}

std::string_view INIFile::GetValue(std::string_view category, std::string_view name) const {
    auto itcat = _values.find(absl::AsciiStrToLower(category));
    if (itcat == _values.end())
        return "";
    const auto &cat = itcat->second;

    auto it = cat.find(absl::AsciiStrToLower(name));
    if (it == cat.end())
        return "";

    return it->second;
}

bool INIFile::GetValueBool(std::string_view category, std::string_view name) const {
    auto value = absl::AsciiStrToLower(GetValue(category, name));

    return (value == "true" || value == "1" || value == "yes" || value == "y");
}

void INIFile::SetValue(std::string_view category, std::string_view name, std::string_view value) {
    _values[absl::AsciiStrToLower(category)][absl::AsciiStrToLower(name)] = value;
}

int INIFile::GetValueInt(std::string_view category, std::string_view name) const {
    auto value = GetValue(category, name);
    int result = 0;
    auto [ptr, ec] = std::from_chars((const char *)value.data(), (const char *)value.data() + value.size(), result, 10);
    if (ec != std::errc() || ptr != (const char *)value.data() + value.size()) {
        throw std::runtime_error(absl::StrFormat("Can't parse %s.%s from ini as int: '%s'", category, name, value));
    }
    return result;
}
float INIFile::GetValueFloat(std::string_view category, std::string_view name) const {
    // TODO: use std::from_chars instead, when it's implemented in libc++
    auto value = GetValue(category, name);
    float result = 0;
    bool fp = false;
    float coef = 1;
    for (char c : value) {
        if (c == '.') {
            if (fp) {
                throw std::runtime_error(absl::StrFormat("Can't parse %s.%s from ini as float: '%s'", category, name, value));
            }
            fp = true;
        } else if (c < '0' || c > '9') {
            throw std::runtime_error(absl::StrFormat("Can't parse %s.%s from ini as float: '%s'", category, name, value));
        } else if (fp) {
            coef *= 0.1;
            result += (c - '0') * coef;
        } else {
            result = result * 10 + (c - '0');
        }
    }
    return result;
}

} // namespace LibAbyss
