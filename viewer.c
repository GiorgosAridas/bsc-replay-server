#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
    if (argc != 3){
        perror("Give hostname and port number\n");
        return 1;
    }
    
    int port, sock;

    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*)&server;
    struct hostent *rem;
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror("Socket\n");

    if ((rem = gethostbyname(argv[1])) == NULL){
        perror("gethostbyname\n");
    }

    port = atoi(argv[2]);
    server.sin_family = AF_INET;
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(port);

    if (connect(sock, serverptr, sizeof(server)) < 0)
        perror("connect\n");
    
    char buffer[100], message2print[100];
    int bytes_read = 0;
    int buffer_len = 0;
    do{
        if ((bytes_read = read(sock, buffer + buffer_len, sizeof(buffer) - buffer_len - 1)) < 0){
            perror("problem in read from socket (viewer)\n");
            return -1;
        }
        else if (bytes_read == 0)
            break;
        
        buffer_len += bytes_read;
        buffer[buffer_len] = '\0';
        char *find_break = strchr(buffer, '\n');
        while (find_break != NULL){
            int position = find_break - buffer + 1;
            strncpy(message2print, buffer, position);
            message2print[position] = '\0';
            printf("%s", message2print);

            int remaining = buffer_len - position;
            memmove(buffer, find_break + 1, remaining);
            buffer_len = remaining;
            buffer[buffer_len] = '\0';
        }

    }while(1);
}
