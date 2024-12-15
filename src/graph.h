#pragma once

#include "linked_list.h"


typedef struct node node_t;
typedef struct graph graph_t;


char *node_get_name(node_t *n);
list_t *node_get_adj(node_t *n);

graph_t *graph_create(void);
void graph_destroy(graph_t *g);

list_t *graph_get_nodes(graph_t *g);

node_t *graph_add_node(graph_t *g, char *name);
void graph_add_edge(node_t *n1, node_t *n2);

