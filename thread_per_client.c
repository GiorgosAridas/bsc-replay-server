#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>

void perror_exit(char *message){
    perror(message);
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[]){
    int port, sock, newsock;
    struct sockaddr_in server, client;
    socklen_t clientlen;

    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct sockaddr *clientptr = (struct sockaddr *)&client;

    struct hostent *rem;

    if (argc != 2){
        printf("Please give port number\n");
        exit(1);
    }
    
    port = atoi(argv[1]);
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror_exit("socket");
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);
    if (bind(sock, serverptr, sizeof(server)) < 0)
        perror_exit("bind");
    if (listen(sock, 5) < 0)
        perror_exit("listen");

    while(1){
        if ((newsock = accept(sock, clientptr, &clientlen)) < 0)
            perror_exit("accept");

    }
}