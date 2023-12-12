#include "DataTableManager.h"

#include "Abyss/AbyssEngine.h"
#include "Abyss/Common/Logging.h"

namespace OD2::Common {

void DataTableManager::addDataTable(const std::string_view name, const std::string_view fileName) {
    Abyss::Common::Log::debug("Loading data table: {} ({})", name, fileName);
    auto splitLine = [](const std::string_view line) {
        std::vector<std::string> result;
        std::string current;
        for (const auto c : line) {
            if (c == '\t') {
                result.push_back(std::move(current));
                current.clear();
            } else {
                current += c;
            }
        }
        result.push_back(std::move(current));
        return result;
    };

    std::string text;
    {
        std::lock_guard lock(_readMutex);
        text = Abyss::AbyssEngine::getInstance().loadString(fileName);
    }
    std::vector<std::string> lines{};
    std::istringstream stream(text);
    std::string line;
    while (std::getline(stream, line)) {
        if (line.empty()) {
            continue;
        }
        lines.push_back(std::move(line));
    }
    DataTable result;
    result.reserve(lines.size());

    // Grab first row and remove from lines
    auto header = splitLine(lines.front());

    for (int i = 1; i < lines.size(); ++i) {
        auto row = splitLine(lines[i]);
        absl::flat_hash_map<std::string, std::string> rowMap;
        for (size_t j = 0; j < header.size(); j++) {
            rowMap.emplace(header[j], std::move(row[j]));
        }
        result.push_back(std::move(rowMap));
    }

    {
        std::lock_guard lock(_writeMutex);
        dataTables.emplace(name, result);
    }
}
DataTable &DataTableManager::getDataTable(const std::string_view name) {
    const auto it = dataTables.find(name);
    if (it == dataTables.end())
        throw std::runtime_error("DataTable not found");

    return it->second;
}

absl::flat_hash_map<std::string, int> DataTableManager::buildIndexBy(std::string_view name, std::string_view column) {
  const auto& table = getDataTable(name);
  absl::flat_hash_map<std::string, int> result;
  for (int i = 0; i < table.size(); ++i) {
    const auto& row = table[i];
    result[row.at(column)] = i;
  }
  return result;
}

DataTable &DataTableManager::operator[](const std::string_view name) { return getDataTable(name); }

} // namespace OD2::Common
