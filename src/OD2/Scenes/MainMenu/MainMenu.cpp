#include "MainMenu.h"
#include "OD2/Scenes/Credits/Credits.h"

namespace OD2::Scenes::MainMenu {

int MainMenu::playedIntroVideos = 0;

void MainMenu::onSinglePlayerClicked() {}

void MainMenu::onMultiplayerClicked() {}

void MainMenu::onCreditsClicked() { Abyss::AbyssEngine::getInstance().setScene(std::make_unique<Credits::Credits>()); }

auto MainMenu::onExitClicked() -> void { Abyss::AbyssEngine::getInstance().quit(); }

MainMenu::MainMenu() {
    if (playedIntroVideos >= 2) {
        return;
    }
    constexpr std::string_view webm = "/data/hd/global/video/blizzardlogos.webm";
    constexpr std::string_view flac = "/data/hd/local/video/blizzardlogos.flac";
    if (Abyss::AbyssEngine::getInstance().fileExists(webm)) {
      Abyss::AbyssEngine::getInstance().playVideoAndAudio(webm, flac);
      Abyss::AbyssEngine::getInstance().setBackgroundMusic(Common::ResourcePaths::Music::Title);
      playedIntroVideos = 2;
    } else {
      Abyss::AbyssEngine::getInstance().playVideo(Common::ResourcePaths::Videos::BlizardStartup1);
      playedIntroVideos = 1;
    }
}

void MainMenu::update(const std::chrono::duration<double> deltaTime) {
    if (playedIntroVideos < 2) {
        Abyss::AbyssEngine::getInstance().playVideo(Common::ResourcePaths::Videos::BlizardStartup2);
        Abyss::AbyssEngine::getInstance().setBackgroundMusic(Common::ResourcePaths::Music::Title);
        playedIntroVideos = 2;
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
    _btnMapTest.draw(264, 370);
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
