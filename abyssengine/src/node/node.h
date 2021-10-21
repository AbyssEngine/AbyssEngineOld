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

#ifndef ABYSS_NODE_H
#define ABYSS_NODE_H

#include <stdbool.h>
#include <stdint.h>

struct engine; // Forward declaration to prevent possible circular dependency
typedef struct node {
    struct node *parent;
    struct node **children;
    uint32_t num_children;
    bool active;
    bool visible;
    int x;
    int y;
    void (*render_callback)(struct node *node, struct engine *source);
    void (*update_callback)(struct node *node, struct engine *source, uint32_t ticks);
    void (*remove_callback)(struct node *node, struct engine *engine);
    void (*destroy_callback)(struct node *node, struct engine *engine);
} node;

void node_initialize(node *source);
void node_append_child(node *source, node *child);

#endif // ABYSS_NODE_H
