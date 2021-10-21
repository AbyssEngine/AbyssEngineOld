/**
 * Copyright (C) 2021 Tim Sarbin
 * This file is part of AbyssEngine <https://github.com/AbyssEngine>.
 *
 * AbyssEngine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AbyssEngine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with AbyssEngine.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include "engine/engine.h"
#ifdef __APPLE__
#include "misc/appcompat.h"
#endif
#include <libabyss/log.h>
#include <libabyss/utils.h>
#include <stdlib.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

int main(int argc, char **argv) {
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        printf("%d.%d", ABYSS_VERSION_MAJOR, ABYSS_VERSION_MINOR);
        return 0;
    }
#ifndef NDEBUG
    log_info("Abyss Engine v%d.%d (DEBUG BUILD)", ABYSS_VERSION_MAJOR, ABYSS_VERSION_MINOR);
#else
    log_info("Abyss Engine v%d.%d", ABYSS_VERSION_MAJOR, ABYSS_VERSION_MINOR);
    log_set_level(LOG_ERROR);
#endif // NDEBUG

    check_app_compat(); // Ahh macOS you pain in the ass

    char cwd_path[4096];
#ifdef _WIN32
    _getcwd(cwd_path, 4096);
#else
    getcwd(cwd_path, 4096);
#endif
    char *ini_file_path = calloc(1, 4096);

    // Try the CWD
    strcat(ini_file_path, cwd_path);
    strcat(ini_file_path, "/config.ini");
    ini_file *ini = ini_file_load(ini_file_path);
    if (ini != NULL) {
        util_normalize_path(ini_file_path);
        util_get_folder_path_part(ini_file_path);
    } else {
        // CWD didn't work, try exe path
        memset(ini_file_path, 0, 4096);
        strcat(ini_file_path, argv[0]);
        util_normalize_path(ini_file_path);
        util_get_folder_path_part(ini_file_path);

        strcat(ini_file_path, "config.ini");
        ini = ini_file_load(ini_file_path);
        util_get_folder_path_part(ini_file_path);

        // If we STILL didn't find it, we may be running as an app package on OSX. Try that...
        if (ini == NULL) {
            memset(ini_file_path, 0, 4096);
            strcat(ini_file_path, argv[0]);
            util_normalize_path(ini_file_path);
            util_get_folder_path_part(ini_file_path);
            strcat(ini_file_path, "/../../../");
            util_normalize_path(ini_file_path);
            strcat(ini_file_path, "config.ini");
            ini = ini_file_load(ini_file_path);
            util_get_folder_path_part(ini_file_path);
        }
    }
    log_info("Engine running from location %s...\n", ini_file_path);
    engine *engine = engine_create(ini_file_path, ini);
    free(ini_file_path);

    engine_set_global_instance(engine);
    engine_run(engine);
    engine_destroy(engine);

    if (ini != NULL) {
        ini_file_destroy(ini);
    }

    return EXIT_SUCCESS;
}
