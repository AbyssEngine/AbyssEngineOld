#include "Abyss/AbyssEngine.h"
#include "Abyss/Common/Logging.h"
#include "Abyss/DataTypes/DC6.h"
#include "Abyss/UI/SpriteFont.h"
#include "Common/ButtonDefManager.h"
#include "Common/DataTableManager.h"
#include "Common/FontManager.h"
#include "Common/PaletteManager.h"
#include "Common/SoundManager.h"
#include "Common/ResourcePaths.h"
#include "Scenes/MainMenu/MainMenu.h"

#include <absl/container/btree_map.h>
#include <future>

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
    auto &soundManager = OD2::Common::SoundManager::getInstance();
    buttonDefsMap.addButtonDef({
        .name = "Wide",
        .resourceName = std::string(OD2::Common::ResourcePaths::UI::WideButtonBlank),
        .palette = paletteManager.getPalette("Units"),
        .font = "btntext",
        .clickSound = soundManager.getSound("cursor_button_click"),
        .clickableRect = {.x = 6, .y = 1, .w = 259, .h = 32},
        .segments = {.x = 2, .y = 1},
        .frames = {.base = 0, .pressed = 1, .disabled = -1},
    });
    buttonDefsMap.addButtonDef({
        .name = "Medium",
        .resourceName = std::string(OD2::Common::ResourcePaths::UI::MediumButtonBlank),
        .palette = paletteManager.getPalette("Units"),
        .font = "btntext",
        .clickSound = soundManager.getSound("cursor_button_click"),
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
    static const absl::btree_map<std::string_view, std::string_view> loadDict = {
        {"LevelPreset", OD2::Common::ResourcePaths::Data::LevelPreset},
        {"LevelType", OD2::Common::ResourcePaths::Data::LevelType},
        {"ObjectType", OD2::Common::ResourcePaths::Data::ObjectType},
        {"LevelWarp", OD2::Common::ResourcePaths::Data::LevelWarp},
        {"LevelDetails", OD2::Common::ResourcePaths::Data::LevelDetails},
        {"LevelMaze", OD2::Common::ResourcePaths::Data::LevelMaze},
        {"LevelSubstitutions", OD2::Common::ResourcePaths::Data::LevelSubstitutions},
        {"ObjectDetails", OD2::Common::ResourcePaths::Data::ObjectDetails},
        {"ObjectMode", OD2::Common::ResourcePaths::Data::ObjectMode},
        {"SoundSettings", OD2::Common::ResourcePaths::Data::SoundSettings},
        {"ItemStatCost", OD2::Common::ResourcePaths::Data::ItemStatCost},
        {"ItemRatio", OD2::Common::ResourcePaths::Data::ItemRatio},
        {"ItemTypes", OD2::Common::ResourcePaths::Data::ItemTypes},
        {"QualityItems", OD2::Common::ResourcePaths::Data::QualityItems},
        {"LowQualityItems", OD2::Common::ResourcePaths::Data::LowQualityItems},
        {"Overlays", OD2::Common::ResourcePaths::Data::Overlays},
        {"Runes", OD2::Common::ResourcePaths::Data::Runes},
        {"Sets", OD2::Common::ResourcePaths::Data::Sets},
        {"SetItems", OD2::Common::ResourcePaths::Data::SetItems},
        {"AutoMagic", OD2::Common::ResourcePaths::Data::AutoMagic},
        {"BodyLocations", OD2::Common::ResourcePaths::Data::BodyLocations},
        {"Events", OD2::Common::ResourcePaths::Data::Events},
        {"Properties", OD2::Common::ResourcePaths::Data::Properties},
        {"Hireling", OD2::Common::ResourcePaths::Data::Hireling},
        {"DifficultyLevels", OD2::Common::ResourcePaths::Data::DifficultyLevels},
        {"AutoMap", OD2::Common::ResourcePaths::Data::AutoMap},
        {"CubeRecipes", OD2::Common::ResourcePaths::Data::CubeRecipes},
        {"CubeModifier", OD2::Common::ResourcePaths::Data::CubeModifier},
        {"Skills", OD2::Common::ResourcePaths::Data::Skills},
        {"SkillDesc", OD2::Common::ResourcePaths::Data::SkillDesc},
        {"SkillCalc", OD2::Common::ResourcePaths::Data::SkillCalc},
        {"MissileCalc", OD2::Common::ResourcePaths::Data::MissileCalc},
        {"TreasureClassEx", OD2::Common::ResourcePaths::Data::TreasureClassEx},
        {"States", OD2::Common::ResourcePaths::Data::States},
        {"SoundEnvirons", OD2::Common::ResourcePaths::Data::SoundEnvirons},
        {"Shrines", OD2::Common::ResourcePaths::Data::Shrines},
        {"MonProp", OD2::Common::ResourcePaths::Data::MonProp},
        {"ElemType", OD2::Common::ResourcePaths::Data::ElemType},
        {"PlrMode", OD2::Common::ResourcePaths::Data::PlrMode},
        {"PetType", OD2::Common::ResourcePaths::Data::PetType},
        {"NPC", OD2::Common::ResourcePaths::Data::NPC},
        {"MonsterUniqueModifier", OD2::Common::ResourcePaths::Data::MonsterUniqueModifier},
        {"MonsterEquipment", OD2::Common::ResourcePaths::Data::MonsterEquipment},
        {"UniqueAppellation", OD2::Common::ResourcePaths::Data::UniqueAppellation},
        {"MonsterLevel", OD2::Common::ResourcePaths::Data::MonsterLevel},
        {"MonsterSound", OD2::Common::ResourcePaths::Data::MonsterSound},
        {"MonsterSequence", OD2::Common::ResourcePaths::Data::MonsterSequence},
        {"PlayerClass", OD2::Common::ResourcePaths::Data::PlayerClass},
        {"PlayerType", OD2::Common::ResourcePaths::Data::PlayerType},
        {"Composite", OD2::Common::ResourcePaths::Data::Composite},
        {"HitClass", OD2::Common::ResourcePaths::Data::HitClass},
        {"ObjectGroup", OD2::Common::ResourcePaths::Data::ObjectGroup},
        {"CompCode", OD2::Common::ResourcePaths::Data::CompCode},
        {"Belts", OD2::Common::ResourcePaths::Data::Belts},
        {"Gamble", OD2::Common::ResourcePaths::Data::Gamble},
        {"Colors", OD2::Common::ResourcePaths::Data::Colors},
        {"StorePage", OD2::Common::ResourcePaths::Data::StorePage},
    };

    // Load the data tables in parallel
    std::vector<std::future<void>> futures;
    futures.reserve(loadDict.size());

    for (const auto &[name, fileName] : loadDict)
        futures.emplace_back(std::async(std::launch::async, [name, fileName] { OD2::Common::DataTableManager::getInstance().addDataTable(name, fileName); }));

    for (auto &future : futures)
        future.wait();

    // dataTableManager.addDataTable("HirelingDescription", OD2::Common::ResourcePaths::Data::HirelingDescription);
    // dataTableManager.addDataTable("CubeType", OD2::Common::ResourcePaths::Data::CubeType);
    // dataTableManager.addDataTable("TreasureClass", OD2::Common::ResourcePaths::Data::TreasureClass);
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
        loadFonts();
        loadDataTables();
        loadButtonDefs();

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
