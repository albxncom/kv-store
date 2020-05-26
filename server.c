/**
 * Server
 *
 * The main() function opens the ports defined in config.h and starts listening on them.
 * As soon as data to handle is received, the ports pass the data to the function handle_request()
 * where the received message is treated. We first parse it and then extract instructions to do
 * (i.e. updating database, etc.).
 */


// LIBRARIES & MODULES
// Standard libraries
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Custom modules
#include "./config.h" // config file 
#include "./networking/networking.h" // used for handling the connections, in particular connections_handler()
#include "./sema/sema.h" // macOS related, used for semaphores. Use sema_* and sema instead of sem_t for declaration.
#include "./ecommerce/ecommerce.h" // the ecommerce overlay on the server-client network 

// PROTOTYPES
void custom_programs_initialize();
void custom_programs_terminate();
void networking(struct connection *network);
void init_server_killer();
void *kill_server();
void wait_on_server_killer(pthread_t *killer);
void handle_request(char *data, int data_length, int from, char *port);

#define DEBUG_SERVER 1

// GLOBAL VARIABLES
sema GLOBAL_kill_server;
int GLOBAL_connections_counter;

int main(int argc, char const *argv[]) {

    // This will prevent our server to crash if a pipe has been closed and we're reading from it
    signal(SIGPIPE, SIG_IGN);

    // Start the server killer
    pthread_t thr_kill_server;
    init_server_killer(&thr_kill_server);

    // Start running the custom programs (e.g. ecommerce)
    custom_programs_initialize();

    // Start the networking, free later, before shut down!
    struct connection *connections;
    networking(connections);

    // Block main function, until server is killed.
    wait_on_server_killer(&thr_kill_server);


    // Terminate custom programs (e.g. ecommerce)
    custom_programs_terminate();

    // Free before shutting down.
    free(connections);

    return 0;
}


/**
 * handle_request
 * This function is always called when new data was received from a client.
 * The data we get from the client is stored in the data argument, which one can
 * parse and process further.
 *
 * @param data        Pointer to received data.
 * @param data_length Length of the received data.
 * @param from        Received from socket number.
 * @param port        Received from port number.
 */
void handle_request(char *data, int data_length, int from, char *port) {
    ecommerce_handle_request(data, data_length, from, port);
}

/**
 * custom_programs_initialize
 * This function is to run the custom overlay e.g. the ecommerce store.
 */
void custom_programs_initialize() {
    ecommerce_initialize(&GLOBAL_kill_server, &GLOBAL_connections_counter);
}

/**
 * custom_programs_terminate
 * This function is to terminate the custom overlay e.g. the ecommerce store.
 */
void custom_programs_terminate() {
    ecommerce_terminate();
}

/**
 * kill_server
 *
 * This is a thread that basically controls the termination of the server. If this thread is killed,
 * the server's main() will terminate. Thus here one could save states or could do some cleaning up
 * before shutting down the server.
 *
 * @return NULL
 */
void *kill_server() {

    // Try to down the semaphore
    sema_wait(&GLOBAL_kill_server);

    // Do some cleaning, i.e. disconnecting from the clients, etc.
    // TO DO

    if(DEBUG_SERVER) printf("server: killed\n");

    return NULL;
}



/**
 * ------------------------------ V ADVANCED SERVER CUSTOMIZATION V ------------------------------
 */



/**
 * networking
 *
 * This function starts the networking of the server. Opens multiple ports and starts listening on sockets.
 */

void networking(struct connection *network) {

    // Malloc data
    struct connection *connections = (struct connection *) calloc(PORTS, sizeof(struct connection));

    // Init the connection handlers
    // This opens for every port defined in config.h a new socket to listen to.
    // Data received from clients is then passed to handle_request().
    pthread_t thr_connections_handler[PORTS];
    //struct connection connections[PORTS];

    GLOBAL_connections_counter = 0;
    int i;
    for (i = 0; i < PORTS+1; ++i) {
        // For every port
        sprintf((connections + i)->port, "%i", PORTS_START + i);
        (connections + i)->handler = handle_request;
        (connections + i)->connections_counter = &GLOBAL_connections_counter;
        (connections + i)->thread = thr_connections_handler[i];
        // Run a thread
        pthread_create(&thr_connections_handler[i], NULL, connections_handler, &connections[i]);
    }

    network = connections;
}



/**
 * server_killer
 *
 * This function kills starts a thread which blocks the main function.
 */

void init_server_killer(pthread_t *thr_kill_server) {
    // Init thread who kills the server.
    // If anything ups the GLOBAL_kill_server variable, the main() function will terminate
    // and the server shuts down.
    sema_init(&GLOBAL_kill_server, 0);
    pthread_create(thr_kill_server, NULL, kill_server, NULL);
}


void wait_on_server_killer(pthread_t *killer) {
    pthread_join(*killer, NULL);
}
