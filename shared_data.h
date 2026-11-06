#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <pthread.h>
#include <stdbool.h>

#define MAX_EVENTS 5000       
#define MAX_LINE_LEN 512      

typedef struct{
    char events[MAX_EVENTS][MAX_LINE_LEN];    //Matrix of the events of a match
    int event_count;                          //Number of haw many events we have currently
    bool match_finished;                      //true = finished | false = not finished
    pthread_mutex_t mutex;                    //locks the struct
    pthread_cond_t new_event;                 //condition variable it sends a signal every time a new event is added
} MatchData;

extern MatchData *matchdata;

#endif