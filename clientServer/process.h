#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 


typedef struct
{
    pthread_t uuid;
    void* input;
    void* output;

} Thread;


int create_thread(Thread* thread, void*(*function)(void* arg));
void wait_for_threads(int nthreads, Thread *threads);
