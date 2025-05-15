#include "cvector.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void initialize (vector *vec, size_t size)
{
    vec->begin = malloc(size);
    vec->end = vec->begin;
    intptr_t begin = (intptr_t)vec->begin;
    vec->storage_end = (int*)(begin + size);
    vec->type_size = size;
}

int is_valid (vector *vec)
{
    if (!vec || !vec->begin || !vec->end || !vec->storage_end || vec->type_size <= 0)
        return 0;

    return 1;
}

void* get_element (vector *vec, int index)
{
    if (index < 0 || index > (int)size(vec))
        return NULL;

    return (void*)((intptr_t)vec->begin + index * vec->type_size);
}

void push_back (vector *vec, void *element)
{
    if (!is_valid(vec))
        return;

    if (vec->end == vec->storage_end)
    {
        resize(vec, size(vec) * 2);
    }

    memcpy(vec->end, element, vec->type_size);

    vec->end = (void*)((intptr_t)vec->end + vec->type_size);
}

void pop_back (vector *vec)
{
    if (!is_valid(vec))
        return;

    if (is_empty(vec))
        return;

    vec->end = (int*)((intptr_t)vec->end - vec->type_size);
}

void resize (vector *vec, size_t capacity)
{
    if (!is_valid(vec))
        return;

    int vec_size = (int)size(vec);
    vec_size /= vec->type_size;
    vec->begin = realloc(vec->begin, capacity * vec->type_size);
    vec->end = (void*)((intptr_t)vec->begin + vec_size * vec->type_size);
    vec->storage_end = (void*)((intptr_t)vec->begin + capacity * vec->type_size);
}

int is_empty (vector *vec) 
{
    if (!is_valid(vec))
        return 1;

    return (vec->begin == vec->end);
}

size_t size (vector *vec)
{
    if (!is_valid(vec))
        return 0;

    intptr_t begin_addr = (intptr_t)vec->begin;
    intptr_t end_addr = (intptr_t)vec->end;
    size_t size = (size_t)(end_addr - begin_addr);
    return size;
}

void free_vec (vector *vec)
{
    free(vec->begin);
    vec->begin = NULL;
    vec->end = NULL;
    vec->storage_end = NULL;
}