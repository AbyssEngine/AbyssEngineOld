#include <libabyss/utils.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif // _WIN32

char *util_fix_mpq_path(char *path) {
    if (path[0] == '\0') {
        return path;
    }

    for (char *ch = path; *ch != '\0'; ch++) {
        if (*ch != '/') {
            continue;
        }
        *ch = '\\';
    }

    char *result = (path[0] == '\\') ? path + 1 : path;
    util_trim_end_string(result);
    return result;
}

void util_normalize_path(char *path) {
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

const char *util_get_cwd() {
    char *result = calloc(1, 2048);

#ifdef _WIN32
    _getcwd(result, 2048);
#else  // _WIN32
    getcwd(result, 2048);
#endif // _WIN32

    return result;
}

void util_create_directory(const char *path) {
#ifdef _WIN32
    CreateDirectoryA(path, NULL);
#else  // _WIN32
    struct stat s = {0};
    if (stat(path, &s) != -1) {
        return;
    }
    mkdir(path, 0777);
#endif // _WIN32
}

void util_create_directories_recursively(const char *path) {
    char *original_path = calloc(1, 2048);
    char *new_path = calloc(1, 2048);
    strcat(original_path, path);
    original_path[(int)(strrchr(original_path, '/') - original_path)] = '\0';

#ifdef _WIN32
    char *token = strtok_s(original_path, "/", &original_path);
#else
    char *token = strtok_r(original_path, "/", &original_path);
#endif // _WIN32

    while (token != NULL) {
        strcat(new_path, "/");
        strcat(new_path, token);
        util_create_directory(new_path);
#ifdef _WIN32
        token = strtok_s(NULL, "/", &original_path);
#else
        token = strtok_r(NULL, "/", &original_path);
#endif // _WIN32
    }

    free(original_path);
    free(new_path);
}

bool util_is_space(unsigned char ch) { return (ch == '\r') || (ch == ' ') || (ch == '\t' || (ch == '\n')); }

void util_trim_end_string(char *str) {
    char *end;

    end = str + strlen(str) - 1;
    while (end > str && util_is_space((unsigned char)*end)) {
        end--;
    }

    end[1] = '\0';
}

char *util_trim_string(char *str) {
    while (util_is_space((unsigned char)*str)) {
        str++;
    }

    if (*str == 0) {
        return str;
    }

    util_trim_end_string(str);

    return str;
}
