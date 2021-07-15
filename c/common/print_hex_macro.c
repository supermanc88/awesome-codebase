#include <stdio.h>
#include <stdlib.h>

#ifndef PRINT_HEX
#define PRINT_HEX(name, buffer, length)                                                                     \
    do{                                                                                                     \
        int i = 0;                                                                                          \
        FILE *fp = stderr;                                                                                  \
        fprintf(fp, "----------------------------------------------\n");                                    \
        if(name != NULL) fprintf(fp, "<%s>\n", name);                                                       \
        if(buffer != NULL && length > 0)                                                                    \
        {                                                                                                   \
            for(i = 0; i < length; i++)                                                                     \
                fprintf(fp, "0x%02hhx%s", (unsigned char)buffer[i], (i+1) % 16 != 0 ? ", " : ",\n");        \
            if(i % 16 != 0) fprintf(fp, "\n");                                                              \
        }                                                                                                   \
        fprintf(fp, "----------------------------------------------\n");                                    \
    }while(0);
#endif
