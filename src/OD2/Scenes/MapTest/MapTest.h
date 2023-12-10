#pragma once

#include "Abyss/Common/Scene.h"
#include "Abyss/DataTypes/DT1.h"

namespace OD2::Scenes::MapTest {

class MapTest final : public Abyss::Common::Scene {
    Abyss::DataTypes::DT1 testdt1;
    Abyss::DataTypes::DT1 testdt2;

  public:
    MapTest();
    void update(std::chrono::duration<double> deltaTime) override;
    void processEvent(const SDL_Event &event) override;
    void render() override;
};

} // namespace OD2::Scenes::MapTest
