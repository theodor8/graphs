#pragma once

#include <stddef.h>
#include <stdbool.h>

typedef union
{
    bool b;
    int i;
    size_t s;
    float f;
    void *p; 
} Elem;


#define INT_ELEM(x) (Elem) { .i=(x) }
#define SIZE_T_ELEM(x) (Elem) { .s=(x) }
#define PTR_ELEM(x) (Elem) { .p=(x) }


typedef struct List List;
List *ListCreate(void);
void ListDestroy(List *list);
size_t ListSize(List *list);
void ListPrepend(List *list, Elem elem);
void ListAppend(List *list, Elem elem);
Elem ListRemoveFirst(List *list);
Elem ListGetFirst(List *list);
Elem ListGetLast(List *list);
void ListApply(List *list, void (*f)(size_t, Elem *));


typedef struct Iter Iter;
Iter *IterCreate(List *list);
void IterDestroy(Iter *iter);
bool IterHasNext(Iter *iter);
Elem IterNext(Iter *iter);


typedef List Queue;
Queue *QueueCreate(void);
void QueueDestroy(Queue *queue);
size_t QueueSize(Queue *queue);
void QueueEnqueue(Queue *queue, Elem elem);
Elem QueueDequeue(Queue *queue);


typedef List Stack;
Stack *StackCreate(void);
void StackDestroy(Stack *stack);
size_t StackSize(Stack *stack);
void StackPush(Stack *stack, Elem elem);
Elem StackPop(Stack *stack);




