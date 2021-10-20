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

#include "compress.h"
#include "streamreader.h"
#include "streamwriter.h"
#include <stdlib.h>
#include <string.h>
#include <libabyss/log.h>

void *compress_decompress_wav(void *buffer, uint32_t buffer_size, int channel_count) {
    int array_1[] = {0x2C, 0x2C};
    int *array_2 = malloc(sizeof(int) * channel_count);
    const int lookup[] = {0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x0010, 0x0011, 0x0013, 0x0015, 0x0017, 0x0019, 0x001C,
                          0x001F, 0x0022, 0x0025, 0x0029, 0x002D, 0x0032, 0x0037, 0x003C, 0x0042, 0x0049, 0x0050, 0x0058, 0x0061, 0x006B, 0x0076,
                          0x0082, 0x008F, 0x009D, 0x00AD, 0x00BE, 0x00D1, 0x00E6, 0x00FD, 0x0117, 0x0133, 0x0151, 0x0173, 0x0198, 0x01C1, 0x01EE,
                          0x0220, 0x0256, 0x0292, 0x02D4, 0x031C, 0x036C, 0x03C3, 0x0424, 0x048E, 0x0502, 0x0583, 0x0610, 0x06AB, 0x0756, 0x0812,
                          0x08E0, 0x09C3, 0x0ABD, 0x0BD0, 0x0CFF, 0x0E4C, 0x0FBA, 0x114C, 0x1307, 0x14EE, 0x1706, 0x1954, 0x1BDC, 0x1EA5, 0x21B6,
                          0x2515, 0x28CA, 0x2CDF, 0x315B, 0x364B, 0x3BB9, 0x41B2, 0x4844, 0x4F7E, 0x5771, 0x602F, 0x69CE, 0x7462, 0x7FFF};
    const int lookup_2[] = {-1, 0, -1, 4, -1, 2, -1, 6, -1, 1, -1, 5, -1, 3, -1, 7, -1, 1, -1, 5, -1, 3, -1, 7, -1, 2, -1, 4, -1, 6, -1, 8};

    streamreader *input = streamreader_create(buffer, buffer_size);
    streamwriter *output = streamwriter_create();

    uint8_t shift = streamreader_read_byte(input);

    for (int i = 0; i < channel_count; i++) {
        uint16_t temp = streamreader_read_int16(input);
        array_2[i] = (int)temp;
        streamwriter_push_int16(output, temp);
    }

    int channel = channel_count - 1;
    if (channel > 1) {
        log_fatal("invalid channel id: %d", channel);
        exit(-1);
    }

    while (!streamreader_eof(input)) {
        uint8_t value = streamreader_read_byte(input);

        if (channel_count == 2) {
            channel = 1 - channel;
        }

        if (value & 0x80) {
            switch (value & 0x7f) {
            case 0:
                if (array_1[channel])
                    array_1[channel]--;
                streamwriter_push_int16(output, array_2[channel]);
                break;
            case 1:
                array_1[channel] += 8;
                if (array_1[channel] > 0x58)
                    array_1[channel] = 0x58;
                if (channel_count == 2)
                    channel = 1 - channel;
                break;
            default:
                array_1[channel] -= 8;
                if (array_1[channel] < 0)
                    array_1[channel] = 0;
                if (channel_count == 2)
                    channel = 1 - channel;
                break;
            }
            continue;
        }

        int temp1 = lookup[array_1[channel]];
        int temp2 = temp1 >> shift;

        if (value & 1)
            temp2 += temp1 >> 0;
        if (value & 2)
            temp2 += temp1 >> 1;
        if (value & 4)
            temp2 += temp1 >> 2;
        if (value & 8)
            temp2 += temp1 >> 3;
        if (value & 0x10)
            temp2 += temp1 >> 4;
        if (value & 0x20)
            temp2 += temp1 >> 5;

        int temp3 = array_2[channel];
        if (value & 0x40) {
            temp3 -= temp2;
            if (temp3 < -32768)
                temp3 = -32768;
        } else {
            temp3 += temp2;
            if (temp3 > 32767)
                temp3 = 32767;
        }
        array_2[channel] = temp3;
        streamwriter_push_int16(output, temp3);
        array_1[channel] += lookup_2[value & 0x1f];

        if (array_1[channel] < 0) {
            array_1[channel] = 0;
        } else if (array_1[channel] > 0x58) {
            array_1[channel] = 0x58;
        }
    }

    if (buffer_size < streamwriter_get_size(output)) {
        free(buffer);
        buffer = malloc(streamwriter_get_size(output));
    }

    memcpy(buffer, streamwriter_get_data(output), streamwriter_get_size(output));

    streamwriter_destroy(output);
    streamreader_destroy(input);
    free(array_2);
    return buffer;
}
