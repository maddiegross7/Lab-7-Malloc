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
    //start at the head of the list
    Chunk *current = free_list_head;
    //and keep moving through until we find a piece big enough
    while(current != NULL){
        if(current->size >= size){
            return current;
        }else{
            current = current->next;
        }
    }
    //return NUll if there is no piece big enough
    return NULL;
}

//finding the correct place for this piece of memory to live in my free list
void insert_chunk(Chunk *newChunk){
    //do i have anything in my list yet?
    if(free_list_head == NULL){
        free_list_head = newChunk;
        return;
    //is the address of the head greater than the address of the new piece?
    }else if(free_list_head > newChunk){
        newChunk->next = free_list_head;
        free_list_head->prev = newChunk;
        free_list_head = newChunk;
    //go through the list and find the place that it fits in the sequence of memory
    //and insert it in that spot, linking everything correctly
    }else{
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

//take this piece out of the free list, reassign the links
void delete_chunk(Chunk *chunkToDelete){
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

//main method
void *my_malloc(size_t bytesToAllocate){
    //figure out how much space needs to be allocated
    int size = (bytesToAllocate + 7 + 8) & -8;

    //see if a piece already exists that is big enough for it
    Chunk *chunk = traverse_free_list(size);
    //no? create a new piece
    if(chunk == NULL){
        chunk = (Chunk *)sbrk(size > 8192 ? size : 8192);
        chunk->size = size > 8192 ? size : 8192;
        chunk->next = NULL;
        chunk->prev = NULL;
        insert_chunk(chunk);
    }

    //if the piece that is left over after the math is too tiny, just give the whole thing to the user
    //and make sure you delete the chunk!!!
    if(chunk->size - size <= 16){ 
        delete_chunk(chunk);
        return ((char *)chunk + 8);
    }
    //if we did not do that then we will adjust the size of the old chunk so that it does not include the new chunk
    chunk->size -= size;

    //create the new chunk
    Chunk *newChunk = (Chunk *)((char *)chunk + chunk->size);
    newChunk->size = size; 
    newChunk->next = NULL;
    newChunk->prev = NULL;
    
    //return a pointer to new piece of memory but do not pass the address of the piece, because
    //that is the size and we do not really want that
    return (char*)newChunk + 8;
}

//will free memory that was used and then insert it back into the free list to be used again
void my_free(void *ptr){
    char *charPtr = (char *)ptr - 8;
    Chunk *chunk = (Chunk *)charPtr;
    chunk->next = NULL;
    chunk->prev = NULL;
    insert_chunk(chunk);
}

//this will add all the pieces of the free list together if they are neighbors in memory, 
void coalesce_free_list(){
    Chunk *current = free_list_head;
    while(current != NULL){
        if ((char *)current + current->size == (char *)current->next){
            current->size += current->next->size;
            delete_chunk(current->next);
        }else{
            current = current->next;
        }
    }
}

void *free_list_begin(){
    return free_list_head;
}

void *free_list_next(void *node){
    Chunk *chunk = (Chunk *)node;
    return chunk->next;
}