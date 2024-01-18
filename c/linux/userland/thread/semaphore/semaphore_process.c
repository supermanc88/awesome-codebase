#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/mman.h>


struct sem_lock_t {
    sem_t semp;
};

struct sem_lock_t *g_sem = NULL;

int main(int argc, char *argv[])
{
    int ret = 0;

    // mmap创建共享内存
    g_sem = mmap(NULL, sizeof(struct sem_lock_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (g_sem == MAP_FAILED) {
        printf("mmap failed\n");
        return -1;
    }

    if (sem_init(&g_sem->semp, 1, 1) < 0) {
        printf("sem_init failed\n");
        return -1;
    }
    pid_t pid = fork();
    if ( pid == 0 ) {
        while ( 1 ) {
            // printf("child wait\n");
            if (sem_wait(&g_sem->semp) < 0) {
                printf("child wait failed\n");
                return -1;
            }
            printf("child wait success\n");
            sleep(1);
            // printf("child post\n");
            if (sem_post(&g_sem->semp) < 0) {
                printf("child post failed\n");
                return -1;
            }
            printf("child post success\n");
            sleep(1);
        }
        return 0;
    } else if ( pid > 0 ) {
        while ( 1 ) {
            // printf("parent wait\n");
            if (sem_wait(&g_sem->semp) < 0) {
                printf("parent wait failed\n");
                return -1;
            }
            printf("parent wait success\n");
            sleep(1);
            // printf("parent post\n");
            if (sem_post(&g_sem->semp) < 0) {
                printf("parent post failed\n");
                return -1;
            }
            printf("parent post success\n");
            sleep(1);
        }
        return 0;
    } else {
        printf("fork failed\n");
        return -1;
    }


    return ret;
}

