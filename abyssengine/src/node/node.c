#include "node.h"
#include "../engine/engine.h"

uint64_t _next_id;

void node_initialize(node *source) {
    source->id = ++_next_id;
    source->parent = NULL;
    source->children = NULL;
    source->num_children = 0;
    source->active = true;
    source->visible = true;
    source->x = 0;
    source->y = 0;
    source->render_callback = NULL;
    source->update_callback = NULL;
    source->remove_callback = NULL;
    source->destroy_callback = NULL;
}
