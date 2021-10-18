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

#include "crypto.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENSURE_CRYPTO_INITIALIZED                                                                                                                    \
    if (!crypto_is_initialized) {                                                                                                                    \
        crypto_initialize();                                                                                                                         \
    }

bool crypto_is_initialized = false;
uint32_t crypto_buffer[0x500];

void crypto_initialize() {
    if (crypto_is_initialized) {
        return;
    }
    crypto_is_initialized = true;

    uint32_t seed = 0x00100001;

    for (int index1 = 0; index1 < 0x100; index1++) {
        int index2 = index1;
        for (int i = 0; i < 5; i++) {
            seed = (seed * 125 + 3) % 0x2AAAAB;
            uint32_t temp1 = (seed & 0xFFFF) << 0x10;

            seed = (seed * 125 + 3) % 0x2AAAAB;
            uint32_t temp2 = seed & 0xFFFF;

            crypto_buffer[index2] = temp1 | temp2;

            index2 += 0x100;
        }
    }
}

uint32_t crypto_lookup(uint32_t index) {
    ENSURE_CRYPTO_INITIALIZED

    return crypto_buffer[index];
}

void crypto_decrypt(uint32_t *data, size_t data_length, uint32_t seed) {
    uint32_t seed2 = 0xEEEEEEEE;

    for (int i = 0; i < data_length; i++) {
        seed2 += crypto_lookup(0x400 + (seed & 0xFF));
        uint32_t result = data[i];
        result ^= seed + seed2;

        seed = ((~seed << 21) + 0x11111111) | (seed >> 11);
        seed2 = result + seed2 + (seed2 << 5) + 3;
        data[i] = result;
    }
}

void crypto_decrypt_bytes(char *data, size_t data_length, uint32_t seed) {
    uint32_t seed2 = 0xEEEEEEEE;

    for (int i = 0; i < data_length - 3; i += 4) {
        seed2 += crypto_lookup(0x400 + (seed & 0xFF));
        uint32_t result = *((uint32_t *)&data[i]);
        result ^= seed + seed2;
        seed = ((~seed << 21) + 0x11111111) | (seed >> 11);
        seed2 = result + seed2 + (seed2 << 5) + 3;

        data[i + 0] = (char)(result & 0xff);
        data[i + 1] = (char)((result >> 8) & 0xff);
        data[i + 2] = (char)((result >> 16) & 0xff);
        data[i + 3] = (char)((result >> 24) & 0xff);
    }
}

uint64_t crypto_hash_filename(char *key) {
    uint32_t a = crypto_hash_string(key, 1);
    uint32_t b = crypto_hash_string(key, 2);

    return (((uint64_t)a) << 32) | ((uint64_t)b);
}

uint32_t crypto_hash_string(const char *key, uint32_t hash_type) {
    uint32_t seed1 = 0x7FED7FED;
    uint32_t seed2 = 0xEEEEEEEE;

    /* prepare seeds. */
    for (int i = 0; i < strlen(key); i++) {
        char ch = (char)toupper(key[i]);
        seed1 = crypto_lookup((hash_type * 0x100) + ((uint32_t)ch)) ^ (seed1 + seed2);
        seed2 = ((uint32_t)ch) + seed1 + seed2 + (seed2 << 5) + 3;
    }

    return seed1;
}

uint32_t *crypto_decrypt_table(FILE *source, uint32_t size, const char *name) {
    uint32_t seed = crypto_hash_string(name, 3);
    uint32_t seed2 = 0xEEEEEEEE;
    size *= 4;

    uint32_t *table = calloc(1, sizeof(uint32_t) * size);

    for (int i = 0; i < size; i++) {
        seed2 += crypto_buffer[0x400 + (seed & 0xff)];
        uint32_t result;
        fread(&result, sizeof(uint32_t), 1, source);
        result ^= seed + seed2;

        seed = ((~seed << 21) + 0x11111111) | (seed >> 11);
        seed2 = result + seed2 + (seed2 << 5) + 3;
        table[i] = result;
    }

    return table;
}
