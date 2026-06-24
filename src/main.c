// main.c

#include <string.h>
#include "splicer.h"

char *prog;

int main(int argc, char *argv[])
{
    prog = argc > 0 ? argv[0] : "main.c";

    if (argc == 1)
    {
        // No files in argument list. Call process_file() using stdin.
        process_file(NULL, NULL);
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

        // If process_file() returns -1, increment status.
        status = process_file(fn, NULL) < 0 ? 1 + status : status;
    }

    return status;
}
