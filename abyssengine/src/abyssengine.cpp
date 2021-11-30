#include <filesystem>
#include "engine/engine.h"
#include "hostnotify/hostnotify.h"
#include <config.h>
#include <memory>
#include "systemio/sdl2/sdl2systemio.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

static std::filesystem::path GetConfigPath(std::string_view exePath) {
    auto testPath = std::filesystem::current_path() / "config.ini";

    // Test working directory path
    if (exists(testPath))
        return testPath.remove_filename();

    // Test executable path
    testPath = std::filesystem::path(exePath).remove_filename() / "config.ini";
    if (exists(testPath))
        return testPath.remove_filename();

#ifdef __APPLE__
    // Test OSX app package path
    testPath = std::filesystem::current_path() / ".." / ".." / ".." / "config.ini";

    if (exists(testPath))
        return testPath.remove_filename();
#endif
    return {};
}

int main(int, char *argv[]) {
    SPDLOG_INFO(ABYSS_VERSION_STRING);

#ifndef NDEBUG
    spdlog::set_level(spdlog::level::trace);
#endif


    try {
        auto configPath = GetConfigPath(argv[0]);

        AbyssEngine::Common::INIFile iniFile(configPath / "config.ini");
        iniFile.SetValue("#Abyss", "BasePath", std::filesystem::absolute(configPath).string());

        std::unique_ptr<AbyssEngine::SystemIO> systemIo = std::make_unique<AbyssEngine::SDL2::SDL2SystemIO>();

        AbyssEngine::Engine engine(std::move(iniFile), std::move(systemIo));
        engine.Run();

    } catch (std::exception &ex) {
        SPDLOG_CRITICAL(ex.what());
        AbyssEngine::HostNotify::Notify(AbyssEngine::eNotifyType::Fatal, "AbyssEngine Crash", ex.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
