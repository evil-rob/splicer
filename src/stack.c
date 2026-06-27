// stack.c

#include <stdio.h>
#include <stdbool.h>
#include "stack.h"

#define MAX_DEPTH 4

void *stack[MAX_DEPTH];
int top = -1;
enum { CLEAR, OVERFLOW, UNDERFLOW } stack_err;

void push(void *p)
{
    if ( top >= MAX_DEPTH - 1 )
    {
        stack_err = OVERFLOW;
        return;
    }
    stack[++top] = p;
}

void *pop(void)
{
    if ( stack_empty() )
    {
        stack_err = UNDERFLOW;
        return NULL;
    }
    return stack[top--];
}

char *stack_gerr(void)
{
    switch (stack_err)
    {
        default:        return "";
        case OVERFLOW:  return "Stack overflow";
        case UNDERFLOW: return "Stack underflow";
    }
}

void stack_perr(char *msg)
{
    char *error = stack_gerr();
    fprintf(stderr, "%s: %s\n", msg, error);
}

bool stack_full(void)   { return top + 1 == MAX_DEPTH; }
bool stack_empty(void)  { return top == -1; }
bool stack_ckerr(void)  { return stack_err != CLEAR; }
