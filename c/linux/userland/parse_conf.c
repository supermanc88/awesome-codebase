#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// parse config file
int parse_conf(char *conf_file, char *key, char *value)
{
    // open config file
    FILE *fp = fopen(conf_file, "r");
    if (fp == NULL) {
        fprintf(stderr, "open config file failed\n");
        return -1;
    }

    // read config file line by line
    char line[1024];
    while (fgets(line, sizeof(line), fp) != NULL) {
        // skip comments
        if (line[0] == '#') {
            continue;
        }

        // parse line
        char *p = line;
        while (*p != '\0' && *p != '=') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }

        // get key
        char *q = line;
        while (q < p) {
            *key++ = *q++;
        }
        *key = '\0';

        // get value
        p++;
        while (*p != '\0') {
            *value++ = *p++;
        }
        *value = '\0';
    }

    // close config file
    fclose(fp);

    // success
    return 0;
}

// main
int main(int argc, char *argv[])
{
    // check args
    if (argc != 2) {
        fprintf(stderr, "Usage: %s config_file\n", argv[0]);
        return -1;
    }

    // parse config file
    char key[1024];
    char value[1024];
    if (parse_conf(argv[1], key, value) != 0) {
        fprintf(stderr, "parse config file failed\n");
        return -1;
    }

    // print key and value
    printf("key: %s, value: %s\n", key, value);
}