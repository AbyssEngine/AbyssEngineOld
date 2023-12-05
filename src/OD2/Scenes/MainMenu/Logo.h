#pragma once

#include <Abyss/Common/Animation.hpp>
#include <Abyss/DataTypes/DC6.hpp>

namespace OD2::Scenes::MainMenu {

class Logo {
    Abyss::DataTypes::DC6Animation _logoLeftBlack;
    Abyss::DataTypes::DC6Animation _logoLeft;
    Abyss::DataTypes::DC6Animation _logoRightBlack;
    Abyss::DataTypes::DC6Animation _logoRight;

  public:
    Logo();
    auto render() -> void;
    auto update(std::chrono::duration<double> deltaTime) -> void;
};

} // namespace OD2::Scenes::MainMenu
