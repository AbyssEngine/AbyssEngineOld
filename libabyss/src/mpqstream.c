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

#include "blast.h"
#include "compress.h"
#include "crypto.h"
#include <assert.h>
#include <libabyss/log.h>
#include <libabyss/mpq.h>
#include <libabyss/mpqstream.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#include <fcntl.h>
#include <io.h>
#define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#define SET_BINARY_MODE(file)
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif // min

typedef struct mpq_stream {
    void *data;
    uint32_t data_length;
    const char *filename;
    uint32_t *positions;
    const mpq *mpq;
    mpq_block *block;
    uint32_t index;
    uint32_t size;
    uint32_t position;
} mpq_stream;

void *mpq_stream_decompress_multi(mpq_stream *source, void *buffer, uint32_t size_compressed, uint32_t size_uncompressed);

bool mpq_stream_load_block_offsets(mpq_stream *source) {
    uint32_t block_position_count = ((source->block->file_size_uncompressed + source->size - 1) / source->size) + 1;
    source->positions = realloc(source->positions, sizeof(uint32_t) * block_position_count);

    FILE *file_stream = mpq_get_file_stream(source->mpq);
    fseek(file_stream, (long)source->block->file_position, SEEK_SET);
    fread(source->positions, sizeof(uint32_t), block_position_count, file_stream);

    if (mpq_block_has_flag(source->block, MPQ_BLOCK_FLAG_ENCRYPTED)) {
        crypto_decrypt(source->positions, block_position_count, mpq_block_get_encryption_seed(source->block, source->filename) - 1);

        uint32_t block_pos_size = block_position_count << 2;
        if (source->positions[0] != block_pos_size) {
            log_error("Decryption of MPQ Failed: block position size mismatch.");
            return false;
        }

        if (source->positions[1] > source->size + block_pos_size) {
            log_error("Decryption of MPQ Failed: source size mismatch.");
            return false;
        }
    }

    return true;
}

mpq_stream *mpq_stream_new(mpq *mpq, mpq_block *block, const char *filename) {
    mpq_stream *result = calloc(1, sizeof(mpq_stream));
    result->mpq = mpq;
    result->block = block;
    result->filename = strdup(filename);
    result->index = 0xFFFFFFFE;

    if (mpq_block_has_flag(block, MPQ_BLOCK_FLAG_FIX_KEY)) {
        result->index = 0xFFFFFFFF;
    }

    result->size = 0x200 << mpq_get_block_size(mpq);

    if (mpq_block_has_flag(block, MPQ_BLOCK_FLAG_PATCH_FILE)) {
        log_error("Patch files are not currently supported.");
        mpq_stream_destroy(result);
        return NULL;
    }

    if ((mpq_block_has_flag(block, MPQ_BLOCK_FLAG_COMPRESS) || mpq_block_has_flag(block, MPQ_BLOCK_FLAG_IMPLODE)) &&
        !mpq_block_has_flag(block, MPQ_BLOCK_FLAG_SINGLE_UNIT)) {
        if (!mpq_stream_load_block_offsets(result)) {
            mpq_stream_destroy(result);
            return NULL;
        }
    }

    return result;
}

void mpq_stream_destroy(mpq_stream *source) {
    if (source->positions != NULL) {
        free(source->positions);
    }

    free((void *)source->filename);
    free(source);
}

bool mpq_stream_load_single_unit(mpq_stream *source) {
    FILE *file_stream = mpq_get_file_stream(source->mpq);
    fseek(file_stream, mpq_get_header_size(source->mpq), SEEK_SET);

    void *data = malloc(source->size);
    if (data == NULL) {
        return false;
    }
    fread(data, source->size, 1, file_stream);

    if (source->size != source->block->file_size_uncompressed) {
        if (mpq_stream_decompress_multi(source, data, source->block->file_size_compressed, source->block->file_size_uncompressed) == NULL) {
            return false;
        }
    }

    if (source->data != NULL) {
        free(source->data);
    }
    source->data = data;

    return true;
}

uint32_t mpq_stream_copy(mpq_stream *source, void *buffer, uint32_t offset, uint32_t position, uint32_t size) {
    int bytes_to_copy = min((int)source->data_length - (int)position, (int)size);

    if (bytes_to_copy <= 0) {
        return 0;
    }

    memcpy((char *)buffer + offset, (char *)source->data + position, bytes_to_copy);
    source->position += bytes_to_copy;
    return bytes_to_copy;
}

uint32_t mpq_stream_read_internal_single_unit(mpq_stream *source, void *buffer, uint32_t offset, uint32_t size) {
    if (source->data_length == 0) {
        if (!mpq_stream_load_single_unit(source)) {
            return 0;
        }
    }

    mpq_stream_copy(source, buffer, offset, source->position, source->data_length);
    return 0;
}

unsigned blast_in_f(void *how, unsigned char **buf) {
    *buf = how;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wvoid-pointer-to-int-cast"
    return (unsigned int)how;
#pragma clang diagnostic pop
}

int blast_out_f(void *how, unsigned char *buf, unsigned len) {
    memcpy(how, buf, len);
    return 0;
}

void *mpq_stream_decompress_multi(mpq_stream *source, void *buffer, uint32_t size_compressed, uint32_t size_uncompressed) {

    uint8_t compression_type = ((char *)buffer)[0];

    switch (compression_type) {
    case 0x01: // Huffman
        log_fatal("Huffman decompression not currently supported.");
        return NULL;
    case 0x02: // ZLib/Deflate
    {
        void *new_buffer = malloc(size_uncompressed);
        z_stream zlib_stream;
        zlib_stream.zalloc = Z_NULL;
        zlib_stream.zfree = Z_NULL;
        zlib_stream.opaque = Z_NULL;
        zlib_stream.avail_in = size_compressed;
        zlib_stream.next_in = (Bytef *)((char *)buffer + 1);
        zlib_stream.avail_out = size_uncompressed;
        zlib_stream.next_out = (Bytef *)new_buffer;

        inflateInit(&zlib_stream);
        int ret = inflate(&zlib_stream, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR) {
            log_fatal("Error decompressing Zlib/deflate.");
            return NULL;
        }
        inflateEnd(&zlib_stream);

        free(buffer);
        return new_buffer;
    }
    case 0x08: // PkLib/Implode
    {
        void *new_buffer = malloc(size_uncompressed);
        if (blast(blast_in_f, ((char *)buffer + 1), blast_out_f, new_buffer, NULL, NULL) != 0) {
            log_fatal("PkLib/Implode decompression error!");
            free(new_buffer);
            free(buffer);
            return NULL;
        }
        free(buffer);
        return new_buffer;
    }
    case 0x10: // BZip2
        log_fatal("BZip2 decompression not currently supported.");
        return NULL;
    case 0x12: // LZMA
        log_fatal("LZMA decompression not currently supported.");
        return NULL;
    case 0x80: // IMA ADPCM Stereo
    {
        return compress_decompress_wav((void *)((char *)buffer + 1), size_compressed, 2);
    }
    case 0x40: // IMA ADPCM Mono
    {
        return compress_decompress_wav((void *)((char *)buffer + 1), size_compressed, 1);
    }
    case 0x22: // Sparse + ZLib
        log_fatal("Sparse + Zlib decompression not currently supported.");
        return NULL;
    case 0x30: // Sparse + BZip2
        log_fatal("Sparse + BZip2 decompression not currently supported.");
        return NULL;
    case 0x41: // Huffman + WAV
        log_fatal("Huffman + WAV decompression not currently supported.");
        return NULL;
    case 0x48: // Pk + WAV
        log_fatal("Pk + WAV decompression not currently supported.");
        return NULL;
    case 0x81: // Huffman + WAV
        log_fatal("Huffman + WAV decompression not currently supported.");
        return NULL;
    case 0x88: // Pk + WAV
        log_fatal("Pk + WAV decompression not currently supported.");
        return NULL;
    }

    log_fatal("Unknown compression code: %#04x for file '%s'", compression_type, source->filename);
    return NULL;
}

void *mpq_stream_load_block(mpq_stream *source, uint32_t index, uint32_t length, uint32_t *to_read) {
    uint32_t offset;

    if (mpq_block_has_flag(source->block, MPQ_BLOCK_FLAG_COMPRESS) || mpq_block_has_flag(source->block, MPQ_BLOCK_FLAG_IMPLODE)) {
        offset = source->positions[index];
        *to_read = source->positions[index + 1] - offset;
    } else {
        offset = index * source->size;
        *to_read = length;
    }

    if (*to_read == 0) {
        return NULL;
    }

    offset += source->block->file_position;
    void *data = malloc(*to_read);

    FILE *file = mpq_get_file_stream(source->mpq);

    fseek(file, (long)offset, SEEK_SET);
    fread(data, *to_read, 1, file);

    if (mpq_block_has_flag(source->block, MPQ_BLOCK_FLAG_ENCRYPTED) && source->block->file_size_uncompressed > 3) {
        uint32_t encryption_seed = mpq_block_get_encryption_seed(source->block, source->filename);
        if (encryption_seed == 0) {
            log_error("Unable to determine encryption key for '%s'.", source->filename);
            free(data);
            return NULL;
        }

        crypto_decrypt_bytes(data, *to_read, index + encryption_seed);
    }

    if (mpq_block_has_flag(source->block, MPQ_BLOCK_FLAG_COMPRESS) && (*to_read != length)) {
        if (!mpq_block_has_flag(source->block, MPQ_BLOCK_FLAG_SINGLE_UNIT)) {
            return mpq_stream_decompress_multi(source, data, *to_read, length);
        }

        log_error("pk decompress");
        exit(EXIT_FAILURE);
    }

    if (mpq_block_has_flag(source->block, MPQ_BLOCK_FLAG_IMPLODE) && (*to_read != length)) {
        log_error("pk decompress");
        exit(EXIT_FAILURE);
    }

    return data;
}

bool mpq_stream_buffer_data(mpq_stream *source) {
    uint32_t block_index = source->position / source->size;

    if (block_index == source->index) {
        return true;
    }

    uint32_t expected_length = min(source->block->file_size_uncompressed - (block_index * source->size), source->size);
    uint32_t data_read;

    void *new_data = mpq_stream_load_block(source, block_index, expected_length, &data_read);

    if (new_data == NULL) {
        return false;
    }

    if (source->data != NULL) {
        free(source->data);
    }
    source->data = new_data;
    source->data_length = expected_length;
    source->index = block_index;

    return true;
}

uint32_t mpq_stream_read_internal(mpq_stream *source, void *buffer, uint32_t offset, uint32_t size) {
    if (!mpq_stream_buffer_data(source)) {
        return 0;
    }

    uint32_t local_position = source->position % source->size;

    return mpq_stream_copy(source, buffer, offset, local_position, size);
}

uint32_t mpq_stream_read(mpq_stream *source, void *buffer, uint32_t offset, uint32_t size) {
    if (mpq_block_has_flag(source->block, MPQ_BLOCK_FLAG_SINGLE_UNIT)) {
        return mpq_stream_read_internal_single_unit(source, buffer, offset, size);
    }

    uint32_t read_total = 0;

    for (uint32_t to_read = size; to_read > 0;) {
        uint32_t read = mpq_stream_read_internal(source, buffer, offset, to_read);

        if (read == 0) {
            break;
        }

        read_total += read;
        offset += read;
        to_read -= read;
    }

    return read_total;
}
