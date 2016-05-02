#ifndef __POOL_H__
#define __POOL_H__

#include <stdlib.h>

struct pool_t{
    void* data;
    size_t element_size;
    size_t size;
    int* used_mark;
    size_t used;
};

struct pool_t* create_pool(size_t, size_t);
void delete_pool(struct pool_t*);
void* pool_allocate(struct pool_t*);
void pool_free(struct pool_t*, void*);


#endif//__POOL_H__
