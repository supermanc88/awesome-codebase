#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 2

sem_t sem;

void *thread_func(void *arg) {
    int thread_id = *(int *)arg;
    printf("Thread %d waiting for semaphore...\n", thread_id);
    sem_wait(&sem);
    printf("Thread %d acquired semaphore!\n", thread_id);
    printf("Thread %d releasing semaphore...\n", thread_id);
    sem_post(&sem);
    printf("Thread %d released semaphore!\n", thread_id);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    sem_init(&sem, 0, 1);
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    sem_destroy(&sem);
    return 0;
}
