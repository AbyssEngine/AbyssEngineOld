#pragma once

#include "Abyss/Common/Scene.h"
#include "Abyss/DataTypes/DS1.h"
#include "Abyss/DataTypes/DT1.h"
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
    std::vector<Abyss::DataTypes::DT1> _dt1s;
    std::unique_ptr<Abyss::DataTypes::DS1> _ds1{};
    SDL_Point _cameraPosition{0, 0};
    SDL_Point _mousePressedPosition{0, 0};
    SDL_Point _startCameraPosition{0, 0};

    bool _isMouseDragging{false};

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
