#include "crypto.h"
#include <libabyss/mpqblock.h>
#include <string.h>

bool mpq_block_has_flag(const mpq_block *source, const mpq_block_flag flag) {
    return (source->flags & flag) != 0;
}

uint32_t mpq_block_get_encryption_seed(mpq_block *source, const char* filename) {
    const char *filename_start = strrchr(filename, '\\');
    if (filename_start == 0) {
        filename_start = filename;
    } else {
        filename_start++;
    }
    uint32_t seed = crypto_hash_string(filename_start, 3);
    return (seed + source->file_position) ^ source->file_size_uncompressed;
}
