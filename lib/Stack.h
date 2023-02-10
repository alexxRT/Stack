#ifndef STACK_H
#define STACK_H

#include <stdio.h>

const int RESIZE_PARAM = 2; // Capacity multiplier for resize routine

typedef int stack_data_t; // The type of elems in the stack

//STACK DEFINITION
typedef struct _stack 
{
    size_t capacity;
    size_t size;
    stack_data_t* data;
    int hash;
    size_t min_capacity;
    size_t alive_status;
}my_stack; // Stack itself :))

int  StackInit    (my_stack* stack, size_t size, size_t min_capacity);

int  StackPush    (my_stack* stack, stack_data_t* elem_ptr);

int  StackPop     (my_stack* stack, stack_data_t* elem_ptr);

int  StackDestroy (my_stack* stack);

#endif
