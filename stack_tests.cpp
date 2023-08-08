#include "stack.h"
#include "stack_debug.h"



int main ()
{
    my_stack stack = {};

    InitDumpFile("DUMP.txt");
    InitLogFile("LOG.txt");

    StackInit (&stack, 10, 10);

    for (int i = 0; i < 11; i++)
    {
        int err = StackPush (&stack, &i);
        PrintErr(err, &stack, __func__, __LINE__);
    }

    StackDump (&stack, __func__, __LINE__, 0);

    for (int i = 0; i < 11; i ++)
    {
        int a = 0;
        int err = StackPop(&stack, &a);
        PrintErr(err, &stack, __func__, __LINE__);
    }
    

    StackDestroy (&stack);

    DestroyDumpFile();
    DestroyLogFile();

    return 0;
}