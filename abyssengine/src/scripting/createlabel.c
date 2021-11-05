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

#include "../engine/engine.h"
#include "../engine/spritefont.h"
#include "../node/label/label.h"
#include "node.h"
#include "scripting.h"

int abyss_lua_label_destroy(lua_State *l) {
    LCHECK_NUMPARAMS(1)
    SCRIPT_GET_LUA_CLASS(source, label, 1)

    label_destroy(source);

    return 0;
}

typedef struct label_caption_dispatch_info {
    char *new_caption;
    label *label;
} label_caption_dispatch_info;
void abyss_lua_label_caption_dispatch(void *data) {
    label_caption_dispatch_info *info = (label_caption_dispatch_info *)data;
    label_set_text(info->label, info->new_caption);
    free(info->new_caption);
    free(info);
}

int abyss_lua_label_caption(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_CLASS(source, label, 1)
        lua_pushstring(l, label_get_text(source));
        return 1;
    }

    if (lua_gettop(l) == 2) {
        SCRIPT_GET_LUA_CLASS(source, label, 1)
        LCHECK_STRING(2);

        const char *new_caption = lua_tostring(l, 2);

        label_caption_dispatch_info *info = malloc(sizeof(label_caption_dispatch_info));
        info->label = source;
        info->new_caption = strdup(new_caption);

        engine_dispatch(engine_get_global_instance(), abyss_lua_label_caption_dispatch, info);
        return 0;
    }

    luaL_error(l, "expected 0 or 1 arguments");
    return 0;
}

int abyss_lua_label_h_align(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_CLASS(source, label, 1)
        lua_pushstring(l, alignment_to_string(label_get_horizontal_alignment(source)));
        return 1;
    }

    if (lua_gettop(l) == 2) {
        SCRIPT_GET_LUA_CLASS(source, label, 1)
        e_alignment align = string_to_alignment(lua_tostring(l, 2));
        if (align == alignment_unknown) {
            luaL_error(l, "invalid alignment");
            return 0;
        }

        label_set_horizontal_alignment(source, align);

        return 0;
    }

    return 0;
}

int abyss_lua_label_v_align(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_CLASS(source, label, 1)
        lua_pushstring(l, alignment_to_string(label_get_vertical_alignment(source)));
        return 1;
    }

    if (lua_gettop(l) == 2) {
        SCRIPT_GET_LUA_CLASS(source, label, 1)
        e_alignment align = string_to_alignment(lua_tostring(l, 2));
        if (align == alignment_unknown) {
            luaL_error(l, "invalid alignment");
            return 0;
        }

        label_set_vertical_alignment(source, align);

        return 0;
    }

    return 0;
}

int abyss_lua_label_blend_mode(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, label)
        lua_pushstring(l, blend_mode_to_string(label_get_blend_mode(source)));
        return 1;
    }

    LCHECK_NUMPARAMS(2)
    LCHECK_STRING(2);

    SCRIPT_GET_LUA_THIS(source, label)

    const e_blend_mode blend_mode = string_to_blend_mode(lua_tostring(l, 2));

    if (blend_mode == blend_mode_invalid) {
        luaL_error(l, "unknown blend mode");
        return 0;
    }

    label_set_blend_mode(source, blend_mode);

    return 0;
}

int abyss_lua_label_color_mod(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, label)
        rgb color = label_get_color_mod(source);
        lua_pushnumber(l, color.r);
        lua_pushnumber(l, color.g);
        lua_pushnumber(l, color.b);
        return 3;
    }

    if (lua_gettop(l) == 4) {
        SCRIPT_GET_LUA_THIS(source, label)
        rgb color = {lua_tonumber(l, 2), lua_tonumber(l, 3), lua_tonumber(l, 4)};
        label_set_color_mod(source, color);

        return 0;
    }

    luaL_error(l, "expected 0 or 3 arguments");
    return 0;
}
int abyss_lua_create_label(lua_State *l) {
    LCHECK_NUMPARAMS(1)
    SCRIPT_GET_LUA_CLASS(font, spritefont, 1)

    label *result = label_create(font);

    SCRIPT_CLASS_RESULT_START(result)
    SCRIPT_CLASS_RESULT_PROPERTY("destroy", abyss_lua_label_destroy)
    SCRIPT_CLASS_RESULT_PROPERTY("caption", abyss_lua_label_caption)
    SCRIPT_CLASS_RESULT_PROPERTY("hAlign", abyss_lua_label_h_align)
    SCRIPT_CLASS_RESULT_PROPERTY("vAlign", abyss_lua_label_v_align)
    SCRIPT_CLASS_RESULT_PROPERTY("blendMode", abyss_lua_label_blend_mode)
    SCRIPT_CLASS_RESULT_PROPERTY("colorMod", abyss_lua_label_color_mod)
    ADD_NODE_SCRIPT_CLASS_RESULT_PROPERTIES

    return 1;
}
