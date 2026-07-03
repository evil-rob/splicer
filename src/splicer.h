// splicer.h

#include <stdlib.h>
extern char *prog;
void print_usage(int);
void parse_args(int *, int, char **);
ssize_t process_file(char *, char *);
