module;

#include <SDL2/SDL.h>
#include <chrono>

export module OD2.Scenes.MainMenu;

import Abyss.AbyssEngine;
import Abyss.UI.Button;
import Abyss.UI.Label;
import Abyss.Common.Scene;
import Abyss.DataTypes.DC6;
import OD2.Common.ButtonDefManager;
import OD2.Common.PaletteManager;
import OD2.Common.ResourcePaths;
import OD2.Common.FontManager;
import OD2.Scenes.MainMenu.Logo;

namespace OD2::Scenes::MainMenu {

enum class ScreenMode {
    TradeMark,
    MainMenu,
};

export class MainMenu final : public Abyss::Common::Scene {
    ScreenMode _screenMode = ScreenMode::TradeMark;
    static bool playedIntroVideo;

    Abyss::DataTypes::DC6 _background = Abyss::DataTypes::DC6(Common::ResourcePaths::MainMenu::GameSelectScreen, GetPalette("Sky"));
    Abyss::DataTypes::DC6 _trademarkBackground = Abyss::DataTypes::DC6(Common::ResourcePaths::MainMenu::TrademarkScreen, GetPalette("Sky"));
    Logo _d2Logo;

    Abyss::UI::Button<Abyss::DataTypes::DC6> _btnSinglePlayer = CreateButton("Wide", "SINGLE PLAYER", [this] { onSinglePlayerClicked(); });
    Abyss::UI::Button<Abyss::DataTypes::DC6> _btnMultiPlayer = CreateButton("Wide", "MULTIPLAYER", [this] { onMultiplayerClicked(); });
    Abyss::UI::Button<Abyss::DataTypes::DC6> _btnCredits = CreateButton("Wide", "CREDITS", [this] { onCreditsClicked(); });
    Abyss::UI::Button<Abyss::DataTypes::DC6> _btnExit = CreateButton("Wide", "EXIT DIABLO II", [this] { onExitClicked(); });

    Abyss::UI::Label _lblCredits = {GetFont("fontformal10"),
                                    {0xff, 0xff, 0x8c, 0xff},
                                    "OpenDiablo2 is neither developed by, nor endorsed by Blizzard or its parent company Activision/Blizzard/Microsoft"};

    auto onSinglePlayerClicked() -> void {}

    auto onMultiplayerClicked() -> void {}

    auto onCreditsClicked() -> void {}

    auto onExitClicked() -> void { Abyss::AbyssEngine::getInstance().quit(); }

  public:
    MainMenu() { Abyss::AbyssEngine::getInstance().playVideo(Common::ResourcePaths::Videos::BlizardStartup1); }

    auto update([[maybe_unused]] const std::chrono::duration<double> deltaTime) -> void {
        if (!playedIntroVideo) {
            Abyss::AbyssEngine::getInstance().playVideo(Common::ResourcePaths::Videos::BlizardStartup2);
            Abyss::AbyssEngine::getInstance().setBackgroundMusic(Common::ResourcePaths::Music::Title);
            playedIntroVideo = true;
            return;
        }
        _d2Logo.update(deltaTime);
    }

    auto processEvent([[maybe_unused]] const SDL_Event &event) -> void {
        if (_screenMode == ScreenMode::TradeMark && event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            _screenMode = ScreenMode::MainMenu;
        }
    }

    auto renderTrademark() -> void {
        _trademarkBackground.draw(0, 0, 0, 4, 3);
        _d2Logo.render();
    }

    auto renderMainMenu() -> void {
        _background.draw(0, 0, 0, 4, 3);
        _d2Logo.render();
        _btnSinglePlayer.draw(264, 290);
        _btnMultiPlayer.draw(264, 330);
        _btnCredits.draw(264, 495);
        _btnExit.draw(264, 535);

        _lblCredits.drawCentered(400, 586);
    }

    auto render() -> void {
        switch (_screenMode) {
        case ScreenMode::TradeMark:
            renderTrademark();
            break;
        case ScreenMode::MainMenu:
            renderMainMenu();
            break;
        }
    }
};

bool MainMenu::playedIntroVideo = false;

} // namespace OD2::Scenes::MainMenu
