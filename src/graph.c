#include "graph.h"

#include <stdlib.h>
#include <assert.h>

#include "list.h"




struct edge
{
    float w; // weight
    size_t n; // to node
};

float edge_weight(edge_t *e)
{
    return e->w;
}

size_t edge_node(edge_t *e)
{
    return e->n;
}




static edge_t *edge_create(float w, size_t n)
{
    edge_t *e = calloc(1, sizeof(edge_t));
    e->w = w;
    e->n = n;
    return e;
}

static void edge_destroy(edge_t *e)
{
    free(e);
}


struct graph
{
    list_t **adj; // array of pointers to adjacency lists of edge_t * 
    size_t adj_i, adj_size;
};

graph_t *graph_create(void)
{
    graph_t *g = calloc(1, sizeof(graph_t));
    g->adj_i = 0;
    g->adj_size = 10;
    g->adj = calloc(g->adj_size, sizeof(list_t *));
    return g;
}

void graph_destroy(graph_t *g)
{
    for (size_t i = 0; i < g->adj_i; ++i)
    {
        iter_t *adj_iter = iter_create(g->adj[i]);
        while (iter_has_next(adj_iter))
        {
            edge_destroy(iter_next(adj_iter).p);
        }
        iter_destroy(adj_iter);
        list_destroy(g->adj[i]);
    }
    free(g->adj);
    free(g);
}



size_t graph_add_node(graph_t *g)
{
    if (g->adj_i == g->adj_size)
    {
        g->adj_size *= 2;
        g->adj = realloc(g->adj, g->adj_size * sizeof(list_t *));
    }
    g->adj[g->adj_i] = list_create();
    return g->adj_i++;
}

void graph_add_edge(graph_t *g, size_t n1, size_t n2, float w)
{
    assert(n1 < g->adj_i && n2 < g->adj_i);
    edge_t *e = edge_create(w, n2);
    list_append(g->adj[n1], PTR_ELEM(e));
}



list_t *graph_adj(graph_t *g, size_t n)
{
    assert(n < g->adj_i);
    return g->adj[n];
}

size_t graph_nodes(graph_t *g)
{
    return g->adj_i;
}
