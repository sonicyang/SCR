#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "pool.h"
#include "list.h"
#include "misc.h"

struct list_t* create_list(size_t e_size){
    struct list_t* list = (struct list_t*)malloc(sizeof(struct list_t));

    if(list == NULL){
        die("Failed to create list");
    }

    list->list_pool = create_pool(sizeof(struct list_element_t));
    list->memory_pool = create_pool(e_size);

    list->size = 0;
    list->head = NULL;
    list->tail = NULL;

    list->lock = malloc(sizeof(pthread_mutex_t));
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
    free(list);

    pthread_mutex_destroy(ptr);
    free(ptr);

    return;
}

struct list_element_t* list_allocate(struct list_t* list){
    struct list_element_t* element;

    if(list == NULL)
        die("No such list");

    element = pool_allocate(list->list_pool);
    element->data = pool_allocate(list->memory_pool);
    element->next = NULL;

    pthread_mutex_lock(list->lock);
    element->prev = list->tail;
    if(list->tail)
        list->tail->next = element;
    else
        list->head = element;
    list->tail = element;
    list->size++;
    pthread_mutex_unlock(list->lock);

    return element;
}

struct list_element_t* list_pop(struct list_t* list){
    struct list_element_t* element = list->head;

    if(list == NULL)
        die("No such list");

    if(list->size <= 0)
        die("List underflow");

    pthread_mutex_lock(list->lock);
    list->head = element->next;
    if(element->next)
        element->next->prev = NULL;
    else
        list->tail = NULL;
    list->size--;
    pthread_mutex_unlock(list->lock);

    return element;
}

void list_free(struct list_t* list, struct list_element_t* element){
    pthread_mutex_lock(list->lock);
    pool_free(list->memory_pool, element->data);
    pool_free(list->list_pool, element);
    pthread_mutex_unlock(list->lock);

    return;
}

void list_delete(struct list_t* list, struct list_element_t* element){
    pthread_mutex_lock(list->lock);
    if(element->next){
        element->next->prev = element->prev;
    }

    if(element->prev){
        element->prev->next = element->next;
    }

    if(element==list->head){
         list->head = element->next;
    }

    if(element==list->tail){
         list->tail = element->prev;
    }

    list->size--;
    pthread_mutex_unlock(list->lock);

}
