// process_file.c

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "splicer.h"

#define BUFFER_SIZE 64

FILE * open_helper(char *);

/* Reads and prints a file line by line. If NULL is the argument provided, 
 * get input from stdin instead of file. Returns the number of bytes read
 * or -1 if there was an error.
 * */
ssize_t process_file(char *fn)
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
    // indiprocess_filees we possibly called fopen() on a directory.
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
