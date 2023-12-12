#pragma once

#include "Configuration.h"

namespace Abyss::Common::CommandLineOpts {

void process(int argc, char **argv, bool &quitOnRun, Configuration &config);

} // namespace Abyss::Common::CommandLineOpts
