#include "help.h"
#include <stdio.h>
#include <config.h>

void help_show_general() {
    fprintf(stdout, "Abyss Engine v%i.%i - https://github.com/AbyssEngine\n", ABYSS_VERSION_MAJOR, ABYSS_VERSION_MINOR);
    printf("\nGeneral Usage:\n    abt [tool] [options]\n");
    printf("Tools:\n");
    printf("   mpq - Provides services for listing, viewing, exporting, and creating MPQ archives");
}

