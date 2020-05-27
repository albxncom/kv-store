// libraries
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "networking.h"

// your request handler
void my_request_handler(char *data, int data_length, int from, char *port){
	// Here you can work with the received data. Store it, print it, etc
	printf("Received data (%i bytes) from socket %i from port %s: %s\n", data_length, from, port, data);
}


int main(){

	// Define your argument here
	int counter;
	char *port = "8000";
	struct connection argument;
	argument.handler = my_request_handler;
	argument.connections_counter = &counter;
	sprintf(argument.port, "%i", 8000);

	// pthread
	pthread_t thr_connections_handler;
	pthread_create(&thr_connections_handler, NULL, connections_handler, &argument);

	// wait for the thread to finish, which never happens. you should create a 
	// second thread wo wait on this is now just a dirty hack.
	pthread_join(thr_connections_handler, NULL);

	return 0;
}