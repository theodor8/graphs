#pragma once

#include <stdlib.h>
#include <stdbool.h>
#


typedef struct GraphEdge GraphEdge;
struct GraphEdge
{
    bool visited;
    int node; // to node
    GraphEdge *next; // next in adj list
};

typedef struct
{
    bool visited;
    float x, y;
    GraphEdge *first, *last;
} GraphNode;

typedef struct
{
    GraphNode *nodes;
    size_t nodes_i, nodes_size;
} Graph;


Graph *GraphCreate(void);
void GraphDestroy(Graph *g);

int GraphAddNode(Graph *g, float x, float y);
void GraphAddEdge(Graph *g, int from, int to);

bool GraphHasEdge(Graph *g, int from, int to);

void GraphClearVisited(Graph *g);




