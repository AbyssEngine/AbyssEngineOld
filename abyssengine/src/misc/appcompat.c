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

#include <libabyss/log.h>
#include <libgen.h>
#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
void check_app_compat_apple() {
    FILE *fp;
    char line[2048];
    char *token;

    // Open the command for reading.
    fp = popen("/usr/bin/locate .app | grep '\\.app$' | grep Steam", "r");
    if (fp == NULL) {
        log_warn("Failed To Check For App Incompatibilities :(");
    }
    const char s[3] = ", ";
    // Read the output a line at a time - output it.
    if (fgets(line, sizeof(line), fp) != NULL) {
        token = basename(strtok(line, s));

        /* walk through other tokens */
        while (token != NULL) {
            if (strcmp(token, "Magnet.app") == 0) {
                log_warn("Detected Installation Of Magnet Window Manager, This may cause lag issues with window dragging, It is recommended to "
                         "disable magnet while using Abyss Engine due to these issues.");
            }
            token = strtok(NULL, s);
        }
        /* close */
        pclose(fp);
    }
}
#endif // __APPLE__

void check_app_compat() {
#ifdef __APPLE__
    check_app_compat_apple();
#endif // __APPLE__
}
