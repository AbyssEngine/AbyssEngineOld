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

#include "mpqtool.h"
#include "libabyss/utils.h"
#include <libabyss/mpq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

int mpqtool_run(int argc, char **argv) {
    if (argc == 0) {
        fprintf(stderr, "No MPQ file specified.\n");
        fprintf(stderr, "Usage:\n    abt mpq file.mpq [command]\n");
        return EXIT_FAILURE;
    }

    const char *mpq_path = argv[0];

    if (argc == 1) {
        fprintf(stderr, "No MPQ command specified.\n");
        fprintf(stderr, "Valid Commands:\n");
        fprintf(stderr, "    list    - Lists the files in the MPQ (based on the listfile)\n");
        fprintf(stderr, "    extract - Extracts one or more files from the MPQ\n");

        return EXIT_SUCCESS;
    }

    const char *command = argv[1];

    if (strcmp(command, "list") == 0) {
        return mpqtool_list(mpq_path);
    }

    if (strcmp(command, "extract") == 0) {
        if (argc != 3 || strcmp(argv[2], "help") == 0) {
            printf("Usage: mpq extract <filepath>\n");
            printf("Note that you can use forward slashes instead of backslashes for path separators.\n");
            return EXIT_SUCCESS;
        }
        return mpqtool_extract(mpq_path, argv[2]);
    }

    return EXIT_SUCCESS;
}

int mpqtool_extract_single_file(mpq *source, const char *output_root_path, const char *file_path) {
    uint32_t file_size;
    char *file_path_copy = strdup(file_path);
    char *final_path = util_fix_mpq_path(file_path_copy);
    util_trim_end_string(final_path);

    printf("Extracting '%s'...\n", final_path);
    char *data = mpq_read_file(source, final_path, &file_size);

    if (data == NULL) {
        fprintf(stderr, "Could not find file: '%s'", final_path);
        free(file_path_copy);
        return EXIT_FAILURE;
    }

    char *output_file_path = calloc(1, 4096);

    strcat(output_file_path, output_root_path);
    strcat(output_file_path, "/extracted/");
    strcat(output_file_path, final_path);
    util_trim_end_string(output_file_path);
    util_normalize_path(output_file_path);

    util_create_directories_recursively(output_file_path);

    FILE *file = fopen(output_file_path, "w");

    if (file == NULL) {
        perror("Could not open output file");
        free(output_file_path);
        free(data);
        return EXIT_FAILURE;
    }

    fwrite(data, 1, file_size, file);
    fclose(file);

    free(file_path_copy);
    free(output_file_path);
    free(data);
    return EXIT_SUCCESS;
}

int mpqtool_extract_all_files(mpq *source, const char *output_root_path) {
    uint32_t file_size;
    char *data = mpq_read_file(source, "(listfile)", &file_size);
    if (data == NULL) {
        fprintf(stderr, "No listfile was present in this archive.\n");
        return EXIT_FAILURE;
    }

#ifdef _WIN32
    char *token = strtok_s(data, "\n", &data);
#else
    char *token = strtok_r(data, "\n", &data);
#endif // _WIN32
    while (token != NULL) {
        mpqtool_extract_single_file(source, output_root_path, token);
#ifdef _WIN32
        token = strtok_s(NULL, "\n", &data);
#else
        token = strtok_r(NULL, "\n", &data);
#endif // _WIN32
    }

    free(data);
    return EXIT_SUCCESS;
}

int mpqtool_extract(const char *mpq_path, char *file_path) {
    mpq *source = mpq_load(mpq_path);

    if (source == NULL) {
        return EXIT_FAILURE;
    }

    const char *cwd = util_get_cwd();

    int result = (strcmp(file_path, "*") != 0) ? mpqtool_extract_single_file(source, cwd, file_path) : mpqtool_extract_all_files(source, cwd);

    free((void *)cwd);
    mpq_destroy(source);

    return result;
}

int mpqtool_list(const char *mpq_path) {
    mpq *source = mpq_load(mpq_path);

    if (source == NULL) {
        return EXIT_FAILURE;
    }

    uint32_t file_size;
    char *data = mpq_read_file(source, "(listfile)", &file_size);
    if (data == NULL) {
        fprintf(stderr, "No listfile was present in this archive.\n");
        mpq_destroy(source);
        return EXIT_FAILURE;
    }

    const char end_line[3] = "\r\n";
    char *token;

    token = strtok(data, end_line);

    while ((token != NULL) && (token < (char *)(data + file_size))) {
        if (strlen(token) == 0) {
            continue;
        }
        printf("%s\n", token);
        token = strtok(NULL, end_line);
    }

    mpq_destroy(source);
    return EXIT_SUCCESS;
}
