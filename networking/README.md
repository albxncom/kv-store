# Documentation `networking`

## Usage

This is basically just one big function which handles all of the networking for you. You pass a pointer to a handler function inside of the argument, which will then be called, whenever new data arrives. The is intended to be called as a thread like this:

```c
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
```


That's it. Now you can connect to the process using the port 8000, for example using `telnet`.