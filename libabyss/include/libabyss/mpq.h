#ifndef LIBABYSS_MPQ_H
#define LIBABYSS_MPQ_H

#include <stdbool.h>
typedef struct mpq mpq;

mpq *mpq_load(const char* file_path);
void mpq_destroy(mpq *source);
bool mpq_file_exists(const mpq *source, const char* filename);
void *mpq_read_file(const mpq *source, const char* filename);

#endif // LIBABYSS_MPQ_H
