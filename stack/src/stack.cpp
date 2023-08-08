#include "stack.h"
#include "memory.h"
#include "stack_debug.h"
#include <string.h>


#define SIZE( size ) 2*sizeof(CANARY) + sizeof(stack_data_t)*size
#define NEW_SIZE( num_of_elems ) num_of_elems*sizeof (stack_data_t) + 2*sizeof (CANARY)

#define CHECK_DOWN( stack_ptr ) stack_ptr->size < (stack_ptr->capacity) / (2*RESIZE_PARAM) && \
stack_ptr->capacity / RESIZE_PARAM > stack_ptr->min_capacity

#define CHECK_UP( stack_ptr ) stack_ptr->size == stack_ptr->capacity



//---------------------------------------INITIAL STACK FUNCTIONS--------------------------------------//

int StackResize (my_stack* stack); 
int ResizeUp    (my_stack* stack);
int ResizeDown  (my_stack* stack);

int StackInit (my_stack* stack, size_t size, size_t min_capacity)
{
    if (stack == NULL) return STACK_NULL;

    stack->capacity = size;
    stack->size = 0;
    stack->hash = 0;
    stack->alive_status = 1;

    #ifdef DBG_MODE
    {
        char* data = CALLOC (SIZE (size), char);
        stack->data = (stack_data_t*)PlaceDataInCanaries (data, size, sizeof(stack_data_t));
    }
    #else
    {
        stack->data = CALLOC (size, stack_data_t);
    }
    #endif

    stack->min_capacity = min_capacity;
    #ifdef DBG_MODE
        StackRehash (stack);
    #endif

    STACK_ASSERT (stack);

    #ifdef DBG_MODE
        if (!stack->alive_status)
        {   
            PrintErr (INIT_FAILED, stack, __func__, __LINE__);                      
            StackDump (stack, __func__, __LINE__, INIT_FAILED);
            return INIT_FAILED;
        }
    #endif

    return SUCCESS;
}

int StackPop (my_stack* stack, stack_data_t* p_elem)
{
    STACK_ASSERT (stack);

    #ifdef DBG_MODE
        if (stack->size == 0)
        {
            PrintErr (STACK_UNDERFLOW, stack, __func__, __LINE__);                      
            StackDump (stack, __func__, __LINE__, STACK_UNDERFLOW);

            return STACK_UNDERFLOW;
        }
    #endif

    stack->size --;
    *p_elem = stack->data [stack->size];

    #ifdef DBG_MODE
        StackRehash (stack);
    #endif

    StackResize (stack);

    STACK_ASSERT (stack);

    return SUCCESS;
} 

int StackPush (my_stack* stack, stack_data_t* elem)
{
    STACK_ASSERT (stack);

    StackResize (stack);
    
    stack->data [stack->size] = *elem;
    stack->size ++;

    #ifdef DBG_MODE
        StackRehash (stack);
    #endif

    STACK_ASSERT (stack);

    return SUCCESS;
}

int StackDestroy (my_stack* stack)
{
    STACK_ASSERT (stack);

    #ifdef DBG_MODE
        void* ptr_to_free = (char*)stack->data - sizeof (CANARY);
    #else
        void* ptr_to_free = stack->data;
    #endif

    FREE (ptr_to_free);

    stack->alive_status = 0;
    stack->capacity     = 0;
    stack->min_capacity = 0;
    stack->hash         = 0;

    #ifdef DBG_MODE
        if (stack->alive_status)
        {
            PrintErr  (DESTROY_FAILED, stack, __func__, __LINE__);                      
            StackDump (stack, __func__, __LINE__, DESTROY_FAILED); 
            return DESTROY_FAILED;
        }
    #endif

    return SUCCESS;
}

//------------------------------------------------------------------------------------------------------//


//--------------------------------------- STACK RESIZE FUNCTIONS ---------------------------------------//

int ResizeDown (my_stack* stack)
{
    STACK_ASSERT (stack);

    #ifdef DBG_MODE
        char* data_start = (char*)stack->data - sizeof (CANARY);
    #else
        char* data_start = (char*)stack->data;
    #endif

    size_t cur_cap = stack->capacity;
    stack->capacity = !cur_cap + cur_cap / 2;

    data_start = REALLOC (data_start, NEW_SIZE (stack->capacity), char);

    #ifdef DBG_MODE
        if (data_start == NULL)
        {
            PrintErr  (REALLOC_FAILED, stack, __func__, __LINE__);                      
            StackDump (stack, __func__, __LINE__, REALLOC_FAILED);
            return REALLOC_FAILED;
        }
    #endif

    #ifdef DBG_MODE
    {
        stack->data = (stack_data_t*)PlaceDataInCanaries (data_start, stack->capacity, sizeof (stack_data_t));
        StackRehash (stack);
    }
    #else
    {
        stack->data = (stack_data_t*)data_start;
    }
    #endif

    STACK_ASSERT (stack);

    return SUCCESS;
}

int ResizeUp (my_stack* stack)
{
    STACK_ASSERT (stack);

    #ifdef DBG_MODE
        char* data_start = (char*)stack->data - sizeof(CANARY);
    #else 
        char* data_start = (char*)stack->data;
    #endif

    size_t cur_cap = stack->capacity;
    stack->capacity = !cur_cap + 2*cur_cap;

    data_start =  REALLOC (data_start, NEW_SIZE (stack->capacity), char);

    #ifdef DBG_MODE
        if (data_start == NULL)
        {
            PrintErr  (REALLOC_FAILED, stack, __func__, __LINE__);                      
            StackDump (stack, __func__, __LINE__, REALLOC_FAILED);
            return REALLOC_FAILED;
        }
    #endif

    #ifdef DBG_MODE
    {
        stack->data = (stack_data_t*)PlaceDataInCanaries (data_start, stack->capacity, sizeof(stack_data_t));
        StackRehash (stack);
    }
    #else
    {
        stack->data = (stack_data_t*)data_start;
    }
    #endif

    STACK_ASSERT (stack);

    return SUCCESS;
}

int StackResize (my_stack* stack) 
{
    STACK_ASSERT (stack);
        
    if (CHECK_UP (stack))
    {
        ResizeUp (stack);
    }

    if (CHECK_DOWN (stack))
    {
        ResizeDown (stack);
    }

    STACK_ASSERT (stack);

    return SUCCESS;
}

//-----------------------------------------------------------------------------------------------------------//
