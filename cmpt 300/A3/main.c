#include "list.h"
#include "semaphore.h"
#include "pcb.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

static List *ready0, *ready1, *ready2, *sendBlocked, *receiveBlocked;
static Semaphore *sem1, *sem2, *sem3, *sem0, *sem4;
static PCB *initPCB;

static PCB *runningProcessPCB;

ssize_t bytes_read;
ssize_t parameter_read;
static int count = 0;
char *success = "Success\n";
char *failure = "Failure\n";

// PROCESS_RUNNING 0
//  PROCESS_BLOCKED 1
//  PROCESS_READY 2

    // takes in the queue name and prints it
    int
    printQueues(List *list)
{
    Node *temp = List_first(list);

    while (temp != NULL)
    {
        PCB *t = List_curr(list);
        printf("Pid:%d\n", t->pid);
        fflush(stdout);
        printf("Status: %d\n", t->status);
        if (t->receivedMsg != NULL)
        {
            printf("Message sent by %d waiting to be received : %s\n", t->senderPid, t->receivedMsg);
        }
        if (t->replyMsg != NULL)
        {
            printf("Reply received: %s\n", t->replyMsg);
        }

        temp = List_next(list);
        // List_next(list);
    }
    printf(" \n");
    fflush(stdout);
    List_first(list);
}

// checks if the ready queues are empty
int readyQueuesEmpty()
{
    if (List_count(ready0) == 0 && List_count(ready1) == 0 && List_count(ready2) == 0)
    {
        return 1;
    }
    return 0;
}

// checks if the ready queues, send, receive and semaphore blocked queues are empty
int allQueuesEmpty()
{
    int sem0Initialized = (sem0 != NULL);
    int sem1Initialized = (sem1 != NULL);
    int sem2Initialized = (sem2 != NULL);
    int sem3Initialized = (sem3 != NULL);
    int sem4Initialized = (sem4 != NULL);

    if (List_count(sendBlocked) == 0 && List_count(receiveBlocked) == 0 &&
        (!sem0Initialized || List_count(sem0->blockedProcesses) == 0) &&
        (!sem1Initialized || List_count(sem1->blockedProcesses) == 0) &&
        (!sem2Initialized || List_count(sem2->blockedProcesses) == 0) &&
        (!sem3Initialized || List_count(sem3->blockedProcesses) == 0) &&
        (!sem4Initialized || List_count(sem4->blockedProcesses) == 0))
    {
        return 1;
    }
    return 0;
}

//removed process from ready queue and makes it the running process
//im not solving the starvation problem and the highest priority task is always chosen
int removeProcessFromReadyQueue()
{
    // checks if priority 0 has itmes, if wes we remove it from the queue and make it the running process
    if (List_count(ready0) != 0)
    {
        // printf("old running process was in queue  %d and   had pid %d\n",runningProcessPCB->priority,runningProcessPCB->pid);
        // fflush(stdout);
        PCB *removed = List_last(ready0);
        // printf("this will be removed now to make it the new running process from queue %d with pid %d\n", removed->priority,removed->pid);
        // fflush(stdout);
        runningProcessPCB = List_trim(ready0);
        runningProcessPCB->status = PROCESS_RUNNING;
        // printf("new running process was in queue  %d and  had pid %d\n",runningProcessPCB->priority,runningProcessPCB->pid);
        // fflush(stdout);

        // printf("Removed element from ready queue 0 pid: %d\n",runningProcessPCB->pid);
        printf("Making process with pid %d the running process\n", runningProcessPCB->pid);
        fflush(stdout);
    }
    else
    {
        if (List_count(ready1) != 0)
        {
            runningProcessPCB = List_last(ready1);
            runningProcessPCB->status = PROCESS_RUNNING;

            // runningProcessPriority = 1;
            List_trim(ready1);
            // printf("Removed element from ready queue 1\n");
            printf("Making process with pid %d the running process\n", runningProcessPCB->pid);

            fflush(stdout);
        }
        else
        {
            if (List_count(ready2) != 0)
            {
                runningProcessPCB = List_last(ready2);
                runningProcessPCB->status = PROCESS_RUNNING;

                // runningProcessPriority = 2;
                List_trim(ready2);
                // printf("Removed element from ready queue 2\n");
                printf("Making process with pid %d the running process\n", runningProcessPCB->pid);
                fflush(stdout);
            }
            else
            {
                //no process on the ready queues
                printf("All queues are empty we will run the init process.\n");
                // runningProcessPCB->pid =999;
                runningProcessPCB = initPCB;
                return 0;
            }
        }
    }

    // we check if this process was once blocked on a Receive call, since its in the ready queue it means that it received a message
    // so we display the received msg and set the boolean to false
    if (runningProcessPCB->wasBLockedOnReceive == true)
    {
        printf("Message received from process with pid %d.\n Message: %s\n", runningProcessPCB->senderPid, runningProcessPCB->receivedMsg);
        runningProcessPCB->wasBLockedOnReceive = false;
        free(runningProcessPCB->receivedMsg);
        runningProcessPCB->receivedMsg = NULL;

        return 1;
    }
    // we also check if it was once blocked on a send, since its in the ready queue it means that a processs replied to it,
    // so we display the reply and set it to NULL
    if (runningProcessPCB->replyMsg != NULL)
    {
        printf("Reply received from process with pid %d.\n Message: %s\n", runningProcessPCB->receiverPid, runningProcessPCB->replyMsg);
        free(runningProcessPCB->replyMsg);
        runningProcessPCB->replyMsg = NULL;

        return 1;
    }
    return 1;
}

// make a process from the ready queue as the running process, if theyre not empty
void removeInitAsRunningProcess()
{
    if (runningProcessPCB->pid == 999)
    {
        if (!readyQueuesEmpty())
        {
            removeProcessFromReadyQueue();
        }
    }
}

//malloc space for the new processes pcb and then insert it in the right ready queue
int readyQueueInsert(char *priority)
{
    int appendResult;

    PCB *item = (PCB *)malloc(sizeof(PCB));

    (item->pid) = count;
    item->status = PROCESS_READY;
    if (*priority == '0')
    {
        item->priority = 0;
        appendResult = List_prepend(ready0, item);
    }
    else if (*priority == '1')
    {
        item->priority = 1;
        appendResult = List_prepend(ready1, item);
    }
    else
    {
        item->priority = 2;
        appendResult = List_prepend(ready2, item);
    }
    if (appendResult == LIST_FAIL)
    {
        free(item);
        return appendResult;
    } //if success
    count++;
    return appendResult;
}

// wrapper function that inserts a process in the ready queue and then if init is the running process, it makes the new process running
int createProcess(char *parameter)
{

    if (parameter_read > 1)
    {

        if (readyQueueInsert(parameter) == LIST_SUCCESS)
        {
            printf("Successfully inserted with pid %d\n", count - 1);
            fflush(stdout);
        }
        else
        {
            printf("Failed to insert\n");
            fflush(stdout);
            return 0;
        }

        removeInitAsRunningProcess();
        return 1;
    }
}

// adds exisiting process to the right queue
int addExistingProcessToQueue(PCB *pcb)
{
    printf("Adding process to queue %d with pid %d\n", pcb->priority, pcb->pid);
    int res;
    pcb->status = PROCESS_READY;

    if (pcb->priority == 0)
    {
        res = List_prepend(ready0, pcb);
    }
    else if (pcb->priority == 1)
    {
        res = List_prepend(ready1, pcb);
    }
    else
    {
        res = List_prepend(ready2, pcb);
    }
    if (res == LIST_SUCCESS)
    {
        removeInitAsRunningProcess();
    }
    return res;
}

// forks the currently running process and adds it to the right queue
int forkProcess()
{
    if (runningProcessPCB->pid == 999)
    {
        printf("Sorry cannot fork init process\n");
        fflush(stdout);
        return 0;
    }
    else
    {
        char p = runningProcessPCB->priority + '0';
        printf("Priority of the forked process %d\n", runningProcessPCB->priority);
        int res = createProcess(&p);
        return 1 && (res == 1);
    }
}

// compare function to find a process
bool compare(void *pItem, void *toSearch)
{

    int intValue = atoi(toSearch);
    // printf("done with compare %d %d %d\n", *((int *)pItem), *((int *)toSearch), intValue);

    fflush(stdout);
    PCB *currentPCB = (PCB *)pItem;
    return (currentPCB->pid == intValue);
}

// checks if the sem is initiliazed then searches for the process in the sem's blocked queue
// returns the sem it is blocked on, otherwise returns -1
Semaphore *searchSemQueues(char *pid)
{
    // printf("inside sem search looking for %d %s\n", *((int *)pid), pid);
    // fflush(stdout);
    if (sem0 != NULL)
    {
        List_first(sem0->blockedProcesses);
        if (List_search(sem0->blockedProcesses, compare, pid) != NULL)
        {
            return sem0;
        }
    }
    if (sem1 != NULL)
    {
        List_first(sem1->blockedProcesses);
        if (List_search(sem1->blockedProcesses, compare, pid) != NULL)
        {
            return sem1;
        }
    }
    if (sem2 != NULL)
    {
        List_first(sem2->blockedProcesses);
        if (List_search(sem2->blockedProcesses, compare, pid) != NULL)
        {
            return sem2;
        }
    }
    if (sem3 != NULL)
    {
        List_first(sem3->blockedProcesses);
        if (List_search(sem3->blockedProcesses, compare, pid) != NULL)
        {
            return sem3;
        }
    }
    if (sem4 != NULL)
    {
        List_first(sem4->blockedProcesses);
        if (List_search(sem4->blockedProcesses, compare, pid) != NULL)
        {
            return sem4;
        }
    }
    return NULL;
}

// returns the list where the process youre looking for is found. then the current pointer of that list is the process youre looking for
// otherwise returns NULL
List *searchQueues(char *pid)
{
    // printf("inside search looking for %d %s\n", *((int *)pid), pid);
    fflush(stdout);

    List_first(ready0);
    List_first(ready1);
    List_first(ready2);

    if (List_search(ready0, compare, pid) == NULL)
    {
        if (List_search(ready1, compare, pid) == NULL)
        {
            if (List_search(ready2, compare, pid) == NULL)
            {
                // printf("Search didnt find\n");
                // fflush(stdout);
                return NULL;
            }
            else
            {
                //found in ready 2
                List_first(ready0);
                List_first(ready1);

                // printf("Found in ready queue 2\n");
                fflush(stdout);
                return ready2;
            }
        }
        else
        {
            //found in ready 1
            List_first(ready0);
            List_first(ready2);

            // printf("Found in ready queue 1\n");
            fflush(stdout);
            return ready1;
        }
    }
    // found in ready 0
    else
    {
        // printf("Found in ready queue 0\n");
        fflush(stdout);
        return ready0;
    }

    // return 0;
}

// searches the send blocked queue for a process
// if found returns 1 otherwise returns 0
// the current ptr of the List id the process youre looking for
int searchSendBlockedQueue(char *pid)
{
    // printf("inside send blocked search looking for %d %s\n", *((int *)pid), pid);
    // fflush(stdout);

    List_first(sendBlocked);

    if (List_search(sendBlocked, compare, pid) != NULL)
    {

        return 1;
    }
    return 0;
}

// searches the receive blocked queue for a process
// if found returns 1 otherwise returns 0
// the current ptr of the List id the process youre looking for
int searchReceiveBlockedQueue(char *pid)
{
    // printf("inside receive blocked search looking for %d %s\n", *((int *)pid), pid);
    // fflush(stdout);

    List_first(receiveBlocked);

    if (List_search(receiveBlocked, compare, pid) != NULL)
    {
        return 1;
    }
    return 0;
}

// Frees all sem
void freeAllSem()
{
    if (sem0 != NULL)
    {
        printf("Freeing sem0\n");
        fflush(stdout);
        free(sem0);
    }
    if (sem1 != NULL)
    {
        printf("Freeing sem1\n");
        fflush(stdout);
        free(sem1);
    }
    if (sem2 != NULL)
    {
        printf("Freeing sem2\n");
        fflush(stdout);
        free(sem2);
    }
    if (sem3 != NULL)
    {
        printf("Freeing sem3\n");
        fflush(stdout);
        free(sem3);
    }
    if (sem4 != NULL)
    {
        printf("Freeing sem4\n");
        fflush(stdout);
        free(sem4);
    }
    return;
}

// Frees all processes and their dynamically allocated mem
void freePCB(PCB *pcbTofree)
{
    if (pcbTofree->receivedMsg != NULL)
    {
        printf("Freeing received mesg\n");
        free(pcbTofree->receivedMsg);
    }
    if (pcbTofree->replyMsg != NULL)
    {
        printf("Freeing reply mesg\n");
        free(pcbTofree->replyMsg);
    }
    printf("freeing\n");
    fflush(stdout);
    free(pcbTofree);
    return;
}

// if init is the running process, it checks if all queues are empty (along with the send/receive and sem blocked queues)
// if theyre not empty then displays error message
// if they're empty then frees all memory and exits
// if init is not the runnning process then kills the running process and frees its memory
int exitProcess()
{
    if (runningProcessPCB->pid != 999)
    {
        freePCB(runningProcessPCB);
        removeProcessFromReadyQueue();
        return 1;
    }
    else
    {
        if (allQueuesEmpty())
        {
            free(initPCB);
            printf("Killing currently running process\n");
            freeAllSem();
            exit(1);
        }
        else
        {
            printf("Cannot kill init process\n");
            fflush(stdout);
        }
    }
}

// checks all queues for the process, if found, kills it and frees all memory
// if not found returns error
int killProcess(char *pid)
{
    int intValuee = atoi(pid);
    // printf("in val is %d %s\n",intValuee,pid);
    //         fflush(stdout);

    if (runningProcessPCB->pid == intValuee)
    {
        if (intValuee == 999)
        {
            if (allQueuesEmpty())
            {
                printf("Killing the init process and exiting\n");
                fflush(stdout);
                exit(1);
            }
        }
        printf("Killing the currently running process and exiting\n");
        fflush(stdout);

        // kill the currently runnig process
        freePCB(runningProcessPCB);
        removeProcessFromReadyQueue(); //if no process is active it will run the init process
        return 1;
    }
    List *res = searchQueues(pid); //returns the queue the process is in
    // printf("search q returned %d\n", res->);
    // fflush(stdout);
    PCB *removed;

    if (res != NULL)
    {

        removed = List_remove(res);

        // if (removed == intValue)
        // {
        //     printf("Remove was successful\n");
        //     fflush(stdout);
        //     return 1;
        // }
    }
    else
    {
        if (!searchSendBlockedQueue(pid))
        {
            if (!searchReceiveBlockedQueue(pid))
            {
                Semaphore *sem = searchSemQueues(pid);
                if (sem == NULL)
                {
                    printf("Process with pid %s not found, so not killed\n", pid);
                    fflush(stdout);
                    return 0;
                }
                else
                {
                    removed = List_remove(sem->blockedProcesses);
                }
            }
            else
            {
                removed = List_remove(receiveBlocked);
            }
        }
        else
        {
            removed = List_remove(sendBlocked);
        }
    }
    freePCB(removed);
    printf("Killing process with pid %s\n", pid);
    fflush(stdout);

    return 1;
}

// to send msg. checks the receive blocked queue and removes the process from queue if found. and adds msg to the process.
// if not found there then checks all priority queues. if found adds the msg to the process.
// if not found there checks sem blocked queues. if found adds msg to the process
// if not found there checks the send blocked queue. if found adds msg to the process
// otherwise throws error msg
// cannot send messages on the init process because we dont want to block it
// also not allowing multiple messages to a recipient process
// if a process sends a message to a process that already has a message waiting
// to be received then that process is not allowed to send it and is also not blocked
int sendMessage(char *pid, char *message)
{

    char *heapMsg = (char *)malloc(sizeof(char) * 40);

    strcpy(heapMsg, message); // Copy entire message string
    PCB *receiver;

    // printf("sending this to the sender %c", *pid);
    if (runningProcessPCB->pid == 999)
    {
        printf("Sorry cannot send from init process\n");
        fflush(stdout);
        return 0;
    }

    // check if the receiver was already waiting on a message
    if (searchReceiveBlockedQueue(pid))
    {
        receiver = List_remove(receiveBlocked);

        printf("Process was waiting for a message, unblocking it and putting it on the ready queue with pid %s", pid);
        addExistingProcessToQueue(receiver);
    }

    //if not look through all the queues and add the message to their pcb
    else
    {

        List *receiverQueue = searchQueues(pid);

        if (receiverQueue != NULL)
        {
            receiver = List_curr(receiverQueue);
            if (receiver->receivedMsg != NULL)
            {
                printf("Sorry, the process already has a message sent to it. You cannot send a message.\n");
                fflush(stdout);
                free(heapMsg);
                return 0;
            }
        }
        else
        { //check the semaphore blocked queue
            Semaphore *receiverSem = searchSemQueues(pid);
            // printf("Looking in sems\n");
            // fflush(stdout);
            if (receiverSem != NULL)
            {
                receiver = List_curr(receiverSem->blockedProcesses);
            }
            else
            {
                //found in the send blocked queue
                // printf("Looking in send blocked\n");
                // fflush(stdout);
                if (searchSendBlockedQueue(pid))
                {
                    receiver = List_curr(sendBlocked);
                }
                else
                {
                    printf("Receiver process doesnt exist. Sending failed. Currently running process with pid %d\n", runningProcessPCB->pid);
                    fflush(stdout);
                    free(heapMsg);
                    return 0;
                }
            }

            if (receiver->receivedMsg != NULL)
            {
                printf("Sorry, the process already has a message sent to it. You cannot send a message.\n");
                fflush(stdout);
                free(heapMsg);
                return 0;
            }

            //block the sending process
        }
    }
    receiver->receivedMsg = heapMsg;
    receiver->senderPid = runningProcessPCB->pid;

    printf("Blocking the currently running process with pid %d\n", runningProcessPCB->pid);
    fflush(stdout);
    runningProcessPCB->status = PROCESS_BLOCKED;
    List_prepend(sendBlocked, (runningProcessPCB));

    // printf("head of send blocked is pointig at",sendBlocked->head->item );
    removeProcessFromReadyQueue();
    return 1;
}

// tor eceive message. if there is a message it is received and displayed
// if no message was sent then the process is blocked
// cannot receive messages on init process because we dont want it to be blocked
int receiveMessage()
{
    if (runningProcessPCB->pid == 999)
    {
        printf("Cannot receive in init process. \n");
        fflush(stdout);
        return 0;
    }
    if (runningProcessPCB->receivedMsg != NULL)
    {
        printf("Message received from %d process. Message: %s\n", runningProcessPCB->senderPid, runningProcessPCB->receivedMsg);
        fflush(stdout);
        free(runningProcessPCB->receivedMsg);
        runningProcessPCB->receivedMsg = NULL;
        return 1;
    }
    else
    {
        printf("Blocking the currently running process with pid %d beacuse it had no messages to receive\n", runningProcessPCB->pid);
        fflush(stdout);
        runningProcessPCB->status = PROCESS_BLOCKED;

        List_prepend(receiveBlocked, runningProcessPCB);
        runningProcessPCB->wasBLockedOnReceive = true;
        removeProcessFromReadyQueue();
        return 0;
    }
}

// replies to a process with a msg. if the process was blocked on the send queue
// then its unblocked and put on the ready queue,the msg will be displayed when it is made the running process
// if it was not blocked on the send queue then you cannot reply to it
int reply(char *pid, char *message)
{
    // sender was waiting on a reply
    if (searchSendBlockedQueue(pid))
    {
        PCB *removedPCB = List_remove(sendBlocked);
        char *heapMsg = (char *)malloc(sizeof(char) * 40);

        strcpy(heapMsg, message); // Copy entire message string

        removedPCB->replyMsg = heapMsg;
        removedPCB->receiverPid = runningProcessPCB->pid;

        printf("Process was waiting for a reply, unblocking it and putting it on the ready queue %d with pid %s", removedPCB->priority, pid);
        printf("Reply that will be received when the process %d is running: %s", removedPCB->pid, removedPCB->replyMsg);
        addExistingProcessToQueue(removedPCB);
        return 1;
    }
    else
    {
        printf("You cannot reply to a process that never did a send in the first place\n");
        return 0;
    }
}

// initialize a sem and a queue for the processes that are blocked on it
int initSemaphore(char *semId, char *semVal)
{
    int semIdInt = *((char *)semId) - '0';
    if (semIdInt > 4 || semIdInt < 0)
    {
        printf("Cannot have id > 4 and < 0\n");
        fflush(stdout);
        printf("Failed to create sempahore\n");
        fflush(stdout);

        return 0;
    }
    switch (semIdInt)
    {
    case 0:
        if (sem0 != NULL)
        {
            return 0;
        }

        sem0 = (Semaphore *)malloc(sizeof(Semaphore));
        sem0->blockedProcesses = List_create();
        sem0->value = atoi(semVal);
        sem0->initialized = true;
        printf("Initialised sem 0 with val %d\n", sem0->value);
        break;

    case 1:
        if (sem1 != NULL)
        {
            return 0;
        }

        sem1 = (Semaphore *)malloc(sizeof(Semaphore));
        sem1->blockedProcesses = List_create();
        sem1->initialized = true;

        sem1->value = atoi(semVal);
        break;

    case 2:
        if (sem2 != NULL)
        {
            return 0;
        }

        sem2 = (Semaphore *)malloc(sizeof(Semaphore));
        sem2->blockedProcesses = List_create();
        sem2->initialized = true;

        sem2->value = atoi(semVal);
        break;

    case 3:
        if (sem3 != NULL)
        {
            return 0;
        }

        sem3 = (Semaphore *)malloc(sizeof(Semaphore));
        sem3->blockedProcesses = List_create();
        sem3->initialized = true;

        sem3->value = atoi(semVal);
        break;

    case 4:
        if (sem4 != NULL)
        {
            return 0;
        }

        sem4 = (Semaphore *)malloc(sizeof(Semaphore));
        sem4->blockedProcesses = List_create();
        sem4->initialized = true;

        sem4->value = atoi(semVal);
        break;
    }
    return 1;
}

// decrement the sem. if < 0 block the process by adding it to the sem's blocked queue
// cannot make a Pcall on the init process because it cannot be blocked
int blockCallingProcessOnP(Semaphore *sem)
{

    fflush(stdout);
    if (runningProcessPCB->pid == 999)
    {
        printf("Sorry cannot block init process.\n");
        fflush(stdout);
        return 0;
    }
    sem->value--;

    if (sem->value < 0)
    {
        printf("Adding running process with pid %d in the semaphore's blocked queue\n", runningProcessPCB->pid);
        runningProcessPCB->status = PROCESS_BLOCKED;
        List_prepend(sem->blockedProcesses, runningProcessPCB);
        // printQueues(sem->blockedProcesses);
        removeProcessFromReadyQueue();
        return 1;
    }
    printf("Did not block the calling process\n");
    return 1;
}

// the wrapper method for making a Pcall on all sems
int Pcall(char *semId)
{
    int semIdInt = *((char *)semId) - '0';

    switch (semIdInt)
    {
    case 0:
        if (sem0 == NULL)
        {
            printf("Semaphore is not initialised\n");
            fflush(stdout);
            return 0;
        }
        blockCallingProcessOnP(sem0);
        // sem
        break;

    case 1:
        if (sem1 == NULL)
        {
            printf("Semaphore is not initialised\n");
            fflush(stdout);
            return 0;
        }
        blockCallingProcessOnP(sem1);
        break;

    case 2:
        if (sem2 == NULL)
        {
            printf("Semaphore is not initialised\n");
            fflush(stdout);
            return 0;
        }
        blockCallingProcessOnP(sem2);
        break;

    case 3:
        if (sem3 == NULL)
        {
            printf("Semaphore is not initialised\n");
            fflush(stdout);
            return 0;
        }
        blockCallingProcessOnP(sem3);
        break;

    case 4:
        if (sem4 == NULL)
        {
            printf("Semaphore is not initialised\n");
            fflush(stdout);
            return 0;
        }
        blockCallingProcessOnP(sem4);
        break;
    }
    return 1;
}

// increment a sem. if sem <= 0 unblock a process from the sem's queue and put ti on the ready queue
// can all from the init process
int unblockCallingProcessOnV(Semaphore *sem)
{
    sem->value++;
    if (sem->value <= 0)
    {
        if (List_count(sem->blockedProcesses) == 0)
        {
            printf("No processes are blocked on this semaphore\n");
            fflush(stdout);
        }
        PCB *unblockedPcb = List_trim(sem->blockedProcesses);
        addExistingProcessToQueue(unblockedPcb);
    }
    return 1;
}

// wrapper function for the Vcall on alls sems
int Vcall(char *semId)
{
    int semIdInt = atoi(semId);

    switch (semIdInt)
    {
    case 0:
        if (sem0 == NULL)
        {
            printf("Semaphore is not initialised\n");
            fflush(stdout);
            return 0;
        }
        unblockCallingProcessOnV(sem0);
        // sem
        break;

    case 1:
        if (sem1 == NULL)
        {
            printf("Semaphore is not initialised\n");
            fflush(stdout);
            return 0;
        }
        unblockCallingProcessOnV(sem1);
        break;

    case 2:
        if (sem2 == NULL)
        {
            printf("Semaphore is not initialised\n");
            fflush(stdout);
            return 0;
        }
        unblockCallingProcessOnV(sem2);
        break;

    case 3:
        if (sem3 == NULL)
        {
            printf("Semaphore is not initialised\n");
            fflush(stdout);
            return 0;
        }
        unblockCallingProcessOnV(sem3);
        break;

    case 4:
        if (sem4 == NULL)
        {
            printf("Semaphore is not initialised\n");
            fflush(stdout);
            return 0;
        }
        unblockCallingProcessOnV(sem4);
        break;
    }
    return 1;
}

// searches all queues and prints info about a process
// also shows messages waiting to be received and reply received
int processInfo(char *pid)
{
    PCB *process;
    List *queue = searchQueues(pid);
    if (queue == NULL)
    {
        Semaphore *sem = searchSemQueues(pid);
        if (sem == NULL)
        {
            if (!searchSendBlockedQueue(pid))
            {
                if (!searchReceiveBlockedQueue(pid))
                {
                    printf("process not found.\n");
                    fflush(stdout);
                    return 0;
                }
                else
                {
                    process = List_curr(receiveBlocked);
                }
            }
            else
            {
                process = List_curr(sendBlocked);
            }
        }
        else
        {
            process = List_curr(sem->blockedProcesses);
        }
    }
    else
    {
        process = List_curr(queue);
    }
    printf("Process state: %d\nProcess priority: %d\n", process->status, process->priority);
    if (process->receivedMsg != NULL)
    {
        printf("Message sent by %d waiting to be received : %s\n", process->senderPid, process->receivedMsg);
    }
    if (process->replyMsg != NULL)
    {
        printf("Reply received: %s\n", process->replyMsg);
    }
    return 1;
}

// prints info about all processes in all queues
void allInfo()
{
    printf("Displaying ready queue with priority 0\n");
    printQueues(ready0);
    printf("Displaying ready queue with priority 1\n");

    printQueues(ready1);
    printf("Displaying ready queue with priority 2\n");

    printQueues(ready2);
    printf("Displaying send blocked queue \n");

    printQueues(sendBlocked);
    printf("Displaying receive blocked queue \n");

    printQueues(receiveBlocked);
    if (sem0 != NULL)
    {
        printf("Sem0 has value %d\n", sem0->value);
        fflush(stdout);
        printf("Displaying processes blocked on sem0 \n");

        printQueues(sem0->blockedProcesses);
    }
    if (sem1 != NULL)
    {
        printf("Sem1 has value %d\n", sem1->value);
        fflush(stdout);
        printf("Displaying processes blocked on sem1 \n");

        printQueues(sem1->blockedProcesses);
    }
    if (sem2 != NULL)
    {
        printf("Sem2 has value %d\n", sem2->value);
        fflush(stdout);
        printf("Displaying processes blocked on sem2 \n");

        printQueues(sem2->blockedProcesses);
    }
    if (sem3 != NULL)
    {
        printf("Sem3 has value %d\n", sem3->value);
        fflush(stdout);
        printf("Displaying processes blocked on sem3 \n");

        printQueues(sem3->blockedProcesses);
    }
    if (sem4 != NULL)
    {
        printf("Sem4 has value %d\n", sem4->value);
        fflush(stdout);
        printf("Displaying processes blocked on sem4 \n");

        printQueues(sem4->blockedProcesses);
    }
    return;
}

// function to receive commands
void processCommands()
{
    while (1)
    {
        char buffer[3000];
        char parameter[100];
        char parameter1[100];

        bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
        // printf("bytes read: %d", bytes_read);
        buffer[bytes_read] = '\0';
        if (bytes_read > 1)
        {
            if (*buffer == 'C' || *buffer == 'c')
            {
                printf("Priority of the process:\n");
                fflush(stdout);
                parameter_read = read(STDIN_FILENO, parameter, sizeof(parameter));

                parameter[parameter_read] = '\0';
                createProcess(parameter);
            }
            else if (*buffer == 'F' || *buffer == 'f')
            {
                if (!forkProcess())
                {

                    printf("Couldnt fork process\n");
                    fflush(stdout);
                }
            }
            else if (*buffer == 'K' || *buffer == 'k')
            {
                printf("Pid of the process:\n");
                fflush(stdout);
                parameter_read = read(STDIN_FILENO, parameter, sizeof(parameter));

                parameter[parameter_read] = '\0';
                //  printf("Pid of the process %s\n",parameter);
                //     fflush(stdout);
                killProcess(parameter);
            }
            else if (*buffer == 'E' || *buffer == 'e')
            {

                exitProcess();
            }
            else if (*buffer == 'Q' || *buffer == 'q')
            {
                //time quantum expired, put present process back in the right
                // queue and make a new process the running process
                PCB *tempPCB = runningProcessPCB;
                // printf("tempPCB that will be put in the ready queue %d has pid %d\n", tempPCB->priority,tempPCB->pid);
                //     fflush(stdout);
                removeProcessFromReadyQueue();
                // printf("tempPCB that will be put in the ready queue %d has pid %d", tempPCB->priority,tempPCB->pid);
                //     fflush(stdout);

                addExistingProcessToQueue(tempPCB);
            }

            else if (*buffer == 'S' || *buffer == 's')
            {
                //send the message to the process with pid, block the currently running process
                // save the msg on the receiver processes field
                printf("Pid of the process to send message to: \n");
                fflush(stdout);
                parameter_read = read(STDIN_FILENO, parameter, sizeof(parameter));
                parameter[parameter_read] = '\0';
                char *pid = parameter;

                printf("Message: \n");
                fflush(stdout);
                parameter_read = read(STDIN_FILENO, parameter1, sizeof(parameter1));
                parameter1[parameter_read] = '\0';
                char *message = parameter1;
                // printf("Msg: %s", message);
                // fflush(stdout);
                // printf("Pid %s", pid);
                // fflush(stdout);
                sendMessage(pid, message);
            }

            else if (*buffer == 'R' || *buffer == 'r')
            {
                receiveMessage();
            }

            else if (*buffer == 'Y' || *buffer == 'y')
            {
                printf("Pid of the process to reply to: \n");
                fflush(stdout);
                parameter_read = read(STDIN_FILENO, parameter, sizeof(parameter));
                parameter[parameter_read] = '\0';
                char *pid = parameter;

                printf("Message: \n");
                fflush(stdout);
                parameter_read = read(STDIN_FILENO, parameter1, sizeof(parameter1));
                parameter1[parameter_read] = '\0';
                char *message = parameter1;
                // printf("Msg: %s", message);
                // fflush(stdout);
                // printf("Pid %s", pid);
                // fflush(stdout);
                reply(pid, message);
            }
            else if (*buffer == 'N' || *buffer == 'n')
            {
                printf("Semaphore id: \n");
                fflush(stdout);
                parameter_read = read(STDIN_FILENO, parameter, sizeof(parameter));
                parameter[parameter_read] = '\0';
                char *semId = parameter;

                printf("Initial semaphore value: \n");
                fflush(stdout);
                parameter_read = read(STDIN_FILENO, parameter1, sizeof(parameter1));
                parameter1[parameter_read] = '\0';
                char *initVal = parameter1;
                initSemaphore(semId, initVal);
            }
            else if (*buffer == 'P' || *buffer == 'p')
            {
                printf("Semaphore id: \n");
                fflush(stdout);
                parameter_read = read(STDIN_FILENO, parameter, sizeof(parameter));
                parameter[parameter_read] = '\0';
                char *semId = parameter;

                // printf("Semaphore id: %s", semId);
                // fflush(stdout);
                Pcall(semId);
            }
            else if (*buffer == 'V' || *buffer == 'v')
            {
                printf("Semaphore id: \n");
                fflush(stdout);
                parameter_read = read(STDIN_FILENO, parameter, sizeof(parameter));
                parameter[parameter_read] = '\0';
                char *semId = parameter;

                // printf("Semaphore id: %s", semId);
                // fflush(stdout);
                Vcall(semId);
            }
            else if (*buffer == 'I' || *buffer == 'i')
            {
                printf("Process id: \n");
                fflush(stdout);
                parameter_read = read(STDIN_FILENO, parameter, sizeof(parameter));
                parameter[parameter_read] = '\0';
                char *pId = parameter;

                // printf("process id %s", pId);
                // fflush(stdout);
                processInfo(pId);
            }
            else if (*buffer == 'T' || *buffer == 't')
            {

                allInfo();
            }
            else
            {
                printf("Please enter a valid command\n");
                fflush(stdout);
            }
        }
        // shows the currently running process after each command
        if (runningProcessPCB->pid == 999)
        {
            printf("Currently running init process\n");
            fflush(stdout);
        }
        else
        {
            printf("Currently running process has pid %d\n", runningProcessPCB->pid);
            fflush(stdout);
        }
        //    allInfo();
    }
    return;
}
// initialises the ready queues and send/receive blocked queues
// makes init process the running process
// calls processCommands()
int main()
{
    ready0 = List_create();
    ready1 = List_create();
    ready2 = List_create();
    sendBlocked = List_create();
    receiveBlocked = List_create();
    int i = 999;

    initPCB = (PCB *)malloc(sizeof(PCB));
    initPCB->pid = i;
    initPCB->status = PROCESS_RUNNING;
    runningProcessPCB = initPCB;
    processCommands();

    return 0;
}