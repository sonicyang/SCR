#ifndef __LIST_H__
#define __LIST_H__

#include <stdlib.h>
#include "pool.h"

struct list_element_t{
    struct list_element_t* prev;
    void* data;
    struct list_element_t* next;
};

struct list_t{
    struct pool_t* list_pool;
    struct pool_t* memory_pool;
    size_t size;
    struct list_element_t* head;
    struct list_element_t* tail;
    pthread_mutex_t* lock;
};

struct list_t* create_list(size_t);
void delete_list(struct list_t*);
void* list_allocate(struct list_t*);
void list_free(struct list_t*, void*);


#endif//__LIST_H__
