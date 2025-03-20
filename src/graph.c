#include "graph.h"

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>






Graph *GraphCreate(void)
{
    Graph *g = calloc(1, sizeof(Graph));
    g->nodes_i = 0;
    g->nodes_size = 10;
    g->nodes = calloc(g->nodes_size, sizeof(GraphNode));
    return g;
}

void GraphDestroy(Graph *g)
{
    for (size_t i = 0; i < g->nodes_i; ++i)
    {
        GraphEdge *e = g->nodes[i].first;
        while (e)
        {
            GraphEdge *next = e->next;
            free(e);
            e = next;
        }
    }
    free(g->nodes);
    free(g);
}


int GraphAddNode(Graph *g, float x, float y)
{
    if (g->nodes_i == g->nodes_size)
    {
        g->nodes_size *= 2;
        g->nodes = realloc(g->nodes, g->nodes_size * sizeof(GraphNode));
    }
    g->nodes[g->nodes_i].x = x;
    g->nodes[g->nodes_i].y = y;
    g->nodes[g->nodes_i].first = NULL;
    g->nodes[g->nodes_i].last = NULL;
    return g->nodes_i++;
}

void GraphAddEdge(Graph *g, int from, int to)
{
    assert(from < g->nodes_i && to < g->nodes_i);

    GraphEdge *e = calloc(1, sizeof(GraphEdge));
    e->node = to;
    e->next = NULL;

    GraphNode *n = &g->nodes[from];
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

bool GraphHasEdge(Graph *g, int from, int to)
{
    assert(from < g->nodes_i && to < g->nodes_i);

    GraphEdge *e = g->nodes[from].first;
    while (e)
    {
        if (e->node == to)
        {
            return true;
        }
        e = e->next;
    }
    return false;
}

void GraphClearVisited(Graph *g)
{
    for (size_t i = 0; i < g->nodes_i; ++i)
    {
        g->nodes[i].visited = false;
        GraphEdge *e = g->nodes[i].first;
        while (e)
        {
            e->visited = false;
            e = e->next;
        }
    }
}





