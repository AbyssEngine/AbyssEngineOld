#ifndef ABYSS_NODE_H
#define ABYSS_NODE_H

#include <stdbool.h>
#include <stdint.h>
#include <uuid/uuid.h>

struct engine; // Forward declaration to prevent possible circular dependency
typedef struct node {
    uuid_t id;
    struct node *parent;
    struct node *children;
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
void node_get_position(node *source, int *out_x, int *out_y);

#endif // ABYSS_NODE_H
