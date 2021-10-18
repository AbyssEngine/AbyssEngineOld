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

#include "help.h"
#include "mpqtool.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc <= 1) {
        help_show_general();
        return EXIT_SUCCESS;
    }

    char *tool_name = argv[1];

    for (char *p = tool_name; *p; p++) {
        *p = (char)tolower(*p);
    }

    if (strcmp(tool_name, "mpq") == 0) {
        return mpqtool_run(argc - 2, &argv[2]);
    } else {
        fprintf(stderr, "Unknown tool '%s' specified.", tool_name);
        return EXIT_FAILURE;
    }
}
