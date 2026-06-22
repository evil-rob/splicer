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

    int status = 0; // Incremented on error.
    for (int i = 1; i <= argc-1; i++)
    {
        /* If "-" is supplied as an argument, that is a signal that input
         * is stdin, so initialize *fn to NULL. Otherwise initialize *fn
         * to argv[i].
         * */
        char *fn = strcmp(argv[i],"-") ? argv[i] : NULL;

        // If cat() returns -1, increment status.
        status = cat(fn) < 0 ? 1 + status : status;
    }

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
 * get input from stdin instead of file. Returns the number of bytes read
 * or -1 if there was an error.
 * */
ssize_t cat(char *fn)
{
    /* input will be a stream from the file provided as fn. If fn is NULL,
     * then input will be stdin. If open_helper() returned NULL, then there
     * was a problem opening the file.
     * */
    FILE *input = fn ? open_helper(fn) : stdin;

    // If file couldn't be opened, return with failure
    if ( input == NULL ) return -1;
    
    char buffer[BUFFER_SIZE];
    size_t total = 0;

    while ( fgets(buffer, sizeof(buffer), input) )
    {
        size_t nread = strnlen(buffer, sizeof(buffer));
        total += nread;
        fputs(buffer, stdout);
    }

    if ( input != stdin ) fclose(input);
    return total;
}
