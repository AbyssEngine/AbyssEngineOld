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

#include "align.h"
#include "string.h"
#include <string.h>

const char *alignment_to_string(e_alignment alignment) {
    switch (alignment) {
    case alignment_start:
        return "start";
    case alignment_middle:
        return "middle";
    case alignment_end:
        return "end";
    default:
        return NULL;
    }
}

e_alignment string_to_alignment(const char *string) {
    if (compare_str_ncase(string, "start"))
        return alignment_start;

    if (compare_str_ncase(string, "middle"))
        return alignment_middle;

    if (compare_str_ncase(string, "end"))
        return alignment_end;

    return alignment_unknown;
}
