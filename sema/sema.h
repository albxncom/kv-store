/**
 * sema.h
 *
 * This is the header of sema.c. Since Apple doesn't really support unnamed semaphores,
 * we're going to differentiate and make different calls depending on which system this
 * program runs. This was taken from https://stackoverflow.com/questions/27736618/why-are-sem-init-sem-getvalue-sem-destroy-deprecated-on-mac-os-x-and-w/27847103#27847103
 * and slightly modified by us.
 * Basically, use semaphores just as you know but use sema_ instead of sem_ prefix.
 */

#ifdef __APPLE__
#include <dispatch/dispatch.h>
#else
#include <semaphore.h>
#endif

#pragma once
struct sema {
#ifdef __APPLE__
    dispatch_semaphore_t sem;
#else
    sem_t sem;
#endif
};

typedef struct sema sema;


// Prototypes
void sema_init(struct sema *s, uint32_t value);
void sema_wait(struct sema *s);
void sema_post(struct sema *s);
void sema_destroy(struct sema *s);