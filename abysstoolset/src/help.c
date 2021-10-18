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
#include <config.h>
#include <stdio.h>

void help_show_general() {
    fprintf(stdout, "Abyss Engine v%i.%i - https://github.com/AbyssEngine\n", ABYSS_VERSION_MAJOR, ABYSS_VERSION_MINOR);
    printf("\nGeneral Usage:\n    abt [tool] [options]\n");
    printf("Tools:\n");
    printf("   mpq - Provides services for listing, viewing, exporting, and creating MPQ archives");
}
