#include "queue.h"


/**
 * This will malloc() space to the passed queue pointer of a
 * pointer and init mutex semaphore.
 * @param  q Pointer to a pointer to a queue.
 * @return   C-Logic: 1 on failure, 0 on success.
 */
int init_queue(queue **q) {

    // Try to malloc
    queue *_q = (queue *) malloc(sizeof(queue));

    // Error
    if(_q == NULL){
        return 1;
    }

    // Successfully malloc()ed. Feed it.
    _q->count = 0;
    _q->front = NULL;
    _q->back = NULL;

    // Init semaphore.
    sema_init(&_q->mutex, 1);

    // Update passed pointer.
    *q = _q;

    return 0;
}

/**
 * Adds a new node with given val to the queue q.
 * @param q     Pointer to a queue.
 * @param val   Pointer to char, essentially the value to be 
 *              stored inside the node.
 */
int enqueue(queue *q, char *val) {

    // Create new Node
    struct node *new_node;
    new_node = (struct node *) malloc(sizeof(struct node));

    if(new_node == NULL){
        return 1;
    }

    // Set members
    strcpy(new_node->val, val);
    new_node->next = NULL;

    // Now we need the mutex
    sema_wait(&q->mutex);
    new_node->prev = q->back;

    // If queue is not empty, update the next property of the 
    // node at the back to point to the newly created node.
    bool empty = q->count == 0;

    if(!empty) {
        q->back->next = new_node;
    }

    // Update the back
    q->back = new_node;

    // If queue is empty, set front to point to back of queue.
    if(empty) {
        q->front = q->back;
    }

    // Increase count
    q->count++;

    sema_post(&q->mutex);

    return 0;
}

/**
 * Remove an element from the front (oldest).
 * Puts (strcpy) the value of the element to the provided
 * ret pointer.
 * @param q   queue
 * @param ret pointer to store the value or can be NULL,
 *            if no return value is needed.
 */
int dequeue(queue *q, char *ret) {

    if(q->count > 0) {
        
        sema_wait(&q->mutex);

        struct node *tmp = q->front;

        if(ret != NULL) {
            strcpy(ret, tmp->val);
        }

        if(q->count == 1) {
            q->front = NULL;
            q->back = NULL;
        } else {
            q->front = q->front->next;
            q->front->prev = NULL;
        }

        q->count--;

        sema_post(&q->mutex);

        free(tmp);

        return 0;
    }

    else {
        if(DEBUG_QUEUE) printf("WARNING: Queue is empty. Can't dequeue.\n");
        return 1;
    }
}

/**
 * Prints the list of the given queue q.
 * @param q Pointer to the queue to be printed back to front.
 */
void print_queue_b2f(queue *q) {
    printf("-------- PRINT QUEUE -------\n");
    struct node *n;
    for (n = q->back; n != NULL; n = n->prev) {
        printf("%s\n", n->val);
    }
    printf("----------------------------\n");
}


/**
 * Prints the list of the given queue q.
 * @param q Pointer to the queue to be printed front to back.
 */
void print_queue_f2b(queue *q) {
    printf("-------- PRINT QUEUE -------\n");
    struct node *n;
    for (n = q->front; n != NULL; n = n->next) {
        printf("%s\n", n->val);
    }
    printf("----------------------------\n");
}

/**
 * Since we're malloc()ing space, we also need to free it. 
 * Also, mutex semaphore is destroyed.
 * @param q Pointer to the queue to be freed.
 */
void free_queue(queue *q) {
    sema_wait(&q->mutex);
    struct node *n;
    for (n = q->back; n != NULL; n = n->prev) {
        free(n);
    }

    sema_post(&q->mutex);

    sema_destroy(&q->mutex);
    
    free(q);
}

int test() {

    queue *q;

    init_queue(&q);

    enqueue(q, "hello1");
    enqueue(q, "hello2");
    enqueue(q, "hello3");
    enqueue(q, "hello4");


    dequeue(q, NULL);
    dequeue(q, NULL);

    enqueue(q, "hello5");

    print_queue_b2f(q);


    dequeue(q, NULL);
    dequeue(q, NULL);

    dequeue(q, NULL);

    dequeue(q, NULL);

    enqueue(q, "hello6");
    enqueue(q, "hello7");
    enqueue(q, "hello8");
    enqueue(q, "hello9");


    print_queue_b2f(q);
    print_queue_f2b(q);


    free_queue(q);
    return 0;
}
