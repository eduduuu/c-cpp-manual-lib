#ifndef CVECTOR_H
#define CVECTOR_H

#include <stdlib.h>

typedef struct Vector {
    void *begin;
    void *end;
    void *storage_end;
    size_t type_size;
} vector;

void initialize(vector *vec, size_t size);
int is_valid(vector *vec);
void* get_element(vector *vec, int index);
void push_back(vector *vec, void *element);
void pop_back(vector *vec);
void resize(vector *vec, size_t capacity);
int is_empty(vector *vec);
size_t size(vector *vec);
void free_vec(vector *vec);


#endif