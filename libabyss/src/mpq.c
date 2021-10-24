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
#include <libabyss/commondef.h>
#include <libabyss/log.h>
#include <libabyss/mpq.h>
#include <libabyss/mpqblock.h>
#include <libabyss/mpqstream.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct mpq_hash_entry {
    uint32_t hash_a;
    uint32_t hash_b;
    uint16_t locale;
    uint16_t platform;
    uint32_t block_index;
} mpq_hash_entry;

// Represents the MPQ header
typedef PACK(struct mpq_header {
    uint8_t magic[4];
    uint32_t header_size;
    uint32_t archive_size;
    uint16_t format_version;
    uint16_t block_size;
    uint32_t hash_table_offset;
    uint32_t block_table_offset;
    uint32_t hash_table_entries;
    uint32_t block_table_entries;
}) mpq_header;

typedef struct mpq {
    const char *file_path;
    FILE *file;
    mpq_header header;
    mpq_hash_entry *hash_entries;
    mpq_block *block_entries;
} mpq;

int mpq_get_hash_index(const mpq *source, uint32_t hash_a, uint32_t hash_b) {
    for (int i = 0; i < source->header.hash_table_entries; i++) {
        if (source->hash_entries[i].hash_a != hash_a) {
            continue;
        }

        if (source->hash_entries[i].hash_b != hash_b) {
            continue;
        }

        return i;
    }

    return -1;
}

mpq_block *mpq_get_block(const mpq *source, const char *filename) {
    int hash_index = mpq_get_hash_index(source, crypto_hash_string(filename, 1), crypto_hash_string(filename, 2));

    if ((hash_index < 0) || (hash_index >= source->header.hash_table_entries)) {
        return NULL;
    }

    if ((source->hash_entries[hash_index].block_index < 0) || (source->hash_entries[hash_index].block_index >= source->header.block_table_entries)) {
        return NULL;
    }
    
    return &source->block_entries[source->hash_entries[hash_index].block_index];
}

mpq_hash_entry *mpq_read_hash_table(const mpq *source) {
    mpq_hash_entry *result = malloc(sizeof(mpq_hash_entry) * source->header.hash_table_entries);

    fseek(source->file, source->header.hash_table_offset, SEEK_SET);
    uint32_t *hash_data = crypto_decrypt_table(source->file, source->header.hash_table_entries, "(hash table)");

    for (int i = 0, n = 0; i < source->header.hash_table_entries; i++, n += 4) {
        result[i].hash_a = hash_data[n];
        result[i].hash_b = hash_data[n + 1];
        result[i].locale = hash_data[n + 2] >> 16;
        result[i].platform = hash_data[n + 2] & 0xFFFF;
        result[i].block_index = hash_data[n + 3];
    }

    free(hash_data);

    return result;
}

mpq_block *mpq_read_block_table(const mpq *source) {
    mpq_block *result = malloc(sizeof(mpq_block) * source->header.block_table_entries);

    fseek(source->file, source->header.block_table_offset, SEEK_SET);
    uint32_t *block_data = crypto_decrypt_table(source->file, source->header.block_table_entries, "(block table)");

    for (int i = 0, n = 0; i < source->header.block_table_entries; i++, n += 4) {
        result[i].file_position = block_data[n];
        result[i].file_size_compressed = block_data[n + 1];
        result[i].file_size_uncompressed = block_data[n + 2];
        result[i].flags = block_data[n + 3];
    }

    free(block_data);

    return result;
}

mpq *mpq_load(const char *file_path) {
    mpq *result = calloc(1, sizeof(mpq));

    result->file = fopen(file_path, "rb");
    if (result->file == NULL) {
        log_error("failed to open '%s'", file_path);
        free(result);
        return NULL;
    }

    result->file_path = strdup(file_path);
    fread(&result->header, sizeof(mpq_header), 1, result->file);

    if (strncmp((char *)result->header.magic, "MPQ\x1A", 4) != 0) {
        log_error("MPQ failed to load due to an invalid header");
        fclose(result->file);
        free(result);
        return NULL;
    }

    result->hash_entries = mpq_read_hash_table(result);
    result->block_entries = mpq_read_block_table(result);

    return result;
}

void mpq_destroy(mpq *source) {
    fclose(source->file);
    free(source->block_entries);
    free(source->hash_entries);
    free((void *)source->file_path);
    free(source);
}

bool mpq_file_exists(const mpq *source, const char *filename) {
    if (mpq_get_hash_index(source, crypto_hash_string(filename, 1), crypto_hash_string(filename, 2)) < 0)
        return false;

    mpq_block *block = mpq_get_block(source, filename);

    if (block == NULL)
        return false;

    return mpq_block_has_flag(block, MPQ_BLOCK_FLAG_EXISTS) && !mpq_block_has_flag(block, MPQ_BLOCK_FLAG_DELETE_MARKER);
}

void *mpq_read_file(mpq *source, const char *filename, uint32_t *file_size) {
    mpq_block *block = mpq_get_block(source, filename);

    if (block == NULL) {
        log_error("File not found '%s'", filename);
        return NULL;
    }

    mpq_stream *stream = mpq_stream_new(source, block, filename);

    if (stream == NULL) {
        log_error("Error creating stream for '%s'", filename);
        return NULL;
    }

    void *result = calloc(1, block->file_size_uncompressed);

    if (file_size != NULL) {
        *file_size = block->file_size_uncompressed;
    }

    mpq_stream_read(stream, result, 0, block->file_size_uncompressed);

    return result;
}

uint32_t mpq_get_header_size(const mpq *source) { return source->header.header_size; }

uint32_t mpq_get_block_size(const mpq *source) { return source->header.block_size; }

FILE *mpq_get_file_stream(const mpq *source) { return source->file; }
