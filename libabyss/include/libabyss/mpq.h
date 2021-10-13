#ifndef LIBABYSS_MPQ_H
#define LIBABYSS_MPQ_H

#include "mpqblock.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
typedef struct mpq mpq;

mpq *mpq_load(const char* file_path);
void mpq_destroy(mpq *source);
bool mpq_file_exists(const mpq *source, const char* filename);
void *mpq_read_file(mpq *source, const char* filename, uint32_t *file_size);
uint32_t mpq_get_block_size(const mpq *source);
FILE *mpq_get_file_stream(const mpq *source);
mpq_block *mpq_get_block(const mpq *source, const char* filename);
uint32_t mpq_get_header_size(const mpq *source);

#endif // LIBABYSS_MPQ_H
