module;

#include <memory>

import Abyss.Common.Logging;
import OD2.Common.ResourcePaths;
import Abyss.Streams.InputStream;
import Abyss.Common.MouseProvider;
import Abyss.Common.MouseState;
import Abyss.Common.RendererProvider;
import Abyss.Enums.BlendMode;
import Abyss.Common.FileProvider;
import OD2.Common.PaletteManager;
import OD2.Common.ButtonDefManager;
import OD2.Common.PaletteManager;
import OD2.Common.ResourcePaths;
import OD2.Scenes.MainMenu;
import Abyss.AbyssEngine;
import Abyss.Common.Logging;
import OD2.Common.ButtonDefManager;
import Abyss.AbyssEngine;

export module OD2.Startup;

namespace OD2 {

export auto loadPalettes() -> void {
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

export auto loadButtonDefs() -> void {
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

export auto startup(const int argc, char **argv) -> int {
    Abyss::Common::Log::info("Starting up...");

    try {
        auto &engine = Abyss::AbyssEngine::getInstance();

        if (!engine.processCommandLineArguments(argc, argv))
            return 0;

        engine.setWindowTitle("OpenDiablo II");

        loadPalettes();
        loadButtonDefs();

        engine.addCursorImage("Default", Common::ResourcePaths::MousePointers::CursorDefault, Common::PaletteManager::getInstance().getPalette("Static"));
        engine.setCursorImage("Default");
        engine.getMouseState().setVisible(true);
        engine.setScene(std::make_unique<Scenes::MainMenu::MainMenu>());

        Abyss::Common::Log::info("Startup complete");

        engine.run();
    } catch (const std::exception &exception) {
        Abyss::Common::Log::error("{}", exception.what());
        // SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Abyss Engine", exception.what(), nullptr);
        return 1;
    } catch (...) {
        Abyss::Common::Log::error("Unknown exception");
        // SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Abyss Engine", "Unknown exception", nullptr);
        return 2;
    }

    return 0;
}

} // namespace OD2