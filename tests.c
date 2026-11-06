#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <assert.h>
#include "ui_colors.h"

void run_sequencenumber_test(int socket_fd);

int main(int argc, char* argv[]){
    int port, sock;
    char *host;

    int opt;
    while ((opt = getopt(argc, argv, "h:p:")) != -1){
        switch (opt){
            case 'h':
                host = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            default:
                printf(BOLD_RED "Correct use: %s [-h hostname] [-p port]\n" COLOR_RESET, argv[0]);
                return -1;
        }
    }
    if (argc != 5){
        printf(BOLD_RED "Correct use: %s [-h hostname] [-p port]\n" COLOR_RESET, argv[0]);
        return -1;
    }


    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*)&server;
    struct hostent *rem;
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror("Socket\n");

    if ((rem = gethostbyname(host)) == NULL){
        perror("gethostbyname\n");
    }
    server.sin_family = AF_INET;
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(port);

    if (connect(sock, serverptr, sizeof(server)) < 0){
        perror("connect\n");
        return -1;
    }
 
    printf(COLOR_BLUE "Starting tests...\n" COLOR_RESET);
    
    run_sequencenumber_test(sock);
    printf(BOLD_GREEN "PASSED ALL TESTS\n" COLOR_RESET);
}

void run_sequencenumber_test(int socket_fd){
    FILE *test;
    if ((test = fdopen(socket_fd, "r")) == NULL){
        printf(COLOR_RED "problem open socket\n" COLOR_RESET);
        return;
    }

    char buffer[1024];
    int given_sequence;
    int correct_sequence = 1;
    while (fgets(buffer, sizeof(buffer), test) != NULL){
        sscanf(buffer, "%d", &given_sequence);

        if (given_sequence != correct_sequence) printf(BOLD_RED "FAILED test sequence number\n" COLOR_RESET);

        assert(given_sequence == correct_sequence);
        correct_sequence++;
    }
    fclose(test);
}