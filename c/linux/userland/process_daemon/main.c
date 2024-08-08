#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <dlfcn.h>
#include <sys/wait.h>


int g_signal = 0;
int g_process_num = 10;
// 保证子进程中有5个进程中获取的g_signal是0, 其余进程中获取的g_signal是1
int g_signal_0_num = 5;
int g_signal_0_count = 0;

// 创建一个数组，存储子进程的pid和g_signal
struct process_t {
    pid_t _pid;
    int _signal;
} ;

struct process_t g_process[10];

int main(int argc ,char *argv[]) {
    printf("Hello, World!\n");

    // fork 多个进程
    int i = 0;
    for (i = 0; i < g_process_num; i++) {

        if (g_signal_0_count < g_signal_0_num) {
            g_signal = 0;
            g_signal_0_count++;
        } else {
            g_signal = 1;
        }

        pid_t pid = fork();
        if (pid == 0) {
            printf("child process %d\n", i);
            // 加载动态库
            void *handle = dlopen("./lib/libtest.so", RTLD_LAZY);
            if (!handle) {
                fprintf(stderr, "%s\n", dlerror());
                exit(EXIT_FAILURE);
            }

            typedef int (*libfunc_t)();
            libfunc_t libfunc = (libfunc_t)dlsym(handle, "libfunc");
            if (!libfunc) {
                fprintf(stderr, "%s\n", dlerror());
                exit(EXIT_FAILURE);
            }
            libfunc();
            break;
        } else if (pid > 0) {
            printf("parent process %d\n", i);

            g_process[i]._pid = pid;
            g_process[i]._signal = g_signal;

        } else {
            printf("fork error\n");
        }
    }

    // daemon
    while (1) {
        int status = 0;
        pid_t pid = wait(&status);

        if (pid > 0) {

            int i = 0;
            for (i = 0; i < g_process_num; i++) {
                if (g_process[i]._pid == pid) {
                    if (g_process[i]._signal == 0) {
                        g_signal_0_count--;
                    }
                    break;
                }
            }

            printf("child process %d exit\n", pid);
            if (g_signal_0_count < g_signal_0_num) {
                g_signal = 0;
                g_signal_0_count++;
            } else {
                g_signal = 1;
            }
            pid = fork();
            if (pid == 0) {
                printf("child process\n");

                // 加载动态库
                void *handle = dlopen("./lib/libtest.so", RTLD_LAZY);
                if (!handle) {
                    fprintf(stderr, "%s\n", dlerror());
                    exit(EXIT_FAILURE);
                }

                typedef int (*libfunc_t)();
                libfunc_t libfunc = (libfunc_t)dlsym(handle, "libfunc");
                if (!libfunc) {
                    fprintf(stderr, "%s\n", dlerror());
                    exit(EXIT_FAILURE);
                }
                libfunc();
                break;
            } else if (pid > 0) {
                printf("parent process\n");

                g_process[i]._pid = pid;
                g_process[i]._signal = g_signal;

            } else {
                printf("fork error\n");
            }
        }
    }

    return 0;
}
