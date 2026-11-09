                                        // FIRST SERVER WE BUILD
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ui_colors.h"
#include "shared_data.h"
#include "replayer.h"
#include "client_handler.h"

void perror_exit(char *message){
    perror(message);
    exit(EXIT_FAILURE);
}

MatchData *matchdata;
ReplayerInfo *replayerinfo;

int main(int argc, char *argv[]) {

    printf("⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
    printf("⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
    printf("⣿⡟⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⢻⣿\n");
    printf("⣿⡇⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⢸⣿\n");
    printf("⣿⡇⠸⠿⠿⠿⣿⣿⣿⣿⣿⣿⣿⣿⡇⢸⣿⣿⣿⣿⣿⣿⣿⣿⠿⠿⠿⠇⢸⣿\n");
    printf("⣿⡇⢰⣶⣶⠀⣿⣿⣿⣿⣿⣿⣿⠛⠁⠈⠛⣿⣿⣿⣿⣿⣿⣿⠀⣶⣶⡆⢸⣿\n");
    printf("⣿⡇⢸⣿⣿⠀⣿⣿⣿⣿⣿⣿⡇⠰⡇⢸⠆⢸⣿⣿⣿⣿⣿⣿⠀⣿⣿⡇⢸⣿\n");
    printf("⣿⡇⠸⠿⠿⠀⣿⣿⣿⣿⣿⣿⣿⣤⡀⢀⣤⣿⣿⣿⣿⣿⣿⣿⠀⠿⠿⠇⢸⣿\n");
    printf("⣿⡇⢰⣶⣶⣶⣿⣿⣿⣿⣿⣿⣿⣿⡇⢸⣿⣿⣿⣿⣿⣿⣿⣿⣶⣶⣶⡆⢸⣿\n");
    printf("⣿⡇⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⢸⣿\n");
    printf("⣿⣧⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣼⣿\n");
    printf("⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
    printf("⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
    printf(COLOR_YELLOW "Waiting for clients...\n" COLOR_RESET);    


    // --- Variable Initialization ---
    int port;
    int sock;
    int N;
    int opt;

    int *newsock;
    pthread_t *client_threads;
    pthread_t replayer_thread;
    
    struct sockaddr_in server;
    struct sockaddr_in client;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct sockaddr *clientptr = (struct sockaddr *)&client;
    socklen_t clientlen;

    replayerinfo = malloc(sizeof(ReplayerInfo));
    // --- Command Line Arguments ---
    while ((opt = getopt(argc, argv, "p:n:s:f:")) != -1) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                break;
            case 'n':
                N = atoi(optarg);
                break;
            case 's':
                replayerinfo->speed = atoi(optarg);
                break;
            case 'f':
                strcpy(replayerinfo->target_file, optarg);
                break;
            default:
                printf(BOLD_RED "Correct use: %s [-p port] [-n clients] [-s speed]\n" COLOR_RESET, argv[0]);
                return -1;
        }
    }
    
    if (argc != 9) {
        printf(BOLD_RED "Correct use: %s [-p port] [-n clients] [-s speed]\n" COLOR_RESET, argv[0]);
        return -1;
    }

    // --- Socket Setup ---
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror_exit("socket");
    }

    opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror_exit("setsockopt SO_REUSEADDR");
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);
    
    if (bind(sock, serverptr, sizeof(server)) < 0) {
        perror_exit("bind");
    }
    
    if (listen(sock, 5) < 0) {
        perror_exit("listen");
    }

    // --- Shared Memory Initialiazation ---
    matchdata = malloc(sizeof(MatchData));
    matchdata->event_count = 0;
    matchdata->match_finished = false;
    
    pthread_mutex_init(&matchdata->mutex, NULL);
    pthread_cond_init(&matchdata->new_event, NULL);

    // --- Clients Allocation ---
    newsock = malloc(N * sizeof(int));
    client_threads = malloc(N * sizeof(pthread_t));
    clientlen = sizeof(client);

    // --- Connect Clients ---
    for (int i = 0; i < N; i++) {
        if ((newsock[i] = accept(sock, clientptr, &clientlen)) < 0) {
            perror_exit("accept");
        }
        
        printf(COLOR_GREEN "[+] New client connected %d\n" COLOR_RESET, newsock[i]);
        pthread_create(&client_threads[i], NULL, serve_client, (void *)&newsock[i]);
    }
    
    // --- Replay Start ---
    printf(BOLD_CYAN "[*] All the clients are connected. Starting Match!\n" COLOR_RESET);
    pthread_create(&replayer_thread, NULL, replayer_thread_function, (void *)replayerinfo);
    
    // Wait until the replayer has done its job 
    pthread_join(replayer_thread, NULL);

    for (int i = 0; i < N; i++) {
        pthread_join(client_threads[i], NULL);  //Wait for the clients to replay entire match
    }

    // --- Cleanup ---
    for (int i = 0; i < N; i++) {
        close(newsock[i]);
    }

    free(newsock);
    free(client_threads);
    
    pthread_mutex_destroy(&matchdata->mutex);
    pthread_cond_destroy(&matchdata->new_event);
    free(matchdata);
    
    close(sock);

    return 0;
}