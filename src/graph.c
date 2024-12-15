#include "graph.h"

#include <stdlib.h>
#include <string.h>

#include "linked_list.h"




struct graph_node
{
    char *name;
    list_t *adj;
};

static graph_node_t *node_create(char *name)
{
    graph_node_t *n = calloc(1, sizeof(graph_node_t));
    n->name = strdup(name);
    n->adj = list_create();
    return n;
}

static void node_destroy(graph_node_t *n)
{
    free(n->name);
    list_destroy(n->adj);
    free(n);
}

char *node_get_name(graph_node_t *n)
{
    return n->name;
}

list_t *node_get_adj(graph_node_t *n)
{
    return n->adj;
}




struct graph
{
    list_t *nodes;
};

graph_t *graph_create(void)
{
    graph_t *g = calloc(1, sizeof(graph_t));
    g->nodes = list_create();
    return g;
}

void graph_destroy(graph_t *g)
{
    iter_t *node_iter = iter_create(g->nodes);
    while (iter_has_next(node_iter))
    {
        graph_node_t *node = (graph_node_t *)iter_next(node_iter);
        node_destroy(node);
    }
    iter_destroy(node_iter);

    list_destroy(g->nodes);
    free(g);
}

list_t *graph_get_nodes(graph_t *g)
{
    return g->nodes;
}

graph_node_t *graph_add_node(graph_t *g, char *name)
{
    graph_node_t *n = node_create(name);
    list_append(g->nodes, n);
    return n;
}

void graph_add_edge(graph_node_t *n1, graph_node_t *n2)
{
    list_append(n1->adj, n2);
    list_append(n2->adj, n1);
}

