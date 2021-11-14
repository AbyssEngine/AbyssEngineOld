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

    assert(child != source);

    if (child->parent == source) {
        return;
    }

    if (child->parent != NULL) {
        node_remove(child, engine_get_global_instance());
    }

    source->children = realloc(source->children, sizeof(node*) * (source->num_children + 1));
    source->children[source->num_children++] = child;
    child->parent = source;
}

bool node_default_mouse_event_callback(struct node *source, engine *e, enum e_mouse_event_type event_type, const mouse_event_info *event_info) {

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

void node_destroy(node *source, engine *e) {
    if (source->parent != NULL)
        node_remove(source, e);

    for (int i = 0; i < source->num_children; i++) {
        node *child_node = source->children[i];

        assert(child_node != source);

        child_node->parent = NULL;
        node_destroy(child_node, e);
    }
    
    free(source->children);

    if (source->destroy_callback != NULL)
        source->destroy_callback(source, e);
}

void node_remove(node *source, engine *e) {
    if (source->parent == NULL)
        return;

    node *parent = source->parent;

    int child_idx = -1;
    for (int i = 0; i < parent->num_children; i++) {
        if (parent->children[i] != source)
            continue;

        child_idx = i;
        break;
    }

    if (child_idx == -1) {
        engine_trigger_crash(e, "Attempted to remove a node but it's parent does not claim it!");
        return;
    }

    if (child_idx < parent->num_children-1) {
        memcpy(&parent->children[child_idx], &parent->children[child_idx + 1], parent->num_children - child_idx - 1);
    }
    parent->num_children--;
    parent->children = realloc(parent->children, parent->num_children * sizeof(node*));
    source->parent = NULL;
}

void node_get_effective_layout(const node *source, int *x1, int *y1) {
    assert(x1 != NULL);
    assert(y1 != NULL);

    *x1 = 0;
    *y1 = 0;

    for (const node *item = source; item != NULL; item = item->parent) {
        *x1 += item->x;
        *y1 += item->y;
    }
}

void node_remove_all_children(node *source, engine *e) {
    while (source->num_children > 0) {
        node *child = source->children[source->num_children - 1];
        node_remove(source->children[source->num_children - 1], engine_get_global_instance());
        node_destroy(child, e);
    }
}
