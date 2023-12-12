#include "MapTest.h"

#include "Abyss/AbyssEngine.h"
#include "Abyss/Common/Logging.h"
#include "OD2/Common/DataTableManager.h"
#include "OD2/Common/PaletteManager.h"

#include <imgui.h>
#include <string>

namespace OD2::Scenes::MapTest {

MapTest::MapTest() {
    // : testdt1("/data/global/tiles/ACT1/TOWN/floor.dt1", Common::PaletteManager::getInstance().getPalette("Act1")),
    //   testdt2("/data/global/tiles/ACT1/TOWN/fence.dt1", Common::PaletteManager::getInstance().getPalette("Act1")) {

    for (const auto &lvlPrest = Common::DataTableManager::getInstance().getDataTable("LevelPreset"); const auto &row : lvlPrest) {
        const auto &levelName = row.at("Name");
        if (levelName == "None" || levelName == "Expansion")
            continue;
        _mapSelections.push_back(levelName);
    }
}

void MapTest::onLevelChanged(const std::string &levelName) {
    Abyss::Common::Log::debug("Loading level: {}", levelName);

    _mapAltSelections.clear();

    const auto &lvlPrest = Common::DataTableManager::getInstance().getDataTable("LevelPreset");
    std::unordered_map<std::string, std::string> levelPrestData;
    // Iterate over the LevelPrest table and find all columns from File1 to File6 that aren't "0" and add them to _mapAltSelections
    for (const auto &row : lvlPrest) {
        if (const auto &lvlPrestName = row.at("Name"); lvlPrestName != levelName)
            continue;

        for (int i = 1; i <= 6; ++i) {
            const auto &file = "File" + std::to_string(i);
            if (const auto &dt1 = row.at(file); dt1 != "0" && dt1 != "None" && dt1 != "Expansion") {
                _mapAltSelections.push_back(dt1);
            }
        }

        if (!_mapAltSelections.empty())
            _selectedLevelAltName = _mapAltSelections.front();

        break;
    }
}

const Common::DataTableRow &MapTest::getLevelPrest(const std::string_view name) {
    for (const auto &lvlPrest = Common::DataTableManager::getInstance().getDataTable("LevelPreset"); const Common::DataTableRow &row : lvlPrest) {
        if (const auto &lvlPrestName = row.at("Name"); lvlPrestName == name)
            return row;
    }
    throw std::runtime_error("LevelPreset not found: " + std::string(name));
}

const Common::DataTableRow &MapTest::getLevelDetails(const int levelId) {
    for (const auto &lvlDetails = Common::DataTableManager::getInstance().getDataTable("LevelDetails"); const Common::DataTableRow &row : lvlDetails) {
        if (const auto &lvlDetailsId = std::stoi(row.at("Id")); lvlDetailsId == levelId)
            return row;
    }
    throw std::runtime_error("LevelDetails not found: " + std::to_string(levelId));
}

const Common::DataTableRow &MapTest::getLevelType(int levelTypeId) {
    for (const auto &lvlTypes = Common::DataTableManager::getInstance().getDataTable("LevelType"); const Common::DataTableRow &row : lvlTypes) {
        if (const auto &lvlTypeId = std::stoi(row.at("Id")); lvlTypeId == levelTypeId)
            return row;
    }
    throw std::runtime_error("LevelType not found: " + std::to_string(levelTypeId));
}

void MapTest::update(std::chrono::duration<double> deltaTime) {}

void MapTest::processEvent(const SDL_Event &event) {}

void MapTest::loadTile(const std::string &altName) {
    Abyss::Common::Log::debug("Loading level alt: {}", altName);
    const auto &levelPrest = getLevelPrest(_selectedLevelName);
    const auto levelId = std::stoi(levelPrest.at("LevelId"));
    const auto &levelDetails = getLevelDetails(levelId);
    const auto &levelTypeId = std::stoi(levelDetails.at("LevelType"));
    const auto act = std::stoi(levelDetails.at("Act")) + 1;
    const auto &levelType = getLevelType(levelTypeId);
    const auto &palette = Common::PaletteManager::getInstance().getPalette("Act" + std::to_string(act));

    dt1s.clear();
    std::vector<std::string> dt1sToLoad{};

    for (int i = 1; i <= 31; ++i) {
        const auto &file = "File " + std::to_string(i);
        auto dt1 = levelType.at(file);
        std::ranges::transform(dt1, dt1.begin(), [](const auto c) { return std::tolower(c); });
        if (dt1 != "None" && dt1 != "Expansion" && dt1 != "0" && std::ranges::find(dt1sToLoad, dt1) == dt1sToLoad.end()) {
            auto filePath = "/data/global/tiles/" + dt1;
            std::ranges::transform(filePath, filePath.begin(), [](const auto c) { return std::tolower(c); });
            dt1s.emplace_back("/data/global/tiles/" + dt1, palette);
            dt1sToLoad.push_back("/data/global/tiles/" + dt1);
        }
    }

    ds1.reset(new Abyss::DataTypes::DS1("/data/global/tiles/" + altName));

    // Add all the DS1 files to the DT1s
    for (auto &file : ds1->files) {
        std::ranges::replace(file, '\\', '/');
        if (file.starts_with("/d2/")) // Because why not?
            file = file.substr(3);
        std::ranges::transform(file, file.begin(), [](const auto c) { return std::tolower(c); });
        if (std::ranges::find(dt1sToLoad, file) == dt1sToLoad.end()) {
            dt1s.emplace_back(file, palette);
            dt1sToLoad.push_back(file);
        }
    }

    int x = 1;

    //
}

void MapTest::render() {
    const auto &renderer = Abyss::AbyssEngine::getInstance().getRenderer();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // testdt1.drawTile(480, 240, 4);
    // testdt1.drawTile(400, 200, 5);
    // testdt1.drawTile(560, 200, 6);
    // testdt1.drawTile(480, 160, 7);
    // testdt2.drawTile(480, 240, 6);

    ImGui::SetNextWindowSize(ImVec2(250, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin("Debug");

    // Level selection
    if (ImGui::BeginCombo("Level", _selectedLevelName.c_str())) {
        for (const auto &levelName : _mapSelections) {
            const bool isSelected = (_selectedLevelName == levelName);
            if (ImGui::Selectable(levelName.c_str(), isSelected))
                if (_selectedLevelName != levelName) {
                    onLevelChanged(levelName);
                    _selectedLevelName = levelName;
                }

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    // Level alt selection
    if (!_mapAltSelections.empty()) {
        if (ImGui::BeginCombo("Level Alt", _selectedLevelAltName.c_str())) {
            for (const auto &altName : _mapAltSelections) {
                const bool isSelected = (_selectedLevelAltName == altName);
                if (ImGui::Selectable(altName.c_str(), isSelected))
                    if (_selectedLevelAltName != altName) {
                        _selectedLevelAltName = altName;
                        loadTile(altName);
                    }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }

    ImGui::End();
}

} // namespace OD2::Scenes::MapTest
