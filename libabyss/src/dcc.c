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

#include "libabyss/log.h"
#include "streamreader.h"
#include <libabyss/dcc.h>
#include <stdlib.h>

#define READ_BYTE(field) dcc->field = streamreader_read_byte(reader)
#define READ_UINT32(field) dcc->field = streamreader_read_uint32(reader)

#define DCC_FILE_SIGNATURE 0x74

bool dcc_decode_header(dcc *dcc, streamreader *reader) {
    uint8_t signature = streamreader_read_byte(reader);

    if (signature != DCC_FILE_SIGNATURE) {
        log_fatal("Unexpected file signature: %x, expecting %x", signature, DCC_FILE_SIGNATURE);
        return false;
    }

    READ_BYTE(version);
    READ_BYTE(number_of_directions);
    READ_UINT32(frames_per_direction);

    return true;
}

bool dcc_decode_body(dcc *dcc, streamreader *reader) { return true; }

dcc *dcc_new_from_bytes(const void *data, uint64_t size) {
    log_fatal("not implemented");
    return NULL;
    dcc *result = calloc(1, sizeof(dcc));
    streamreader *reader = streamreader_create(data, size);

    if (!dcc_decode_header(result, reader)) {
        free(result);
        return NULL;
    }

    dcc_decode_body(result, reader);

    streamreader_destroy(reader);
    return result;
}

void dcc_destroy(dcc *source) { free(source); }
