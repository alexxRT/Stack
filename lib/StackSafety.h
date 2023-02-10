#ifndef STACK_DEBUG_H
#define STACK_DEBUG_H

#include "Stack.h"

//#define DBG_MODE //This define turns on all custom asserts, use for DEBUG

const long long int CANARY = 2398573459345735352L;

enum ERROR_LIST  
{
    SUCCESS = 0,
    STACK_NULL,
    STACK_DEAD,
    DATA_NULL,
    REALLOC_FAILED,
    STACK_OVERFLOW,
    INIT_FAILED,
    DESTROY_FAILED, 
    STACK_UNDERFLOW,
    HASH_DAMAGED,
    RIGHT_CANARY_DAMAGED, 
    LEFT_CANARY_DAMAGED
};

void  InitDumpFile    ();
void  DestroyDumpFile ();
void  InitLogFile     ();
void  DestroyLogFile  ();

void  StackDump  (my_stack* stack, const char* func, int line, int ErrCode);
int   StackPrint (FILE* file, my_stack* stack, size_t nel);

void* PlaceDataInCanaries (void* data, size_t num_of_elems, size_t width);
int   StackRehash         (my_stack* stack);

int   StackCheckInvariants (my_stack* stack);
void  PrintErr (int ErrCode, my_stack* stack, const char* func_name, int line);

#ifdef DBG_MODE
#define STACK_ASSERT( stack_ptr )                                           \
do {                                                                        \
int ErrCode = 0;                                                            \
if ((ErrCode = StackCheckInvariants (stack_ptr)) != 0)                      \
{                                                                           \
    PrintErr (ErrCode, stack_ptr, __func__, __LINE__);                      \
    StackDump (stack_ptr, __func__, __LINE__, ErrCode);                     \
    return ErrCode;                                                         \
}                                                                           \
else                                                                        \
    StackDump (stack_ptr, __func__, __LINE__, ErrCode);                     \
}                                                                           \
while (0)

#else
#define STACK_ASSERT( stack_ptr ) (void*)0

#endif


#ifdef DBG_MODE
#define STACK_NO_HASH_ASSERT( stack_ptr )\
do {\
int ErrCode = 0;\
if (((ErrCode = StackCheckInvariants (stack_ptr)) != 0) && ErrCode != HASH_DAMAGED) \
{                                                                                   \
    PrintErr (ErrCode, stack_ptr, __func__, __LINE__);                              \
    StackDump (stack_ptr, __func__, __LINE__, ErrCode);                             \
    return ErrCode;                                                                 \
}                                                                                   \
else                                                                                \
    StackDump (stack_ptr, __func__, __LINE__, ErrCode);                             \
} while (0)

#else
#define STACK_NO_HASH_ASSERT( stack_ptr ) (void*)0
#endif


#endif