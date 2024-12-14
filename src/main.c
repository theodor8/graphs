#include <stdio.h>

#include "linked_list.h"


int main(void)
{
    printf("hejsan\n");

    list_t *list = list_create();

    list_append(list, "hej");

    list_destroy(list);

    return 0;
}
