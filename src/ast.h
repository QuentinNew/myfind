#ifndef AST_H
#define AST_H

#include "tokens.h"

struct node
{
    struct token *token;
    struct node *left;
    struct node *right;
};

struct node *build_ast(struct token_list *tl);

void free_nodes(struct node *ast);

#endif /* !AST_H */
