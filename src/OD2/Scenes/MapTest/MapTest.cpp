#include "MapTest.h"

#include "Abyss/AbyssEngine.h"
#include "Abyss/Common/Logging.h"
#include "OD2/Common/DataTableManager.h"
#include "OD2/Common/PaletteManager.h"

#include <imgui.h>
#include <memory>
#include <string>
#include <absl/strings/str_replace.h>

namespace OD2::Scenes::MapTest {

MapTest::MapTest() {
    for (const auto &lvlPrest = Common::DataTableManager::getInstance().getDataTable("LevelPreset"); const auto &row : lvlPrest) {
        const auto &levelName = row.at("Name");
        if (levelName == "None" || levelName == "Expansion")
            continue;
        _mapSelections.push_back(levelName);
    }

    _selectedLevelName = _mapSelections.front();
    onLevelChanged(_selectedLevelName);
    if (!_selectedLevelAltName.empty()) {
        _selectedLevelAltName = _mapAltSelections.front();
        loadTile(_selectedLevelAltName);
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
    for (const auto &lvlDetails = Common::DataTableManager::getInstance().getDataTable("LevelDetails"); const Common::DataTableRow &row :
         lvlDetails) {

        if (row.at("Id") == "None" || row.at("Id") == "Expansion" || row.at("Id") == "")
            continue;

        if (const auto &lvlDetailsId = std::stoi(row.at("Id")); lvlDetailsId == levelId)
            return row;

    }
    throw std::runtime_error("LevelDetails not found: " + std::to_string(levelId));
}

const Common::DataTableRow &MapTest::getLevelType(const int levelTypeId) {
    for (const auto &lvlTypes = Common::DataTableManager::getInstance().getDataTable("LevelType"); const Common::DataTableRow &row : lvlTypes) {
        if (row.at("Id") == "None" || row.at("Id") == "Expansion" || row.at("Id") == "")
            continue;

        if (const auto &lvlTypeId = std::stoi(row.at("Id")); lvlTypeId == levelTypeId)
            return row;
    }
    throw std::runtime_error("LevelType not found: " + std::to_string(levelTypeId));
}

void MapTest::update(std::chrono::duration<double> deltaTime) {
}

void MapTest::processEvent(const SDL_Event &event) {
    switch (event.type) {
    case SDL_MOUSEBUTTONDOWN:
        switch (event.button.button) {
        case SDL_BUTTON_RIGHT:
            _isMouseDragging = true;
            int mx;
            int my;
            Abyss::AbyssEngine::getInstance().getMouseState().getPosition(mx, my);
            _mousePressedPosition.x = mx;
            _mousePressedPosition.y = my;
            _mapEngine->getCameraPosition(_startCameraPosition.x, _startCameraPosition.y);
            SDL_CaptureMouse(SDL_TRUE);
            break;
        }
        break;
    case SDL_MOUSEBUTTONUP:
        switch (event.button.button) {
        case SDL_BUTTON_RIGHT:
            SDL_CaptureMouse(SDL_FALSE);
            _isMouseDragging = false;
            break;
        }
        break;
    case SDL_MOUSEMOTION:
        if (_isMouseDragging) {
            // Drag but take scale into account
            int mx;
            int my;
            Abyss::AbyssEngine::getInstance().getMouseState().getPosition(mx, my);
            _mapEngine->setCameraPosition(_startCameraPosition.x + (_mousePressedPosition.x - mx),
                                          _startCameraPosition.y + (_mousePressedPosition.y - my));
        }
        break;
    }
}

void MapTest::loadTile(const std::string &altName) {
    Abyss::Common::Log::debug("Loading level alt: {}", altName);
    const auto &levelPrest = getLevelPrest(_selectedLevelName);
    const auto levelId = std::stoi(levelPrest.at("LevelId"));
    const auto &levelDetails = getLevelDetails(levelId);
    const auto &levelTypeId = std::stoi(levelDetails.at("LevelType"));
    //const auto act = std::stoi(levelDetails.at("Act")) + 1;
    const auto act = std::stoi(_selectedLevelName.substr(4, 1));
    const auto &levelType = getLevelType(levelTypeId);
    const auto &palette = Common::PaletteManager::getInstance().getPalette("Act" + std::to_string(act));

    std::vector<std::string> dt1sToLoad{};

    for (int i = 1; i <= 31; ++i) {
        const auto &file = "File " + std::to_string(i);
        auto dt1 = levelType.at(file);
        std::ranges::transform(dt1, dt1.begin(), [](const auto c) { return std::tolower(c); });
        if (dt1 != "None" && dt1 != "Expansion" && dt1 != "0" && std::ranges::find(dt1sToLoad, dt1) == dt1sToLoad.end()) {
            auto filePath = "/data/global/tiles/" + dt1;
            std::ranges::transform(filePath, filePath.begin(), [](const auto c) { return std::tolower(c); });
            dt1sToLoad.push_back("/data/global/tiles/" + dt1);
        }
    }

    Abyss::DataTypes::DS1 ds1("/data/global/tiles/" + altName);

    // Add all the DS1 files to the DT1s
    for (auto &file : ds1.files) {
        std::ranges::replace(file, '\\', '/');
        if (file.starts_with("/d2/")) // Because why not?
            file = file.substr(3);
        std::ranges::transform(file, file.begin(), [](const auto c) { return std::tolower(c); });
        if (std::ranges::find(dt1sToLoad, file) == dt1sToLoad.end()) {
            absl::StrReplaceAll({{".tg1", ".dt1"}}, &file); // Replace tg1 with dt1 because blizzard
            absl::StrReplaceAll({{".ds1", ".dt1"}}, &file); // Why yes, this is also a thing....
            // remove  /d2 if it starts with it
            if (file.starts_with("/d2/"))
                file = file.substr(3);

            dt1sToLoad.push_back(file);
        }
    }

    const auto dsSubIndex = std::ranges::find(_mapAltSelections, altName) - _mapAltSelections.begin();
    // Load all dt1s into a vector
    std::vector<Abyss::DataTypes::DT1> dt1s{};
    dt1s.reserve(dt1sToLoad.size());
    for (const auto &dt1 : dt1sToLoad)
        dt1s.emplace_back(dt1, palette);

    const auto mapWidth = ds1.width;
    const auto mapHeight = ds1.height;

    _mapEngine = std::make_unique<Abyss::MapEngine::MapEngine>(mapWidth, mapHeight, std::move(dt1s), std::vector{std::move(ds1)});
    _mapEngine->stampDs1(0, 0, 0);

    // Center the map
    const auto mapCenterX = mapWidth / 2;
    const auto mapCenterY = mapHeight / 2;

    // Convert center to ortho
    const auto orthoCenterX = (mapCenterX - mapCenterY) * 80;
    const auto orthoCenterY = (mapCenterX + mapCenterY) * 40;

    // Adjust camera position
    _mapEngine->setCameraPosition(orthoCenterX, orthoCenterY);
}

void MapTest::render() {
    SDL_SetRenderDrawColor(Abyss::AbyssEngine::getInstance().getRenderer(), 0, 0, 0, 255);
    SDL_RenderClear(Abyss::AbyssEngine::getInstance().getRenderer());

    if (_mapEngine)
        _mapEngine->render();

    ImGui::SetNextWindowSize(ImVec2(350, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin("Debug");

    // Level selection
    if (ImGui::BeginCombo("Level", _selectedLevelName.c_str())) {
        for (const auto &levelName : _mapSelections) {
            const bool isSelected = (_selectedLevelName == levelName);
            if (ImGui::Selectable(levelName.c_str(), isSelected))
                if (_selectedLevelName != levelName) {
                    onLevelChanged(levelName);
                    _selectedLevelName = levelName;
                    _selectedLevelAltName = _mapAltSelections.empty() ? "" : _mapAltSelections.front();
                    if (!_selectedLevelAltName.empty())
                        loadTile(_selectedLevelAltName);
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

    ImGui::Separator();

    // Show map information
    if (_mapEngine) {
        int mapWidth;
        int mapHeight;
        _mapEngine->getMapSize(mapWidth, mapHeight);
        int cameraPosX;
        int cameraPosY;
        _mapEngine->getCameraPosition(cameraPosX, cameraPosY);

        ImGui::LabelText("Map size", "%d, %d", mapWidth, mapHeight);
        ImGui::LabelText("Camera position", "%d, %d", cameraPosX, cameraPosY);
        ImGui::LabelText("DS1", "%s", _selectedLevelAltName.c_str());
    }

    ImGui::End();
}

} // namespace OD2::Scenes::MapTest
