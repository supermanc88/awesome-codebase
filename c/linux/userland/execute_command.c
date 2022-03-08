#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 1024

/**
 * @brief Get the command output object
 * 
 * @param command 
 * @param output 注意需要在外部free
 * @param outlen 
 * @return int 
 */
int get_command_output(char *command, char **output, int *outlen)
{
    int ret = 0;
    FILE * fp = popen(command, "r");
    *output = 0;
    if (fp == NULL) {
        printf("Failed to run command\n" );
        ret = -1;
    } else {
        *output = malloc(BLOCK_SIZE);
        if (*output == NULL) {
            printf("Failed to allocate memory\n");
            ret = -1;
        } else {
            ssize_t read_size;
            while ((read_size = fread(*output + *outlen, sizeof(char), BLOCK_SIZE, fp)) != -1) {
                *outlen += read_size;
                *output = realloc(*output, *outlen + BLOCK_SIZE);
                if (*output == NULL) {
                    printf("Failed to allocate memory\n");
                    ret = -1;
                    break;
                }
            }

            pclose(fp);
        }
    }
    return ret;
}


/**
 * @brief Get the command ret object
 * 
 * @param comand 
 * @return int 
 */
int get_command_ret(char *comand)
{
    int ret = 0;

    ret = system(comand);

    if (ret == -1) {
        printf("Failed to run command\n" );
        ret = -1;
    } else {
        if (WIFEXITED(ret)) {
            printf("command terminated normally, exit status = %d\n", WEXITSTATUS(ret));
            ret = WEXITSTATUS(ret);
        } else {
            printf("command terminated abnormally\n");
            ret = -1;
        }
    }
    return ret;
}