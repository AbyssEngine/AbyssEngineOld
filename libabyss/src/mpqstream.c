#include "crypto.h"
#include <libabyss/log.h>
#include <libabyss/mpq.h>
#include <libabyss/mpqblock.h>
#include <libabyss/mpqstream.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct mpq_stream {
    void *data;
    const char *filename;
    uint32_t *positions;
    const mpq *mpq;
    mpq_block *block;
    uint32_t index;
    uint32_t size;
    uint32_t position;
} mpq_stream;

bool mpq_stream_load_block_offsets(mpq_stream *source) {
    uint32_t block_position_count = ((source->block->file_size_uncompressed + source->size - 1) / source->size) + 1;
    source->positions = realloc(source->positions, sizeof(uint32_t) * block_position_count);

    FILE *file_stream = mpq_get_file_stream(source->mpq);
    fseek(file_stream, source->block->file_position, SEEK_SET);
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

uint32_t mpq_stream_read_internal_single_unit(mpq_stream *stream, void *buffer, uint32_t offset, uint32_t size) {
    log_info("single unit!");
    exit(-1);
    return 0;
}

uint32_t mpq_stream_read_internal(mpq_stream *source, void *buffer, uint32_t offset, uint32_t read) {
    log_info("multi unit!");
    exit(-1);
    return 0;
}

uint32_t mpq_stream_read(mpq_stream *source, void* buffer, uint32_t offset, uint32_t size) {
    if (mpq_block_has_flag(source->block, MPQ_BLOCK_FLAG_SINGLE_UNIT)) {
        return mpq_stream_read_internal_single_unit(source, buffer, offset, size);
    }

    uint32_t read_total = 0;

    for (uint32_t to_read = size; to_read > 0;) {
        uint32_t read = mpq_stream_read_internal(source, buffer, offset, to_read);

        if (read==0) {
            break;
        }

        read_total += read;
        offset += read;
        to_read -= read;
    }

    return read_total;
}
