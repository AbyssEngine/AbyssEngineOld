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

#include "moderun.h"

void engine_render_run(engine *src) {
    SDL_Renderer *renderer = engine_get_renderer(src);
    SDL_RenderClear(renderer);
    node *root_node = engine_get_root_node(src);

    if (root_node == NULL || root_node->render_callback == NULL)
        return;

    root_node->render_callback(root_node, src, 0, 0);
}

void engine_update_run(engine *src, const uint32_t tick_diff) {
    node *root_node = engine_get_root_node(src);

    if (root_node == NULL || root_node->update_callback == NULL)
        return;

    root_node->update_callback(root_node, src, tick_diff);
}

void moderun_set_callbacks(engine *src) { engine_set_callbacks(src, engine_render_run, engine_update_run); }
