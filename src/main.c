#include <stdio.h>

#include "graph.h"
#include "linked_list.h"


int main(void)
{
    printf("hejsan\n");

    graph_t *g = graph_create();

    node_t *a = graph_add_node(g, "a");
    node_t *b = graph_add_node(g, "b");
    node_t *c = graph_add_node(g, "c");
    graph_add_node(g, "d");
    graph_add_node(g, "f");

    graph_add_edge(a, b);
    graph_add_edge(a, c);



    list_t *nodes = graph_get_nodes(g);

    iter_t *nodes_iter = iter_create(nodes);
    while (iter_has_next(nodes_iter))
    {
        node_t *n = iter_next(nodes_iter);
        printf("%s -> ", node_get_name(n));
        iter_t *adj_iter = iter_create(node_get_adj(n));
        while (iter_has_next(adj_iter))
        {
            node_t *adj = iter_next(adj_iter);
            printf("%s, ", node_get_name(adj));
        }
        iter_destroy(adj_iter);
        printf("\n");
    }
    iter_destroy(nodes_iter);



    graph_destroy(g);

    return 0;
}
