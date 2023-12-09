#pragma once

#include "Abyss/DataTypes/DC6.h"
#include "OD2/Common/PaletteManager.h"
#include "OD2/Common/ResourcePaths.h"

#include <chrono>

namespace OD2::Scenes::MainMenu {

class Logo {
    Abyss::DataTypes::DC6Animation _logoLeftBlack;
    Abyss::DataTypes::DC6Animation _logoLeft;
    Abyss::DataTypes::DC6Animation _logoRightBlack;
    Abyss::DataTypes::DC6Animation _logoRight;

  public:
    Logo();
    void render();
    void update(const std::chrono::duration<double> deltaTime);
};

} // namespace OD2::Scenes::MainMenu
