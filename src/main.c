#include <stdio.h>


#include "graph.h"
#include "list.h"



void print_graph(graph_t *g)
{
    size_t nodes = graph_nodes(g);
    for (size_t n = 0; n < nodes; ++n)
    {
        printf("%zu -> ", n);
        list_t *adj_list = graph_adj(g, n);
        iter_t *adj_list_iter = iter_create(adj_list);
        while (iter_has_next(adj_list_iter))
        {
            edge_t *edge = iter_next(adj_list_iter).p;
            printf("%zu(w=%f), ", edge_node(edge), edge_weight(edge));
        }
        iter_destroy(adj_list_iter);
        printf("\n");
    }
}

void print_list_elem(size_t i, elem_t *e)
{
    printf("%d, ", e->i);
}

void print_list(list_t *l)
{
    printf("[");
    list_apply(l, print_list_elem);
    printf("]\n");
}


void list_apply_fn(size_t i, elem_t *e)
{
    e->i = i * 2;
}

int main(void)
{
    // graph_t *g = graph_create();
    // for (int i = 0; i < 20; ++i)
    // {
    //     graph_add_node(g);
    // }
    // for (int i = 0; i < 20; ++i)
    // {
    //     graph_add_edge(g, i, (i + 1) % 20, i * 2);
    // }
    // print_graph(g);
    // graph_destroy(g);

    list_t *l = list_create();
    for (int i = 0; i < 10; ++i)
    {
        list_append(l, INT_ELEM(0));
    }

    print_list(l);
    list_apply(l, list_apply_fn);
    print_list(l);

    list_destroy(l);


    return 0;
}
