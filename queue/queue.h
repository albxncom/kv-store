/**
 * queue
 *
 * A queue implemented as a linked list with mutex enqueue and dequeue.
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "./../sema/sema.h" // macOS related, used for semaphores. Use sema_* and sema instead of sem_t for declaration.

#ifndef QL
#define QL 256
#endif


#define DEBUG_QUEUE 0

// STRUCTS
struct node {
    char val[QL];
    struct node *prev;
    struct node *next;
};

struct queue {
    int count;
    sema mutex;
    struct node *front;
    struct node *back;
};
typedef struct queue queue;

// PROTOTYPES
int init_queue(queue **q);
int enqueue(queue *q, char *val);
int dequeue(queue *q, char *ret);
void print_queue_b2f(queue *q);
void print_queue_f2b(queue *q);
void free_queue(queue *q);