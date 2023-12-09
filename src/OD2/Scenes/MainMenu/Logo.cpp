#include "Logo.h"

namespace OD2::Scenes::MainMenu {

Logo::Logo()
    : _logoLeftBlack(Common::ResourcePaths::MainMenu::Diablo2LogoBlackLeft), _logoLeft(Common::ResourcePaths::MainMenu::Diablo2LogoFireLeft),
      _logoRightBlack(Common::ResourcePaths::MainMenu::Diablo2LogoBlackRight), _logoRight(Common::ResourcePaths::MainMenu::Diablo2LogoFireRight) {
    const auto paletteUnits = Common::PaletteManager::getInstance().getPalette("Units");

    _logoLeftBlack.setPalette(paletteUnits);
    _logoLeftBlack.setBlendMode(Abyss::Enums::BlendMode::Blend);

    _logoLeft.setPalette(paletteUnits);
    _logoLeft.setBlendMode(Abyss::Enums::BlendMode::Add);

    _logoRightBlack.setPalette(paletteUnits);
    _logoRightBlack.setBlendMode(Abyss::Enums::BlendMode::Blend);

    _logoRight.setPalette(paletteUnits);
    _logoRight.setBlendMode(Abyss::Enums::BlendMode::Add);
}

void Logo::render() {
    _logoLeftBlack.draw(400, 120);
    _logoLeft.draw(400, 120);
    _logoRightBlack.draw(400, 120);
    _logoRight.draw(400, 120);
}

void Logo::update(const std::chrono::duration<double> deltaTime) {
    _logoLeft.update(deltaTime);
    _logoLeftBlack.update(deltaTime);
    _logoRight.update(deltaTime);
    _logoRightBlack.update(deltaTime);
}

} // namespace OD2::Scenes::MainMenu
