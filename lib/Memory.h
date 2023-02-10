#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc/malloc.h>



static const void* POISNED_PTR = (const void*)0x78;
static const char POISNED_ELEM = 0x77;

void* Recalloc (void* ptr, size_t new_size);

#define CALLOC( count, type )            (type*)calloc(count, sizeof(type))
#define RECALLOC( ptr, new_size, type )  (type*)Recalloc(ptr, new_size)
#define FREE( ptr ) { memset (ptr, POISNED_ELEM, malloc_size (ptr)); free (ptr); }

#endif