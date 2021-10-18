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

#ifndef LIBABYSS_MPQ_STREAM
#define LIBABYSS_MPQ_STREAM

typedef struct mpq_stream mpq_stream;

mpq_stream *mpq_stream_new(mpq *mpq, mpq_block *block, const char *filename);
void mpq_stream_destroy(mpq_stream *source);
uint32_t mpq_stream_read(mpq_stream *source, void *buffer, uint32_t offset, uint32_t size);
#endif // LIBABYSS_MPQ_STREAM
