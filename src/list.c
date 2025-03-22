#include "list.h"

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef struct Node Node;
struct Node
{
    Elem elem;
    Node *next;
};

static Node *NodeCreate(Elem elem, Node *next)
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

void ListPrepend(List *list, Elem elem)
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
void ListAppend(List *list, Elem elem)
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



Elem ListRemoveFirst(List *list)
{
    assert(list->first != NULL);
    Node *node = list->first;
    list->first = node->next;
    if (list->last == node)
    {
        assert(list->size == 1 && list->first == NULL);
        list->last = NULL;
    }
    Elem elem = node->elem;
    NodeDestroy(node);
    list->size--;
    return elem;
}


Elem ListGetFirst(List *list)
{
    assert(list->first != NULL);
    return list->first->elem;
}

Elem ListGetLast(List *list)
{
    assert(list->last != NULL);
    return list->last->elem;
}

void ListApply(List *list, void (*f)(size_t, Elem *))
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

Elem IterNext(Iter *iter)
{
    assert(iter->next != NULL);
    Elem elem = iter->next->elem;
    iter->next = iter->next->next;
    return elem;
}


Queue *QueueCreate(void)
{
    return (Queue *)ListCreate();
}
void QueueDestroy(Queue *queue)
{
    ListDestroy((List *)queue);
}
size_t QueueSize(Queue *queue)
{
    return ListSize((List *)queue);
}
void QueueEnqueue(Queue *queue, Elem elem)
{
    ListAppend((List *)queue, elem);
}
Elem QueueDequeue(Queue *queue)
{
    return ListRemoveFirst((List *)queue);
}


Stack *StackCreate(void)
{
    return (Stack *)ListCreate();
}
void StackDestroy(Stack *stack)
{
    ListDestroy((List *)stack);
}
size_t StackSize(Stack *stack)
{
    return ListSize((List *)stack);
}
void StackPush(Stack *stack, Elem elem)
{
    ListPrepend((List *)stack, elem);
}
Elem StackPop(Stack *stack)
{
    return ListRemoveFirst((List *)stack);
}




