#include <stdio.h>

#include "graph.h"
#include "graph_vis.h"
#include "linked_list.h"


void print_graph(graph_t *g)
{
    list_t *nodes = graph_get_nodes(g);
    printf("nodes_size = %zu\n", list_size(nodes));
    iter_t *nodes_iter = iter_create(nodes);
    while (iter_has_next(nodes_iter))
    {
        graph_node_t *n = iter_next(nodes_iter);
        printf("%s -> ", node_get_name(n));
        iter_t *adj_iter = iter_create(node_get_adj(n));
        while (iter_has_next(adj_iter))
        {
            graph_node_t *adj = iter_next(adj_iter);
            printf("%s, ", node_get_name(adj));
        }
        iter_destroy(adj_iter);
        printf("\n");
    }
    iter_destroy(nodes_iter);
}


int main(void)
{
    printf("hejsan\n");

    graph_t *g = graph_create();

    graph_node_t *a = graph_add_node(g, "a");
    graph_node_t *b = graph_add_node(g, "b");
    graph_node_t *c = graph_add_node(g, "c");
    graph_node_t *d = graph_add_node(g, "d");
    graph_node_t *f = graph_add_node(g, "f");

    graph_add_edge(a, b);
    graph_add_edge(a, c);
    graph_add_edge(d, f);

    graph_vis_t *gv = graph_vis_create(g);
    graph_vis_destroy(gv);




    graph_destroy(g);

    return 0;
}
