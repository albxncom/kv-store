/**
 * Networking
 *
 * The networking functions used by the server. 
 *
 * The connection_handler(struct connection) opens a socket on 
 * port connection.port, where it accepts incoming 
 * connections and waits for data to be received from the clients. 
 * 
 * Finally, one can work with the data received from the clients by providing 
 * a pointer to a function in connection.handler with four arguments:
 * 	- char *data: pointer to the data
 * 	- int data_length: length of char *data
 * 	- int from: the file descriptor "id", i.e. which client sent the data; can be used to send back a message to the client.
 *  - char *port: the port on which the data was received
 * 
 * Essentially, connection_handler() will call your provided function e.g.
 * handle_data(char *data, int data_length, int from) in which you can handle and manipulate the data. 
 */


// LIBRARIES & MODULES
// Standard libraries
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

// Custom modules
#include "./../config.h" // Config file 


// PROTOTYPES
void *connections_handler(void *arg);
void *get_in_addr(struct sockaddr *sa);
void send_to_client(int client, char *msg);

// STRUCTS
// connection 
#pragma once
struct connection {
	char port[6];
	int *connections_counter;
	void (*handler)();
	pthread_t thread;
};

#define DEBUG_NETWORKING 0