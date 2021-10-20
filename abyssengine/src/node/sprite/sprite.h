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

#ifndef ABYSS_SPRITE_H
#define ABYSS_SPRITE_H

#include "../../engine/engine.h"
#include "../node.h"

typedef struct sprite sprite;

sprite *sprite_load(const char *file_path, const char *palette_name);
void sprite_destroy(sprite *source);
void sprite_regenerate_atlas(sprite *source);
void sprite_render_callback(node *node, engine *source);
void sprite_remove_callback(node *node, engine *source);
void sprite_destroy_callback(node *node, engine *source);
void sprite_update_callback(node *node, engine *source, uint32_t ticks);

#endif // ABYSS_SPRITE_H
