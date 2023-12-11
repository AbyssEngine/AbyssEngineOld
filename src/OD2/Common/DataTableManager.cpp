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

    // Grab first row and remove from lines
    auto header = splitLine(lines.front());

    for (int i = 1; i < lines.size(); ++i) {
        auto row = splitLine(lines[i]);
        std::unordered_map<std::string, std::string> rowMap;
        for (size_t j = 0; j < header.size(); j++) {
            rowMap.emplace(header[j], std::move(row[j]));
        }
        result.push_back(rowMap);
    }

    {
        std::lock_guard lock(_writeMutex);
        dataTables.emplace(std::string(name), result);
    }
}
DataTable &DataTableManager::getDataTable(const std::string_view name) {
    const auto it = dataTables.find(std::string(name));
    if (it == dataTables.end())
        throw std::runtime_error("DataTable not found");

    return it->second;
}

DataTable &DataTableManager::operator[](const std::string_view name) { return getDataTable(name); }

} // namespace OD2::Common
