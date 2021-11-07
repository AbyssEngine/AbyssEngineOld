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

#include <libabyss/error.h>
#include <stdlib.h>
#include <string.h>

libabyss_error *libabyss_error_create(e_libabyss_error_code error_code, const char *message) {
    libabyss_error *result = malloc(sizeof(libabyss_error));

    if (message != NULL)
        result->error_message = strdup(message);

    result->error_code = error_code;

    return result;
}

void libabyss_error_destroy(libabyss_error *error) {
    if (error->error_message != NULL)
        free(error->error_message);

    free(error);
}
