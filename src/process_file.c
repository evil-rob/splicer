// process_file.c

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "splicer.h"
#include "quoted_printable.h"
#include "stack.h"

#define BUFFER_SIZE 512
#define MAX_DEPTH 4

FILE * open_helper(char *, char *);

enum states { TEXT, PARSE, INCLUDE, BINARY, QUOTED, REJECT, ERROR };

/* process_file():
 * Reads and writes a file line by line. If in_fn is NULL, get input from stdin
 * instead of file. If out_fn is NULL, write to stdout. Returns the number of
 * bytes read or -1 if there was an error. If open_helper() returns NULL, then
 * there was a problem opening the file.
 * */
ssize_t process_file(char *in_fn, char *out_fn)
{
    FILE *input = in_fn ? open_helper(in_fn, "r") : stdin;
    FILE *output = out_fn ? open_helper(out_fn, "a") : stdout;

    // If file couldn't be opened, return with failure
    if ( input == NULL || output == NULL ) return -1;
    
    char buffer[BUFFER_SIZE * 2];
    char *err_str = "";
    size_t total = 0;
    enum states state = TEXT;

    Stack_t *fstack = stack_create(MAX_DEPTH, sizeof(FILE *));
    do {
        FILE* (*file_handler)(char *fn, char *mode) = NULL;
        while ( fgets(buffer, sizeof(buffer)/2, input) )
        {
            /* Scan line to see if there is a directive. If there is
             * nothing to interpret, then continue processing file.
             * */

            size_t nread = strnlen(buffer, sizeof(buffer)/2);

            /* Get the last character if the string. The buffer might contain a
             * newline character. If the newline is present, the last character
             * is at buffer[nread-2], else buffer[nread-1]. If the buffer only
             * contains a single character (nl or other), end will point to the
             * first and only character.
             * */
            char *end = buffer;
            if ( nread > 1 )
                end = buffer + nread - (buffer[nread-1] == '\n' ? 2 : 1);
            
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
                state = PARSE;
            
            do {
                switch (state)
                {
                    default:
                        state = TEXT;
                        total += nread;
                        fputs(buffer, output);
                        break;

                    case PARSE:
                        // Make copy of string at &buffer[BUFFER_SIZE]
                        strcpy(buffer + BUFFER_SIZE, buffer);
                        /* *end points to the last character in the buffer
                         * preceeding the newline (if present). Use end to
                         * calculate the offset into copy to remove the nl.
                         * */
                        end[BUFFER_SIZE] = '\0';
                        char* token = strtok(buffer + BUFFER_SIZE, "{$ }");
                        if ( strcmp(token, "I") == 0 )
                        {
                            state = INCLUDE;
                            file_handler = open_helper;
                            break;
                        }
                        else if ( strcmp(token, "B") == 0 )
                            state = BINARY;
                        else if ( strcmp(token, "Q") == 0 )
                        {
                            state = QUOTED;
                            file_handler = quoted_filter;
                        }
                        else
                            state = REJECT;
                        break;

                    case INCLUDE:
                        char *include_fn = strtok(NULL, "}");
                        if ( include_fn == NULL )
                        {
                            err_str = "Bad or missing filename";
                            goto INCLUDE_err_and_break;
                        }
                        if ( stack_full(fstack) )
                        {
                            err_str = "Max include depth exceeded";
                            goto INCLUDE_err_and_break;
                        }
                        /* Save current input stream on the stack. Attempt to
                         * open include_fn. If attempt fails, pop the previous
                         * stream off if the stack. No need to change to an
                         * error state since open_helper() handles the failure.
                         * */
                        push(fstack, &input);
                        input = file_handler(include_fn, "r");
                        if ( input == NULL ) input = *(FILE **)pop(fstack);
                        state = TEXT;
                        goto INCLUDE_next;
INCLUDE_err_and_break:  state = ERROR;
INCLUDE_next:           break;

                    case QUOTED:
                        char *source_fn = strtok(NULL, "}");
                        if ( source_fn == NULL )
                        {
                            err_str = "Bad or missing filename";
                            goto QUOTED_err_and_break;
                        }
                        if ( stack_full(fstack) )
                        {
                            err_str = "Max include depth exceeded";
                            goto QUOTED_err_and_break;
                        }
                        push(fstack, &input);
                        input = file_handler(source_fn, "r");
                        if ( input == NULL ) input = *(FILE **)pop(fstack);
                        state = TEXT;
                        goto QUOTED_next;
QUOTED_err_and_break:   state = ERROR;
QUOTED_next:            break;

                    case BINARY: // TODO
                        err_str = "Feature not implemented yet";
                        state = ERROR;
                        break;

                    case ERROR:
                        fprintf(stderr, "%s: %s: %s\n",
                                prog, in_fn ? in_fn : "stdin",
                                err_str);
                        err_str = "";
                        state = TEXT;
                }
            } while ( state != TEXT );
        } // fgets loop
        if ( ferror(input) )
            perror(prog);
        if ( input != stdin )
            fclose(input);
        // pop() returns NULL when no more files to process
        input = stack_empty(fstack) ? NULL : *(FILE **)pop(fstack);
    } while ( input );
    if ( fstack )
        stack_destroy(&fstack);
    if ( output != stdout )
        fclose(output);

    return total;
}

FILE * open_helper(char *fn, char *mode)
{
    FILE *fp = fopen(fn, mode);

    if (fp == NULL)
    {
        // Error opening file.
        perror(fn);
        goto cleanup_none;
    }

    if ( strcmp(mode, "r") == 0 )
    {
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
    }
    goto cleanup_none;

cleanup_file:
    fclose(fp);
    fp = NULL;
cleanup_none:
    return fp;
}
