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
        printf("current size: %u\n", current->size);
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
    printf("Inserting chunk again\n");
    if(free_list_head == NULL){
        free_list_head = newChunk;
        return;
    }else if(free_list_head > newChunk){
        newChunk->next = free_list_head;
        free_list_head->prev = newChunk;
        free_list_head = newChunk;
    }else{
        printf("do we get here?\n");
        Chunk *current = free_list_head;
        while(current->next != NULL && current->next < newChunk){
            current = current->next;
        }
        newChunk->next = current->next;
        newChunk->prev = current;
        if (current->next != NULL) {
            current->next->prev = newChunk;
        }
        current->next = newChunk;
    }
}


void delete_chunk(Chunk *chunkToDelete){
    printf("Deleting chunk\n");
    if (free_list_head == chunkToDelete) {
        free_list_head = chunkToDelete->next;
    }
    if(chunkToDelete->prev != NULL){
        chunkToDelete->prev->next = chunkToDelete->next;
    }
    if(chunkToDelete->next != NULL){
        chunkToDelete->next->prev = chunkToDelete->prev;
    }
}

void *my_malloc(size_t bytesToAllocate){
    printf("Allocating memory\n");
    int size = (bytesToAllocate + 7 + 8) & -8;

    Chunk *chunk = traverse_free_list(size);
    if(chunk == NULL){
        printf("No chunk found, allocating new chunk\n");
        chunk = (Chunk *)sbrk(size > 8192 ? size : 8192);
        chunk->size = size > 8192 ? size : 8192;
        chunk->next = NULL;
        chunk->prev = NULL;
        insert_chunk(chunk);
    }
    printf("hello?\n");
    printf("new chunk size 1: %u\n", size);
    printf("chunk size: %u\n", chunk->size);

    if(chunk->size - size <= 16){ 
        printf("we are in here\n");
        delete_chunk(chunk);
        printf("chunk size 1: %u\n", chunk->size);
        return ((char *)chunk + 8);
    }

    chunk->size -= size;  // Reduce the size of the current chunk
    printf("new chunk size 1: %u\n", size);
    printf("chunk size 2: %u\n", chunk->size);

    Chunk *newChunk = (Chunk *)((char *)chunk + chunk->size);
    newChunk->size = size;  // Assign size to the new chunk
    newChunk->next = NULL;
    newChunk->prev = NULL;
    
    // Correct the chunk pointer arithmetic here
    printf("malloced chunk address?: %u\n", ((char *)chunk + chunk->size + 8));
    return (char*)newChunk + 8;
}



void my_free(void *ptr){
    printf("Freeing memory\n");
    char *charPtr = (char *)ptr - 8;
    Chunk *chunk = (Chunk *)charPtr;
    chunk->next = NULL;
    chunk->prev = NULL;
    insert_chunk(chunk);
}

void coalesce_free_list(){
    printf("Coalescing free list\n");
    Chunk *current = free_list_head;
    while(current != NULL){
        if ((char *)current + current->size == (char *)current->next){
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