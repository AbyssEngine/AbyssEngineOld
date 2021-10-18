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

#ifndef LIBABYSS_CRYPTO_H
#define LIBABYSS_CRYPTO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void crypto_initialize();
uint32_t crypto_lookup(uint32_t index);
void crypto_decrypt(uint32_t *data, size_t data_length, uint32_t seed);
void crypto_decrypt_bytes(char *data, size_t data_length, uint32_t seed);
uint32_t crypto_hash_string(const char *key, uint32_t hash_type);
uint32_t *crypto_decrypt_table(FILE *source, uint32_t size, const char *name);
#endif // LIBABYSS_CRYPTO_H
