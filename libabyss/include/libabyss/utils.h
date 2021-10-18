#ifndef LIBABYSS_UTILS_H
#define LIBABYSS_UTILS_H

#include <stdbool.h>

char *util_fix_mpq_path(char *path);
void util_get_folder_path_part(char *path);
void util_normalize_path(char *path);
const char *util_get_cwd();
void util_create_directory(const char *path);
void util_create_directories_recursively(const char *path);
char *util_trim_string(char *str);
void util_trim_end_string(char *str);
bool util_is_space(unsigned char ch);

#endif // LIBABYSS_UTILS_H
