// stack.c

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "stack.h"

typedef struct stack {
    size_t size;
    size_t element_size;
    int top;
    enum { CLEAR, OVERFLOW, UNDERFLOW } error;
    unsigned char data[];
} Stack_t;

Stack_t *stack_create(size_t size, size_t element_size)
{
    Stack_t *stack = malloc(sizeof(Stack_t) + element_size * size);
    if ( stack )
    {
        stack->size = size;
        stack->element_size = element_size;
        stack->top = -1;
        stack->error = CLEAR;
    }
    return stack;
}

void stack_destroy(Stack_t **stack)
{
    if ( stack == NULL ) return;
    free(*stack);
    *stack = NULL;
}

void push(Stack_t *stack, void *p)
{
    if ( stack == NULL ) return;
    if ( stack_full(stack) )
    {
        stack->error = OVERFLOW;
        return;
    }
    memcpy(&stack->data[++stack->top * stack->element_size], p, stack->element_size);
}

void *pop(Stack_t *stack)
{
    if ( stack == NULL ) return NULL;
    if ( stack_empty(stack) )
    {
        stack->error = UNDERFLOW;
        return NULL;
    }
    return &stack->data[stack->top-- * stack->element_size];
}

char *stack_gerr(Stack_t *stack)
{
    char *ret = NULL;
    if ( stack == NULL ) return ret;
    switch (stack->error)
    {
        case OVERFLOW:  ret = "Stack overflow";  break;
        case UNDERFLOW: ret = "Stack underflow"; break;
        default:        ret = "";                break;
    }
    stack->error = CLEAR;
    return ret;
}

void stack_perr(Stack_t *stack, char *msg)
{
    char *error = stack_gerr(stack);
    if ( error )
        fprintf(stderr, "%s: %s\n", msg, error);
}

bool stack_full(Stack_t *stack)
{
    return stack ? stack->top >= (int)(stack->size - 1) : true;
}
bool stack_empty(Stack_t *stack) { return stack?stack->top < 0 : true; }
bool stack_ckerr(Stack_t *stack) { return stack?stack->error!=CLEAR:false; }
