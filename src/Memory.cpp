#include <stdlib.h>
#include <strings.h>
#include <malloc/malloc.h>
#include "../lib/Memory.h"


void* Recalloc (void* ptr, size_t new_size)
{
    if (ptr == NULL) return NULL;
    
    size_t old_size = malloc_size (ptr);
    ptr = realloc (ptr, new_size);

    if (ptr == NULL) return NULL;

    if (new_size > old_size) 
    bzero ((char*)ptr + old_size, new_size - old_size);

    return ptr;
}

//TO DO
//plans for calloc and realloc safe functions modificadions, like placing canaries and hash