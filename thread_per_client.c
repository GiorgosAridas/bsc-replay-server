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
#include "ui_colors.h"

MatchData *matchdata;
void *serve_client(void* arg);
void *replayer_thread_function(void* arg);

void perror_exit(char *message){
    perror(message);
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[]){
    int port, sock, N, speed;
    struct sockaddr_in server, client;
    socklen_t clientlen;

    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct sockaddr *clientptr = (struct sockaddr *)&client;

    struct hostent *rem;
        
    int opt;
    while ((opt = getopt(argc, argv, "p:n:s:")) != -1){
        switch (opt){
            case 'p':
                port = atoi(optarg);
                break;
            case 'n':
                N = atoi(optarg);
                break;
            case 's':
                speed = atoi(optarg);
                break;
            default:
                printf(BOLD_RED "Correct use: %s [-p port] [-n clients] [-s speed]\n" COLOR_RESET, argv[0]);
                return -1;
        }
    }
    if (argc != 7){
        printf(BOLD_RED "Correct use: %s [-h hostname] [-p port]\n" COLOR_RESET, argv[0]);
        return -1;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) perror_exit("socket");

    opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror_exit("setsockopt SO_REUSEADDR");
    }

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
    clientlen = sizeof(client);

    for(int i=0; i< N; i++){
        if ((newsock[i] = accept(sock, clientptr, &clientlen)) < 0) perror_exit("accept");
        printf(COLOR_GREEN "[+] New client connected %d\n", newsock[i]);
        pthread_create(&client_threads[i], NULL, serve_client, (void *)&newsock[i]);
    }
    pthread_t replayer_thread;
    printf(BOLD_CYAN "[*] All the clents are connected. Starting Match!\n");
    pthread_create(&replayer_thread, NULL, replayer_thread_function, (void *)&speed);
    pthread_join(replayer_thread, NULL);

    for(int i=0; i< N; i++){
        pthread_join(client_threads[i], NULL);
    }

    free(newsock);
    free(client_threads);
    pthread_mutex_destroy(&matchdata->mutex);
    pthread_cond_destroy(&matchdata->new_event);
    free(matchdata);
    close(sock);

    return 0;
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

void *replayer_thread_function(void* arg){
    int speed = *(int *)arg;
    FILE *inputfile = fopen("event reading/results.txt", "r");
    char buffer[256];
    int previous_milsec = 0;
    int current_milsec = 0;
    int sleep_time = 0;
    if (inputfile != NULL){
        while(fgets(buffer, sizeof(buffer), inputfile)){
            sscanf(buffer, "%d", &current_milsec);
            sleep_time = current_milsec - previous_milsec;
            usleep(sleep_time*1000/speed);
            //printf("%s", buffer);
            pthread_mutex_lock(&matchdata->mutex);
            strcpy(matchdata->events[matchdata->event_count], buffer);
            matchdata->event_count++;
            pthread_cond_broadcast(&matchdata->new_event);
            pthread_mutex_unlock(&matchdata->mutex);
            previous_milsec = current_milsec;
        }
        fclose(inputfile);
        pthread_mutex_lock(&matchdata->mutex);
        matchdata->match_finished = true;
        pthread_cond_broadcast(&matchdata->new_event);
        pthread_mutex_unlock(&matchdata->mutex);
    }
    else{
        perror("Cant open file\n");
    }

    return NULL;
}