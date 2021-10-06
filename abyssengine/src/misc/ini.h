#ifndef ABYSS_INI_H
#define ABYSS_INI_H

typedef struct ini_file_entry {
    char *name;
    char *value;
} ini_file_entry;

typedef struct ini_file_category {
    char *name;
    ini_file_entry **entries;
    int num_entries;
} ini_file_category;

typedef struct ini_file {
    ini_file_category **categories;
    int num_categories;
} ini_file;

ini_file *ini_file_load(const char *file_path);
void ini_file_destroy(ini_file *source);
ini_file_category *ini_file_add_category(ini_file *source, const char *category_name);
ini_file_category *ini_file_get_category(ini_file *source, const char *category_name);
ini_file_entry *ini_file_add_entry(ini_file_category *source, const char *name, const char *value);
void ini_file_destroy_category(ini_file_category *source);
void ini_file_destroy_entry(ini_file_entry *source);
ini_file_entry *ini_file_get_entry(ini_file_category *source, const char *name);
const char *init_file_get_value(ini_file *source, const char *category, const char *name);
#endif // ABYSS_INI_H
