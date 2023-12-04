#include "OD2/Scenes/MainMenu/MainMenu.h"

#include <Abyss/AbyssEngine.h>
#include <Abyss/Common/Logging.h>
#include <OD2/Common/ResourcePaths.h>
#include <OD2/Startup.h>
#include <stdexcept>

auto main(const int argc, char **argv) -> int {
    try {
        auto &engine = Abyss::AbyssEngine::getInstance();
        if (!engine.processCommandLineArguments(argc, argv)) {
            return 0;
        }

        OD2::startup();
        engine.setCursorImage(OD2::Common::ResourcePaths::MousePointers::CursorDefault, OD2::Common::PaletteManager::getInstance().getPalette("Static"));
        engine.getMouseState().setVisible(true);
        engine.setScene(std::make_unique<OD2::Scenes::MainMenu::MainMenu>());
        engine.run();
    } catch (const std::exception &exception) {
        Abyss::Common::Log::error("{}", exception.what());
        return 1;
    }
    return 0;
}
