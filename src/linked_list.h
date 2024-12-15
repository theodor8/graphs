#pragma once

#include <stddef.h>
#include <stdbool.h>



typedef struct list list_t;

list_t *list_create(void);
void list_destroy(list_t *list);

size_t list_size(list_t *list);
void list_append(list_t *list, void *elem);

void *list_get_first(list_t *list);




typedef struct iter iter_t;

iter_t *iter_create(list_t *list);
void iter_destroy(iter_t *iter);

bool iter_has_next(iter_t *iter);
void *iter_next(iter_t *iter);

