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
            _startCameraPosition = _cameraPosition;
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
            _cameraPosition.x = _startCameraPosition.x + (_mousePressedPosition.x - mx);
            _cameraPosition.y = _startCameraPosition.y + (_mousePressedPosition.y - my);
        }
        break;
    case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_w:
            _cameraPosition.y -= 100;
            break;
        case SDLK_s:
            _cameraPosition.y += 100;
            break;
        case SDLK_a:
            _cameraPosition.x -= 100;
            break;
        case SDLK_d:
            _cameraPosition.x += 100;
            break;
        default:
            break;
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

    _dt1s.clear();
    std::vector<std::string> dt1sToLoad{};

    for (int i = 1; i <= 31; ++i) {
        const auto &file = "File " + std::to_string(i);
        auto dt1 = levelType.at(file);
        std::ranges::transform(dt1, dt1.begin(), [](const auto c) { return std::tolower(c); });
        if (dt1 != "None" && dt1 != "Expansion" && dt1 != "0" && std::ranges::find(dt1sToLoad, dt1) == dt1sToLoad.end()) {
            auto filePath = "/data/global/tiles/" + dt1;
            std::ranges::transform(filePath, filePath.begin(), [](const auto c) { return std::tolower(c); });
            _dt1s.emplace_back("/data/global/tiles/" + dt1, palette);
            dt1sToLoad.push_back("/data/global/tiles/" + dt1);
        }
    }

    _ds1 = std::make_unique<Abyss::DataTypes::DS1>("/data/global/tiles/" + altName);

    // Add all the DS1 files to the DT1s
    for (auto &file : _ds1->files) {
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

            _dt1s.emplace_back(file, palette);
            dt1sToLoad.push_back(file);
        }
    }

    _ds1->bindTileReferences(_dt1s);

    // // Center the map
    const auto mapCenterX = _ds1->width / 2;
    const auto mapCenterY = _ds1->height / 2;
    //
    // // Convert center to ortho
    const auto orthoCenterX = (mapCenterX - mapCenterY) * 80;
    const auto orthoCenterY = (mapCenterX + mapCenterY) * 40;

    // Adjust camera position
    _cameraPosition.x = orthoCenterX - 320;
    _cameraPosition.y = orthoCenterY - 260;

}

void MapTest::render() {
    const auto &renderer = Abyss::AbyssEngine::getInstance().getRenderer();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (_ds1) {
        for (int tileY = 0; tileY < _ds1->height; ++tileY) {
            for (int tileX = 0; tileX < _ds1->width; ++tileX) {
                const auto posX = (tileX - tileY) * 80;
                const auto posY = (tileX + tileY) * 40;

                // Draw lower walls
                for (const auto &layer : _ds1->layers.wall) {
                    if (const auto &tile = layer[tileX + tileY * _ds1->width];
                        tile.dt1Ref && tile.type >= Abyss::DataTypes::TileType::LowerWallsEquivalentToLeftWall)
                        tile.dt1Ref->drawTile(posX - _cameraPosition.x, posY - _cameraPosition.y, tile.dt1Index);
                }

                // Draw floors
                for (const auto &layer : _ds1->layers.floor) {
                    if (const auto &tile = layer[tileX + tileY * _ds1->width]; tile.dt1Ref)
                        tile.dt1Ref->drawTile(posX - _cameraPosition.x, posY - _cameraPosition.y, tile.dt1Index);
                }
                // Draw shadows
                for (const auto &layer : _ds1->layers.shadow) {
                    if (const auto &tile = layer[tileX + tileY * _ds1->width]; tile.dt1Ref)
                        tile.dt1Ref->drawTile(posX - _cameraPosition.x, posY - _cameraPosition.y, tile.dt1Index);
                }
            }
        }

        for (int tileY = 0; tileY < _ds1->height; ++tileY) {
            for (int tileX = 0; tileX < _ds1->width; ++tileX) {
                const auto posX = (tileX - tileY) * 80;
                const auto posY = (tileX + tileY) * 40;

                // Draw upper
                for (const auto &layer : _ds1->layers.wall) {
                    if (const auto &tile = layer[tileX + tileY * _ds1->width];
                        tile.dt1Ref && ((tile.type >= Abyss::DataTypes::TileType::LeftWall && tile.type <=
                                         Abyss::DataTypes::TileType::RightWallWithDoor) || (
                                            tile.type >= Abyss::DataTypes::TileType::PillarsColumnsAndStandaloneObjects && tile.type <=
                                            Abyss::DataTypes::TileType::Tree))) {
                        tile.dt1Ref->drawTile(posX - _cameraPosition.x, posY - _cameraPosition.y + 96, tile.dt1Index);

                        // Super special condition. This was fun to figure out :(
                        if (tile.type == Abyss::DataTypes::TileType::RightPartOfNorthCornerWall)
                            tile.dt1Ref->drawTile(posX - _cameraPosition.x, posY - _cameraPosition.y + 96, tile.dt1IndexAlt);
                    }

                }

            }
        }

        for (int tileY = 0; tileY < _ds1->height; ++tileY) {
            for (int tileX = 0; tileX < _ds1->width; ++tileX) {
                const auto posX = (tileX - tileY) * 80;
                const auto posY = (tileX + tileY) * 40;

                // Draw Roof
                for (const auto &layer : _ds1->layers.wall) {
                    if (const auto &tile = layer[tileX + tileY * _ds1->width];
                        tile.dt1Ref && tile.type == Abyss::DataTypes::TileType::Roof)
                        tile.dt1Ref->drawTile(posX - _cameraPosition.x, posY - _cameraPosition.y - 72, tile.dt1Index);
                }

            }
        }
    }

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
                    _selectedLevelAltName = "";
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
