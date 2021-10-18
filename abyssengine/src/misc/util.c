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

#include "util.h"

#include "../engine/engine.h"
#include <libabyss/log.h>
#include <png.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static const char *b64bytes = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static inline int find_idx(const char ch) { return (int)(strchr(b64bytes, ch) - b64bytes); }

typedef struct png_read_data_handle {
    unsigned long pos;
    const void *data;
} png_read_data_handle;

void *util_base64_decode(const char *source, unsigned int bytes) {
    uint8_t *result = malloc(bytes);
    uint8_t *result_ptr = result;

    const char *source_ptr = source;
    const char *source_ptr_end = source + strlen(source);

    char b[4];

    while (source_ptr != source_ptr_end) {
        for (int i = 0; i < 4; i++) {
            b[i] = *source_ptr++;
        }

        *result_ptr++ = (find_idx(b[0]) << 2) | (find_idx(b[1]) >> 4);

        if (b[2] == '=') {
            continue;
        }

        *result_ptr++ = ((find_idx(b[1]) & 0x0F) << 4) + (find_idx(b[2]) >> 2);

        if (b[3] == '=') {
            continue;
        }

        *result_ptr++ = ((find_idx(b[2]) & 0x03) << 6) | find_idx(b[3]);
    }

    return result;
}

void png_error_func(png_structp png, png_const_charp error_message) { log_error("LibPNG: %s", error_message); }

void png_warn_func(png_structp png, png_const_charp warn_message) { log_warn("LibPNG: %s", warn_message); }

void png_reader(png_structp png, png_bytep data_out, png_size_t to_read) {
    png_read_data_handle *data_handle = (png_read_data_handle *)png_get_io_ptr(png);
    const png_byte *read_src = (png_byte *)data_handle->data + data_handle->pos;
    memcpy(data_out, read_src, to_read);
    data_handle->pos += to_read;
}

SDL_Texture *util_load_texture_png(const void *source, int *xwidth, int *xheight) {
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, png_error_func, png_warn_func);
    if (png == NULL) {
        log_error("Could not initialize LibPNG");
        abort();
    }
    png_infop info = png_create_info_struct(png);
    if (info == NULL) {
        log_error("Could not initialize the LibPNG info structure");
        abort();
    }
    png_read_data_handle data_handle;
    data_handle.pos = 0;
    data_handle.data = source;
    png_set_read_fn(png, &data_handle, png_reader);
    png_read_info(png, info);

    png_uint_32 width = png_get_image_width(png, info);
    png_uint_32 height = png_get_image_height(png, info);
    png_byte depth = png_get_bit_depth(png, info);
    png_byte color_type = png_get_color_type(png, info);

    if (depth == 16) {
        png_set_strip_16(png);
    }

    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png);
    } else if ((color_type == PNG_COLOR_TYPE_GRAY) && (depth < 8)) {
        png_set_expand_gray_1_2_4_to_8(png);
    }

    if (png_get_valid(png, info, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png);
    }

    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(png);
    }

    if ((color_type == PNG_COLOR_TYPE_RGB) || (color_type == PNG_COLOR_TYPE_GRAY) || (color_type == PNG_COLOR_TYPE_PALETTE)) {
        png_set_add_alpha(png, 0xFF, PNG_FILLER_AFTER);
    }

    png_read_update_info(png, info);
    size_t pitch = png_get_rowbytes(png, info);

    png_bytep *row_data = (png_bytep *)malloc(sizeof(png_bytep) * height);
    for (int y = 0; y < height; y++) {
        row_data[y] = (png_byte *)malloc(pitch);
    }

    png_read_image(png, row_data);

    void *pixels = malloc(height * pitch);
    uint8_t *ptr = pixels;
    for (int y = 0; y < height; y++) {
        memcpy(ptr, (uint8_t *)row_data[y], pitch);
        ptr += pitch;
    }

    SDL_Texture *texture = SDL_CreateTexture(engine_get_renderer(engine_get_global_instance()), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC,
                                             (int)width, (int)height);

    SDL_UpdateTexture(texture, NULL, pixels, pitch);

    free(pixels);
    for (int y = 0; y < height; y++) {
        free(row_data[y]);
    }
    free(row_data);
    png_destroy_read_struct(&png, &info, NULL);

    if (xwidth != NULL) {
        *xwidth = width;
    }

    if (xheight != NULL) {
        *xheight = height;
    }

    return texture;
}

char *str_replace(char *string, const char *substr, const char *replacement) {
    char *tok = NULL;
    char *newstr = NULL;
    char *oldstr = NULL;

    if (substr == NULL || replacement == NULL) {
        return strdup(string);
    }

    newstr = strdup(string);

    while ((tok = strstr(newstr, substr))) {
        oldstr = newstr;
        newstr = malloc(strlen(oldstr) - strlen(substr) + strlen(replacement) + 1);

        if (newstr == NULL) {
            free(oldstr);
            return NULL;
        }

        memcpy(newstr, oldstr, tok - oldstr);
        memcpy(newstr + (tok - oldstr), replacement, strlen(replacement));
        memcpy(newstr + (tok - oldstr) + strlen(replacement), tok + strlen(substr), strlen(oldstr) - strlen(substr) - (tok - oldstr));
        memset(newstr + strlen(oldstr) - strlen(substr) + strlen(replacement), 0, 1);

        free(oldstr);
    }

    free(string);

    return newstr;
}

bool is_space(unsigned char ch) { return (ch == '\r') || (ch == ' ') || (ch == '\t' || (ch == '\n')); }

char *trim_string(char *str) {
    char *end;

    while (is_space((unsigned char)*str)) {
        str++;
    }

    if (*str == 0) {
        return str;
    }

    end = str + strlen(str) - 1;
    while (end > str && is_space((unsigned char)*end)) {
        end--;
    }

    end[1] = '\0';

    return str;
}
