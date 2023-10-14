#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t sem1, sem2;

void* thread1_func(void* arg) {
    for (int i = 0; i < 5; i++) {
        sem_wait(&sem1); // 降低sem1的值，如果它的值为0，则线程阻塞
        printf("Thread 1: Hello from thread 1!\n");
        sem_post(&sem2); // 增加sem2的值
    }
    return NULL;
}

void* thread2_func(void* arg) {
    for (int i = 0; i < 5; i++) {
        sem_wait(&sem2); // 降低sem2的值，如果它的值为0，则线程阻塞
        printf("Thread 2: Hello from thread 2!\n");
        sem_post(&sem1); // 增加sem1的值
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    // 初始化信号量
    sem_init(&sem1, 0, 1); // 第一个参数是信号量，第二个参数是0（线程间同步）或1（进程间同步），第三个参数是初始化值
    sem_init(&sem2, 0, 0);

    pthread_create(&thread1, NULL, thread1_func, NULL);
    pthread_create(&thread2, NULL, thread2_func, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    sem_destroy(&sem1);
    sem_destroy(&sem2);

    return 0;
}