#pragma once

#include <Abyss/Common/Button.hpp>
#include <Abyss/Common/Scene.hpp>
#include <Abyss/Common/SpriteFont.hpp>
#include <Abyss/DataTypes/DC6.hpp>
#include <OD2/Scenes/MainMenu/Logo.h>
#include <SDL2/SDL.h>

namespace OD2::Scenes::MainMenu {

class MainMenu final : public Abyss::Common::Scene {
    Abyss::DataTypes::DC6 _background;
    Logo _d2Logo;
    Abyss::Common::Button<Abyss::DataTypes::DC6> _btnExit;
    Abyss::Common::SpriteFont<Abyss::DataTypes::DC6> _font;

    auto onExitClicked() -> void;

  public:
    MainMenu();

    auto update([[maybe_unused]] std::chrono::duration<double> deltaTime) -> void override;
    auto processEvent([[maybe_unused]] const SDL_Event &event) -> void override;
    auto render() -> void override;
};

} // namespace OD2::Scenes::MainMenu
