#include "networking.h"

#include <errno.h>

/**
 * connections_handler
 *
 * This is a function (used as a thread) which handles the connections
 * to clients (shops, customers). You provide as an argument a pointer to a (void *) struct connection argument.
 * It will then start listening to the port defined in argument and call the handler function also defined in the argument,
 * whenever it receives new data.
 *
 * @param _arg: a pointer to a (void*) struct connection
 * @return NULL
 */
void *connections_handler(void *_arg) {

    // Extract arg
    struct connection *arg = (struct connection *)_arg;
    char *port = arg->port;
    void (*handler)() = arg->handler;

    if(DEBUG_NETWORKING) printf("server: connections_handler started at port %s\n", port);

    // Create socket.
    int status;
    struct addrinfo hints;
    struct addrinfo *result, *p; // to store the result of getaddrinfo()

    memset(&hints, 0, sizeof hints); // empty hints
    hints.ai_family = AF_UNSPEC;     // either IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // use local ip

    status = getaddrinfo(NULL, port, &hints, &result);
    if (status != 0) {
        printf("error: getaddrinfo failed on port %s: %s; thread exited.\n", port, gai_strerror(status));
        pthread_exit(NULL);
    }

    // Bind the socket
    int listener;
    for(p = result; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener == -1) {
            continue;
        }

        // Set option socket to reuse the same address for master socket
        int opt = 1;
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // Check if binding failed
    if (p == NULL) {
        printf("error: binding failed on port %s; thread exited.\n", port);
        pthread_exit(NULL);
    }

    // Free result
    freeaddrinfo(result);

    // Start listening and set incoming queue of size 10
    if (listen(listener, BACKLOG_PER_THREAD) == -1) {
        printf("error: listening failed on port %s; thread exited.\n", port);
        pthread_exit(NULL);
    }

    // Construct master set
    fd_set master_set;
    FD_ZERO(&master_set); //clear

    // Add this listener to master set.
    FD_SET(listener, &master_set);

    // Set fd_max to the latest listener
    int fd_max; // maximum file descriptor number
    fd_max = listener;


    // Finally, let's get started with looping
    fd_set read_fds;
    for(;;) {
        read_fds = master_set; // copy master set

        // Select the connections that have something to tell you (new data available)
        if (select(fd_max + 1, &read_fds, NULL, NULL, NULL) == -1) {
            printf("warning: select failed on port %s\n", port);
        } else {

            // Now read_fds is modified and we can check what's new.

            // Vars used in our loop

            int i, j; // Indexes

            // Master: Add new connections
            struct sockaddr_storage client_addr; // Client address
            socklen_t client_addr_len; // Length of client address
            char client_ip[INET6_ADDRSTRLEN]; // Client IP address in IPv4 or IPv6
            int new_fd; // Newly accept()ed socket/file descriptor

            // Clients: do work
            int read_bytes; // recv() return value, how many bytes were read
            char buffer[256]; // size of our buffer
            int test; // to test if forcefully disconnected


            // Go through the list of file descriptors
            for(i = 0; i <= fd_max; i++) {

                // Check if the file descriptor is set, i.e. new data is here
                if (FD_ISSET(i, &read_fds)) {

                    //Now, if it's the master socket, it means we have a new connection incoming
                    if (i == listener) {

                        // Accept new connection
                        client_addr_len = sizeof(client_addr);
                        new_fd = accept(listener, (struct sockaddr *)&client_addr, &client_addr_len);

                        // Check if error happened
                        if (new_fd == -1) {
                            if(DEBUG_NETWORKING) printf("warning: accept failed on port %s\n", port);
                        }
                        // Update our list
                        else {
                            FD_SET(new_fd, &master_set); // add to master_set set
                            if (new_fd > fd_max) {    // keep track of the max
                                fd_max = new_fd;
                            }

                            char *client_ip_addr = (char *)inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr), client_ip, INET6_ADDRSTRLEN);
                            if(DEBUG_NETWORKING) printf("server:%s: new connection from %s on socket %d\n", port, client_ip_addr, new_fd);

                            (*arg->connections_counter)++;
                        }
                    }

                    // If it's not from the master socket, we received data from a client
                    else {
                        read_bytes = recv(i, buffer, sizeof(buffer), 0);

                        // <= 0 bytes read iff client disconnected or error
                        if (read_bytes <= 0) {
                            if(read_bytes == -1) {
                                if(DEBUG_NETWORKING) printf("error: recv() failed on port %s\n", port);
                                if(DEBUG_NETWORKING) printf("error: %s\n", strerror(errno));
                            } else {
                                if(DEBUG_NETWORKING) printf("server:%s: socket %d disconnected\n", port, i);
                                
                            }

                            // Disconnect and remove from master set
                            close(i);
                            FD_CLR(i, &master_set);
                            (*arg->connections_counter)--;
                        }

                        // Client wants to tell us something!
                        else {

                            // Test if forcefully connected
                            test = (int) buffer[0];

                            if(test != 4) { // If it's NOT the EOT character (dec val =4), thus a "real message"
                                if(DEBUG_NETWORKING) printf("server:%s: received from socket %d (%i bytes) : %s\n", port, i, read_bytes, buffer);
                                // Now call the data handler function that was provided with the function call of connections_handler()
                                handler(buffer, read_bytes, i, port);
                            }

                            // This happens if the client forcefullys quits the process
                            // without sending a disconnect signal (it actually sends an EOT character)
                            // Catch here this bug.
                            else {
                                if(DEBUG_NETWORKING) printf("server:%s: socket %d will disconnect forcefully\n", port, i);
                            }
                        }

                        // Reset buffer and read_bytes
                        memset(&read_bytes, 0, sizeof(read_bytes));
                        memset(&buffer, 0, sizeof(buffer));
                    }
                }
            }
        }
    }

    return NULL;
}



/**
 * get_in_addr description
 * Returns pointer to IPv4 or IPv6 address.
 * Source: http://beej.us/guide/bgnet/html
 *
 * @param  sa struct sockaddr
 * @return    Pointer to IPv4 or IPv6 address
 */
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

