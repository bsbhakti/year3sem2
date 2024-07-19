#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#include <semaphore.h>
#include <stdbool.h>

static List *toSendList, *toReceiveList;
static pthread_mutex_t toSend, toReceive;
pthread_t thread1, thread2, thread3, thread4;

static pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond4 = PTHREAD_COND_INITIALIZER; //check if there are elements to send from the list
static int elementsInList1 = 0;
static int spaceOnList1 = 100;
static int elementsInList2 = 0;
static int spaceOnList2 = 100;
static bool exitSent = false;
static bool end;
int s, s1, result_thread4;
int serv_portno, my_portno;
struct sockaddr_in serv_addr, cli_addr, my_addr;
ssize_t bytes_read, bytes_received;

void *cancelAllThreads(pthread_t callingThread, pthread_t t1, pthread_t t2, pthread_t t3)
{

    int result_thread1 = pthread_cancel(t1);
    if (result_thread1 != 0)
    {
        printf("Error canceling thread1: %d\n", result_thread1);
    }

    int result_thread2 = pthread_cancel(t2);
    if (result_thread2 != 0)
    {
        printf("Error canceling thread2: %d\n", result_thread2);
    }

    int result_thread3 = pthread_cancel(t3);
    if (result_thread3 != 0)
    {
        printf("Error canceling thread3: %d\n", result_thread3);
    }

    if (pthread_cond_destroy(&cond1) != 0)
    {
        perror("Error destroying cond1");
    }
    if (pthread_cond_destroy(&cond2) != 0)
    {
        perror("Error destroying cond2");
    }
    if (pthread_cond_destroy(&cond3) != 0)
    {
        perror("Error destroying cond3");
    }
    if (pthread_cond_destroy(&cond4) != 0)
    {
        perror("Error destroying cond4");
    }

    // if (close(s) == -1)
    // {
    //     printf("error in closing s1\n");
    // }
    // if (close(s1) == -1)
    // {
    //     printf("error in closing s2\n");
    // }
    result_thread4 = pthread_cancel(callingThread);
    if (result_thread4 != 0)
    {
        printf("Error canceling thread4: %d\n", result_thread4);
    }

    return NULL;
}

void *keyboard_thread(void *msg)
{
    // printf("%s \n", msg);
    while (1 && !end)
    {
        // printf("reinitialising\n");
        char buffer[3000];
        bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
        // printf("bytes read: %d", bytes_read);
        buffer[bytes_read] = '\0';
        // printf("typedb: %s \n", buffer);

        if (bytes_read < 1)
        {
            perror("read");
            return NULL;
        }
        else
        {
            // printf("typed: %s \n", buffer);
            pthread_mutex_lock(&toSend); //lock the mutex
            if (spaceOnList1 == 0)
            {
                // printf("waiting in keyboard thread");
                pthread_cond_wait(&cond2, &toSend);
            }

            if (buffer[0] == '!')
            {
                // printf("making true \n");
                exitSent = true;
            }
            List_prepend(toSendList, (buffer)); // item is added to the list

            pthread_mutex_unlock(&toSend);
            // printf("posting on full1\n");
            elementsInList1++;
            // printf("elementsInList1 incremented: %d\n", elementsInList1);
            spaceOnList1--;
            pthread_cond_signal(&cond1);
        }
    }
    return NULL;
}

void *send_thread(void *msg)
{
    // printf("%s \n", msg);
    while (1 && !end)
    {

        pthread_mutex_lock(&toSend); //lock mutex
        if (elementsInList1 == 0)
        {
            // printf("waiting in send thread");
            pthread_cond_wait(&cond1, &toSend);
        }

        void *item = List_trim(toSendList); //consume the element

        pthread_mutex_unlock(&toSend);
        // printf("Sending message: ");
        sendto(s, item, strlen(item), 0, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
        if (exitSent)
        {
            printf("Exiting\n");
            // exit(0);
            cancelAllThreads(thread2, thread1, thread3, thread4);
        }
        // printf("posting on empty1\n");
        elementsInList1--;
        spaceOnList1++;
        pthread_cond_signal(&cond2);
    }
    return NULL;
}

void *recv_thread(void *msg)
{
    // printf("%s \n", msg);
    while (1)
    {
        // printf("here\n");
        char buffer2[3000];
        socklen_t client_addr_len = sizeof(struct sockaddr_in);
        bytes_received = recvfrom(s1, &buffer2, sizeof(buffer2), 0, (struct sockaddr *)&cli_addr, &client_addr_len);
        // printf("bytes received from receive thread: %zd", bytes_received);
        buffer2[bytes_received] = '\0';
        if (bytes_received < 0)
        {
            perror("recvfrom");
            // Handle error
        }
        else
        {
            // Process received data
            // printf("bytes received from receive thread: %zd", bytes_received);
            // printf("buffer: %s", buffer);
            // write(STDOUT_FILENO, buffer2, bytes_received);
            pthread_mutex_lock(&toReceive);
            if (spaceOnList2 == 0)
            {
                pthread_cond_wait(&cond3, &toReceive);
            }
            if (buffer2[0] == '!')
            {
                printf("Exit Received\n");
                // exit(0);
                pthread_mutex_unlock(&toReceive);
                cancelAllThreads(thread3, thread1, thread2, thread4);
            }
            List_prepend(toReceiveList, (buffer2));
            pthread_mutex_unlock(&toReceive);
            elementsInList2++;
            spaceOnList2--;
            pthread_cond_signal(&cond4);
        }
    }
    return NULL;
}

void *screen_thread(void *msg)
{
    // printf("%s \n", msg);
    while (1)
    {
        pthread_mutex_lock(&toReceive); //lock mutex
        if (elementsInList2 == 0)
        {
            pthread_cond_wait(&cond4, &toReceive);
        }
        char *item = List_trim(toReceiveList); //consume the element

        pthread_mutex_unlock(&toReceive);
        printf("Message received: \n");
        write(STDOUT_FILENO, item, strlen(item));
        elementsInList2--;
        spaceOnList2++;
        pthread_cond_signal(&cond3);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        exit(0);
    }

    // for sending
    toSendList = List_create();
    pthread_mutex_init(&toSend, NULL);

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == -1)
    {
        perror("socket");
        return 1;
    }
    struct addrinfo hints = {0};
    struct addrinfo *res;

    // Set up hints for address resolution
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    bzero((char *)&serv_addr, sizeof(serv_addr));
    char *nodeName = argv[2];
    serv_portno = atoi(argv[3]); //this is where the other person is listening
    getaddrinfo(nodeName, argv[3], &hints, &res);
    struct addrinfo *p;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_portno);

    for (p = res; p != NULL; p = p->ai_next)
    {
        if (p->ai_family == AF_INET)
        {
            struct sockaddr_in *addr_in = (struct sockaddr_in *)p->ai_addr;
            memcpy(&serv_addr, addr_in, sizeof(struct sockaddr_in));
            break;
        }
    }
    freeaddrinfo(res);

    //for receiving
    toReceiveList = List_create();

    pthread_mutex_init(&toReceive, NULL);
    s1 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s1 == -1)
    {
        perror("socket");
        return 1;
    }
    bzero((char *)&my_addr, sizeof(my_addr));
    my_portno = atoi(argv[1]); //this is where I am listening
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(my_portno);

    int bind_result = bind(s1, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_in));
    if (bind_result == -1)
    {
        perror("bind failed");
        close(s1);
        return 1;
    }

    char *message1 = "Thread 1";
    char *message2 = "Thread 2";
    char *message3 = "Thread 3";
    char *message4 = "Thread 4";

    int iret1, iret2, iret3, iret4;
    iret1 = pthread_create(&thread1, NULL, keyboard_thread, (void *)message1);
    iret2 = pthread_create(&thread2, NULL, send_thread, (void *)message2);
    iret3 = pthread_create(&thread3, NULL, recv_thread, (void *)message3);
    iret4 = pthread_create(&thread4, NULL, screen_thread, (void *)message4);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);

    return 0;
}