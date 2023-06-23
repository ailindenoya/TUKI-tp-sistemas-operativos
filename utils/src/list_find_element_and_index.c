#include <stdlib.h>
#include <commons/collections/list.h>

void* list_find_element_and_index(t_list *self, bool(*condition)(void*), int* index);

void* list_find_element_and_index(t_list *self, bool(*condition)(void*), int* index) {
    t_link_element *element = self->head;
    int position = 0;

    while (element != NULL && !condition(element->data)) {
        element = element->next;
        position++;
    }

    if (index != NULL) {
        *index = position;
    }

    return element != NULL ? element->data : NULL;
}

