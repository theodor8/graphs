#pragma once

#include "linked_list.h"


typedef struct graph_node graph_node_t;
typedef struct graph graph_t;


char *node_get_name(graph_node_t *n);
list_t *node_get_adj(graph_node_t *n);

graph_t *graph_create(void);
void graph_destroy(graph_t *g);

list_t *graph_get_nodes(graph_t *g);

graph_node_t *graph_add_node(graph_t *g, char *name);
void graph_add_edge(graph_node_t *n1, graph_node_t *n2);

