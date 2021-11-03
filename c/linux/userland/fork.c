#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char *argv[])
{
    // fork test
    pid_t pid = fork();

    if (pid == 0) {
        printf("I am child process, my pid is %d\n", getpid());
        printf("I am child process, my ppid is %d\n", getppid());
    } else if (pid > 0) {
        printf("I am parent process, my pid is %d\n", getpid());
        printf("I am parent process, my ppid is %d\n", getppid());
    } else {
        printf("fork error\n");
    }

    // wait test
    pid_t wpid = wait(NULL);
    printf("child process %d exit\n", wpid);

    return 0;
}