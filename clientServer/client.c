#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "process.h"


/* Porta do servidor */
#define PORT 8080


/* Tamanho do buffer */
#define LEN 4096


/* Endereço do servidor */
#define SERVER_ADDR "127.0.0.1"


/*
 * Função responsável em receber todas as mensagens enviada pelo servidor
 */
void* receiverPrint(void *arg){
    Thread* self = (Thread*)arg;
    int* sockfd = (int*)self->input;

    char buffer_in[LEN];

    int slen;
    while ((slen = recv(*sockfd, buffer_in, LEN, 0)) > 0){
        buffer_in[slen] = '\0';
        fprintf(stdout, "%s", buffer_in);
    }
    
    return NULL;
}


/*
 * Função Main do cliente
 */
int main(int argc, char *argv[]) {

    /* Server socket struct */
    struct sockaddr_in server;
    /* Descritores de arquivo do cliente para o socket */
    int sockfd;

    int len = sizeof(server);

    /* buffer de envio */
    char buffer_out[LEN] = {0};


    fprintf(stdout, "Starting Client ...\n");


    /*
     * Criando um socket para o cliente
     */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error on client socket creation:");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Client socket created with fd: %d\n", sockfd);


    /* Definições das propriedades da conexão*/
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    memset(server.sin_zero, 0x0, 8);


    /* Tenta se conectar ao servidor */
    if (connect(sockfd, (struct sockaddr*) &server, len) == -1) {
        perror("Can't connect to server");
        return EXIT_FAILURE;
    }

    /* Recebe a mensagem de apresentação do servidor */
    char buffer_in[LEN] = {0};
    int slen;
    if ((slen = recv(sockfd, buffer_in, LEN, 0)) > 0){
        printf("%s\n", buffer_in);
    }

    /* Recebe as mensagens do servidor em uma thread */
    
    Thread receiverThread = {0};
    receiverThread.input = &sockfd;
    create_thread(&receiverThread, receiverPrint);


    /*
     * Comunicando com o servidor
     */
    while (true) {

        /* limpando o buffer */
        memset(buffer_out, 0x0, LEN);

        fgets(buffer_out, LEN, stdin); // esperando comandos do usuário

        if (buffer_out[0] == '\n'){
            continue;
        }else if(strcmp(buffer_out, "exit\n") == 0){
            break;
        }
        

        /* montando o protocolo da aplicação para envio da informações */
        for (size_t i = 0; i < LEN; i++){
            if (buffer_out[i] == ' '){
                buffer_out[i] = ';';
            }
        }
        

        /* Envio das informações para o servidor */
        send(sockfd, buffer_out, strlen(buffer_out), 0);

    }


    /* fecha a conexão com o servidor */
    close(sockfd);

    fprintf(stdout, "\nConnection closed\n\n");

    return EXIT_SUCCESS;
}