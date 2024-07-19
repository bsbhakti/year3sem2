#include "list.h"
#include <stdio.h>

typedef struct Semaphore_s Semaphore;
struct Semaphore_s
{
    int value;
    List *blockedProcesses;
    bool initialized;
};
