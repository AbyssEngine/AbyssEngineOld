#ifndef ABYSSTOOLSET_COMMON_H
#define ABYSSTOOLSET_COMMON_H

#include <stdbool.h>

char* common_fix_mpq_path(char *path);
void common_normalize_path(char *path);
const char* common_get_cwd();
void common_create_directory(const char *path);
void common_create_directories_recursively(const char *path);
char *common_trim_string(char *str);
void common_trim_end_string(char *str);
bool common_is_space(unsigned char ch);

#endif // ABYSSTOOLSET_COMMON_H
