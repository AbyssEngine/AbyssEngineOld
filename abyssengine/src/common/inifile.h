#ifndef ABYSS_INIFILE_H
#define ABYSS_INIFILE_H


#include <filesystem>
#include <string>
#include <string_view>
#include <absl/container/flat_hash_map.h>

namespace AbyssEngine::Common {
    class INIFile {
    public:
        explicit INIFile(const std::filesystem::path &iniFilePath);

        std::string GetValue(std::string_view category, std::string_view name);

        bool GetValueBool(std::string_view category, std::string_view name);

        void SetValue(std::string_view category, std::string_view name, std::string_view value);

    private:
        absl::flat_hash_map<std::string, absl::flat_hash_map<std::string, std::string>> _values;
    };

}

#endif //ABYSS_INIFILE_H
