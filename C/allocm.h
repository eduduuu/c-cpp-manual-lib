#ifndef ALLOCM_H
#define ALLOCM_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct Chunk
{
    struct Chunk *next;
} chunk;

typedef struct Block
{
    struct Block *next;
    struct Block *prev;
    bool in_use;
} block;

void* allocm (size_t size);
block* find_free_block(size_t size);
void split(block *block_ptr, size_t size);
size_t get_block_size(block *block_ptr);
void print_memory();
void freem(void *block_ptr);
void freeallm();


#endif