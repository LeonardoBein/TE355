#include "process.h"

#define THREAD_NUMBER 6

enum { 
    A = 0, 
    B, 
    C, 
    D, 
    E, 
    F };

char threadCharacter[THREAD_NUMBER] = "abcdef";
int queue[] = {
    C , C , C , C , C , C , C , C , C , C , C , C , C , C , C , C , C , C , C, 
    D , D , D , D,
    B , B , B , B , B , B , B , B , B , B , B , B , B , B , B , B, 
    D , D , D , D , D , D , D , D , D , D , D , D , D , D , D , D , D, 
    A , A , A , A , A , A , A , A , A , A , A , A , A , A , A , A , A, 
    C , C , C , C , C , C , C , C , C , C , C , C , C, 
    E , E , E , E , E , E , E , E , E , E , E, 
    F , F , F , F , F , F , F , F , F , F , F
};
int nQueue = 108;

char isStop = 0;
sem_t mutex;

// thread pai, responsavel em controlar os filhos
void *threadMaster(void *arg){
    Thread* self = ((Thread*)arg);
    Thread* childs = (Thread*)self->input;
    
    for (int i = 0; i < nQueue; i++){
        sem_wait(&mutex);
        sem_post(&childs[queue[i]].mutex);     
    }
    sleep(1);
    
    // NÃO FUNCIONA, SEI LÁ PQ!!!!
    // isStop = 1;
    // for (int i = 0; i < THREAD_NUMBER; i++){
    //     sem_post(&childs[queue[i]].mutex);
    //     sleep(1);
    // }

    exit(0);
    
    
}

// Threads filhas
void* printChar(void *arg){
    Thread* self = ((Thread*)arg);
    char* character = self->input;

    while (1){
        sem_wait(&(self->mutex));

        if (isStop == 1){ return NULL; }

        printf("\t- %c\n", *character);
        sem_post(&mutex);
    }
}



int main(int argc, char const *argv[])
{
    printf("***GRR20172158\n* Saída:\n");

    sem_init(&mutex, 0, 1);
    
    Thread thread[THREAD_NUMBER];
    Thread master;

    
    for (int i = 0; i < THREAD_NUMBER; i++){
        thread[i].input = (void *)&threadCharacter[i];
        create_thread(&thread[i], printChar, 0);
    }

    master.input = (void *) thread;
    create_thread(&master, threadMaster, 1);
    
    wait_for_threads(1, &master);
    sem_destroy(&mutex);



    wait_for_threads(THREAD_NUMBER, thread);

    return 0;
}
