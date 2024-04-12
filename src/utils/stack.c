#include "stack.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct stack *stack_push(struct stack *s, struct node *node)
{
    struct stack *new = malloc(sizeof(struct stack));

    if (new == NULL)
        return s;
    if (s == NULL)
        new->next = NULL;

    new->next = s;

    new->node = node;

    return new;
}

struct stack *stack_pop(struct stack *s, struct node **t)
{
    if (s == NULL)
    {
        *t = NULL;
        return NULL;
    }

    struct stack *top = s->next;

    *t = s->node;
    s->next = NULL;
    free(s);
    return top;
}

struct node *stack_peek(struct stack *s)
{
    if (s == NULL)
        return NULL;

    return s->node;
}
