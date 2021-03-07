#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "myprotocol.h"
#include "process.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/* Porta do servidor */
#define MY_PORT 8080

/* Tamanho do buffer */
#define BUFFER_LENGTH 4096

/*
 * Função reponsavel em comunicar com o cliente e executar as instruções
 */
void* functionClient(void* arg){
    Thread* self = (Thread*)arg;
    int* clientfd = (int*) self->input; /*Descritores de arquivo do cliente*/

    MyProtocol* protocol; // Meu protocolo de aplicação simples 
    char path[100] = {0}; // Memória do diretorio RAIZ
    char buffer[BUFFER_LENGTH] = {0};
    FILE* fp; // PIPE dos processos internos (mv,rename,rm,cd,ls,mkdir)
    
    
    if (getcwd(path, sizeof(path)) == NULL){
        printf("Error getcwd");
        exit(1);
    }

    

    strcpy(buffer, "Hello, client!\n\0");


    /* Envia a primeira mensagem ao cilente */
    if (send(*clientfd, buffer, strlen(buffer), 0)) {
        fprintf(stdout, "Client connected.\nWaiting for client message ...\n");

        /* limpa o buffer */
        memset(buffer, 0x0, BUFFER_LENGTH);

        /* Comunica-se com o cliente */
        int message_len;
        while((message_len = recv(*clientfd, buffer, BUFFER_LENGTH, 0)) > 0){

            printf("Client sent: %s\n", buffer);

            protocol = NULL;
            
            protocol = parseProtocol(buffer);
            printProtocol(protocol);
            
            if (!validProtocol(protocol)){
                send(*clientfd, "Invalid\n", 9, 0);
                continue;
            }

            if (strcmp(protocol->command, "pwd") == 0){
                send(*clientfd, path, strlen(path), 0);
                send(*clientfd, "\n", 1, 0);
                continue;
            }
            

            char command[1000] = {0};
            char responseCommand[4096];
            
            strcpy(command, "cd ");
            strcat(command, path);
            strcat(command, " && ");

            strcat(command, protocol->command);

            if (strcmp(protocol->command, "ls") == 0){
                strcat(command, " -1");
            }if (
                strcmp(protocol->command, "mkdir") == 0 || 
                strcmp(protocol->command, "rm") == 0 ||
                strcmp(protocol->command, "rmdir") == 0 ||
                strcmp(protocol->command, "mv") == 0 ||
                strcmp(protocol->command, "rename") == 0   ){
            
                strcat(command, " -v");

                if (strcmp(protocol->command, "mkdir") == 0){
                    strcat(command, " -p");
                }
                
            
            }


            for (int i = 0; i < protocol->argc; i++){
                strcat(command, " "); 
                strcat(command, protocol->argv[i]);
            }

            strcat(command, " 2>&1");

            if (strcmp(protocol->command, "cd") == 0){
                strcat(command, " && pwd");
            }
            
            
            printf("COMMAND: %s\n", command);
            fp = popen(command, "r");

            if (fp == NULL) {
                printf("Failed to run command\n" );
                exit(1);
            }

            while (fgets(responseCommand, sizeof(responseCommand), fp) != NULL) {
                send(*clientfd, responseCommand, strlen(responseCommand), 0);
                
                if (strlen(responseCommand) > 2 && strcmp(protocol->command, "cd") == 0 && responseCommand[0] == '/'){
                    if (responseCommand[strlen(responseCommand) - 1] == '\n'){
                        responseCommand[strlen(responseCommand) - 1] = '\0';
                    }
                    strcpy(path, responseCommand);
                }
                
            }

            pclose(fp);
            printf("PATH_MEMORY: %s\n", path);


        }

        if (message_len == -1){ /* Conexão do cliente ERROR */
            printf("Error recv in client\n");
        } else if (message_len == 0){ /* Conexão do cliente Close */
            printf("Client disconnected\n");
            close(*clientfd);
        }
        
    }

    return NULL;

} 


/*
 * Função main do servidor
 */

int main(int argc, char const *argv[]) {

    struct sockaddr_in client, server; /* sockets struct do cliente e servidor */
    int serverfd; /* Descritores de arquivo do servidor */

    
    fprintf(stdout, "Starting server\n");

    /* Cria um IPv4 socket */
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverfd == -1) {
        perror("Can't create the server socket:");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Server socket created with fd: %d\n", serverfd);


    /* Define as propriedades do socket do servidor */
    server.sin_family = AF_INET;
    server.sin_port = htons(MY_PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(server.sin_zero, 0x0, 8);


    /* Lidar com o erro da porta já em uso */
    int yes = 1;
    if(setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR,
                  &yes, sizeof(int)) == -1) {
        perror("Socket options error:");
        return EXIT_FAILURE;
    }


    /* vincula o socket a uma porta */
    if(bind(serverfd, (struct sockaddr*)&server, sizeof(server)) == -1 ) {
        perror("Socket bind error:");
        return EXIT_FAILURE;
    }


    /* Começa a esperar conexões de clientes */
    if(listen(serverfd, 1) == -1) {
        perror("Listen error:");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Listening on port %d\n", MY_PORT);

    while (1){
        socklen_t client_len = sizeof(client);
        int* clientfd = calloc(1, sizeof(int));
        if ((*clientfd=accept(serverfd, (struct sockaddr *) &client, &client_len )) == -1) {
            perror("Accept error:");
            continue;
        }

        Thread process;
        process.input = clientfd;
        create_thread(&process, functionClient); 


    }
    

    /* fecha o socket local */
    close(serverfd);

    printf("Connection closed\n\n");

    return EXIT_SUCCESS;
}

