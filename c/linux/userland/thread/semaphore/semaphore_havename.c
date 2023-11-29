#include <sys/semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <semaphore.h>

struct sem_lock_t {
    sem_t *semp;
    char name[256];
};

struct sem_lock_t *s_alloc()
{
    struct sem_lock_t *s = NULL;
    static int cnt = 0;
    if (!s) {
        printf("malloc failed\n");
        return NULL;
    }
    
    do {
        snprintf(s->name, sizeof(s->name), "/%ld.%d", (long)getpid(), cnt++);
        s->semp = sem_open(s->name, O_CREAT | O_EXCL, 0644, 1);
    } while (s->semp == SEM_FAILED && errno == EEXIST);
    if (s->semp == SEM_FAILED) {
        printf("sem_open failed\n");
        return NULL;
    }

    sem_unlink(s->name);
    return s;
}

void s_free(struct sem_lock_t *s)
{
    sem_close(s->semp);
    free(s);
}

int s_lock(struct sem_lock_t *s)
{
    return sem_wait(s->semp);
}

int s_trylock(struct sem_lock_t *s)
{
    return sem_trywait(s->semp);
}

int s_unlock(struct sem_lock_t *s)
{
    return sem_post(s->semp);
}