#include "Abyss/AbyssEngine.h"
#include "Abyss/Common/Logging.h"
#include "Abyss/DataTypes/DC6.h"
#include "Abyss/UI/SpriteFont.h"
#include "Common/ButtonDefManager.h"
#include "Common/FontManager.h"
#include "Common/PaletteManager.h"
#include "Common/ResourcePaths.h"
#include "Scenes/MainMenu/MainMenu.h"

void loadPalettes() {
    Abyss::Common::Log::info("Loading palettes...");
    auto &paletteManager = OD2::Common::PaletteManager::getInstance();
    paletteManager.addPalette({OD2::Common::ResourcePaths::Palettes::Act1, "Act1"});
    paletteManager.addPalette({OD2::Common::ResourcePaths::Palettes::Act2, "Act2"});
    paletteManager.addPalette({OD2::Common::ResourcePaths::Palettes::Act3, "Act3"});
    paletteManager.addPalette({OD2::Common::ResourcePaths::Palettes::Act4, "Act4"});
    paletteManager.addPalette({OD2::Common::ResourcePaths::Palettes::Act5, "Act5"});
    paletteManager.addPalette({OD2::Common::ResourcePaths::Palettes::EndGame, "EndGame"});
    paletteManager.addPalette({OD2::Common::ResourcePaths::Palettes::EndGame2, "EndGame2"});
    paletteManager.addPalette({OD2::Common::ResourcePaths::Palettes::Fechar, "Fechar"});
    paletteManager.addPalette({OD2::Common::ResourcePaths::Palettes::Loading, "Loading"});
    paletteManager.addPalette({OD2::Common::ResourcePaths::Palettes::Menu0, "Menu0"});
    paletteManager.addPalette({OD2::Common::ResourcePaths::Palettes::Menu1, "Menu1"});
    paletteManager.addPalette({OD2::Common::ResourcePaths::Palettes::Menu2, "Menu2"});
    paletteManager.addPalette({OD2::Common::ResourcePaths::Palettes::Menu3, "Menu3"});
    paletteManager.addPalette({OD2::Common::ResourcePaths::Palettes::Menu4, "Menu4"});
    paletteManager.addPalette({OD2::Common::ResourcePaths::Palettes::Sky, "Sky"});
    paletteManager.addPalette({OD2::Common::ResourcePaths::Palettes::Static, "Static"});
    paletteManager.addPalette({OD2::Common::ResourcePaths::Palettes::Trademark, "Trademark"});
    paletteManager.addPalette({OD2::Common::ResourcePaths::Palettes::Units, "Units"});
}

void loadButtonDefs() {
    Abyss::Common::Log::info("Loading button definitions...");
    auto &buttonDefsMap = OD2::Common::ButtonDefManager::getInstance();
    auto &paletteManager = OD2::Common::PaletteManager::getInstance();
    buttonDefsMap.addButtonDef({
        .name = "Wide",
        .resourceName = std::string(OD2::Common::ResourcePaths::UI::WideButtonBlank),
        .palette = paletteManager.getPalette("Units"),
        .font = "btntext",
        .clickSound = std::string(OD2::Common::ResourcePaths::SFX::ButtonClick),
        .clickableRect = {.x = 6, .y = 1, .w = 259, .h = 32},
        .segments = {.x = 2, .y = 1},
        .frames = {.base = 0, .pressed = 1, .disabled = -1},
    });
    buttonDefsMap.addButtonDef({
        .name = "Medium",
        .resourceName = std::string(OD2::Common::ResourcePaths::UI::MediumButtonBlank),
        .palette = paletteManager.getPalette("Units"),
        .font = "btntext",
        .clickSound = std::string(OD2::Common::ResourcePaths::SFX::ButtonClick),
        .clickableRect = {.x = 6, .y = 1, .w = 118, .h = 32},
        .segments = {.x = 1, .y = 1},
        .frames = {.base = 0, .pressed = 1, .disabled = -1},
    });
}

void loadFonts() {
    const auto addFont = [](const std::string_view fontPath, const std::string &paletteName) -> void {
        const auto getFontName = [](const std::string_view path) -> std::string {
            const auto pos = path.find_last_of('/');
            if (pos == std::string_view::npos)
                return std::string(path);

            return std::string(path.substr(pos + 1));
        };

        OD2::Common::FontManager::getInstance().addFont(
            getFontName(fontPath), std::move(std::make_unique<Abyss::UI::SpriteFont<Abyss::DataTypes::DC6>>(fontPath, OD2::Common::GetPalette(paletteName))));
    };

    Abyss::Common::Log::info("Loading fonts...");
    addFont(OD2::Common::ResourcePaths::FontsAndLocales::Font6, "Static");
    addFont(OD2::Common::ResourcePaths::FontsAndLocales::Font8, "Static");
    addFont(OD2::Common::ResourcePaths::FontsAndLocales::Font16, "Static");
    addFont(OD2::Common::ResourcePaths::FontsAndLocales::Font24, "Static");
    addFont(OD2::Common::ResourcePaths::FontsAndLocales::Font30, "Static");
    addFont(OD2::Common::ResourcePaths::FontsAndLocales::Font42, "Static");
    addFont(OD2::Common::ResourcePaths::FontsAndLocales::FontFormal12, "Static");
    addFont(OD2::Common::ResourcePaths::FontsAndLocales::FontFormal11, "Static");
    addFont(OD2::Common::ResourcePaths::FontsAndLocales::FontFormal10, "Static");
    addFont(OD2::Common::ResourcePaths::FontsAndLocales::FontExocet10, "Static");
    addFont(OD2::Common::ResourcePaths::FontsAndLocales::FontExocet8, "Static");
    addFont(OD2::Common::ResourcePaths::FontsAndLocales::FontSucker, "Static");
    addFont(OD2::Common::ResourcePaths::FontsAndLocales::FontRediculous, "Static");

    OD2::Common::FontManager::getInstance().addFont(
        "btntext", std::move(std::make_unique<Abyss::UI::SpriteFont<Abyss::DataTypes::DC6>>(OD2::Common::ResourcePaths::FontsAndLocales::FontExocet10,
                                                                                            OD2::Common::GetPalette("Units"))));
}

int main(const int argc, char **argv) {
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

        engine.addCursorImage("Default", OD2::Common::ResourcePaths::MousePointers::CursorDefault,
                              OD2::Common::PaletteManager::getInstance().getPalette("Static"));
        engine.setCursorImage("Default");
        engine.getMouseState().setVisible(true);
        engine.setScene(std::make_unique<OD2::Scenes::MainMenu::MainMenu>());

        Abyss::Common::Log::info("Startup complete");
        // engine.setMasterVolumeLevel(0.1f);

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
