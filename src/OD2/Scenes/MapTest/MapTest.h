#pragma once

#include "Abyss/Common/Scene.h"
#include "Abyss/MapEngine/MapEngine.h"
#include "OD2/Common/DataTableManager.h"

#include <memory>
#include <string>
#include <vector>

namespace OD2::Scenes::MapTest {

class MapTest final : public Abyss::Common::Scene {
    std::vector<std::string> _mapSelections{};
    std::vector<std::string> _mapAltSelections{};
    std::string _selectedLevelName{};
    std::string _selectedLevelAltName{};
    SDL_Point _mousePressedPosition{0, 0};
    SDL_Point _startCameraPosition{0, 0};
    bool _isMouseDragging{false};
    std::unique_ptr<Abyss::MapEngine::MapEngine> _mapEngine;
    int _mapWidth;

    void onLevelChanged(const std::string &levelName);
    static const Common::DataTableRow &getLevelPrest(std::string_view name);
    static const Common::DataTableRow &getLevelDetails(int levelId);
    static const Common::DataTableRow &getLevelType(int levelTypeId);

public:
    MapTest();
    void update(std::chrono::duration<double> deltaTime) override;
    void processEvent(const SDL_Event &event) override;
    void loadTile(const std::string &altName);
    void render() override;
};

} // namespace OD2::Scenes::MapTest
