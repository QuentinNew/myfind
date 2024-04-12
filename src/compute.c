#include "compute.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

static int compute_file(const char *file, struct node *ast)
{
    int left = 0;

    if (ast == NULL)
        return 1;

    switch (ast->token->exp->type)
    {
    case TEST:
    case ACTION:
        return ast->token->exp->op(ast->token->arg, file);
    case OP_AND:
        left = compute_file(file, ast->left);
        if (left != 1)
            return left;
        return compute_file(file, ast->right);

    case OP_OR:
        left = compute_file(file, ast->left);
        if (left == 1)
            return 1;
        return compute_file(file, ast->right);

    case NOT:
        left = compute_file(file, ast->left);
        if (left == -1)
            return -1;
        return !left;
    default:
        return 1; // should not happend (parenthesis are removed)
    }

    return -1;
}
int compute_all(char *path, struct node *ast, struct options *options)
{
    struct namelist *nl = get_all_files(path, options);
    int res = 0;
    for (int i = 0; i < nl->size; i++)
    {
        if (compute_file(nl->names[i], ast) == -1)
        {
            res = 1;
        }
    }

    name_list_free(nl);
    free(path);
    return res;
}
