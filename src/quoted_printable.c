#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "splicer.h"
#include "quoted_printable.h"

FILE * quoted_filter(char *fn, char *mode)
{
    // Prevent integer overflow during allocation
    size_t fn_len = strlen(fn);
    if ( fn_len > SIZE_MAX - 11 )
        return NULL;

    // Allocate space for "qprint -e " filename and null char
    size_t buf_size = fn_len + 11;
    char *cmd = malloc(buf_size);
    if ( cmd == NULL )
        return NULL;
    memcpy(cmd, "qprint -e ", 10);
    memcpy(&cmd[10], fn, fn_len + 1);

    FILE *fp = popen(cmd, mode);
    if (fp == NULL)
    {
        // Error opening pipe.
        perror(cmd);
        return NULL;
    }

    return fp;
}
