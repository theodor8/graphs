#include "linked_list.h"

#include <stdlib.h>
#include <assert.h>



typedef struct node node_t;
struct node
{
    void *elem;
    node_t *next;
};

node_t *node_create(void *elem, node_t *next)
{
    node_t *node = calloc(1, sizeof(node_t));
    node->elem = elem;
    node->next = next;
    return node;
}

void node_destroy(node_t *node)
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

void list_append(list_t *list, void *elem)
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


