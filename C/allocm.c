#include "allocm.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#define CHUNK_SIZE 4096
#define BLOCK_SIZE 256
#define META_CHUNK_SIZE sizeof(chunk)
#define META_BLOCK_SIZE sizeof(block)

chunk *base_ptr = NULL;

void* allocm (size_t size)
{
    block *new_block_ptr =find_free_block(size);
    
    if (!new_block_ptr)
    {
        chunk *new_chunk = mmap(NULL, CHUNK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        void *end_addr = (void*)((intptr_t)new_chunk + CHUNK_SIZE);

        if (!base_ptr)
        {
            base_ptr = new_chunk;
            base_ptr->next = NULL;
        } else
        {
            new_chunk->next = base_ptr;
            base_ptr = new_chunk;
        }

        block *block_ptr = (block*)((intptr_t)base_ptr + META_CHUNK_SIZE);

        block_ptr->prev = NULL;
        block_ptr->next = NULL;
        block_ptr->in_use = false;

        while ((intptr_t)block_ptr + META_BLOCK_SIZE + BLOCK_SIZE < (intptr_t)end_addr)
        {
            block *new_block = (block*)((intptr_t)block_ptr + META_BLOCK_SIZE + BLOCK_SIZE);
            new_block->prev = block_ptr;
            new_block->next = NULL;
            new_block->in_use = false;
            block_ptr->next = new_block;
            block_ptr = block_ptr->next;
        }

        new_block_ptr = find_free_block(size);
    }

    if (!new_block_ptr)
    {
        return (void*)-1;
    }

    if (get_block_size(new_block_ptr) > size + META_BLOCK_SIZE + 4)
        split(new_block_ptr, size);
    new_block_ptr->in_use = true;

    return (void*)((intptr_t)new_block_ptr + META_BLOCK_SIZE);
}

block* find_free_block(size_t size)
{
    chunk *chunk_ptr = base_ptr;

    while (chunk_ptr)
    {
        block *block_ptr = (block*)((intptr_t)chunk_ptr + META_CHUNK_SIZE);
        while (block_ptr)
        {
            if (!block_ptr->in_use && get_block_size(block_ptr) > size)
                return block_ptr;
    
            block_ptr = block_ptr->next;
        }

        chunk_ptr = chunk_ptr->next;
    }

    return NULL;
}

void split(block *block_ptr, size_t size)
{
    if (size >= get_block_size(block_ptr) + META_BLOCK_SIZE)
        return;
    if (block_ptr->in_use)
        return;

    block *new_block = (block*)((intptr_t)block_ptr + META_BLOCK_SIZE + size);
    new_block->next = block_ptr->next;
    block_ptr->next = new_block;
    new_block->prev = block_ptr;
    new_block->in_use = false;

    if (new_block->next)
        new_block->next->prev = new_block;
}

size_t get_block_size(block *block_ptr)
{

    chunk *chunk_ptr = base_ptr;
    while ((intptr_t)block_ptr - (intptr_t)chunk_ptr > 4096)
    {
        chunk_ptr = chunk_ptr->next;
    }

    void *end_ptr = block_ptr->next;
    if (!end_ptr)
    {
        end_ptr = (void*)((intptr_t)chunk_ptr + CHUNK_SIZE);
    }

    return (size_t)((intptr_t)end_ptr - (intptr_t)block_ptr - META_BLOCK_SIZE);
}

void print_memory()
{
    chunk *chunk_ptr = base_ptr;

    while (chunk_ptr)
    {
        block *block_ptr = (block*)((intptr_t)chunk_ptr + META_CHUNK_SIZE);
        while (block_ptr)
        {
            printf("addr: %p | size: %zu | inuse: %d\n", block_ptr, get_block_size(block_ptr), block_ptr->in_use);
            block_ptr = block_ptr->next;
        }
        printf("\n");
        chunk_ptr = chunk_ptr->next;
    }
    printf("_______________________________________________\n");
}

void freem(void *ptr)
{
    block *block_ptr = (block*)((intptr_t)ptr - META_BLOCK_SIZE);
    block_ptr->in_use = false;
    size_t size = get_block_size(block_ptr);
    
    block *aux_block_ptr = block_ptr->prev;
    while (aux_block_ptr && !aux_block_ptr->in_use)
    {
        if (get_block_size(aux_block_ptr) + size + META_BLOCK_SIZE > BLOCK_SIZE)
        {
            break;
        }
    
        aux_block_ptr->next = block_ptr->next;
        if (aux_block_ptr->next)
            aux_block_ptr->next->prev = aux_block_ptr;

        block_ptr = aux_block_ptr; 
        aux_block_ptr = aux_block_ptr->prev;
        size = get_block_size(block_ptr);
    }
    
    aux_block_ptr = block_ptr->next;
    while (aux_block_ptr && !aux_block_ptr->in_use)
    {
        if (get_block_size(aux_block_ptr) + size + META_BLOCK_SIZE > BLOCK_SIZE)
        {
            break;
        }

        block_ptr->next = aux_block_ptr->next;
        if (aux_block_ptr->next)
        {
            aux_block_ptr->next->prev = block_ptr;
        }
        aux_block_ptr = block_ptr->next;
        size = get_block_size(block_ptr);
    }
}

void freeallm()
{
    chunk *next_ptr = base_ptr;
    while (next_ptr)
    {   
        next_ptr = next_ptr->next;
        munmap(base_ptr, CHUNK_SIZE);
        base_ptr = next_ptr;
    }
}