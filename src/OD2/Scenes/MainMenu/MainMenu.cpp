#include "MainMenu.h"

#include "OD2/Common/ButtonDefManager.h"

#include <Abyss/DataTypes/DC6.h>
#include <OD2/Common/PaletteManager.h>
#include <OD2/Common/ResourcePaths.h>

namespace OD2::Scenes::MainMenu {

MainMenu::MainMenu()
    : _background(Abyss::DataTypes::DC6(Common::ResourcePaths::MainMenu::GameSelectScreen)),
      _btnExit(Common::ButtonDefManager::getInstance()["Wide"], "Exit Diablo II", [this] { onExitClicked(); }) {
    _background.setPalette(Common::PaletteManager::getInstance()["Sky"]);
    // Abyss::AbyssEngine::getInstance().setBackgroundMusic(Common::ResourcePaths::Music::Title);
}

auto MainMenu::update([[maybe_unused]] const std::chrono::duration<double> deltaTime) -> void { _d2Logo.update(deltaTime); }

auto MainMenu::processEvent([[maybe_unused]] const SDL_Event &event) -> void {}

auto MainMenu::render() -> void {
    _background.draw(0, 0, 0, 4, 3);
    _d2Logo.render();

    _btnExit.draw(264, 535);
}

auto MainMenu::onExitClicked() -> void {}

} // namespace OD2::Scenes::MainMenu
