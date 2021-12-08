#ifndef ABYSS_INIFILE_H
#define ABYSS_INIFILE_H


#include <filesystem>
#include <string>
#include <string_view>
#include <absl/container/flat_hash_map.h>

namespace LibAbyss {
    class INIFile {
    public:
        explicit INIFile(const std::filesystem::path &iniFilePath);

        std::string_view GetValue(std::string_view category, std::string_view name) const;

        bool GetValueBool(std::string_view category, std::string_view name) const;

        int GetValueInt(std::string_view category, std::string_view name) const;

        void SetValue(std::string_view category, std::string_view name, std::string_view value);

        float GetValueFloat(std::string_view category, std::string_view name) const;

    private:
        absl::flat_hash_map<std::string, absl::flat_hash_map<std::string, std::string>> _values;
    };

}

#endif //ABYSS_INIFILE_H
