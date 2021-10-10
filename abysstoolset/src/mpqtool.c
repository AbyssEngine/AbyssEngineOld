#include "mpqtool.h"
#include <libabyss/mpq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int mpqtool_run(int argc, char** argv) {
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

        return EXIT_FAILURE;
    }

    const char *command = argv[1];

    if (strcmp(command, "list") == 0) {
        return mpqtool_list(mpq_path);
    }

    return EXIT_SUCCESS;
}

int mpqtool_list(const char * mpq_path) {
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

    /* walk through other tokens */
    while( (token != NULL) && (token < (char*)(data+file_size))) {
        printf( "%s\n", token );

        token = strtok(NULL, end_line);
    }


    mpq_destroy(source);
    return EXIT_SUCCESS;
}
