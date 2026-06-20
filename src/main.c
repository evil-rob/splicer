#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 64

char *prog;

ssize_t cat(char *);
int main(int argc, char *argv[])
{
    prog = argc > 0 ? argv[0] : "main.c";

    if (argc == 1)
    {
        // No files in argument list. Call cat() using stdin.
        cat(NULL);
        return 0;
    }

    int status = 0;
    for (int i = 1; i <= argc-1; i++)
        status = cat(argv[i]) < 0 ? 1 + status : status;

    return status;
}

FILE * open_helper(char *fn)
{
    FILE *fp = NULL;

    if ( (fp = fopen(fn, "r")) == NULL ) goto cleanup_none; // Error opening file.

    // Stat file to make sure it's a regular file.
    int fd = fileno(fp);
    struct stat statbuf;
    if ( fstat(fd, &statbuf) != 0 ) goto cleanup_file; // Error trying to stat file.

    if ( ! S_ISREG(statbuf.st_mode) )
    {
        // Not a regular file. Probably a directory.
        fprintf(stderr, "%s: Is not a regular file\n", fn);
        goto cleanup_none;
    }

    goto cleanup_none;

cleanup_file:
    fclose(fp);
cleanup_none:
    return fp;
}

/* Reads and prints a file line by line. If NULL is the argument provided, 
 * get input from stdin instead of file. */
ssize_t cat(char *fn)
{
    FILE *input = stdin;
    bool is_file = false;
    char buffer[BUFFER_SIZE];
    size_t total = 0;

    if ( fn && strcmp(fn, "-") )
    {
        // If file couldn't be opened, return with failure
        if ( (input = open_helper(fn)) == NULL ) return -1;

        is_file = true;
    }

    while ( fgets(buffer, sizeof(buffer), input) )
    {
        size_t nread = strnlen(buffer, sizeof(buffer));
        total += nread;
        fputs(buffer, stdout);
    }

    if (is_file) fclose(input);
    return total;
}
