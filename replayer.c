#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "shared_data.h"
#include "replayer.h"

void *replayer_thread_function(void *arg) {
    // --- Variable initialiazation ---
    int speed = ((ReplayerInfo *)arg)->speed;   //The speed at which we replay the match
    int previous_milsec = 0;
    int current_milsec = 0;
    int sleep_time = 0;
    int sequence_num;
    
    long long start_ns;
    long long due_time_ns;
    
    char buffer[1024];
    char new_buffer[1024];
    char *rest;
    char *target_file;
    target_file = ((ReplayerInfo *)arg)->target_file;
    
    struct timespec start_time;
    FILE *inputfile = fopen(target_file, "r");

    // --- Time initialization ---
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    start_ns = (long long)start_time.tv_sec * 1000000000LL + start_time.tv_nsec;

    
    if (inputfile != NULL) {
        
        while (fgets(buffer, sizeof(buffer), inputfile)) {      //we read from the file line by line. buffer is where stored
            
            //Compute how much time we need to wait 
            sscanf(buffer, "%d", &current_milsec);
            sleep_time = current_milsec - previous_milsec;  //The time difference between current event and previous event
            
            if (sleep_time < 0) {
                sleep_time = 0;
            }
            usleep((sleep_time * 1000) / speed);    //we wait the difference between the events divided by the speed
            
            //Change milliseconds and write the due time
            rest = strchr(buffer, '\t');
            if (rest == NULL) {
                rest = "\n";
            }
            due_time_ns = start_ns + ((long long)current_milsec * 1000000LL) / speed;
            
            // Locking the shared data in order to write 
            pthread_mutex_lock(&matchdata->mutex);
            
            sequence_num = matchdata->event_count + 1;      //The sequence number is the order in which the event came
            sprintf(new_buffer, "%d\t%lld%s", sequence_num, due_time_ns, rest);     //the new buffer is what we store. Here we build it
            strcpy(matchdata->events[matchdata->event_count], new_buffer);
            matchdata->event_count++;   //New event jusst added. We add 1 
            
            pthread_cond_broadcast(&matchdata->new_event);  //New event just added => wake up all the clients
            
            pthread_mutex_unlock(&matchdata->mutex);    //Stop using the shared memory
            
            previous_milsec = current_milsec;       //At the next iteration the current event becomes the previous one 
       }
        
        // Ending the game
        fclose(inputfile);
        
        pthread_mutex_lock(&matchdata->mutex);
        matchdata->match_finished = true;
        pthread_cond_broadcast(&matchdata->new_event);
        pthread_mutex_unlock(&matchdata->mutex);
        
    } else {
        perror("Cant open file\n");
    }

    return NULL;
}