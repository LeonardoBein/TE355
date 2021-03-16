#include "process.h"

sem_t mutex;
Thread* myProcess = NULL;
int myProcessSize = 0;

void initial_controller_thread(){
    myProcess = NULL;
    sem_init(&mutex, 0, 1);
}

Thread* new_thread(){
    sem_wait(&mutex);
    if (myProcess == NULL){
        myProcessSize = 1;
        myProcess = calloc(1, sizeof(Thread));
    }else{
        myProcessSize++;
        myProcess = realloc(myProcess, sizeof(Thread)*myProcessSize);
    }
    sem_post(&mutex);

    return &myProcess[myProcessSize-1];
}

Thread* get_threads(int* size){
    *size = myProcessSize;
    return myProcess;
}

void remove_thread(Thread* process){
    sem_wait(&mutex);
    int index = -1;
    
    if (myProcessSize == 1){
        myProcessSize = 0;
        free(myProcess);
        myProcess = NULL;
        sem_post(&mutex);
        return;
    }
    

    for (int i = 0; i < myProcessSize; i++){
        if (process == &myProcess[i]){
            index = i;
            break;
        }
        
    }

    if (index == -1){
        printf("error when deleting the process\n"); exit(1);
    }

    for(int i=index; i < myProcessSize; i++){
        myProcess[i] = myProcess[i + 1];
    }
    myProcessSize--;
    if (myProcessSize <= 0){
        myProcessSize = 0;
        free(myProcess);
    }else{
        myProcess = realloc(myProcess, sizeof(Thread)*myProcessSize);

    }
    sem_post(&mutex);
}



/**
 * Função cria uma thread
**/

int create_thread(Thread* thread, void*(*function)(void* arg)){
    return pthread_create(&thread->uuid, NULL, function, (void *)thread);
}

/**
 * Função espera o termino das thread
**/

void wait_for_threads(int nthreads, Thread *threads){
    for (int i = 0; i < nthreads; i++){
        pthread_join(threads[i].uuid, NULL);
    }
    
}