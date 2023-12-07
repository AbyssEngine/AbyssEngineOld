module;

#include <SDL2/SDL.h>
#include <chrono>

export module OD2.Scenes.MainMenu;

import Abyss.UI.Button;
import Abyss.Common.Scene;
import Abyss.UI.SpriteFont;
import Abyss.DataTypes.DC6;
import OD2.Scenes.MainMenu.Logo;
import OD2.Common.PaletteManager;
import OD2.Common.ButtonDefManager;
import OD2.Common.ResourcePaths;
import Abyss.UI.Label;

namespace OD2::Scenes::MainMenu {

export class MainMenu final : public Abyss::Common::Scene {
    Abyss::DataTypes::DC6 _background;
    Logo _d2Logo;
    Abyss::UI::Button<Abyss::DataTypes::DC6> _btnExit;
    Abyss::UI::SpriteFont<Abyss::DataTypes::DC6> _font;
    Abyss::UI::Label _lblTest;

    auto onExitClicked() -> void {}

  public:
    MainMenu()
        : _background(Abyss::DataTypes::DC6(Common::ResourcePaths::MainMenu::GameSelectScreen)),
          _btnExit(Common::ButtonDefManager::getInstance().getButtonDef("Wide"), "Exit Diablo II", [this] { onExitClicked(); }),
          _font(Common::ResourcePaths::FontsAndLocales::FontExocet10, Common::PaletteManager::getInstance().getPalette("Static")), _lblTest(_font) {
        _background.setPalette(Common::PaletteManager::getInstance().getPalette("Sky"));
        _lblTest.setText("EXIT DIABLO II");
        // Abyss::AbyssEngine::getInstance().setBackgroundMusic(Common::ResourcePaths::Music::Title);
    }

    auto update([[maybe_unused]] const std::chrono::duration<double> deltaTime) -> void { _d2Logo.update(deltaTime); }

    auto processEvent([[maybe_unused]] const SDL_Event &event) -> void {}

    auto render() -> void {
        _background.draw(0, 0, 0, 4, 3);
        _d2Logo.render();
        _btnExit.draw(264, 535);
        _lblTest.draw(314, 545);
    }
};

} // namespace OD2::Scenes::MainMenu
