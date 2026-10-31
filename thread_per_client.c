#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include "thread_per_client.h"

MatchData *matchdata;

void perror_exit(char *message){
    perror(message);
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[]){
    int port, sock;
    struct sockaddr_in server, client;
    socklen_t clientlen;

    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct sockaddr *clientptr = (struct sockaddr *)&client;

    struct hostent *rem;

    if (argc != 3) perror_exit("Please give port number and number of clients\n");
        
    
    
    port = atoi(argv[1]);
    int N = atoi(argv[2]);
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) perror_exit("socket");
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);
    if (bind(sock, serverptr, sizeof(server)) < 0) perror_exit("bind");
    if (listen(sock, 5) < 0) perror_exit("listen");

    //Initialize shared memory
    matchdata = malloc(sizeof(MatchData));
    matchdata->event_count = 0;
    matchdata->match_finished = false;
    pthread_mutex_init(&matchdata->mutex, NULL);
    pthread_cond_init(&matchdata->new_event, NULL);

    //create a table with length N for the newsock vars and for the threads
    int *newsock = malloc(N*sizeof(int));
    pthread_t *client_threads = malloc(N*sizeof(pthread_t));

    for(int i=0; i< N; i++){
        if ((newsock[i] = accept(sock, clientptr, &clientlen)) < 0) perror_exit("accept");
        
        pthread_create(&client_threads[i], NULL, serve_client, (void *)&newsock[i]);

    }
}


void *serve_client(void* arg){
    int sock = *(int *)arg;
    int where_stopped = 0;      //points to the position of the table where the thread is left after its previous write
    while(1){
        pthread_mutex_lock(&matchdata->mutex);
        while (where_stopped == matchdata->event_count && !matchdata->match_finished){
            pthread_cond_wait(&matchdata->new_event, &matchdata->mutex);
        }

        if (matchdata->match_finished && where_stopped == matchdata->event_count){
            pthread_mutex_unlock(&matchdata->mutex);
            break;
        }

        for (int i=where_stopped; i<matchdata->event_count; i++){
            write(sock, matchdata->events[i], strlen(matchdata->events[i]));
        }
        where_stopped = matchdata->event_count;
        pthread_mutex_unlock(&matchdata->mutex);
    }

    close(sock);
    return NULL;
}