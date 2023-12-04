#pragma once

#include <Abyss/Common/Configuration.h>
#include <Abyss/Common/Logging.h>
#include <cxxopts.hpp>
#include <filesystem>
#include <string>
#include <vector>

namespace Abyss::Common::CommandLineOpts {

auto process(int argc, char **argv, bool &quitOnRun, Configuration &config) -> void;

} // namespace Abyss::Common::CommandLineOpts
