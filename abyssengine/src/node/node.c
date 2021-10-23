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

#include "node.h"
#include "../engine/engine.h"
#include <assert.h>

bool node_default_update_callback(node *source, engine *engine, uint32_t ticks) {
    for (int idx = 0; idx < source->num_children; idx++) {
        node *child = source->children[idx];
        if (!child->active || child->update_callback == NULL)
            continue;

        if (child->update_callback(child, engine, ticks))
            return true;
    }

    return false;
}

void node_default_render_callback(node *source, engine *engine, int offset_x, int offset_y) {
    for (int idx = 0; idx < source->num_children; idx++) {
        node *child = source->children[idx];
        if (!child->active || !child->visible || child->render_callback == NULL)
            continue;

        child->render_callback(child, engine, source->x + offset_x, source->y + offset_y);
    }
}

void node_initialize(node *source) {
    source->parent = NULL;
    source->children = NULL;
    source->num_children = 0;
    source->active = true;
    source->visible = true;
    source->x = 0;
    source->y = 0;
    source->render_callback = node_default_render_callback;
    source->update_callback = node_default_update_callback;
    source->remove_callback = NULL;
    source->destroy_callback = NULL;
}

void node_append_child(node *source, node *child) {
    VERIFY_ENGINE_THREAD

    assert(child->parent == NULL);
    assert(child != source);

    source->children = realloc(source->children, sizeof(node) * (source->num_children + 1));
    source->children[source->num_children++] = child;
    child->parent = source;
}

bool node_default_mouse_event_callback(struct node *source, struct engine *e, enum e_mouse_event_type event_type,
                                       const mouse_event_info *event_info) {

    for (int idx = 0; idx < source->num_children; idx++) {
        node *child = source->children[idx];
        if (!child->active || !child->visible || child->mouse_event_callback == NULL)
            continue;

        if (child->mouse_event_callback(child, e, event_type, event_info)) {
            return true;
        }
    }

    return false;
}
