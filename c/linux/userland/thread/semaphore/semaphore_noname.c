#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <semaphore.h>

struct sem_lock_t {
    sem_t semp;
};

struct sem_lock_t *s_alloc()
{
    struct sem_lock_t *s = malloc(sizeof(struct sem_lock_t));
    if (!s) {
        printf("malloc failed\n");
        return NULL;
    }
    
    if (sem_init(&s->semp, 0, 1) < 0) {
        printf("sem_init failed\n");
        return NULL;
    }
    return s;
}

void s_free(struct sem_lock_t *s)
{
    sem_destroy(&s->semp);
    free(s);
}

int s_lock(struct sem_lock_t *s)
{
    return sem_wait(&s->semp);
}

int s_trylock(struct sem_lock_t *s)
{
    return sem_trywait(&s->semp);
}

int s_unlock(struct sem_lock_t *s)
{
    return sem_post(&s->semp);
}
