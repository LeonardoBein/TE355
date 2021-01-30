#include "process.h"


/**
 * Função cria uma thread
**/

int create_thread(Thread* thread, void*(*function)(void* arg), int mutex_value){
    sem_init(&thread->mutex, 0, mutex_value);
    return pthread_create(&thread->uuid, NULL, function, (void *)thread);
}

/**
 * Função espera o termino das thread
**/

void wait_for_threads(int nthreads, Thread *threads){
    for (int i = 0; i < nthreads; i++){
        pthread_join(threads[i].uuid, NULL);
    }
    for (int i = 0; i < nthreads; i++){
        sem_destroy(&(threads[i].mutex));
    }
    
}