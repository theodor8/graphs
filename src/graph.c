#include "graph.h"

#include <stdlib.h>
#include <assert.h>






graph_t *graph_create(void)
{
    graph_t *g = calloc(1, sizeof(graph_t));
    g->nodes_i = 0;
    g->nodes_size = 10;
    g->nodes = calloc(g->nodes_size, sizeof(node_t));
    return g;
}

void graph_destroy(graph_t *g)
{
    for (size_t i = 0; i < g->nodes_i; ++i)
    {
        edge_t *e = g->nodes[i].first;
        while (e)
        {
            edge_t *next = e->next;
            free(e);
            e = next;
        }
    }
    free(g->nodes);
    free(g);
}


size_t graph_add_node(graph_t *g, float x, float y)
{
    if (g->nodes_i == g->nodes_size)
    {
        g->nodes_size *= 2;
        g->nodes = realloc(g->nodes, g->nodes_size * sizeof(node_t));
    }
    g->nodes[g->nodes_i].x = x;
    g->nodes[g->nodes_i].y = y;
    g->nodes[g->nodes_i].first = NULL;
    g->nodes[g->nodes_i].last = NULL;
    return g->nodes_i++;
}

void graph_add_edge(graph_t *g, size_t from, size_t to)
{
    assert(from < g->nodes_i && to < g->nodes_i);

    edge_t *e = calloc(1, sizeof(edge_t));
    e->node = to;
    e->next = NULL;

    node_t *n = &g->nodes[from];
    if (n->last)
    {
        assert(n->first);
        n->last = n->last->next = e;
    }
    else
    {
        assert(n->first == NULL);
        n->first = n->last = e;
    }
}




