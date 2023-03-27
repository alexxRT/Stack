#include "../lib/Stack.h"
#include "../lib/StackSafety.h"



int main ()
{
    my_stack stack = {};

    InitDumpFile();
    InitLogFile();

    StackInit (&stack, 10, 10);

    for (int i = 0; i < 11; i++)
    {
        int err = StackPush (&stack, &i);
    }

    StackDump (&stack, __func__, __LINE__, 0);

    for (int i = 0; i < 11; i ++)
    {
        int a = 0;
        int err = StackPop(&stack, &a);
    }
    

    StackDestroy (&stack);

    DestroyDumpFile();
    DestroyLogFile();

    return 0;
}


