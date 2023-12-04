#include "Logging.h"

namespace Abyss::Common::Log {

void Initialize() {
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::info);
#endif
    const auto console = spdlog::stdout_color_mt("console");
    console->set_pattern("[%H:%M:%S.%e] [%^%l%$] -> %v");
    spdlog::set_default_logger(console);
}

} // namespace Abyss::Common::Log
