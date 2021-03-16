#ifndef _MY_PROCESS_H_
#define _MY_PROCESS_H_ 1

#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h> 
#include <semaphore.h> 

typedef struct
{
    pthread_t uuid;
    void* input;
    void* output;

} Thread;

void remove_thread(Thread* process);
Thread* new_thread();
void initial_controller_thread();
Thread* get_threads(int* size);

int create_thread(Thread* thread, void*(*function)(void* arg));
void wait_for_threads(int nthreads, Thread *threads);

#endif // _MY_PROCESS_H_


