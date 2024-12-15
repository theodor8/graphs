#include "graph_vis.h"

#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "graph.h"
#include "linked_list.h"




struct graph_vis
{
    list_t *nodes; // list of graph_vis_node_t
};

graph_vis_t *graph_vis_create(graph_t *g)
{
    graph_vis_t *gv = calloc(1, sizeof(graph_vis_t));
    list_t *graph_nodes = graph_get_nodes(g);
    gv->nodes = list_create();

    iter_t *graph_nodes_iter = iter_create(graph_nodes);
    srand(time(NULL));
    while (iter_has_next(graph_nodes_iter))
    {
        graph_node_t *graph_node = iter_next(graph_nodes_iter);
        graph_vis_node_t *gv_node = calloc(1, sizeof(graph_vis_node_t));
        gv_node->graph_node = graph_node;
        gv_node->x = (float)rand() / (float)RAND_MAX;
        gv_node->y = (float)rand() / (float)RAND_MAX;
        list_append(gv->nodes, gv_node);
    }
    iter_destroy(graph_nodes_iter);

    return gv;
}

void graph_vis_destroy(graph_vis_t *gv)
{
    iter_t *nodes_iter = iter_create(gv->nodes);
    while (iter_has_next(nodes_iter))
    {
        graph_vis_node_t *node = iter_next(nodes_iter);
        free(node);
    }
    iter_destroy(nodes_iter);
    list_destroy(gv->nodes);
    free(gv);
}

list_t *graph_vis_get_nodes(graph_vis_t *gv)
{
    return gv->nodes;
}

void graph_vis_update(graph_vis_t *gv, float dt)
{
    // TODO:
}

