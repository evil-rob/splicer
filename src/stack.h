// stack.h

void push(void *);
void *pop(void);
bool stack_full(void);
bool stack_empty(void);
bool stack_ckerr(void);
char *stack_gerr(void);
void stack_perr(char *);
