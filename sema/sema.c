#include "./sema.h"

inline void sema_init(struct sema *s, uint32_t value) {
#ifdef __APPLE__
    dispatch_semaphore_t *sem = &s->sem;

    *sem = dispatch_semaphore_create(value);
#else
    sem_init(&s->sem, 0, value);
#endif
}

inline void sema_wait(struct sema *s) {

#ifdef __APPLE__
    dispatch_semaphore_wait(s->sem, DISPATCH_TIME_FOREVER);
#else
    int r;

    do {
        r = sem_wait(&s->sem);
    } while (r == -1 && errno == EINTR);
#endif
}

inline void sema_post(struct sema *s) {

#ifdef __APPLE__
    dispatch_semaphore_signal(s->sem);
#else
    sem_post(&s->sem);
#endif
}

inline void sema_destroy(struct sema *s) {

#ifdef __APPLE__
    // Do nothing because apple doesn't need you to destroy it, I guess?
    // I couldn't find anything in the documentation.
#else
    sem_destroy(&s->sem);
#endif
}