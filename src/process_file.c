// process_file.c

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "splicer.h"

#define BUFFER_SIZE 64

FILE * open_helper(char *);

enum states { TEXT, INCLUDE, BINARY, QUOTED, ERROR };

/* process_file():
 * Reads and writes a file line by line. If in_fn is NULL, get input from stdin
 * instead of file. If out_fn is NULL, write to stdout. Returns the number of
 * bytes read or -1 if there was an error. If open_helper() returns NULL, then
 * there was a problem opening the file.
 * */
ssize_t process_file(char *in_fn, char *out_fn)
{
    FILE *input = in_fn ? open_helper(in_fn) : stdin;
    FILE *output = out_fn ? stdout : stdout;

    // If file couldn't be opened, return with failure
    if ( input == NULL ) return -1;
    
    char buffer[BUFFER_SIZE];
    char *err_str = "";
    size_t total = 0;
    enum states state = TEXT;

    while ( state == ERROR || fgets(buffer, sizeof(buffer), input) )
    {
        /* Scan line to see if there is a directive. If there is
         * nothing to interpret, then continue processing file.
         * */

        size_t nread = strnlen(buffer, sizeof(buffer));

        /* Get the last character if the string. The buffer might contain a
         * newline character. If the newline is present, the last character
         * is at buffer[nread-2], else buffer[nread-1].
         * */
        char *end = buffer + nread - (buffer[nread-1] == '\n' ? 2 : 1);
        
        /* A valid directive must be no less than 6 characters, start with
         * "{$", and then one of the following and end with '}':
         *     I <file> - Include file
         *     B <file> - Binary file (Base64 encoded)
         *     Q <file> - Quoted printable
         *
         *  Example: {$I file.inc}
         * */

        char *str = strstr(buffer,  "{$");
        if ( nread >= 6 && str == buffer && *end == '}' )
        {
            str = strtok(str, "{$ }");
            if ( strcmp(str, "I") == 0 )
                state = INCLUDE;
            else if ( strcmp(str, "B") == 0 )
                state = BINARY;
            else if ( strcmp(str, "Q") == 0 )
                state = QUOTED;
        }

        do
        {
            switch (state)
            {
                case TEXT:
                    total += nread;
                    fputs(buffer, output);
                    break;

                case INCLUDE:
                    char *include_fn = strtok(NULL, "}\n");
                    if ( include_fn )
                        fprintf(output, "Including %s\n", include_fn);
                    else
                    {
                        state = ERROR;
                        err_str = "Bad or missing filename";
                    }
                    break;

                case BINARY:
                    fputs("Binary not implemented yet\n", stderr);
                    break;

                case QUOTED:
                    fputs("Quoted Printable not implemented yet\n", stderr);
                    break;

                case ERROR:
                    fprintf(stderr, "%s: %s: %s\n", prog, in_fn ? in_fn : "stdin", err_str);
                    err_str = "";
                    state = TEXT;
            }
        }
        while ( state == ERROR );
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
