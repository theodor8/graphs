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
} elem_t;

#define INT_ELEM(x) (elem_t) { .i=(x) }
#define SIZE_T_ELEM(x) (elem_t) { .s=(x) }
#define PTR_ELEM(x) (elem_t) { .p=(x) }




typedef struct list list_t;

list_t *list_create(void);
void list_destroy(list_t *list);

size_t list_size(list_t *list);
void list_append(list_t *list, elem_t elem);

elem_t list_get_first(list_t *list);
elem_t list_get_last(list_t *list);

void list_apply(list_t *list, void (*f)(size_t, elem_t *));




typedef struct iter iter_t;

iter_t *iter_create(list_t *list);
void iter_destroy(iter_t *iter);

bool iter_has_next(iter_t *iter);
elem_t iter_next(iter_t *iter);

