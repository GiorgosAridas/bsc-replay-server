# Project Status

## Milestone 1: Completed
* Data Parser: Developed python script to parse the information from the json files
* Replayer: Created a C program to replay the events of a match at a given speed
* MakeFile: MakeFile created for easier build
* Documentation: Added a short description of the first milestone in thesis

## Current State
* Milestone 1 fully completed
* A bit of concern for matches that go into extra time

## Next Steps
* Starting Milestone 1

## Milestone 2: Completed
* Thread_per_client.c: Developed the first server that opens a new thread for every new client
* viewer.c: The client process. Connects with the server through TCP sockets and takes the events of a  match
* replayer.c: Change the non thread function that was previously implemented. The new functions sends the events to the shared memory
* client_handler.c: The client threads. Every time a new event is uploaded by the replayer the thread wakes up and sends the event to the client through the socket

## Current State
* Milestone 2 fully implemented
* One match has a bug that needs fixing

## Next Steps
* Starting Milestone 3
