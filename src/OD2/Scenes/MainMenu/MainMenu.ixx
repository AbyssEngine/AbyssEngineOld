module;

#include <SDL2/SDL.h>
#include <chrono>

export module OD2.Scenes.MainMenu;

import Abyss.AbyssEngine;
import Abyss.UI.Button;
import Abyss.Common.Scene;
import Abyss.DataTypes.DC6;
import OD2.Scenes.MainMenu.Logo;
import OD2.Common.ButtonDefManager;
import OD2.Common.PaletteManager;
import OD2.Common.ResourcePaths;
import OD2.Common.FontManager;
import Abyss.UI.Label;

namespace OD2::Scenes::MainMenu {

export class MainMenu final : public Abyss::Common::Scene {
    Abyss::DataTypes::DC6 _background;
    Logo _d2Logo;

    Abyss::UI::Button<Abyss::DataTypes::DC6> _btnCredits;
    Abyss::UI::Button<Abyss::DataTypes::DC6> _btnExit;

    auto onCreditsClicked() -> void {}

    auto onExitClicked() -> void { Abyss::AbyssEngine::getInstance().quit(); }

  public:
    MainMenu()
        : _background(Abyss::DataTypes::DC6(Common::ResourcePaths::MainMenu::GameSelectScreen)),
          _btnCredits(CreateButton("Wide", "Credits", [this] { onCreditsClicked(); })),
          _btnExit(CreateButton("Wide", "Exit Diablo II", [this] { onExitClicked(); })) {
        _background.setPalette(GetPalette("Sky"));
        // Abyss::AbyssEngine::getInstance().setBackgroundMusic(Common::ResourcePaths::Music::Title);
    }

    auto update([[maybe_unused]] const std::chrono::duration<double> deltaTime) -> void { _d2Logo.update(deltaTime); }

    auto processEvent([[maybe_unused]] const SDL_Event &event) -> void {}

    auto render() -> void {
        _background.draw(0, 0, 0, 4, 3);
        _d2Logo.render();
        _btnCredits.draw(264, 500);
        _btnExit.draw(264, 535);
    }
};

} // namespace OD2::Scenes::MainMenu
