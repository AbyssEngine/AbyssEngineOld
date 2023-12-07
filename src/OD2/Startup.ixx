module;

#include <memory>
#include <string>

export module OD2.Startup;

import Abyss.Common.Logging;
import OD2.Common.ResourcePaths;
import Abyss.FileSystem.InputStream;
import Abyss.Common.MouseProvider;
import Abyss.Common.MouseState;
import Abyss.Common.RendererProvider;
import Abyss.Enums.BlendMode;
import Abyss.FileSystem.FileLoader;
import OD2.Common.PaletteManager;
import OD2.Common.ButtonDefManager;
import OD2.Common.PaletteManager;
import OD2.Common.ResourcePaths;
import OD2.Scenes.MainMenu;
import Abyss.AbyssEngine;
import Abyss.Common.Logging;
import OD2.Common.ButtonDefManager;
import Abyss.AbyssEngine;
import OD2.Common.FontManager;
import Abyss.UI.SpriteFont;
import Abyss.DataTypes.DC6;

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
        .font = "btntext",
        .clickableRect = {.x = 6, .y = 1, .w = 259, .h = 32},
        .segments = {.x = 2, .y = 1},
        .frames = {.base = 0, .pressed = 1, .disabled = -1},
    });
}

auto loadFonts() -> void {
    const auto addFont = [](const std::string_view fontPath, const std::string paletteName) -> void {
        const auto getFontName = [](const std::string_view path) -> std::string {
            const auto pos = path.find_last_of('/');
            if (pos == std::string_view::npos)
                return std::string(path);

            return std::string(path.substr(pos + 1));
        };

        Common::FontManager::getInstance().addFont(
            getFontName(fontPath), std::move(std::make_unique<Abyss::UI::SpriteFont<Abyss::DataTypes::DC6>>(fontPath, GetPalette(paletteName))));
    };

    Abyss::Common::Log::info("Loading fonts...");
    addFont(Common::ResourcePaths::FontsAndLocales::Font6, "Static");
    addFont(Common::ResourcePaths::FontsAndLocales::Font8, "Static");
    addFont(Common::ResourcePaths::FontsAndLocales::Font16, "Static");
    addFont(Common::ResourcePaths::FontsAndLocales::Font24, "Static");
    addFont(Common::ResourcePaths::FontsAndLocales::Font30, "Static");
    addFont(Common::ResourcePaths::FontsAndLocales::Font42, "Static");
    addFont(Common::ResourcePaths::FontsAndLocales::FontFormal12, "Static");
    addFont(Common::ResourcePaths::FontsAndLocales::FontFormal11, "Static");
    addFont(Common::ResourcePaths::FontsAndLocales::FontFormal10, "Static");
    addFont(Common::ResourcePaths::FontsAndLocales::FontExocet10, "Static");
    addFont(Common::ResourcePaths::FontsAndLocales::FontExocet8, "Static");
    addFont(Common::ResourcePaths::FontsAndLocales::FontSucker, "Static");
    addFont(Common::ResourcePaths::FontsAndLocales::FontRediculous, "Static");

    Common::FontManager::getInstance().addFont("btntext", std::move(std::make_unique<Abyss::UI::SpriteFont<Abyss::DataTypes::DC6>>(
                                                              Common::ResourcePaths::FontsAndLocales::FontExocet10, GetPalette("Units"))));
}

export auto startup(const int argc, char **argv) -> int {
    Abyss::Common::Log::info("Starting up...");

    try {
        auto &engine = Abyss::AbyssEngine::getInstance();

        if (!engine.processCommandLineArguments(argc, argv))
            return 0;

        engine.setWindowTitle("OpenDiablo II");

        engine.initializeFiles();
        loadPalettes();
        loadButtonDefs();
        loadFonts();

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
