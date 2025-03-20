#include "list.h"

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef struct Node Node;
struct Node
{
    ListElem elem;
    Node *next;
};

static Node *NodeCreate(ListElem elem, Node *next)
{
    Node *node = calloc(1, sizeof(Node));
    node->elem = elem;
    node->next = next;
    return node;
}

static void NodeDestroy(Node *node)
{
    free(node);
}

struct List
{
    Node *first;
    Node *last;
    size_t size;
};

List *ListCreate(void)
{
    List *list = calloc(1, sizeof(List));
    return list;
}

void ListDestroy(List *list)
{
    Node *node = list->first;
    while (node)
    {
        Node *next = node->next;
        NodeDestroy(node);
        node = next; 
    }
    free(list);
}

size_t ListSize(List *list)
{
    return list->size;
}

void ListPrepend(List *list, ListElem elem)
{
    Node *node = NodeCreate(elem, list->first);
    if (list->first)
    {
        list->first = node;
    }
    else
    {
        assert(list->last == NULL);
        assert(list->size == 0);
        list->first = list->last = node;
    }
    list->size++;
}
void ListAppend(List *list, ListElem elem)
{
    Node *node = NodeCreate(elem, NULL);
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



ListElem ListRemoveFirst(List *list)
{
    assert(list->first != NULL);
    Node *node = list->first;
    list->first = node->next;
    if (list->last == node)
    {
        assert(list->size == 1 && list->first == NULL);
        list->last = NULL;
    }
    ListElem elem = node->elem;
    NodeDestroy(node);
    list->size--;
    return elem;
}


ListElem ListGetFirst(List *list)
{
    assert(list->first != NULL);
    return list->first->elem;
}

ListElem ListGetLast(List *list)
{
    assert(list->last != NULL);
    return list->last->elem;
}

void ListApply(List *list, void (*f)(size_t, ListElem *))
{
    Node *node = list->first;
    size_t i = 0;
    while (node != NULL)
    {
        f(i, &node->elem);
        node = node->next;
        ++i;
    }
    assert(i == list->size);
}

struct Iter
{
    Node *next;
};

Iter *IterCreate(List *list)
{
    Iter *iter = calloc(1, sizeof(Iter));
    iter->next = list->first;
    return iter;
}

void IterDestroy(Iter *iter)
{
    free(iter);
}

bool IterHasNext(Iter *iter)
{
    return iter->next != NULL;
}

ListElem IterNext(Iter *iter)
{
    assert(iter->next != NULL);
    ListElem elem = iter->next->elem;
    iter->next = iter->next->next;
    return elem;
}
