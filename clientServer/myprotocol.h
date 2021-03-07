#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

//[COMMANDO];[ARG1];[ARG2]...
typedef struct {
    char* command;
    char** argv;
    int argc;

} MyProtocol;



int validProtocol(MyProtocol* protocol);
MyProtocol* parseProtocol(char* protocolRaw);
char* createProtocol(char* command, char *args, ...);
void printProtocol(MyProtocol* protocol);