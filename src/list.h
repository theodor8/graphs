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
} ListElem;

#define INT_ELEM(x) (ListElem) { .i=(x) }
#define SIZE_T_ELEM(x) (ListElem) { .s=(x) }
#define PTR_ELEM(x) (ListElem) { .p=(x) }

typedef struct List List;

List *ListCreate(void);
void ListDestroy(List *list);

size_t ListSize(List *list);
void ListPrepend(List *list, ListElem elem);
void ListAppend(List *list, ListElem elem);

ListElem ListRemoveFirst(List *list);

ListElem ListGetFirst(List *list);
ListElem ListGetLast(List *list);

void ListApply(List *list, void (*f)(size_t, ListElem *));

typedef struct Iter Iter;

Iter *IterCreate(List *list);
void IterDestroy(Iter *iter);

bool IterHasNext(Iter *iter);
ListElem IterNext(Iter *iter);
