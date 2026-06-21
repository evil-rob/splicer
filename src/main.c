#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/* Reads and prints a file line by line. If NULL is the argument provided, 
 * get input from stdin instead of file. */
ssize_t cat(char *fn)
{
    FILE *input = stdin;

    if ( fn )
    {
        // Filename string provided. Stat it to make sure it's a file.
        struct stat statbuf;
        if ( stat(fn, &statbuf) != 0 )
        {
            // Error trying to stat file.
            perror(fn);
            return -1;
        }

        //if ( (statbuf.st_mode & S_IFMT) != S_IFREG )
        if ( ! S_ISREG(statbuf.st_mode) )
        {
            // Not a regular file. Probably a directory.
            fprintf(stderr, "%s: Is not a regular file\n", fn);
            return -1;
        }

        // File is a regular file. Lets open it for reading.
        if ( (input = fopen(fn, "r")) == NULL )
        {
            // Error opening file.
            perror(fn);
            return -1;
        }
    }

    char buffer[BUFFER_SIZE];
    size_t total = 0;

    while ( fgets(buffer, sizeof(buffer), input) )
    {
        size_t nread = strnlen(buffer, sizeof(buffer));
        total += nread;
        fputs(buffer, stdout);
    }

    return total;
}
