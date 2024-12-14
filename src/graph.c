#include "graph.h"

#include "linked_list.h"

typedef struct
{
    list_t *adj;
} node_t;

struct graph
{
    list_t *nodes;
};


