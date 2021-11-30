#include "inifile.h"
#include <algorithm>
#include <spdlog/spdlog.h>
#include <fstream>
#include <absl/strings/str_split.h>

AbyssEngine::Common::INIFile::INIFile(const std::filesystem::path &iniFilePath) {
    SPDLOG_TRACE("creating ini file from '{0}'", iniFilePath.string());

    std::ifstream input_file(iniFilePath);

    if (!input_file.is_open())
        throw std::runtime_error("file not found");


    std::string fileString((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    auto lines = absl::StrSplit(fileString, "\n");

    std::string currentCategory;

    auto lineIdx = 0;
    for (const auto &lineSrc : lines) {
        lineIdx++;

        auto line = absl::StripAsciiWhitespace(lineSrc);

        if (line.empty())
            continue;

        if (line.front() == '[') {
            if (line.back() != ']') {
                spdlog::error("error parsing INI file '{0}' on line {1}: Malformed Category", iniFilePath.string(), lineIdx);
                continue;
            }

            currentCategory = absl::AsciiStrToLower(line.substr(1, line.length()-2));

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

std::string AbyssEngine::Common::INIFile::GetValue(std::string_view category, std::string_view name) {
    if (!_values.contains(absl::AsciiStrToLower(category)))
        return {};

    if (!_values[absl::AsciiStrToLower(category)].contains(absl::AsciiStrToLower(name)))
        return {};

    auto result = _values[absl::AsciiStrToLower(category)][absl::AsciiStrToLower(name)];
    return result;
}

bool AbyssEngine::Common::INIFile::GetValueBool(std::string_view category, std::string_view name) {
    auto value = absl::AsciiStrToLower(GetValue(category, name));

    return (value == "true" || value == "1" || value == "yes" || value == "y");
}

void AbyssEngine::Common::INIFile::SetValue(std::string_view category, std::string_view name, std::string_view value) {
    _values[absl::AsciiStrToLower(category)][absl::AsciiStrToLower(name)] = value;
}
