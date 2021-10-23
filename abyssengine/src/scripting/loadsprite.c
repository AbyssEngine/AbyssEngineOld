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
#include "../node/sprite/sprite.h"
#include "node.h"
#include "scripting.h"

int abyss_lua_sprite_cell_size(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, sprite)

        int cell_size_x;
        int cell_size_y;
        sprite_get_cell_size(source, &cell_size_x, &cell_size_y);

        lua_pushnumber(l, cell_size_x);
        lua_pushnumber(l, cell_size_y);

        return 2;
    }

    if (lua_gettop(l) != 3) {
        luaL_error(l, "expected cell sizes");
        return 0;
    }

    LCHECK_NUMBER(2);
    LCHECK_NUMBER(3);
    SCRIPT_GET_LUA_THIS(source, sprite)

    sprite_set_cell_size(source, lua_tonumber(l, 2), lua_tonumber(l, 3));

    return 0;
}

int abyss_lua_sprite_bottom_origin(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, sprite)

        lua_pushboolean(l, sprite_get_bottom_origin(source));

        return 1;
    }

    if (lua_gettop(l) != 2) {
        luaL_error(l, "expected origin boolean");
        return 0;
    }

    LCHECK_BOOLEAN(2);
    SCRIPT_GET_LUA_THIS(source, sprite)

    sprite_set_bottom_origin(source, lua_toboolean(l, 2));

    return 0;
}

int abyss_lua_sprite_blend_mode(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, sprite)
        lua_pushstring(l, blend_mode_to_string(sprite_get_blend_mode(source)));
        return 1;
    }

    LCHECK_NUMPARAMS(2)
    LCHECK_STRING(2);

    SCRIPT_GET_LUA_THIS(source, sprite)

    enum e_sprite_blend_mode blend_mode = string_to_blend_mode(lua_tostring(l, 2));

    if (blend_mode == sprite_blend_mode_unknown) {
        luaL_error(l, "unknown blend mode");
        return 0;
    }

    sprite_set_blend_mode(source, blend_mode);

    return 0;
}

int abyss_lua_sprite_play_mode(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, sprite)
        lua_pushstring(l, play_mode_to_string(sprite_get_play_mode(source)));
        return 1;
    }

    LCHECK_NUMPARAMS(2)
    LCHECK_STRING(2);

    SCRIPT_GET_LUA_THIS(source, sprite)

    enum e_sprite_play_mode play_mode = string_to_play_mode(lua_tostring(l, 2));

    if (play_mode == sprite_blend_mode_unknown) {
        luaL_error(l, "unknown play mode");
        return 0;
    }

    sprite_set_play_mode(source, play_mode);

    return 0;
}

int abyss_lua_sprite_play_length(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, sprite)
        lua_pushnumber(l, sprite_get_play_length(source));
        return 1;
    }

    LCHECK_NUMPARAMS(2)
    LCHECK_NUMBER(2);

    SCRIPT_GET_LUA_THIS(source, sprite)

    sprite_set_play_length(source, lua_tonumber(l, 2));

    return 0;
}

int abyss_lua_sprite_animation(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, sprite)
        lua_pushnumber(l, sprite_get_animation(source));
        return 1;
    }

    LCHECK_NUMPARAMS(2)
    LCHECK_NUMBER(2);

    SCRIPT_GET_LUA_THIS(source, sprite)

    sprite_set_animation(source, lua_tonumber(l, 2));

    return 0;
}

int abyss_lua_sprite_frame(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, sprite)
        lua_pushnumber(l, sprite_get_frame(source));
        return 1;
    }

    LCHECK_NUMPARAMS(2)
    LCHECK_NUMBER(2);

    SCRIPT_GET_LUA_THIS(source, sprite)

    sprite_set_frame(source, lua_tonumber(l, 2));

    return 0;
}

int abyss_lua_sprite_total_animations(lua_State *l) {
    LCHECK_NUMPARAMS(1)
    SCRIPT_GET_LUA_THIS(source, sprite)
    lua_pushnumber(l, sprite_get_animation_count(source));

    return 1;
}

int abyss_lua_sprite_frames_per_animation(lua_State *l) {
    LCHECK_NUMPARAMS(1)
    SCRIPT_GET_LUA_THIS(source, sprite)
    lua_pushnumber(l, sprite_get_frames_per_animation(source));

    return 1;
}

int abyss_lua_sprite_on_mouse_enter(lua_State *l) {
    LCHECK_NUMPARAMS(2)

    if (!lua_isfunction(l, 2)) {
        luaL_error(l, "a function must be supplied");
        return 0;
    }

    int ref = luaL_ref(l, LUA_REGISTRYINDEX);
    SCRIPT_GET_LUA_THIS(source, sprite);

    sprite_set_lua_mouse_enter_callback(source, ref);

    return 0;
}

int abyss_lua_sprite_on_mouse_leave(lua_State *l) {
    LCHECK_NUMPARAMS(2)

    if (!lua_isfunction(l, 2)) {
        luaL_error(l, "a function must be supplied");
        return 0;
    }

    int ref = luaL_ref(l, LUA_REGISTRYINDEX);
    SCRIPT_GET_LUA_THIS(source, sprite);

    sprite_set_lua_mouse_leave_callback(source, ref);

    return 0;
}

int abyss_lua_sprite_on_mouse_move(lua_State *l) {
    LCHECK_NUMPARAMS(2)

    if (!lua_isfunction(l, 2)) {
        luaL_error(l, "a function must be supplied");
        return 0;
    }

    int ref = luaL_ref(l, LUA_REGISTRYINDEX);
    SCRIPT_GET_LUA_THIS(source, sprite);

    sprite_set_lua_mouse_move_callback(source, ref);

    return 0;
}

int abyss_lua_sprite_on_mouse_button_up(lua_State *l) {
    LCHECK_NUMPARAMS(2)

    if (!lua_isfunction(l, 2)) {
        luaL_error(l, "a function must be supplied");
        return 0;
    }

    int ref = luaL_ref(l, LUA_REGISTRYINDEX);
    SCRIPT_GET_LUA_THIS(source, sprite);

    sprite_set_lua_mouse_up_callback(source, ref);

    return 0;
}

int abyss_lua_sprite_on_mouse_button_down(lua_State *l) {
    LCHECK_NUMPARAMS(2)

    if (!lua_isfunction(l, 2)) {
        luaL_error(l, "a function must be supplied");
        return 0;
    }

    int ref = luaL_ref(l, LUA_REGISTRYINDEX);
    SCRIPT_GET_LUA_THIS(source, sprite);

    sprite_set_lua_mouse_down_callback(source, ref);

    return 0;
}

int abyss_lua_load_sprite(lua_State *l) {
    LCHECK_NUMPARAMS(2)
    LCHECK_STRING(1);
    LCHECK_STRING(2);

    const char *path = lua_tostring(l, 1);
    const char *palette_name = lua_tostring(l, 2);

    sprite *result = sprite_load(path, palette_name);

    if (result == NULL) {
        luaL_error(l, "Error loading sprite.\nInvalid file or palette name.");
        return 0;
    }

    SCRIPT_CLASS_RESULT_START(result)
    SCRIPT_CLASS_RESULT_PROPERTY("cellSize", abyss_lua_sprite_cell_size)
    SCRIPT_CLASS_RESULT_PROPERTY("bottomOrigin", abyss_lua_sprite_bottom_origin)
    SCRIPT_CLASS_RESULT_PROPERTY("blendMode", abyss_lua_sprite_blend_mode)
    SCRIPT_CLASS_RESULT_PROPERTY("playMode", abyss_lua_sprite_play_mode)
    SCRIPT_CLASS_RESULT_PROPERTY("playLength", abyss_lua_sprite_play_length)
    SCRIPT_CLASS_RESULT_PROPERTY("animation", abyss_lua_sprite_animation)
    SCRIPT_CLASS_RESULT_PROPERTY("frame", abyss_lua_sprite_frame)
    SCRIPT_CLASS_RESULT_PROPERTY("totalAnimations", abyss_lua_sprite_total_animations)
    SCRIPT_CLASS_RESULT_PROPERTY("framesPerAnimation", abyss_lua_sprite_frames_per_animation)
    SCRIPT_CLASS_RESULT_PROPERTY("onMouseEnter", abyss_lua_sprite_on_mouse_enter)
    SCRIPT_CLASS_RESULT_PROPERTY("onMouseLeave", abyss_lua_sprite_on_mouse_leave)
    SCRIPT_CLASS_RESULT_PROPERTY("onMouseMove", abyss_lua_sprite_on_mouse_move)
    SCRIPT_CLASS_RESULT_PROPERTY("onMouseButtonDown", abyss_lua_sprite_on_mouse_button_down)
    SCRIPT_CLASS_RESULT_PROPERTY("onMouseButtonUp", abyss_lua_sprite_on_mouse_button_up)
    ADD_NODE_SCRIPT_CLASS_RESULT_PROPERTIES

    return 1;
}
