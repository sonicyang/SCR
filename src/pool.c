#include <stdlib.h>
#include <string.h>

#include "pool.h"

struct pool_t* create_pool(size_t e_size){
    struct pool_t* pool = (struct pool_t*)malloc(sizeof(struct pool_t));
    int i;

    pool->size = 2;
    pool->element_size = e_size;
    pool->used = 0;
    pool->used_mark = malloc(sizeof(int) * pool->size);
    memset((void*)pool->used_mark, 0, sizeof(int) * pool->size);
    pool->data = malloc(sizeof(void*) * pool->size);
    memset((void*)pool->data, 0, sizeof(void*) * pool->size);

    for(i = 0; i < pool->size; i++){
        pool->data[i] = malloc(pool->element_size);
    }

    return pool;
}

void delete_pool(struct pool_t* pool){
    int i;

    for(i = 0; i < pool->size; i++){
        free(pool->data[i]);
    }
    free(pool->data);
    free(pool->used_mark);
    free(pool);

    return;
}

static void resize_pool(struct pool_t* pool, size_t size){
    int i;

    pool->data = realloc(pool->data, pool->element_size * size);
    for(i = pool->size; i < size; i++){
        pool->data[i] = malloc(pool->element_size);
    }

    pool->used_mark = realloc(pool->used_mark, sizeof(int) * size);
    memset((void*)(pool->used_mark + size - pool->size), 0, sizeof(int) * (size - pool->size));
    pool->size = size;

    return;
}

void* pool_allocate(struct pool_t* pool){
    int i = 0;

    if(pool->used == pool->size){
        resize_pool(pool, pool->size*2);
    }

    for(i = 0; i < pool->size; i++){
        if(pool->used_mark[i] == 0){
            pool->used_mark[i] = 1;
            pool->used++;
            return pool->data[i];
        }
    }

    return NULL; //Should not happen
}

void pool_free(struct pool_t* pool, void* ptr){
    int i;

    for(i = 0; i < pool->size; i++){
        if(pool->data[i] == ptr){
            pool->used_mark[i] = 0;
            pool->used--;
            break;
        }
    }

    return;
}
