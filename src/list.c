#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "pool.h"
#include "list.h"
#include "misc.h"

struct list_t* create_list(size_t e_size){
    struct list_t* list = (struct list_t*)malloc(sizeof(struct list_t));

    list->list_pool = create_pool(sizeof(struct list_element_t));
    list->memory_pool = create_pool(e_size);

    list->size = 0;
    list->head = 0;
    list->tail = 0;

    pthread_mutex_init(list->lock, NULL);

    return list;
}

void delete_list(struct list_t* list){

    if(list == NULL)
        die("No such list");

    pthread_mutex_t* ptr = list->lock;

    pthread_mutex_lock(list->lock);

    delete_pool(list->memory_pool);
    delete_pool(list->list_pool);

    pthread_mutex_destroy(ptr);
    free(ptr);

    return;
}

void* list_allocate(struct list_t* list){
    struct list_element_t* element;

    if(list == NULL)
        die("No such list");

    element = pool_allocate(list->list_pool);
    element->data = pool_allocate(list->memory_pool);
    element->next = NULL;

    pthread_mutex_lock(list->lock);
    element->prev = list->tail;
    list->tail->next = element;
    list->tail = element;
    list->size++;
    pthread_mutex_unlock(list->lock);

    return element->data;
}

void list_pop(struct list_t* list){
    struct list_element_t* element = list->head;

    if(list == NULL)
        die("No such list");

    pthread_mutex_lock(list->lock);
    list->head = element->next;
    element->next->prev = NULL;
    list->size--;
    pthread_mutex_unlock(list->lock);

    pool_free(list->memory_pool, element->data);
    pool_free(list->list_pool, element);

    return;
}
