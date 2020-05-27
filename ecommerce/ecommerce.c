#include "./ecommerce.h"

queue *jobs;
sema jobs_count;
sema mutex_queue;
sema mutex_ht;
hash_table *db;
int *connections_counter;

/**
 * This is the worker which works on the queue. Should be called in a thread.
 * @param  _arg Here we're just using 1 argument, namely the pointer to the kill_server semaphore,
 *              which we can use to kill the server. 
 * @return      NULL
 */
void *ecommerce_worker(void *_arg) {

    if(DEBUG_ECOMMERCE) printf("debug: ecommerce: worker started\n");

    // Declare here our var to be used later
    char msg_to_send_back[256];

    while(1) {
        // Go to sleep if no jobs
        sema_wait(&jobs_count);

        // This will always wait 5 seconds before executing the job
        // if SIMULATION_MUTLIACCESS is enabled.
        if(SIMULATION_MUTLIACCESS) sleep(5);

        // Dequeue element
        char job[QL], cp[QL];
        sema_wait(&mutex_queue);
        dequeue(jobs, job);
        sema_post(&mutex_queue);

        // Get CMD
        char *cmd;
        char *del = " ";
        strcpy(cp, job);
        cmd = strtok(cp, del); // strtok manipulates given input, thus make copy

        // Do now ecommerce logic here
        // -----------------------------------------
        // List of commands:
        // GET key:str (client_id:int)
        // BUY key:str amount:int (client_id:int)
        // UPD key:str amount:int (client_id:int)
        // ADD key:str amount:int (client_id:int)
        // SER KILL
        // SER PRINT_JOBS
        // SER PRINT_HT
        // -----------------------------------------


        /**
         * GET command
         * Syntax: `GET key:str (client_id:int)`
         * - client_id is appended automatically by the request handler.
         */
        if(strcmp(cmd, "GET") == 0) {

            // Get args
            char *key = strtok(NULL, del);
            char *client_id_as_str = strtok(NULL, del);
            char *null = strtok(NULL, del); // used to control the number of args

            if(key == NULL || client_id_as_str == NULL || null != NULL) {
                if(client_id_as_str != NULL) {
                    msg_to_client_argc(atoi(client_id_as_str));
                }
                continue;
            }

            // Convert strings to integers
            int client_id = atoi(client_id_as_str);


            // Get val
            int *get_val;
            size_t *get_val_len;

            sema_wait(&mutex_ht);
            get_ht(db, key, (void **)&get_val, &get_val_len);
            sema_post(&mutex_ht);

            // If val found
            if(get_val != NULL) {
                sprintf(msg_to_send_back, "Item `%s` stock: %i\n", key, *get_val);
            } else {
                sprintf(msg_to_send_back, "Item `%s` was not found in the database.\n", key);
            }

            // Send to client success or failure message
            send(client_id, msg_to_send_back, strlen(msg_to_send_back), 0);
        }

        /**
         * BUY command
         * Syntax: `BUY key:str amount:int(+) (client_id:int)`
         * - amount should be positive and bigger than 0,
         * - client_id is appended automatically by the request handler.
         */
        else if(strcmp(cmd, "BUY") == 0) {

            // Get args
            char *key = strtok(NULL, del);
            char *amount_to_buy_as_str = strtok(NULL, del);
            char *client_id_as_str = strtok(NULL, del);
            char *null = strtok(NULL, del); // used to control the number of args


            if(key == NULL || amount_to_buy_as_str == NULL || client_id_as_str == NULL || null != NULL) {
                if(client_id_as_str != NULL) {
                    msg_to_client_argc(atoi(client_id_as_str));
                }
                continue;
            }

            // Convert strings to integers
            int amount_to_buy = atoi(amount_to_buy_as_str);
            int client_id = atoi(client_id_as_str);

            // If amount_to_buy is > 0
            if(amount_to_buy > 0) {
                // Get val
                int *get_val;
                size_t *get_val_len;

                sema_wait(&mutex_ht);
                get_ht(db, key, (void **)&get_val, &get_val_len);
                sema_post(&mutex_ht);

                // If val found
                if(get_val != NULL) {
                    int new_val = *get_val - amount_to_buy;
                    if(new_val >= 0) {

                        sema_wait(&mutex_ht);
                        upd_ht(db, key, (void *) &new_val, sizeof(int));
                        sema_post(&mutex_ht);
                        sprintf(msg_to_send_back, "Item `%s` succesfully bought. Enjoy.\n", key);
                    } else {
                        sprintf(msg_to_send_back, "Item `%s` could not be bought, amount selected not available.\n", key);
                    }
                } else {
                    sprintf(msg_to_send_back, "Item `%s` could not be bought, because it was not found in the database.\n", key);
                }
            } else {
                sprintf(msg_to_send_back, "Item `%s` could not be bought, amount must be > 0.\n", key);
            }

            // Send to client success or failure message
            send(client_id, msg_to_send_back, strlen(msg_to_send_back), 0);
        }

        /**
         * UPD command
         * Syntax: `UPD key:str amount:int(+/-) (client_id:int)`
         * - amount either positive to increase or negative to decrease
         * - client_id is appended automatically by the request handler
         */
        else if(strcmp(cmd, "UPD") == 0) {

            // Get args
            char *key = strtok(NULL, del);
            char *update_as_str = strtok(NULL, del);
            char *client_id_as_str = strtok(NULL, del);
            char *null = strtok(NULL, del); // used to control the number of args

            // Check args
            if(key == NULL || update_as_str == NULL || client_id_as_str == NULL || null != NULL) {
                if(client_id_as_str != NULL) {
                    msg_to_client_argc(atoi(client_id_as_str));
                }
                continue;
            }

            // Convert strings to integers
            int update_new_amount = atoi(update_as_str);
            int client_id = atoi(client_id_as_str);

            // Get val
            int *get_val;
            size_t *get_val_len;

            sema_wait(&mutex_ht);
            get_ht(db, key, (void **)&get_val, &get_val_len);
            sema_post(&mutex_ht);

            // If val found
            if(get_val != NULL) {
                int new_val = *get_val + update_new_amount;
                if(new_val >= 0) {

                    sema_wait(&mutex_ht);
                    upd_ht(db, key, (void *) &new_val, sizeof(int));
                    sema_post(&mutex_ht);

                    sprintf(msg_to_send_back, "Item `%s` succesfully updated, new amount: %i\n", key, new_val);
                } else {
                    sprintf(msg_to_send_back, "Item `%s` could not be updated, amount selected to decrease not available.\n", key);
                }
            } else {
                sprintf(msg_to_send_back, "Item `%s` could not be updated, because it was not found in the database.\n", key);
            }

            // Send to client success or failure message
            send(client_id, msg_to_send_back, strlen(msg_to_send_back), 0);
        }

        /**
         * ADD command
         * Syntax: `ADD key:str amount:int(+) (client_id:int)`
         * - key is the item to add
         * - amount to add to shop
         * - client_id is appended automatically by the request handler
         */
        else if(strcmp(cmd, "ADD") == 0) {
            // Get args
            char *key = strtok(NULL, del);
            char *amount_to_add_as_str = strtok(NULL, del);
            char *client_id_as_str = strtok(NULL, del);
            char *null = strtok(NULL, del); // used to control the number of args


            if(key == NULL || amount_to_add_as_str == NULL || client_id_as_str == NULL || null != NULL) {
                if(client_id_as_str != NULL) {
                    msg_to_client_argc(atoi(client_id_as_str));
                }
                continue;
            }

            // Convert strings to integers
            int amount_to_add = atoi(amount_to_add_as_str);
            int client_id = atoi(client_id_as_str);

            // If amount_to_add is > 0
            if(amount_to_add > 0) {

                sema_wait(&mutex_ht);
                int add = add_ht(db, key, (void *)&amount_to_add, sizeof(int));
                sema_post(&mutex_ht);

                if(add == 0) { // success
                    sprintf(msg_to_send_back, "Item `%s` succesfully added.\n", key);
                } else { // failure
                    sprintf(msg_to_send_back, "Item `%s` could not be added. add_ht() did not succeed.\n", key);
                }
            } else {
                sprintf(msg_to_send_back, "Item `%s` could not be added, amount must be > 0.\n", key);
            }

            send(client_id, msg_to_send_back, strlen(msg_to_send_back), 0);
        }

        /**
         * SER commands
         * - `SER KILL` kills the server
         * - `SER PRINT_JOBS` prints the job list
         * - `SER PRINT_HT` prints the hash_table
         */
        else if(strcmp(cmd, "SER") == 0) {
            char *arg = strtok(NULL, del);
            if (strcmp(arg, "KILL") == 0) {
                sema_post((sema *)_arg);
            } else if(strcmp(arg, "PRINT_JOBS") == 0) {
                print_queue_b2f(jobs);
            } else if(strcmp(arg, "PRINT_HT") == 0) {
                print_ht(db);
            }
        }
    }

    return NULL;
}

/**
 * Parse the received request (buffer). If it's valid syntax,
 * then add it to jobs queue.
 * @param data        The buffer of data received, max 256 bytes. 
 * @param data_length The actual length of the data.
 * @param from        From which socket the data was received, can be 
 *                    used to send() back messages
 * @param port        From which port the data was received.
 */
void ecommerce_handle_request(char *data, int data_length, int from, char *port) {

    // Remove line break if command has been sent through terminal
    if(*(data + data_length - 1) == '\n') {
        *(data + data_length - 2) = '\0';
    }

    // data is a buffer we receive, thus let's split it
    char cp[257];
    char *line;
    char *del = "\n";

    strcpy(cp, data); // strtok manipulates given input, thus make copy

    // This is our "line"
    line = strtok(cp, del);

    // To arse line, whether in valid format
    char *pattern = "^[A-Z]{3}([ ][0-9A-Za-z_-]{1,99})*$";
    char to_add[256 + 21]; // 256 is buffer size and 21 is digit length int can take on a 64bit machine
    printf("%s\n", data);
    while(line != NULL){
        if(match(line, pattern)) {
            snprintf(to_add, 256 + 21, "%s %i", line, from);

            // Add jobs to queue
            sema_wait(&mutex_queue);
            enqueue(jobs, to_add);
            sema_post(&mutex_queue);
            sema_post(&jobs_count);

            // Debug
            if(DEBUG_ECOMMERCE) printf("debug: ecommerce: handler: enqueued from port %i from socket %s: %s\n", from, port, to_add);
        } else {
            if(DEBUG_ECOMMERCE) printf("debug: ecommerce: handler: not valid syntax, not enqueued from socket %i from port %s: %s\n", from, port, line);
        }
        
        memset(to_add, 0, sizeof(to_add));
        line = strtok(NULL, del);
    }

}

/**
 * This is our initializer, which starts queue and the hash table.
 * @param gl_kill_server         Pointer to global kill server semaphore.
 * @param gl_connections_counter Pointer to integer of global variable for 
 *                               counting the active connections.
 */
void ecommerce_initialize(sema *gl_kill_server, int *gl_connections_counter) {

    // Pass pointer to connections counter
    connections_counter = gl_connections_counter;

    // Init queue for jobs
    if(init_queue(&jobs) != 0) {
        printf("error: could not init jobs.\n");
        exit(1);
    };


    // Init semaphore for jobs count
    sema_init(&jobs_count, 0);
    sema_init(&mutex_queue, 1);
    sema_init(&mutex_ht, 1);

    // Init hash_table of size 997 (use here prime or 2^n number, because hash fn works better on these)
    if(init_ht(&db, 997, HT_VAL_PRINTABLE, sprint_val_int) == 1) {
        printf("error: could not init hash_table.\n");
        exit(1);
    }

    // Load database
    load_ht(db, SERVER_DATABASE_FILE, HT_TYPE_INT, " ");

    // Init monitor
    pthread_t thr_ecommerce_monitor;
    //pthread_create(&thr_ecommerce_monitor, NULL, ecommerce_monitor, NULL);

    // Init ecommerce_worker
    pthread_t thr_ecommerce_worker;
    if(!DISABLE_WORKER) pthread_create(&thr_ecommerce_worker, NULL, ecommerce_worker, gl_kill_server);

}

/**
 * custom_programs_terminate
 * This function is to terminate the custom overlay e.g. the ecommerce store.
 */
void ecommerce_terminate() {
    // Free queue
    free_queue(jobs);

    // Store server state
    store_ht(db, SERVER_DATABASE_FILE, " ");

    // Free hash table
    free_ht(db);
}


/**
 * To not repeatedly write the same 2 lines, this is a shortcut 
 * which sends back a "args count wrong message".
 * @param client_id The socket id to send back the message.
 */
void msg_to_client_argc(int client_id) {
    char *msg_to_send_back = "The arguments count is not right. Please look at the documentation for the commands.\n";
    send(client_id, msg_to_send_back, strlen(msg_to_send_back), 0);
}

/**
 * This is used as a monitor to check job count and clients count.
 * @param  _arg [description]
 * @return      [description]
 */
void *ecommerce_monitor(void *_arg){
    while (1) {
        printf("\rJOBS: %i \t CLIENTS: %i", jobs->count, *connections_counter);
        fflush(stdout);
        sleep(1);
    }

    return 0;
}

/*
 * Taken from this man page: https://pubs.opengroup.org/onlinepubs/009695399/functions/regcomp.html
 * Match string against the extended regular expression in
 * pattern, treating errors as no match.
 *
 * Return 1 for match, 0 for no match.
 */

int match(const char *string, char *pattern) {
    int    status;
    regex_t    re;

    if (regcomp(&re, pattern, REG_EXTENDED | REG_NOSUB) != 0) {
        return(0);      /* Report error. */
    }
    status = regexec(&re, string, (size_t) 0, NULL, 0);
    regfree(&re);
    if (status != 0) {
        return(0);      /* Report error. */
    }
    return(1);
}