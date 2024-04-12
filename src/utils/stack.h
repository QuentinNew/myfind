#ifndef STACK_H
#define STACK_H

#include "../ast.h"

struct stack
{
    struct node *node;
    struct stack *next;
};

struct stack *stack_push(struct stack *s, struct node *node);
struct stack *stack_pop(struct stack *s, struct node **t);
struct node *stack_peek(struct stack *s);
void stack_free(struct stack *s);

#endif /* !STACK_H */
