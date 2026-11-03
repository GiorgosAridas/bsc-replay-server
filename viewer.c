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
#include "ui_colors.h"

int main(int argc, char *argv[]){
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

    if (connect(sock, serverptr, sizeof(server)) < 0)
        perror("connect\n");
    
    char buffer[2048], message2print[2048];
    int bytes_read = 0;
    int buffer_len = 0;
    do{
        int space_left = sizeof(buffer) - buffer_len - 1;
        
        if (space_left <= 0) {
            printf("Error: Buffer is full!\n");
            break;
        }

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
            //printf("%s", message2print);
            /*==============UI CODE===============*/
            char *millisecs = strtok(message2print, "\t");
            char *clock = strtok(NULL, "\t");
            char *event_type = strtok(NULL, "\t");
            char *x = strtok(NULL, "\t");
            char *y = strtok(NULL, "\t");
            char *desc = strtok(NULL, "\n"); 

            if (clock != NULL && event_type != NULL && x != NULL && y != NULL && desc != NULL) {
                // 2. Επιλέγουμε χρώμα ανάλογα με το γεγονός
                char *color = COLOR_RESET;
                if (strstr(event_type, "Pass")) color = COLOR_CYAN;
                else if (strstr(event_type, "Shot")) color = BOLD_YELLOW;
                else if (strstr(event_type, "Foul")) color = BOLD_RED;
                else if (strstr(event_type, "Tackle")) color = COLOR_BLUE;

                // 3. Τυπώνουμε μορφοποιημένα, συμπεριλαμβάνοντας τα X, Y
                printf(BOLD_WHITE "[ %s ] " COLOR_RESET, clock);
                printf("%s%-15s" COLOR_RESET, color, event_type);
                
                // Προσθήκη X, Y με %-6s για να πιάνουν σταθερό χώρο και να στοιχίζονται οι περιγραφές
                printf(" | X: %-6.1f Y: %-6.1f | %s\n", atof(x), atof(y), desc);
            }
            /*==============UI CODE===============*/

            int remaining = buffer_len - position;
            memmove(buffer, find_break + 1, remaining);
            buffer_len = remaining;
            buffer[buffer_len] = '\0';
            find_break = strchr(buffer, '\n');
        }

    }while(1);
}
