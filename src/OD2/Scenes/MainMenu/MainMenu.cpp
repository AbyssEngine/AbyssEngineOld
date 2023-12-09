#include "MainMenu.h"

namespace OD2::Scenes::MainMenu {

bool MainMenu::playedIntroVideo = false;

void MainMenu::onSinglePlayerClicked() {}

void MainMenu::onMultiplayerClicked() {}

void MainMenu::onCreditsClicked() {}

auto MainMenu::onExitClicked() -> void { Abyss::AbyssEngine::getInstance().quit(); }

MainMenu::MainMenu() { Abyss::AbyssEngine::getInstance().playVideo(Common::ResourcePaths::Videos::BlizardStartup1); }

void MainMenu::update(const std::chrono::duration<double> deltaTime) {
    if (!playedIntroVideo) {
        Abyss::AbyssEngine::getInstance().playVideo(Common::ResourcePaths::Videos::BlizardStartup2);
        Abyss::AbyssEngine::getInstance().setBackgroundMusic(Common::ResourcePaths::Music::Title);
        playedIntroVideo = true;
        return;
    }
    _d2Logo.update(deltaTime);
}

void MainMenu::processEvent(const SDL_Event &event) {
    if (_screenMode == ScreenMode::TradeMark && event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        _screenMode = ScreenMode::MainMenu;
    }
}

void MainMenu::renderTrademark() {
    _trademarkBackground.draw(0, 0, 0, 4, 3);
    _d2Logo.render();
}

void MainMenu::renderMainMenu() {
    _background.draw(0, 0, 0, 4, 3);
    _d2Logo.render();
    _btnSinglePlayer.draw(264, 290);
    _btnMultiPlayer.draw(264, 330);
    _btnCredits.draw(264, 495);
    _btnExit.draw(264, 535);

    _lblCredits.drawCentered(400, 586);
}

void MainMenu::render() {
    switch (_screenMode) {
    case ScreenMode::TradeMark:
        renderTrademark();
        break;
    case ScreenMode::MainMenu:
        renderMainMenu();
        break;
    }
}

} // namespace OD2::Scenes::MainMenu
