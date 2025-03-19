#pragma once

#include <stdlib.h>


typedef struct edge edge_t;
struct edge
{
    size_t node; // to node
    edge_t *next; // next in adj list
};

typedef struct
{
    float x, y;
    edge_t *first, *last;
} node_t;

typedef struct
{
    node_t *nodes;
    size_t nodes_i, nodes_size;
} graph_t;


graph_t *graph_create(void);
void graph_destroy(graph_t *g);

size_t graph_add_node(graph_t *g, float x, float y);
void graph_add_edge(graph_t *g, size_t from, size_t to);




