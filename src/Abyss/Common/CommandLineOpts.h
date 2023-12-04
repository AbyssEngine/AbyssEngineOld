#pragma once

#include "Configuration.h"

namespace Abyss::Common::CommandLineOpts {

auto process(int argc, char **argv, bool &quitOnRun, Configuration &config) -> void;

} // namespace Abyss::Common::CommandLineOpts
