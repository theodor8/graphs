#pragma once

#include "list.h"

typedef struct edge edge_t;

float edge_weight(edge_t *e);
size_t edge_node(edge_t *e);




typedef struct graph graph_t;

graph_t *graph_create(void);
void graph_destroy(graph_t *g);

size_t graph_add_node(graph_t *g);
void graph_add_edge(graph_t *g, size_t n1, size_t n2, float w);

list_t *graph_adj(graph_t *g, size_t n);
size_t graph_nodes(graph_t *g);



