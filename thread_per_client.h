#include <pthread.h>
#include <stdbool.h>

#define MAX_EVENTS 5000       
#define MAX_LINE_LEN 256      

typedef struct{
    char events[MAX_EVENTS][MAX_LINE_LEN];
    int event_count;
    bool match_finished;
    pthread_mutex_t mutex;
    pthread_cond_t new_event;
} MatchData;