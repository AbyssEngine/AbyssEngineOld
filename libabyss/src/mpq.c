#include "crypto.h"
#include <libabyss/log.h>
#include <libabyss/mpq.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint32_t mpq_file_flag;

#define MPQ_FILE_FLAG_IMPLODE (mpq_file_flag)0x00000100   // File is compressed using PKWARE Data compression library
#define MPQ_FILE_FLAG_COMPRESS (mpq_file_flag)0x00000200  // File is compressed using combination of compression methods
#define MPQ_FILE_FLAG_ENCRYPTED (mpq_file_flag)0x00010000 // The file is encrypted
#define MPQ_FILE_FLAG_FIX_KEY (mpq_file_flag)0x00020000 // The decryption key for the file is altered base on the position of the file in the archive
#define MPQ_FILE_FLAG_PATCH_FILE (mpq_file_flag)0x00100000  // The file contains incremental patch for an existing file in base MPQ
#define MPQ_FILE_FLAG_SINGLE_UNIT (mpq_file_flag)0x01000000 // Instead of being divided to 0x1000-bytes blocks, the file is stored as single unit
#define MPQ_FILE_FLAG_DELETED (mpq_file_flag)0x02000000     // File is a deletion marker, indicating that the file no longer exists.
#define MPQ_FILE_FLAG_SECTOR_CRC (mpq_file_flag)0x04000000  // File has checksums for each sector. Ignored if file is not compressed or imploded.
#define MPQ_FILE_FLAG_EXISTS (mpq_file_flag)0x80000000      // Set if file exists, reset when the file was deleted

typedef struct mpq_hash_entry {
    uint32_t hash_a;
    uint32_t hash_b;
    uint16_t locale;
    uint16_t platform;
    uint32_t block_index;
} mpq_hash_entry;

typedef struct mpq_block_entry {
    uint32_t file_position;
    uint32_t file_size_compressed;
    uint32_t file_size_uncompressed;
    uint32_t flags;
} mpq_block_entry;

// Represents the MPQ header
typedef struct __attribute__((__packed__)) mpq_header {
    uint8_t magic[4];
    uint32_t header_size;
    uint32_t archive_size;
    uint16_t format_version;
    uint16_t block_size;
    uint32_t hash_table_offset;
    uint32_t block_table_offset;
    uint32_t hash_table_entries;
    uint32_t block_table_entries;
} mpq_header;

// Represents an entry in the block table
typedef struct __attribute__((__packed__)) mpq_block {
    uint32_t file_position;
    uint32_t compressed_file_size;
    uint32_t uncompressed_file_size;
    mpq_file_flag flags;
} mpq_block;

typedef struct mpq {
    const char *file_path;
    FILE *file;
    mpq_header header;
    mpq_hash_entry *hash_entries;
    mpq_block_entry *block_entries;
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

mpq_hash_entry *mpq_read_hash_table(const mpq *source) {
    mpq_hash_entry *result = malloc(source->header.hash_table_entries);

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

mpq_block_entry *mpq_read_block_table(const mpq *source) {
    mpq_block_entry *result = malloc(source->header.block_table_entries);

    fseek(source->file, source->header.block_table_offset, SEEK_SET);
    uint32_t *block_data = crypto_decrypt_table(source->file, source->header.block_table_entries, "(block table)");

    for (int i = 0, n = 0; i < source->header.hash_table_entries; i++, n += 4) {
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

bool mpq_file_exists(const mpq *source, const char* filename) {
    return mpq_get_hash_index(source, crypto_hash_string(filename, 1), crypto_hash_string(filename, 2)) >= 0;
}

void *mpq_read_file(const mpq *source, const char* filename) {
    int hash_index = mpq_get_hash_index(source, crypto_hash_string(filename, 1), crypto_hash_string(filename, 2));

    if (hash_index < 0) {
        return NULL;
    }



    return NULL;
}
