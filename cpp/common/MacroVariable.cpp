#include <stdio.h>

#define RANDOM_VARIABLE(VAR)  random_variable_##VAR##__COUNTER__

#define PROJECT_NAME

#ifndef PROJECT_NAME
#error No defined constant Symbol "PROJECT_NAME"
#endif

int main() {

    int RANDOM_VARIABLE(test) = 5;

    printf("%d\n", RANDOM_VARIABLE(test));

    printf("%d\n", RANDOM_VARIABLE(test));

    printf("%s\n", __FILE__);
    printf("%d\n", __COUNTER__);
    printf("%d\n", __COUNTER__);
    printf("%d\n", __COUNTER__);

    printf("Hello, World!\n");
    return 0;
}