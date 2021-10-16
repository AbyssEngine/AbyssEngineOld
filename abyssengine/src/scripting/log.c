#include "libabyss/log.h"
#include "scripting.h"
#include <string.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"
int abyss_lua_log(lua_State *l) {
    LCHECK_NUMPARAMS(2)

    LCHECK_STRING(1);
    LCHECK_STRING(2);

    int log_level;
    const char *logType = lua_tostring(l, 1);
    const char *message = lua_tostring(l, 2);

    if (strcmp(logType, "info") == 0) {
        log_level = LOG_INFO;
    } else if (strcmp(logType, "error") == 0) {
        log_level = LOG_ERROR;
    } else if (strcmp(logType, "fatal") == 0) {
        log_level = LOG_FATAL;
    } else if (strcmp(logType, "warn") == 0) {
        log_level = LOG_WARN;
    } else if (strcmp(logType, "debug") == 0) {
        log_level = LOG_DEBUG;
    } else if (strcmp(logType, "trace") == 0) {
        log_level = LOG_TRACE;
    } else {
        log_error("bla");
        luaL_argerror(l, 1, "unexpected log type");
        return 0;
    }

    lua_Debug ar;
    lua_getstack(l, 1, &ar);
    lua_getinfo(l, "nSl", &ar);
    log_log(log_level, &ar.source[1], ar.currentline, "%s", message);

    return 0;
}
#pragma clang diagnostic pop
