#include "ast.h"

#include <stddef.h>
#include <stdlib.h>

#include "error.h"
#include "utils/stack.h"

static struct node *create_node(struct token *t, struct node *l, struct node *r)
{
    struct node *node = malloc(sizeof(struct node));
    node->token = t;
    node->left = l;
    node->right = r;
    return node;
}

static void unstack(struct stack **operators, struct stack **operands)
{
    struct node *op = NULL;
    struct node *l = NULL;
    struct node *r = NULL;

    *operators = stack_pop(*operators, &op);
    *operands = stack_pop(*operands, &r);
    if (op->token->exp->type != NOT)
    {
        *operands = stack_pop(*operands, &l);
    }
    else
    {
        l = r;
        r = NULL;
    }

    if (is_operator(op->token) && op->token->exp->type != NOT)
    {
        if (l == NULL && r == NULL)
        {
            error_exit("invalid expressions; you have used a binary operator "
                       "with nothing before or after it.");
        }
    }
    op->left = l;
    op->right = r;
    *operands = stack_push(*operands, op);
}

static void add_to_operator(struct token *t, struct stack **operators,
                            struct stack **operands)
{
    while (*operators != NULL)
    {
        enum my_type type = stack_peek(*operators)->token->exp->type;

        if ((type == OP_AND && t->exp->type == OP_OR) || type == LEFT_PARENT
            || (t->exp->type == NOT && (type == OP_AND || type == OP_OR)))
            break;

        unstack(operators, operands);
    }
    *operators = stack_push(*operators, create_node(t, NULL, NULL));
}

static void handle_parenthesis(struct stack **operators,
                               struct stack **operands)
{
    while (*operators != NULL)
    {
        if (stack_peek(*operators)->token->exp->type == LEFT_PARENT)
            break;

        unstack(operators, operands);
    }
    if (*operators == NULL)
    {
        error_exit("parenthesis unmatched");
    }
    struct node *tmp = NULL;
    *operators = stack_pop(*operators, &tmp);
    free_token(tmp->token); // free the left parenthesis
    free(tmp);
}

static void build(struct token *t, struct stack **operators,
                  struct stack **operands)
{
    if (t->exp->type == LEFT_PARENT)
    {
        *operators = stack_push(*operators, create_node(t, NULL, NULL));
    }
    else if (t->exp->type == RIGHT_PARENT)
    {
        handle_parenthesis(operators, operands);
        free_token(t);
    }
    else if (is_operator(t))
    {
        add_to_operator(t, operators, operands);
    }
    else
    {
        *operands = stack_push(*operands, create_node(t, NULL, NULL));
    }
}

struct node *build_ast(struct token_list *tl)
{
    if (tl->size == 0)
    {
        return create_node(create_quick_token("-print"), NULL, NULL);
    }
    struct stack *operators = NULL;
    struct stack *operands = NULL;
    for (int i = 0; i < tl->size; i++)
    {
        build(tl->tokens[i], &operators, &operands);
    }
    while (operators != NULL)
    {
        unstack(&operators, &operands);
    }
    struct node *ast = NULL;
    operands = stack_pop(operands, &ast);
    if (operands != NULL)
    {
        error_exit("Operands not empty");
    }
    return ast;
}

void free_nodes(struct node *ast)
{
    if (ast == NULL)
        return;

    free_nodes(ast->left);
    free_nodes(ast->right);
    free_token(ast->token);
    free(ast); // Don't free the tokens inside
}
