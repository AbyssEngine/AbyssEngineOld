#include "config.h"
#include "engine/engine.h"
#include <stdlib.h>
#include <libabyss/log.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

int main(int argc, char **argv) {
    log_info("Abyss Engine v%d.%d", ABYSS_VERSION_MAJOR, ABYSS_VERSION_MINOR);

    char cwd_path[4096];
#ifdef _WIN32
    _getcwd(cwd_path, 4096);
#else
    getcwd(cwd_path, 4096);
#endif

    ini_file *ini = ini_file_load("config.ini");

    engine *engine = engine_create(cwd_path, ini);
    engine_set_global_instance(engine);

    engine_run(engine);
    engine_destroy(engine);
    ini_file_destroy(ini);

    return EXIT_SUCCESS;
}
