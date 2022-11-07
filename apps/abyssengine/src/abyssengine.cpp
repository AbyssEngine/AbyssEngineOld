#include "config.h"
#include "engine/engine.h"
#include "hostnotify/hostnotify.h"
#include "systemio/sdl2/sdl2systemio.h"
#include <filesystem>
#include <libavcodec/version.h>
#include <libavfilter/version.h>
#include <libavformat/version.h>
#include <libavutil/log.h>
#include <libavutil/version.h>
#include <libswresample/version.h>
#include <memory>
#include <typeinfo>
#include <spdlog/spdlog.h>

#if _WINDOWS
#define _WIN32_WINNT 0x0502
#include <Windows.h>
#endif

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
#if _WINDOWS
    // Windows is 'special' and doesn't output to console even if the app is launched
    // from the console (who would want that?!). Lets ensure the console actually
    // gets the output when launched on the console. -_-
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        freopen("CONOUT$", "wb", stdout);
        freopen("CONOUT$", "wb", stderr);
    }
#endif

    SPDLOG_INFO(ABYSS_VERSION_STRING);

#ifndef NDEBUG
    spdlog::set_level(spdlog::level::trace);
#endif

#ifdef LUAJIT_VERSION
    SPDLOG_INFO("LuaJIT Version " LUA_VERSION " / " LUAJIT_VERSION);
#else
    SPDLOG_INFO("Lua Version " LUA_VERSION);
#endif
    SPDLOG_INFO("SOL version " SOL_VERSION_STRING);
    SPDLOG_INFO("FFMPEG Library:");
    SPDLOG_INFO("   AVFormat Version    - " AV_STRINGIFY(LIBAVFORMAT_VERSION));
    SPDLOG_INFO("   AVResample Version  - " AV_STRINGIFY(LIBAVRESAMPLE_VERSION));
    SPDLOG_INFO("   AVUtil Version      - " AV_STRINGIFY(LIBAVUTIL_VERSION));
    SPDLOG_INFO("   AVCodec Version     - " AV_STRINGIFY(LIBAVCODEC_VERSION));
    SPDLOG_INFO("   AVFilter Version    - " AV_STRINGIFY(LIBAVFILTER_VERSION));
    SPDLOG_INFO("   SwResample Version  - " AV_STRINGIFY(LIBSWRESAMPLE_VERSION));

    av_log_set_level(AV_LOG_QUIET);

    try {
        auto configPath = GetConfigPath(argv[0]);
        std::filesystem::current_path(configPath);

        LibAbyss::INIFile iniFile(configPath / "config.ini");
        iniFile.SetValue("#Abyss", "BasePath", std::filesystem::absolute(configPath).string());

        std::unique_ptr<AbyssEngine::SystemIO> systemIo = std::make_unique<AbyssEngine::SDL2::SDL2SystemIO>();

        AbyssEngine::Engine engine(std::move(iniFile), std::move(systemIo));
        engine.Run();

    } catch (std::exception &ex) {
        SPDLOG_CRITICAL("Caught exception {}: {}", typeid(ex).name(), ex.what());
        AbyssEngine::HostNotify::Notify(AbyssEngine::eNotifyType::Fatal, "AbyssEngine Crash", ex.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
