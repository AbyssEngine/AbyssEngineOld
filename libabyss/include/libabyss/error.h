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

#ifndef LIBABYSS_ERROR_H
#define LIBABYSS_ERROR_H

typedef enum e_libabyss_error_code { E_LIBABYSS_OK, E_LIBABYSS_ERROR } e_libabyss_error_code;

typedef struct libabyss_error {
    e_libabyss_error_code error_code;
    char *error_message;
} libabyss_error;

libabyss_error *libabyss_error_create(e_libabyss_error_code error_code, const char *message);
void libabyss_error_destroy(libabyss_error *error);

#define LIBABYSS_ERROR_CONTAINER libabyss_error *libabyss_err;
#define LIBABYSS_NO_ERROR(x) x->libabyss_err = libabyss_error_create(E_LIBABYSS_OK, NULL);
#define LIBABYSS_ERROR(x, msg) x->libabyss_err = libabyss_error_create(E_LIBABYSS_ERROR, msg);

#endif // LIBABYSS_ERROR_H
