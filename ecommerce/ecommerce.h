#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "./../config.h"
#include "./../queue/queue.h" // mutex queue used for jobs storing
#include "./../hash_table/hash_table.h" // hash_table to store our data
#include "./../sema/sema.h"


void *ecommerce_worker(void *_arg);
void ecommerce_handle_request(char *data, int data_length, int from, char *port);
void ecommerce_initialize(sema *gl_kill_server, int *gl_connections_counter);
void ecommerce_terminate();
void *ecommerce_monitor(void *_arg);

// Some standard messages
void msg_to_client_argc(int client_id);


void parser_get_command(char *str, char *cmd);
int match(const char *string, char *pattern);


#define DEBUG_ECOMMERCE 0