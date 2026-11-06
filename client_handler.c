#include <unistd.h>
#include <string.h>
#include "shared_data.h"
#include "client_handler.h"

void *serve_client(void* arg){
    int sock = *(int *)arg;
    int where_stopped = 0;      //points to the position of the table where the thread is left after its previous write
    while(1){
        pthread_mutex_lock(&matchdata->mutex);   //locking the shared data to read from there
        while (where_stopped == matchdata->event_count && !matchdata->match_finished){     //when there are no new events wait
            pthread_cond_wait(&matchdata->new_event, &matchdata->mutex);
        }

        if (matchdata->match_finished && where_stopped == matchdata->event_count){        //if the match is finished and we read all the events stop
            pthread_mutex_unlock(&matchdata->mutex);
            break;
        }
        int target = matchdata->event_count;        //target points to the position of the event's matrix until we need to send the events
        pthread_mutex_unlock(&matchdata->mutex);    //Stop using the shared memory
        for (int i=where_stopped; i<target; i++){   //Send all the events
            write(sock, matchdata->events[i], strlen(matchdata->events[i]));
        }
        where_stopped = target;             //where stop points to the position we lastly stopped
    }

    close(sock);
    return NULL;
}