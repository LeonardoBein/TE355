#include "myprotocol.h"


/**
 * Função valida se o protocolo é valido
**/

int validProtocol(MyProtocol* protocol){
    if (protocol == NULL){ return 0; }


    if (    strcmp(protocol->command, "mkdir") == 0 || 
            strcmp(protocol->command, "cd") == 0 || 
            strcmp(protocol->command, "rm") == 0 ||
            strcmp(protocol->command, "rmdir") == 0
        ){
        if (protocol->argc < 1){ return 0; }
        return 1;
    } else if (strcmp(protocol->command, "rename") == 0 || strcmp(protocol->command, "mv") == 0) {
        if (protocol->argc < 2){ return 0; }
        return 1;
    } else if (strcmp(protocol->command, "pwd") == 0 || strcmp(protocol->command, "ls") == 0){
        return 1;
    }
    
    return 0;
}


/**
 * Função analisa e retorna uma struct contendo as informações do protocolo
**/
MyProtocol* parseProtocol(char* protocolRaw){
    MyProtocol* protocol = (MyProtocol*) calloc(1, sizeof(MyProtocol));
    
    char* aux = protocolRaw;

    while (protocolRaw){
        if (*protocolRaw == '\n' || *protocolRaw == '\r'){
            *protocolRaw = '\0';
            break;
        }
        protocolRaw++;
    }

    protocolRaw = aux;

    char* pch;
    pch = strtok(protocolRaw,";");
    
    if (pch == NULL){ return NULL; }
    
    if (strcmp(pch, "rename") == 0){
        protocol->command = (char*) calloc(3, sizeof(char));
        strcpy(protocol->command, "mv"); 
    } else {
        protocol->command = pch;
    }
    
    protocol->argc = 1;
    protocol->argv = (char** )calloc(protocol->argc, sizeof(char*)); // array de tamanho 1
    protocol->argv[0] = NULL;

    pch = strtok (NULL, ";");

    // // contem args no protocolo ?
    while (pch != NULL){
        protocol->argv[protocol->argc - 1] = pch;
        pch = strtok (NULL, ";");
        
        if (pch != NULL){
            protocol->argc += 1;
            protocol->argv = realloc(protocol->argv, protocol->argc * sizeof(char*));

            if (protocol->argv == NULL){
                printf("ERROR\n");
                exit(1);
            }   
        }
    }

    if (protocol->argv[0] == NULL){ 
        protocol->argc = 0;
        free(protocol->argv);
    }

    return protocol;
}

/**
 * Função cria o protocol a ser enviado pelo cliente
**/
char* createProtocol(char* command, char* arg1, ...){
    char* protocol = (char*) calloc(100, sizeof(char));

    va_list ap;
    char* aux;

    va_start(ap, arg1); 
    aux = arg1;

    strcat(protocol, command);

    while (aux != NULL){
        strcat(protocol, ";");
        strcat(protocol, aux);
        aux = va_arg(ap, char*);
    }

    va_end(ap);

    return protocol;
}

/**
 * Função imprime os valores da struct
**/
void printProtocol(MyProtocol* p){
    if (p == NULL){
        return;
    }
    
    printf("** MY PROTOCOL **\n");
    printf("command: %s\n", p->command);
    printf("argc: %d\n", p->argc);
    for (size_t i = 0; i < p->argc; i++){
        printf("arg: %s\n", p->argv[i]);
    }
    printf("** **************\n");
}