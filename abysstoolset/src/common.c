#include "common.h"
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif // _WIN32

char * common_fix_mpq_path(char * path) {
    if (path[0] == '\0') {
        return path;
    }
    
    for (char *ch = path; *ch != '\0'; ch++) {
        if (*ch != '/') {
            continue;
        }
        *ch = '\\';
    }
    
    return (path[0] == '\\')
        ? path+1
        : path;
}

void common_normalize_path(char *path) {
    if (path[0] == '\0') {
        return;
    }
    
    for (char *ch = path; *ch != '\0'; ch++) {
        if (*ch != '\\') {
            continue;
        }
        
        *ch = '/';
    }
}

const char* common_get_cwd() {
    char *result = calloc(1, 2048);
    
#ifdef _WIN32
    _getcwd(result, 2048);
#else // _WIN32
    getcwd(result, 2048);
#endif // _WIN32
    
    return result;
}

void common_create_directory(const char *path) {
#ifdef _WIN32
    // TODO: Win32
    fprintf(stderr, "Win32 not implemented for common_create_directory");
    exit(-1);
#else // _WIN32
    struct stat s = {0};
    if (stat(path, &s) != -1) {
        return;
    }
    mkdir(path, 0777);
#endif // _WIN32
}

void common_create_directories_recursively(const char *path) {
    char *original_path = calloc(1, 2048);
    char *new_path = calloc(1, 2048);
    strcat(original_path, path);
    original_path[(int)(strrchr(original_path, '/')-original_path)] = '\0';
    
    char *token = strtok_r(original_path, "/", &original_path);
    
    while (token != NULL) {
        strcat(new_path, "/");
        strcat(new_path, token);
        common_create_directory(new_path);
        token = strtok_r(NULL, "/", &original_path);
    }
    
    free(original_path);
    free(new_path);
}


bool common_is_space(unsigned char ch) {
    return (ch == '\r') || (ch == ' ') || (ch == '\t' || (ch == '\n'));
}

void common_trim_end_string(char *str) {
    char *end;

    end = str + strlen(str) - 1;
    while (end > str && common_is_space((unsigned char)*end)) {
        end--;
    }

    end[1] = '\0';
}


char *common_trim_string(char *str) {
    while (common_is_space((unsigned char)*str)) {
        str++;
    }

    if (*str == 0) {
        return str;
    }

    common_trim_end_string(str);

    return str;
}
