// stack.h

#include <stdbool.h>

struct stack;
typedef struct stack Stack_t;

/* stack_create(): Creates a stack on the heap and returns a pointer to
 * a Stack_t. Memory used by Stack_t structure should be freed before
 * program exits. The first argument is the number of elements to allocate.
 * The second argument is the size of each element.
 */
Stack_t *stack_create(size_t, size_t);

/* stack_destroy(): Deallocates memory used by the stack. Takes a pointer to
 * a pointer to a Stack_t. After calling free, *stack is set to NULL.
 */
void stack_destroy(Stack_t **);

/* push(): takes a pointer to a Stack_t and
 * a pointer to the data to push onto the stack.
 * Does not return anything.
 */
void push(Stack_t *, void *);

/* pop(): takes a pointer to a Stack_t. Returns a pointer to the array element.
 * The pointer should be cast to a pointer of whatever type the array element
 * is. For example: If the element is a pointer to a struct, then cast to a
 * pointer to a pointer to a struct. If the stack is empty when pop() is called,
 * NULL is returned and error condition for UNDERFLOW is set.
 */
void *pop(Stack_t *);

/* stack_full(): checks to see if the stack is full */
bool stack_full(Stack_t *);

/* stack_empty: checks to see if the stack is empty */
bool stack_empty(Stack_t *);

/* stack_ckerr(): checks if there is an error condition set */
bool stack_ckerr(Stack_t *);

/* stack_gerr(): returns a cstring indicating an error condition.
 * If no error is set, then an empty string is returned. The error
 * is cleared before returning.
 */
char *stack_gerr(Stack_t *);

/* stack_perr(): prints the cstring argument and the current error condition */
void stack_perr(Stack_t *, char *);
