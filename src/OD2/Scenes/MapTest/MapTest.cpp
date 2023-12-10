#include "MapTest.h"

#include "Abyss/AbyssEngine.h"
#include "OD2/Common/PaletteManager.h"

#include <imgui.h>

namespace OD2::Scenes::MapTest {

MapTest::MapTest()
    : testdt1("/data/global/tiles/ACT1/TOWN/floor.dt1", Common::PaletteManager::getInstance().getPalette("Act1")),
      testdt2("/data/global/tiles/ACT1/TOWN/fence.dt1", Common::PaletteManager::getInstance().getPalette("Act1")) {
    //
}

void MapTest::update(std::chrono::duration<double> deltaTime) {}

void MapTest::processEvent(const SDL_Event &event) {}

void MapTest::render() {
    const auto &renderer = Abyss::AbyssEngine::getInstance().getRenderer();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    testdt1.drawTile(480, 240, 4);
    testdt1.drawTile(400, 200, 5);
    testdt1.drawTile(560, 200, 6);
    testdt1.drawTile(480, 160, 7);
    testdt2.drawTile(480, 240, 6);

    ImGui::SetNextWindowSize(ImVec2(250, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin("Debug");

    ImGui::End();
}

} // namespace OD2::Scenes::MapTest
