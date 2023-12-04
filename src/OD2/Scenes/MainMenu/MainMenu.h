#pragma once

#include <Abyss/AbyssEngine.h>
#include <Abyss/Common/Button.h>
#include <Abyss/Common/Scene.h>
#include <Abyss/DataTypes/DC6.h>
#include <OD2/Scenes/MainMenu/Logo.h>
#include <SDL2/SDL.h>

namespace OD2::Scenes::MainMenu {

class MainMenu final : public Abyss::Common::Scene {
    Abyss::DataTypes::DC6 _background;
    Logo _d2Logo;
    Abyss::Common::Button<Abyss::DataTypes::DC6> _btnExit;
    auto onExitClicked() -> void;

  public:
    MainMenu();

    auto update([[maybe_unused]] std::chrono::duration<double> deltaTime) -> void override;
    auto processEvent([[maybe_unused]] const SDL_Event &event) -> void override;
    auto render() -> void override;
};

} // namespace OD2::Scenes::MainMenu
