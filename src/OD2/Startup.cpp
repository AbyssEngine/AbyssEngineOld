#include "Startup.h"

#include "Common/ButtonDefManager.h"
#include "Common/PaletteManager.h"
#include "Common/ResourcePaths.h"
#include <Abyss/Common/Logging.hpp>

namespace OD2 {

auto loadPalettes() -> void {
    Abyss::Common::Log::info("Loading palettes...");
    auto &paletteManager = Common::PaletteManager::getInstance();
    paletteManager.addPalette({Common::ResourcePaths::Palettes::Act1, "Act1"});
    paletteManager.addPalette({Common::ResourcePaths::Palettes::Act2, "Act2"});
    paletteManager.addPalette({Common::ResourcePaths::Palettes::Act3, "Act3"});
    paletteManager.addPalette({Common::ResourcePaths::Palettes::Act4, "Act4"});
    paletteManager.addPalette({Common::ResourcePaths::Palettes::Act5, "Act5"});
    paletteManager.addPalette({Common::ResourcePaths::Palettes::EndGame, "EndGame"});
    paletteManager.addPalette({Common::ResourcePaths::Palettes::EndGame2, "EndGame2"});
    paletteManager.addPalette({Common::ResourcePaths::Palettes::Fechar, "Fechar"});
    paletteManager.addPalette({Common::ResourcePaths::Palettes::Loading, "Loading"});
    paletteManager.addPalette({Common::ResourcePaths::Palettes::Menu0, "Menu0"});
    paletteManager.addPalette({Common::ResourcePaths::Palettes::Menu1, "Menu1"});
    paletteManager.addPalette({Common::ResourcePaths::Palettes::Menu2, "Menu2"});
    paletteManager.addPalette({Common::ResourcePaths::Palettes::Menu3, "Menu3"});
    paletteManager.addPalette({Common::ResourcePaths::Palettes::Menu4, "Menu4"});
    paletteManager.addPalette({Common::ResourcePaths::Palettes::Sky, "Sky"});
    paletteManager.addPalette({Common::ResourcePaths::Palettes::Static, "Static"});
    paletteManager.addPalette({Common::ResourcePaths::Palettes::Trademark, "Trademark"});
    paletteManager.addPalette({Common::ResourcePaths::Palettes::Units, "Units"});
}

auto loadButtonDefs() -> void {
    Abyss::Common::Log::info("Loading button definitions...");
    auto &buttonDefsMap = Common::ButtonDefManager::getInstance();
    auto &paletteManager = Common::PaletteManager::getInstance();
    buttonDefsMap.addButtonDef({
        .name = "Wide",
        .resourceName = Common::ResourcePaths::UI::WideButtonBlank,
        .palette = paletteManager.getPalette("Units"),
        .fontPath = Common::ResourcePaths::FontsAndLocales::FontExocet10,
        .clickableRect = {.x = 6, .y = 1, .w = 259, .h = 32},
        .segments = {.x = 2, .y = 1},
        .frames = {.base = 0, .pressed = 1, .disabled = -1},
    });
}

auto startup() -> void {
    Abyss::Common::Log::info("Starting up...");
    loadPalettes();
    loadButtonDefs();
    Abyss::Common::Log::info("Startup complete");
}

} // namespace OD2
