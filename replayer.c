#include <stdio.h>
#include <error.h>
#include <string.h>
#include <stdlib.h>
#include<unistd.h>

int main(int argc, char *argv[]){
    if (argc != 2){
        perror("Not enough arguments\n");
        return -1;
    }
    int speed = atoi(argv[1]);
    FILE *inputfile = fopen("event reading/results.txt", "r");
    char buffer[100];
    int previous_milsec = 0;
    int current_milsec = 0;
    int sleep_time = 0;
    if (inputfile != NULL){
        while(fgets(buffer, 100, inputfile)){
            sscanf(buffer, "%d", &current_milsec);
            sleep_time = current_milsec - previous_milsec;
            usleep(sleep_time*1000/speed);
            printf("%s", buffer);
            previous_milsec = current_milsec;
        }
        fclose(inputfile);
    }
    else{
        perror("Cant open file\n");
    }
    return 0;
}