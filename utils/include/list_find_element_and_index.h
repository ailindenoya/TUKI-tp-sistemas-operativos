#ifndef LIST_FIND_ELEMENT_AND_INDEX_H_INCLUDED
#define LIST_FIND_ELEMENT_AND_INDEX_H_INCLUDED

#include <stdlib.h>
#include <commons/collections/list.h>

void* list_find_element_and_index(t_list *self, bool(*condition)(void*), int* index);

#endif