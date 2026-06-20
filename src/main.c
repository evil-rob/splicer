#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

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
    FILE *fp = fopen(fn, "r");

    if (fp == NULL)
    {
        // Error opening file.
        perror(fn);
        goto cleanup_none;
    }

    // Test if reading the file causes an error. On Linux, an error
    // indicates we possibly called fopen() on a directory.
    int c = fgetc(fp);
    if ( c == EOF && ferror(fp) )
    {
        fprintf(stderr, "%s: %s: Unexpected EOF\n", prog, fn);
        goto cleanup_file;
    }

    // We successfully read a character from the stream.
    // Put it back on the stream and return successfully.
    ungetc(c, fp);
    goto cleanup_none;

cleanup_file:
    fclose(fp);
    fp = NULL;
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
