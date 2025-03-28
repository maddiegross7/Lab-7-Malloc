#include "mymalloc.h"
#include <stdlib.h> 
#include <unistd.h>


typedef struct chunk {
    size_t size;
    struct chunk *next;
    struct chunk *prev;
}Chunk;

Chunk *free_list_head = NULL;

void *traverse_free_list(size_t size){
    printf("Traversing free list\n");
    Chunk *current = free_list_head;
    while(current != NULL){
        if(current->size >= size){
            printf("current: %ul\n", current);
            return current;
        }else{
            current = current->next;
        }
    }
    return NULL;
}

void insert_chunk(Chunk *newChunk){
    printf("Inserting chunk\n");
    if(free_list_head == NULL){
        free_list_head = newChunk;
    }else if(free_list_head > newChunk){
        newChunk->next = free_list_head;
        free_list_head->prev = newChunk;
        free_list_head = newChunk;
    }else{
        Chunk *current = free_list_head;
        while(current->next != NULL && !(current < newChunk)){
            current = current->next;
        }
        newChunk->next = current;
        newChunk->prev = current->prev;
        current->prev = newChunk;
        if(newChunk->prev != NULL){
            newChunk->prev->next = newChunk;
        }
    }
}

void *my_malloc(size_t bytesToAllocate){
    printf("Allocating memory\n");
    int size = (bytesToAllocate + 7 + 8) & -8;

    Chunk *chunk = traverse_free_list(size);
    if(chunk == NULL){
        chunk = (Chunk *)sbrk(size > 8192 ? size : 8192);
        chunk->size = size > 8192 ? size : 8192;
        chunk->next = NULL;
        chunk->prev = NULL;
        insert_chunk(chunk);
    }
    printf("hello?\n");
    printf("chunk size: %u\n", chunk->size);
    if(chunk->size - size <= 16){
        return chunk;
    }
    chunk->size -= size;
    printf("malloced chunk address?: %u\n", (chunk + size + 8));
    return chunk + size + 8;
}

void delete_chunk(Chunk *chunkToDelete){
    printf("Deleting chunk\n");
    if(chunkToDelete->prev != NULL){
        chunkToDelete->prev->next = chunkToDelete->next;
    }
    if(chunkToDelete->next != NULL){
        chunkToDelete->next->prev = chunkToDelete->prev;
    }
}

void my_free(void *ptr){
    printf("Freeing memory\n");
    ptr -= 8;
    Chunk *chunk = (Chunk *)ptr;
    insert_chunk(chunk);
}

void coelasce_free_list(){
    printf("Coalescing free list\n");
    Chunk *current = free_list_head;
    while(current != NULL){
        if(current + current->size == current->next){
            current->size += current->next->size;
            delete_chunk(current->next);
        }
        current = current->next;
    }
}

void *free_list_begin(){
    printf("Free list begin\n");
    return free_list_head;
}

void *free_list_next(void *node){
    printf("Free list next\n");
    Chunk *chunk = (Chunk *)node;
    return chunk->next;
}