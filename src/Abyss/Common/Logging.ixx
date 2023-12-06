module;

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

export module Abyss.Common.Logging;

namespace Abyss::Common::Log {

export inline auto Initialize() -> void {
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::info);
#endif
    const auto console = spdlog::stdout_color_mt("console");
    console->set_pattern("[%H:%M:%S.%e] [%^%l%$] -> %v");
    set_default_logger(console);
}

export template <typename... Args> void trace(spdlog::format_string_t<Args...> fmt, Args &&...args) { spdlog::trace(fmt, std::forward<Args>(args)...); }
export template <typename... Args> void debug(spdlog::format_string_t<Args...> fmt, Args &&...args) { spdlog::debug(fmt, std::forward<Args>(args)...); }
export template <typename... Args> void info(spdlog::format_string_t<Args...> fmt, Args &&...args) { spdlog::info(fmt, std::forward<Args>(args)...); }
export template <typename... Args> void warn(spdlog::format_string_t<Args...> fmt, Args &&...args) { spdlog::warn(fmt, std::forward<Args>(args)...); }
export template <typename... Args> void error(spdlog::format_string_t<Args...> fmt, Args &&...args) { spdlog::error(fmt, std::forward<Args>(args)...); }
export template <typename... Args> void critical(spdlog::format_string_t<Args...> fmt, Args &&...args) { spdlog::critical(fmt, std::forward<Args>(args)...); }

} // namespace Abyss::Common::Log
