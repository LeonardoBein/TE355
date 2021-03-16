#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "process.h"
#include "crypt.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/* Porta do servidor */
#define MY_PORT 8080

/* Tamanho do buffer */
#define BUFFER_LENGTH 4098
#define BUFFER_LENGTH_AUX 4202

#define ENCRYPTED_LENGTH 4098
#define DECRYPTED_LENGTH 4098

unsigned char  encrypted[ENCRYPTED_LENGTH] = {0};
unsigned char  decrypted[DECRYPTED_LENGTH] = {0};

char publicKey[]="-----BEGIN PUBLIC KEY-----\n"\
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAy8Dbv8prpJ/0kKhlGeJY\n"\
"ozo2t60EG8L0561g13R29LvMR5hyvGZlGJpmn65+A4xHXInJYiPuKzrKUnApeLZ+\n"\
"vw1HocOAZtWK0z3r26uA8kQYOKX9Qt/DbCdvsF9wF8gRK0ptx9M6R13NvBxvVQAp\n"\
"fc9jB9nTzphOgM4JiEYvlV8FLhg9yZovMYd6Wwf3aoXK891VQxTr/kQYoq1Yp+68\n"\
"i6T4nNq7NWC+UNVjQHxNQMQMzU6lWCX8zyg3yH88OAQkUXIXKfQ+NkvYQ1cxaMoV\n"\
"PpY72+eVthKzpMeyHkBn7ciumk5qgLTEJAfWZpe4f4eFZj/Rc8Y8Jj2IS5kVPjUy\n"\
"wQIDAQAB\n"\
"-----END PUBLIC KEY-----\n";
  
 char privateKey[]="-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEowIBAAKCAQEAy8Dbv8prpJ/0kKhlGeJYozo2t60EG8L0561g13R29LvMR5hy\n"\
"vGZlGJpmn65+A4xHXInJYiPuKzrKUnApeLZ+vw1HocOAZtWK0z3r26uA8kQYOKX9\n"\
"Qt/DbCdvsF9wF8gRK0ptx9M6R13NvBxvVQApfc9jB9nTzphOgM4JiEYvlV8FLhg9\n"\
"yZovMYd6Wwf3aoXK891VQxTr/kQYoq1Yp+68i6T4nNq7NWC+UNVjQHxNQMQMzU6l\n"\
"WCX8zyg3yH88OAQkUXIXKfQ+NkvYQ1cxaMoVPpY72+eVthKzpMeyHkBn7ciumk5q\n"\
"gLTEJAfWZpe4f4eFZj/Rc8Y8Jj2IS5kVPjUywQIDAQABAoIBADhg1u1Mv1hAAlX8\n"\
"omz1Gn2f4AAW2aos2cM5UDCNw1SYmj+9SRIkaxjRsE/C4o9sw1oxrg1/z6kajV0e\n"\
"N/t008FdlVKHXAIYWF93JMoVvIpMmT8jft6AN/y3NMpivgt2inmmEJZYNioFJKZG\n"\
"X+/vKYvsVISZm2fw8NfnKvAQK55yu+GRWBZGOeS9K+LbYvOwcrjKhHz66m4bedKd\n"\
"gVAix6NE5iwmjNXktSQlJMCjbtdNXg/xo1/G4kG2p/MO1HLcKfe1N5FgBiXj3Qjl\n"\
"vgvjJZkh1as2KTgaPOBqZaP03738VnYg23ISyvfT/teArVGtxrmFP7939EvJFKpF\n"\
"1wTxuDkCgYEA7t0DR37zt+dEJy+5vm7zSmN97VenwQJFWMiulkHGa0yU3lLasxxu\n"\
"m0oUtndIjenIvSx6t3Y+agK2F3EPbb0AZ5wZ1p1IXs4vktgeQwSSBdqcM8LZFDvZ\n"\
"uPboQnJoRdIkd62XnP5ekIEIBAfOp8v2wFpSfE7nNH2u4CpAXNSF9HsCgYEA2l8D\n"\
"JrDE5m9Kkn+J4l+AdGfeBL1igPF3DnuPoV67BpgiaAgI4h25UJzXiDKKoa706S0D\n"\
"4XB74zOLX11MaGPMIdhlG+SgeQfNoC5lE4ZWXNyESJH1SVgRGT9nBC2vtL6bxCVV\n"\
"WBkTeC5D6c/QXcai6yw6OYyNNdp0uznKURe1xvMCgYBVYYcEjWqMuAvyferFGV+5\n"\
"nWqr5gM+yJMFM2bEqupD/HHSLoeiMm2O8KIKvwSeRYzNohKTdZ7FwgZYxr8fGMoG\n"\
"PxQ1VK9DxCvZL4tRpVaU5Rmknud9hg9DQG6xIbgIDR+f79sb8QjYWmcFGc1SyWOA\n"\
"SkjlykZ2yt4xnqi3BfiD9QKBgGqLgRYXmXp1QoVIBRaWUi55nzHg1XbkWZqPXvz1\n"\
"I3uMLv1jLjJlHk3euKqTPmC05HoApKwSHeA0/gOBmg404xyAYJTDcCidTg6hlF96\n"\
"ZBja3xApZuxqM62F6dV4FQqzFX0WWhWp5n301N33r0qR6FumMKJzmVJ1TA8tmzEF\n"\
"yINRAoGBAJqioYs8rK6eXzA8ywYLjqTLu/yQSLBn/4ta36K8DyCoLNlNxSuox+A5\n"\
"w6z2vEfRVQDq4Hm4vBzjdi3QfYLNkTiTqLcvgWZ+eX44ogXtdTDO7c+GeMKWz4XX\n"\
"uJSUVL5+CVjKLjZEJ6Qc2WZLl94xSwL71E41H4YciVnSCQxVc4Jw\n"\
"-----END RSA PRIVATE KEY-----\n";



void broadcast(char* msg, pthread_t thread);

/*
 * Função reponsavel em comunicar com o cliente e executar as instruções
 */
void* functionClient(void* arg){
    Thread* self = (Thread*)arg;
    int* clientfd = (int*) self->input; /*Descritores de arquivo do cliente*/
    pthread_t uuid = self->uuid;
    char buffer[BUFFER_LENGTH] = {0};
    char name[100] = "unknown\0";
    char buffer1[BUFFER_LENGTH_AUX] = {0};
    


    /* Envia a primeira mensagem ao cilente */
    if (send(*clientfd, publicKey, strlen(publicKey), 0)) {
        fprintf(stdout, "Client connected.\nWaiting for client message ...\n");

        /* limpa o buffer */
        memset(encrypted, 0x0, ENCRYPTED_LENGTH);

        /* Comunica-se com o cliente */
        int message_len;
        while((message_len = recv(*clientfd, encrypted, ENCRYPTED_LENGTH, 0)) > 0){
            message_len = privateDecrypt(encrypted, message_len, (unsigned char *)privateKey, decrypted);
            
            if (message_len == -1){
                continue;
            }

            strcpy(buffer, (char*)decrypted);
                        
            buffer[strcspn(buffer, "\r\n")] = 0;
            printf("Client sent: %s\n", buffer);
            
            if (strcmp(name, "unknown") == 0){
                strcpy(name, buffer);
            } else{
                memset(buffer1, 0x0, BUFFER_LENGTH_AUX);
                snprintf(buffer1, BUFFER_LENGTH_AUX,"%s -> %s\n", name, buffer);
                
                
                broadcast(buffer1, uuid);
            }
            memset(buffer, 0x0, BUFFER_LENGTH);
        }

        if (message_len == -1){ /* Conexão do cliente ERROR */
            printf("Error recv in client\n");
        } else if (message_len == 0){ /* Conexão do cliente Close */
            printf("Client disconnected\n");
            close(*clientfd);
        }
        
    }

    remove_thread(self);
    return NULL;

} 

void broadcast(char* msg, pthread_t uuid){
    int size = 0;
    Thread* threads = NULL;
    threads = get_threads(&size);

    if (threads == NULL || size == 0){
        return;
    }

    int encrypted_length = privateEncrypt((unsigned char*)msg, strlen(msg), (unsigned char*)privateKey, encrypted);
    if (encrypted_length == -1){
        printf("Data encryption error\n");
        return;
    }
        
    for (int i = 0; i < size; i++){
        if (uuid != threads[i].uuid){
            send(*(int*)threads[i].input, encrypted, encrypted_length, 0);
        }
    }
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

    initial_controller_thread();
    
    while (1){
        socklen_t client_len = sizeof(client);
        int* clientfd = calloc(1, sizeof(int));
        if ((*clientfd=accept(serverfd, (struct sockaddr *) &client, &client_len )) == -1) {
            perror("Accept error:");
            continue;
        }

        Thread* p = new_thread();
        p->input = clientfd;
        create_thread(p, functionClient); 

    }
    

    /* fecha o socket local */
    close(serverfd);

    printf("Connection closed\n\n");

    return EXIT_SUCCESS;
}
