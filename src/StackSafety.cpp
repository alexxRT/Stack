#include <stdio.h>
#include <assert.h>
#include <cstddef>
#include "../lib/StackSafety.h"
#include "../lib/Stack.h"

static FILE* dump_file = nullptr; 
static FILE* log_file  = nullptr;

//-----------------------------------SYSTEM FILES FOR DEBUG ONLY--------------------------------//

void InitDumpFile ()
{
    dump_file = fopen ("../sys/DUMP.txt", "a");
    assert (dump_file != NULL);
}

void DestroyDumpFile ()
{
    fflush (dump_file);
    fclose (dump_file);
    dump_file = NULL;
}

void InitLogFile ()
{
    log_file = fopen ("../sys/LOG_FILE.txt", "a");
    assert (log_file != NULL);
}

void DestroyLogFile ()
{
    fflush (log_file);
    fclose (log_file);
    log_file = NULL;
}

//-----------------------------------------------------------------------------------------------//


//---------------------------------------STACK DUMP----------------------------------------------//

void PrintHexBytes  (FILE* file, void* elem, size_t size);

void StackDump (my_stack* stack, const char* func, int line, int ErrCode)
{
    fprintf (dump_file, "--------------STACK DUMP occurred----------------\n");
    fprintf (dump_file, "It was called in func %s, and on the line %d\n", func, line);

    if (ErrCode == RIGHT_CANARY_DAMAGED || ErrCode == LEFT_CANARY_DAMAGED || ErrCode == HASH_DAMAGED)
    {
        fprintf (dump_file, "Your data might be damaged, check the log file\n");
    }

    fprintf (dump_file, "The stack address: [%p]\n", stack);

    if (ErrCode == STACK_NULL)
    {
        fprintf (dump_file, "Dump cannot be processed further becouse stack has NULL pointer\n");
        fprintf (dump_file, "------------- DUMP finished ---------------\n\n");

        fflush(dump_file);
        return;
    }

    fprintf (dump_file, "The data begining address: [%p]\n", stack->data);

    if (ErrCode == DATA_NULL)
    {
        fprintf (dump_file, "Data cannot be revealed becouse data has NULL pointer\n");
        fprintf (dump_file, "------------- DUMP finished ---------------\n\n");

        fflush (dump_file);
        return;
    }

    if (ErrCode == STACK_OVERFLOW)
    {
        fprintf (dump_file, "!!!OVERFLOW!!!\n");
        StackPrint (dump_file, stack, stack->capacity);
        fprintf (dump_file, "------------- DUMP finished ---------------\n\n");

        fflush (dump_file);
        return;
    }

    fprintf (dump_file, "Stack capacity now is [%lu]\n", stack->capacity);
    fprintf (dump_file, "Number of elems in stack now is [%lu]\n", stack->size);

    StackPrint (dump_file, stack, stack->size);
    fprintf (dump_file, "------------- DUMP finished ---------------\n\n");

    fflush (dump_file);

    return;
}

int StackPrint (FILE* file, my_stack* stack, size_t nel)
{
    for (int i = 0; i < nel; i ++)
    {
        PrintHexBytes (file, stack->data + i, sizeof(stack_data_t));
    }
    return SUCCESS;
}

void PrintHexBytes (FILE* file, void* elem, size_t size)
{ 
    assert (file != NULL);
    assert (elem != NULL);

    for (int i = 0; i < size; i++)
    {
        fprintf (file, "%x ", *((unsigned char*)elem + i));
    }

    fprintf (file, "\n");

    return;
}

//----------------------------------------------------------------------------------------------------//


//----------------------------------------ERRORS CHECKERS---------------------------------------------//

int StackBaseInvariants (my_stack* stack);
int StackCheckCanary    (my_stack* stack);
int StackCompHash       (my_stack* stack);


int StackCheckInvariants (my_stack* stack)
{
    int ErrCode = SUCCESS;

    ErrCode = StackBaseInvariants (stack);
    if (ErrCode) return ErrCode;

    ErrCode = StackCheckCanary (stack);
    if (ErrCode) return ErrCode;

    ErrCode = StackCompHash (stack);
    if (ErrCode) return ErrCode;

    return SUCCESS;
}

void PrintErr (int ErrCode, my_stack* stack, const char* func_name, int line)
{
    switch (ErrCode)
    {
    case STACK_NULL:
        fprintf (log_file, "Stack has NULL pointer in func %s ", func_name);
        break;  
    case STACK_DEAD:
        fprintf (log_file, "Your stack was already destroyed in func %s ", func_name);
        break;                    
    case DATA_NULL:
        fprintf (log_file, "Data has NULL pointer in func %s ", func_name);
        break;                      
    case STACK_OVERFLOW:
        fprintf (log_file, "Stack overflowed in func %s ", func_name);
        break;                      
    case STACK_UNDERFLOW:
        fprintf (log_file, "Stack underflowed in func %s ", func_name);
        break;     
    case REALLOC_FAILED:
        fprintf (log_file, "Stack capacity didn't changed in func %s ", func_name);
        break;
    case DESTROY_FAILED:
        fprintf (log_file, "Your aka alive stack is dead in func %s ", func_name);
        break;
    case INIT_FAILED:
        fprintf (log_file, "Your aka dead stack is alive in func %s ", func_name);
    case HASH_DAMAGED:
        fprintf (log_file, "Hash damage in func %s ", func_name);
        break;                     
    case LEFT_CANARY_DAMAGED:
        fprintf (log_file, "Left konoreyka damaged in func %s ", func_name);
        break;
    case RIGHT_CANARY_DAMAGED:
        fprintf (log_file, "Right konoreyka damaged in func %s ", func_name);
        break;
    case SUCCESS:
        return;
        break;

    default:
        assert(0 && "Unexpected error code returned"); 
        break;
    }

    fprintf (log_file, "and on the line %d\n", line);
}

int StackBaseInvariants (my_stack* stack)
{
    if (stack       ==  NULL)            return STACK_NULL    ;
    if (!stack->alive_status)            return STACK_DEAD    ;
    if (stack->data ==  NULL)            return DATA_NULL     ;
    if (stack->size  >  stack->capacity) return STACK_OVERFLOW;

    return SUCCESS;
}

//-------------------------------------------------------------------------------------------------//


//-----------------------------------------CANARIES PROTECT----------------------------------------//

void* PlaceDataInCanaries (void* data, size_t num_of_elems, size_t width)
{
    assert (data != NULL);

    *(long long *)data = CANARY;
    *(long long *)((char*)data + sizeof(CANARY) + num_of_elems * width) = CANARY;

    return (char*)data + sizeof(CANARY);
}


int StackCheckCanary (my_stack* stack)
{
    if (*((long long int*)stack->data - 1) != CANARY) return LEFT_CANARY_DAMAGED;
    if (*(long long int*)(stack->data + stack->capacity) != CANARY) return RIGHT_CANARY_DAMAGED;

    return SUCCESS;
}

//--------------------------------------------------------------------------------------------------//



//-------------------------------------------HASH PROTECT-------------------------------------------//

long long int Hash (void* data, size_t num_of_bytes)
{
    int hash = 0;

    for (int i = 0; i < num_of_bytes; i ++)
    {
        hash += *((char*)data + i);
    }

    return hash;
}

int StackRehash (my_stack* stack)
{
    STACK_NO_HASH_ASSERT (stack); 

    int data_hash = Hash (stack->data, stack->capacity * sizeof (stack_data_t));
    stack->hash = 0;

    int stack_hash = Hash (stack, sizeof (my_stack));
    stack->hash = data_hash + stack_hash;
    
    STACK_ASSERT (stack);

    return SUCCESS;
}

int StackCompHash (my_stack* stack)
{
    int stack_hash = stack -> hash;

    int func_hash = Hash (stack -> data, sizeof (stack_data_t) * stack->capacity); 
    stack -> hash = 0;
    func_hash += Hash (stack, sizeof (my_stack));

    stack -> hash = stack_hash;

    return (stack_hash == func_hash) ? SUCCESS : HASH_DAMAGED;
}

//-----------------------------------------------------------------------------------------------------//