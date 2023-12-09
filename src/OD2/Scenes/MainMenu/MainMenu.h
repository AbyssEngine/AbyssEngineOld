#pragma once

#include "Abyss/AbyssEngine.h"
#include "Abyss/Common/Scene.h"
#include "Abyss/DataTypes/DC6.h"
#include "Abyss/UI/Button.h"
#include "Logo.h"
#include "OD2/Common/ButtonDefManager.h"
#include "OD2/Common/FontManager.h"
#include "OD2/Common/PaletteManager.h"
#include "OD2/Common/ResourcePaths.h"

#include <SDL2/SDL.h>
#include <chrono>

namespace OD2::Scenes::MainMenu {

enum class ScreenMode {
    TradeMark,
    MainMenu,
};

class MainMenu final : public Abyss::Common::Scene {
    ScreenMode _screenMode = ScreenMode::TradeMark;
    static bool playedIntroVideo;

    Abyss::DataTypes::DC6 _background = Abyss::DataTypes::DC6(Common::ResourcePaths::MainMenu::GameSelectScreen, Common::GetPalette("Sky"));
    Abyss::DataTypes::DC6 _trademarkBackground = Abyss::DataTypes::DC6(Common::ResourcePaths::MainMenu::TrademarkScreen, Common::GetPalette("Sky"));
    Logo _d2Logo;

    Abyss::UI::Label _lblCredits = {Common::GetFont("fontformal10"),
                                    {0xff, 0xff, 0x8c, 0xff},
                                    "OpenDiablo2 is neither developed by, nor endorsed by Blizzard or its parent company Activision/Blizzard/Microsoft"};

    void onSinglePlayerClicked();
    void onMultiplayerClicked();
    void onCreditsClicked();
    auto onExitClicked() -> void;

    Abyss::UI::Button<Abyss::DataTypes::DC6> _btnSinglePlayer = Common::CreateButton("Wide", "SINGLE PLAYER", [this] { onSinglePlayerClicked(); });
    Abyss::UI::Button<Abyss::DataTypes::DC6> _btnMultiPlayer = Common::CreateButton("Wide", "MULTIPLAYER", [this] { onMultiplayerClicked(); });
    Abyss::UI::Button<Abyss::DataTypes::DC6> _btnCredits = Common::CreateButton("Wide", "CREDITS", [this] { onCreditsClicked(); });
    Abyss::UI::Button<Abyss::DataTypes::DC6> _btnExit = Common::CreateButton("Wide", "EXIT DIABLO II", [this] { onExitClicked(); });

  public:
    MainMenu();
    void update(std::chrono::duration<double> deltaTime) override;
    void processEvent(const SDL_Event &event) override;
    void renderTrademark();
    void renderMainMenu();
    void render() override;
};

} // namespace OD2::Scenes::MainMenu
