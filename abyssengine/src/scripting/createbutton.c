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

#include "../common/string.h"
#include "../engine/spritefont.h"
#include "../node/button/button.h"
#include "node.h"
#include "scripting.h"

int abyss_lua_button_destroy(lua_State *l) {
    LCHECK_NUMPARAMS(1)
    SCRIPT_GET_LUA_CLASS(source, button, 1)

    button_destroy(source);

    return 0;
}

int abyss_lua_button_state(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, button)
        lua_pushstring(l, button_state_to_string(button_get_state(source)));
        return 1;
    }

    if (lua_gettop(l) == 2) {
        SCRIPT_GET_LUA_THIS(source, button)
        e_button_state new_state = string_to_button_state(lua_tostring(l, 2));

        if (new_state == button_state_unknown) {
            luaL_error(l, "invalid button state");
            return 0;
        }

        return 0;
    }

    luaL_error(l, "invalid number of arguments.");
    return 0;
}

int abyss_lua_button_caption(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, button)
        lua_pushstring(l, button_get_caption(source));
        return 1;
    }

    if (lua_gettop(l) == 2) {
        SCRIPT_GET_LUA_THIS(source, button)
        button_set_caption(source, lua_tostring(l, 2));
        return 0;
    }

    luaL_error(l, "invalid number of arguments.");
    return 0;
}

int abyss_lua_button_checked(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, button)
        lua_pushboolean(l, button_get_checked(source));
        return 1;
    }

    if (lua_gettop(l) == 2) {
        SCRIPT_GET_LUA_THIS(source, button)
        button_set_checked(source, lua_toboolean(l, 2));
        return 0;
    }

    luaL_error(l, "invalid number of arguments.");
    return 0;
}

int abyss_lua_button_checkbox(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, button)
        lua_pushboolean(l, button_get_is_checkbox(source));
        return 1;
    }

    if (lua_gettop(l) == 2) {
        SCRIPT_GET_LUA_THIS(source, button)
        button_set_is_checkbox(source, lua_toboolean(l, 2));
        return 0;
    }

    luaL_error(l, "invalid number of arguments.");
    return 0;
}

int abyss_lua_button_segments(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, button)
        lua_pushnumber(l, button_get_x_segments(source));
        lua_pushnumber(l, button_get_y_segments(source));
        return 2;
    }

    if (lua_gettop(l) == 3) {
        SCRIPT_GET_LUA_THIS(source, button)
        button_set_x_segments(source, lua_tointeger(l, 2));
        button_set_y_segments(source, lua_tointeger(l, 3));
        return 0;
    }

    luaL_error(l, "invalid number of arguments.");
    return 0;
}

int abyss_lua_button_text_offset(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, button)
        lua_pushnumber(l, button_get_text_offset_x(source));
        lua_pushnumber(l, button_get_text_offset_y(source));
        return 2;
    }

    if (lua_gettop(l) == 3) {
        SCRIPT_GET_LUA_THIS(source, button)
        button_set_text_offset_x(source, lua_tointeger(l, 2));
        button_set_text_offset_y(source, lua_tointeger(l, 3));
        return 0;
    }

    luaL_error(l, "invalid number of arguments.");
    return 0;
}

int abyss_lua_button_size(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, button)
        lua_pushnumber(l, button_get_fixed_width(source));
        lua_pushnumber(l, button_get_fixed_height(source));
        return 2;
    }
    int x = lua_gettop(l);
    if (lua_gettop(l) == 3) {
        SCRIPT_GET_LUA_THIS(source, button)
        button_set_fixed_width(source, lua_tointeger(l, 2));
        button_set_fixed_height(source, lua_tointeger(l, 3));
        return 0;
    }

    luaL_error(l, "invalid number of arguments.");
    return 0;
}

int abyss_lua_button_label_color(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, button)
        rgb color = button_get_label_color(source);
        lua_pushinteger(l, color.r);
        lua_pushinteger(l, color.g);
        lua_pushinteger(l, color.g);
        return 3;
    }

    if (lua_gettop(l) == 4) {
        SCRIPT_GET_LUA_THIS(source, button)
        rgb color;
        color.r = lua_tointeger(l, 2);
        color.g = lua_tointeger(l, 3);
        color.b = lua_tointeger(l, 4);
        button_set_label_color(source, color);
        return 0;
    }

    luaL_error(l, "invalid number of arguments.");
    return 0;
}

int abyss_lua_button_color_mod(lua_State *l) {
    if (lua_gettop(l) == 1) {
        SCRIPT_GET_LUA_THIS(source, button)
        rgb color = button_get_color_mod(source);
        lua_pushinteger(l, color.r);
        lua_pushinteger(l, color.g);
        lua_pushinteger(l, color.g);
        return 3;
    }

    if (lua_gettop(l) == 4) {
        SCRIPT_GET_LUA_THIS(source, button)
        rgb color;
        color.r = lua_tointeger(l, 2);
        color.g = lua_tointeger(l, 3);
        color.b = lua_tointeger(l, 4);
        button_set_color_mod(source, color);
        return 0;
    }

    luaL_error(l, "invalid number of arguments.");
    return 0;
}

int abyss_lua_button_frame_index(lua_State *l) {
    if (lua_gettop(l) == 2) {
        SCRIPT_GET_LUA_THIS(source, button)
        const char *frame_type = lua_tostring(l, 2);
        int frame_index = -1;
        if (compare_str_ncase(frame_type, "normal")) {
            frame_index = button_get_is_checkbox(source) ? button_get_frame_index_checked_normal(source) : button_get_frame_index_normal(source);
        } else if (compare_str_ncase(frame_type, "pressed")) {
            frame_index = button_get_is_checkbox(source) ? button_get_frame_index_checked_pressed(source) : button_get_frame_index_pressed(source);
        } else if (compare_str_ncase(frame_type, "hovered")) {
            frame_index = button_get_is_checkbox(source) ? button_get_frame_index_checked_hover(source) : button_get_frame_index_hover(source);
        } else if (compare_str_ncase(frame_type, "disabled")) {
            frame_index = button_get_frame_index_disabled(source);
        } else {
            luaL_error(l, "invalid state type.");
            return 0;
        }

        lua_pushinteger(l, frame_index);
        return 1;
    }

    if (lua_gettop(l) == 3) {
        SCRIPT_GET_LUA_THIS(source, button)
        const char *frame_type = lua_tostring(l, 2);
        const int frame_index = lua_tointeger(l, 3);

        if (compare_str_ncase(frame_type, "normal")) {
            if (button_get_is_checkbox(source))
                button_set_frame_index_checked_normal(source, frame_index);
            else
                button_set_frame_index_normal(source, frame_index);
        } else if (compare_str_ncase(frame_type, "pressed")) {
            if (button_get_is_checkbox(source))
                button_set_frame_index_checked_pressed(source, frame_index);
            else
                button_set_frame_index_pressed(source, frame_index);
        } else if (compare_str_ncase(frame_type, "hovered")) {
            if (button_get_is_checkbox(source))
                button_set_frame_index_checked_hover(source, frame_index);
            else
                button_set_frame_index_hover(source, frame_index);
        } else if (compare_str_ncase(frame_type, "disabled")) {
            button_set_frame_index_disabled(source, frame_index);
        } else {
            luaL_error(l, "invalid state type.");
            return 0;
        }

        return 0;
    }

    luaL_error(l, "invalid number of arguments.");
    return 0;
}

int abyss_lua_create_button(lua_State *l) {
    LCHECK_NUMPARAMS(3)
    SCRIPT_GET_LUA_CLASS(font, spritefont, 1)
    const char *file_path = lua_tostring(l, 2);
    const char *palette_name = lua_tostring(l, 3);

    button *result = button_load(font, file_path, palette_name);

    SCRIPT_CLASS_RESULT_START(result)
    SCRIPT_CLASS_RESULT_PROPERTY("destroy", abyss_lua_button_destroy)
    SCRIPT_CLASS_RESULT_PROPERTY("state", abyss_lua_button_state);
    SCRIPT_CLASS_RESULT_PROPERTY("caption", abyss_lua_button_caption);
    SCRIPT_CLASS_RESULT_PROPERTY("checked", abyss_lua_button_checked);
    SCRIPT_CLASS_RESULT_PROPERTY("checkbox", abyss_lua_button_checkbox);
    SCRIPT_CLASS_RESULT_PROPERTY("segments", abyss_lua_button_segments);
    SCRIPT_CLASS_RESULT_PROPERTY("textOffset", abyss_lua_button_text_offset);
    SCRIPT_CLASS_RESULT_PROPERTY("size", abyss_lua_button_size);
    SCRIPT_CLASS_RESULT_PROPERTY("labelColor", abyss_lua_button_label_color);
    SCRIPT_CLASS_RESULT_PROPERTY("colorMod", abyss_lua_button_color_mod);
    SCRIPT_CLASS_RESULT_PROPERTY("frameIndex", abyss_lua_button_frame_index);
    ADD_NODE_SCRIPT_CLASS_RESULT_PROPERTIES

    return 1;
}
