// main.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "splicer.h"

char *prog;
const char *ver = "0.1.0";

int main(int argc, char *argv[])
{
    prog = argc > 0 ? argv[0] : "main.c";

    if (argc == 1)
    {
        // No files in argument list. Call process_file() using stdin.
        process_file(NULL, NULL);
        return 0;
    }

    int *args = malloc(sizeof(int) * argc);
    if ( args == NULL )
    {
        fprintf(stderr, "%s: Error parsing arguments\n", prog);
        return 1;
    }
    parse_args(args, argc, argv);

    int status = 0; // Incremented on error.
    char *out_fn = args[0] ? argv[args[0]] : NULL;
    for (int i = 1; i < argc && args[i]; i++)
    {
        /* If "-" is supplied as an argument, that is a signal that input
         * is stdin, so initialize *in_fn to NULL. Otherwise initialize
         * *in_fn to argv[args[i]].
         * */
        char *in_fn = strcmp(argv[args[i]],"-") ? argv[args[i]] : NULL;

        // If process_file() returns -1, increment status.
        status += process_file(in_fn, out_fn) < 0 ? 1 : 0;
    }

    free(args);
    return status;
}

void parse_args(int *args, int argc, char **argv)
{
    int status = EXIT_SUCCESS;
    int *p = memset(args, 0, sizeof(int) * argc);
    p++;
    for (int i = 1; i < argc; i++)
    {
        if ( strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 )
            goto cleanup_and_usage;
        if ( strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0 )
            goto cleanup_and_ver;
        if ( strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0 )
        {
            // Look ahead to see if an argument for "-o" was given. If missing
            if ( i > argc - 1 )
            {
                // No outfile was given after "-o".
                fprintf(stderr, "%s: Missing argument for %s\n", prog, argv[i]);
                status = EXIT_FAILURE;
                goto cleanup_and_usage;
            }
            *args = ++i; // Store index of outfile
        }
        else
            *p++ = i; // Store index of infile
    }
    return;

cleanup_and_ver:
    printf("%s v%s\n", prog, ver);
    goto cleanup;
cleanup_and_usage:
    fprintf(stderr, "Usage: %s [-o|--output <file>] [file...]\n"
                    "       %s -h|--help\n"
                    "       %s -v|--version\n",
                    prog, prog, prog
            );
cleanup:
    free(args);
    exit(status);
}
