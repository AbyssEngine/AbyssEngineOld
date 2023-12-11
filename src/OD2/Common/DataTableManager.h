#pragma once

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace OD2::Common {

using DataTable = std::vector<std::unordered_map<std::string, std::string>>;

class DataTableManager {
    DataTableManager() = default;
    std::unordered_map<std::string, DataTable> dataTables{};
    std::mutex _readMutex{};
    std::mutex _writeMutex{};

  public:
    static DataTableManager &getInstance() {
        static DataTableManager instance;
        return instance;
    }

    void addDataTable(std::string_view name, std::string_view fileName);
    DataTable &getDataTable(std::string_view name);

    DataTable &operator[](std::string_view name);
};

} // namespace OD2::Common
