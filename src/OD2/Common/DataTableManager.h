#pragma once

#include <absl/container/flat_hash_map.h>
#include <mutex>
#include <string>
#include <vector>

namespace OD2::Common {

using DataTableRow = absl::flat_hash_map<std::string, std::string>;
using DataTable = std::vector<DataTableRow>;

class DataTableManager {
    DataTableManager() = default;
    absl::flat_hash_map<std::string, DataTable> dataTables{};
    std::mutex _readMutex{};
    std::mutex _writeMutex{};

  public:
    static DataTableManager &getInstance() {
        static DataTableManager instance;
        return instance;
    }

    void addDataTable(std::string_view name, std::string_view fileName);
    DataTable &getDataTable(std::string_view name);

    absl::flat_hash_map<std::string, int> buildIndexBy(std::string_view table, std::string_view column);

    DataTable &operator[](std::string_view name);
};

} // namespace OD2::Common
