#include "list.h"

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>



typedef struct node node_t;
struct node
{
    elem_t elem;
    node_t *next;
};

static node_t *node_create(elem_t elem, node_t *next)
{
    node_t *node = calloc(1, sizeof(node_t));
    node->elem = elem;
    node->next = next;
    return node;
}

static void node_destroy(node_t *node)
{
    free(node);
}




struct list
{
    node_t *first;
    node_t *last;
    size_t size;
};

list_t *list_create(void)
{
    list_t *list = calloc(1, sizeof(list_t));
    return list;
}

void list_destroy(list_t *list)
{
    node_t *node = list->first;
    while (node)
    {
        node_t *next = node->next;
        node_destroy(node);
        node = next; 
    }
    free(list);
}

size_t list_size(list_t *list)
{
    return list->size;
}

void list_append(list_t *list, elem_t elem)
{
    node_t *node = node_create(elem, NULL);
    if (list->last)
    {
        list->last->next = node;
        list->last = node;
    }
    else
    {
        assert(list->first == NULL);
        assert(list->size == 0);
        list->first = list->last = node;
    }
    list->size++;
}


elem_t list_get_first(list_t *list)
{
    assert(list->first != NULL);
    return list->first->elem;
}

elem_t list_get_last(list_t *list)
{
    assert(list->last != NULL);
    return list->last->elem;
}

void list_apply(list_t *list, void (*f)(size_t, elem_t *))
{
    node_t *n = list->first;
    size_t i = 0;
    while (n != NULL)
    {
        f(i, &n->elem);
        n = n->next;
        ++i;
    }
    assert(i == list->size);
}



struct iter
{
    node_t *next;
};

iter_t *iter_create(list_t *list)
{
    iter_t *iter = calloc(1, sizeof(iter_t));
    iter->next = list->first;
    return iter;
}

void iter_destroy(iter_t *iter)
{
    free(iter);
}

bool iter_has_next(iter_t *iter)
{
    return iter->next != NULL;
}

elem_t iter_next(iter_t *iter)
{
    assert(iter->next != NULL);
    elem_t elem = iter->next->elem;
    iter->next = iter->next->next;
    return elem;
}


