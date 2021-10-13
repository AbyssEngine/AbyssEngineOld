#include "help.h"
#include "mpqtool.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc <= 1) {
        help_show_general();
        return EXIT_SUCCESS;
    }

    char *tool_name = argv[1];

    for (char *p=tool_name; *p; p++) {
        *p = (char)tolower(*p);
    }

    if (strcmp(tool_name, "mpq") == 0) {
        return mpqtool_run(argc-2, &argv[2]);
    } else {
        fprintf(stderr, "Unknown tool '%s' specified.", tool_name);
        return EXIT_FAILURE;
    }}
