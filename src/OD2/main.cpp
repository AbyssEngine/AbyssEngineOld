#include "Abyss/AbyssEngine.h"
#include "Abyss/Common/Logging.h"
#include "Abyss/DataTypes/DC6.h"
#include "Abyss/UI/SpriteFont.h"
#include "Common/ButtonDefManager.h"
#include "Common/DataTableManager.h"
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

void loadDataTables() {
    Abyss::Common::Log::info("Loading data tables...");
    auto &dataTableManager = OD2::Common::DataTableManager::getInstance();
    dataTableManager.addDataTable("LevelPreset", OD2::Common::ResourcePaths::Data::LevelPreset);
    dataTableManager.addDataTable("LevelType", OD2::Common::ResourcePaths::Data::LevelType);
    dataTableManager.addDataTable("ObjectType", OD2::Common::ResourcePaths::Data::ObjectType);
    dataTableManager.addDataTable("LevelWarp", OD2::Common::ResourcePaths::Data::LevelWarp);
    dataTableManager.addDataTable("LevelDetails", OD2::Common::ResourcePaths::Data::LevelDetails);
    dataTableManager.addDataTable("LevelMaze", OD2::Common::ResourcePaths::Data::LevelMaze);
    dataTableManager.addDataTable("LevelSubstitutions", OD2::Common::ResourcePaths::Data::LevelSubstitutions);
    dataTableManager.addDataTable("ObjectDetails", OD2::Common::ResourcePaths::Data::ObjectDetails);
    dataTableManager.addDataTable("ObjectMode", OD2::Common::ResourcePaths::Data::ObjectMode);
    dataTableManager.addDataTable("SoundSettings", OD2::Common::ResourcePaths::Data::SoundSettings);
    dataTableManager.addDataTable("ItemStatCost", OD2::Common::ResourcePaths::Data::ItemStatCost);
    dataTableManager.addDataTable("ItemRatio", OD2::Common::ResourcePaths::Data::ItemRatio);
    dataTableManager.addDataTable("ItemTypes", OD2::Common::ResourcePaths::Data::ItemTypes);
    dataTableManager.addDataTable("QualityItems", OD2::Common::ResourcePaths::Data::QualityItems);
    dataTableManager.addDataTable("LowQualityItems", OD2::Common::ResourcePaths::Data::LowQualityItems);
    dataTableManager.addDataTable("Overlays", OD2::Common::ResourcePaths::Data::Overlays);
    dataTableManager.addDataTable("Runes", OD2::Common::ResourcePaths::Data::Runes);
    dataTableManager.addDataTable("Sets", OD2::Common::ResourcePaths::Data::Sets);
    dataTableManager.addDataTable("SetItems", OD2::Common::ResourcePaths::Data::SetItems);
    dataTableManager.addDataTable("AutoMagic", OD2::Common::ResourcePaths::Data::AutoMagic);
    dataTableManager.addDataTable("BodyLocations", OD2::Common::ResourcePaths::Data::BodyLocations);
    dataTableManager.addDataTable("Events", OD2::Common::ResourcePaths::Data::Events);
    dataTableManager.addDataTable("Properties", OD2::Common::ResourcePaths::Data::Properties);
    dataTableManager.addDataTable("Hireling", OD2::Common::ResourcePaths::Data::Hireling);
    //dataTableManager.addDataTable("HirelingDescription", OD2::Common::ResourcePaths::Data::HirelingDescription);
    dataTableManager.addDataTable("DifficultyLevels", OD2::Common::ResourcePaths::Data::DifficultyLevels);
    dataTableManager.addDataTable("AutoMap", OD2::Common::ResourcePaths::Data::AutoMap);
    dataTableManager.addDataTable("CubeRecipes", OD2::Common::ResourcePaths::Data::CubeRecipes);
    dataTableManager.addDataTable("CubeModifier", OD2::Common::ResourcePaths::Data::CubeModifier);
    //dataTableManager.addDataTable("CubeType", OD2::Common::ResourcePaths::Data::CubeType);
    dataTableManager.addDataTable("Skills", OD2::Common::ResourcePaths::Data::Skills);
    dataTableManager.addDataTable("SkillDesc", OD2::Common::ResourcePaths::Data::SkillDesc);
    dataTableManager.addDataTable("SkillCalc", OD2::Common::ResourcePaths::Data::SkillCalc);
    dataTableManager.addDataTable("MissileCalc", OD2::Common::ResourcePaths::Data::MissileCalc);
    //dataTableManager.addDataTable("TreasureClass", OD2::Common::ResourcePaths::Data::TreasureClass);
    dataTableManager.addDataTable("TreasureClassEx", OD2::Common::ResourcePaths::Data::TreasureClassEx);
    dataTableManager.addDataTable("States", OD2::Common::ResourcePaths::Data::States);
    dataTableManager.addDataTable("SoundEnvirons", OD2::Common::ResourcePaths::Data::SoundEnvirons);
    dataTableManager.addDataTable("Shrines", OD2::Common::ResourcePaths::Data::Shrines);
    dataTableManager.addDataTable("MonProp", OD2::Common::ResourcePaths::Data::MonProp);
    dataTableManager.addDataTable("ElemType", OD2::Common::ResourcePaths::Data::ElemType);
    dataTableManager.addDataTable("PlrMode", OD2::Common::ResourcePaths::Data::PlrMode);
    dataTableManager.addDataTable("PetType", OD2::Common::ResourcePaths::Data::PetType);
    dataTableManager.addDataTable("NPC", OD2::Common::ResourcePaths::Data::NPC);
    dataTableManager.addDataTable("MonsterUniqueModifier", OD2::Common::ResourcePaths::Data::MonsterUniqueModifier);
    dataTableManager.addDataTable("MonsterEquipment", OD2::Common::ResourcePaths::Data::MonsterEquipment);
    dataTableManager.addDataTable("UniqueAppellation", OD2::Common::ResourcePaths::Data::UniqueAppellation);
    dataTableManager.addDataTable("MonsterLevel", OD2::Common::ResourcePaths::Data::MonsterLevel);
    dataTableManager.addDataTable("MonsterSound", OD2::Common::ResourcePaths::Data::MonsterSound);
    dataTableManager.addDataTable("MonsterSequence", OD2::Common::ResourcePaths::Data::MonsterSequence);
    dataTableManager.addDataTable("PlayerClass", OD2::Common::ResourcePaths::Data::PlayerClass);
    dataTableManager.addDataTable("PlayerType", OD2::Common::ResourcePaths::Data::PlayerType);
    dataTableManager.addDataTable("Composite", OD2::Common::ResourcePaths::Data::Composite);
    dataTableManager.addDataTable("HitClass", OD2::Common::ResourcePaths::Data::HitClass);
    dataTableManager.addDataTable("ObjectGroup", OD2::Common::ResourcePaths::Data::ObjectGroup);
    dataTableManager.addDataTable("CompCode", OD2::Common::ResourcePaths::Data::CompCode);
    dataTableManager.addDataTable("Belts", OD2::Common::ResourcePaths::Data::Belts);
    dataTableManager.addDataTable("Gamble", OD2::Common::ResourcePaths::Data::Gamble);
    dataTableManager.addDataTable("Colors", OD2::Common::ResourcePaths::Data::Colors);
    dataTableManager.addDataTable("StorePage", OD2::Common::ResourcePaths::Data::StorePage);
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
        loadDataTables();

        engine.addCursorImage("Default", OD2::Common::ResourcePaths::MousePointers::CursorDefault,
                              OD2::Common::PaletteManager::getInstance().getPalette("Static"));
        engine.setCursorImage("Default");
        engine.getMouseState().setVisible(true);
        engine.setScene(std::make_unique<OD2::Scenes::MainMenu::MainMenu>());

        Abyss::Common::Log::info("Startup complete");
        // engine.setMasterVolumeLevel(0.0f);

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
