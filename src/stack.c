// stack.c

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "stack.h"

typedef struct stack {
    size_t size[2];
    int top;
    enum { CLEAR, OVERFLOW, UNDERFLOW } error;
    unsigned char array[];
} Stack_t;

Stack_t *stack_create(size_t size, size_t element_size)
{
    Stack_t *stack = malloc(sizeof(Stack_t) + sizeof(uint8_t) * element_size * size);
    if ( stack )
    {
        stack->size[0] = size;
        stack->size[1] = element_size;
        stack->top = -element_size;
        stack->error = CLEAR;
    }
    return stack;
}

void push(Stack_t *stack, void *p)
{
    if ( stack_full(stack) )
    {
        if ( stack )
            stack->error = OVERFLOW;
        return;
    }
    stack->top += stack->size[1];
    memcpy(stack->array + stack->top, p, stack->size[1]);
}

void *pop(Stack_t *stack)
{
    if ( stack_empty(stack) )
    {
        if ( stack )
            stack->error = UNDERFLOW;
        return NULL;
    }
    void *p = stack->array + stack->top;
    stack->top -= stack->size[1];
    return p;
}

char *stack_gerr(Stack_t *stack)
{
    switch (stack->error)
    {
        default:        return "";
        case OVERFLOW:  return "Stack overflow";
        case UNDERFLOW: return "Stack underflow";
    }
}

void stack_perr(Stack_t *stack, char *msg)
{
    char *error = stack_gerr(stack);
    fprintf(stderr, "%s: %s\n", msg, error);
}

bool stack_full(Stack_t *stack)
{
    return stack ?
        stack->top == (int)((stack->size[0] - 1) * stack->size[1]) : 1;
}
bool stack_empty(Stack_t *stack) { return stack?stack->top < 0 : 1; }
bool stack_ckerr(Stack_t *stack) { return stack?stack->error!=CLEAR:0; }
