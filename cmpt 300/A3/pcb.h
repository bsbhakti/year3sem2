#include "list.h"
#include <stdio.h>


#define PROCESS_RUNNING 0
#define PROCESS_BLOCKED 1
#define PROCESS_READY 2

#define LIST_FAIL -1

typedef struct Pcb_s PCB;
struct Pcb_s
{
    int pid;
    int priority;
    int status;
    char *receivedMsg;
    char *replyMsg;
    int senderPid;
    int receiverPid;
    bool wasBLockedOnReceive;
 

};
