#pragma once

#include <stddef.h>

typedef struct list list_t;

list_t *list_create(void);
void list_destroy(list_t *list);

size_t list_size(list_t *list);
void list_append(list_t *list, void *elem);



