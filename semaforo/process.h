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
    sem_t mutex;

} Thread;


int create_thread(Thread* thread, void*(*function)(void* arg), int mutex_value);
void wait_for_threads(int nthreads, Thread *threads);
