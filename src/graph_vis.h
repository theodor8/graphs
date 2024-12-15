#pragma once

#include "graph.h"
#include "linked_list.h"

typedef struct
{
    float x, y, vx, vy; // x,y: 0 --> 1
    graph_node_t *graph_node;
} graph_vis_node_t;

typedef struct graph_vis graph_vis_t;


graph_vis_t *graph_vis_create(graph_t *g);
void graph_vis_destroy(graph_vis_t *gv);

list_t *graph_vis_get_nodes(graph_vis_t *gv);



void graph_vis_update(graph_vis_t *gv, float dt);

