#include "tokens.h"
#define _DEFAULT_SOURCE
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "error.h"

static int is_valid_perm(char *arg)
{
    if (arg[0] == '-' || arg[0] == '/')
    {
        if (strlen(arg) != 4)
            return 0;
        arg += 1;
    }

    else if (strlen(arg) != 3)
        return 0;

    for (int i = 0; i < 3; i++)
    {
        if (arg[i] < '0' || arg[i] > '7')
            return 0;
    }
    return 1;
}

static int is_valid_arg_type(char *arg)
{
    char valid[8] = { 'b', 'c', 'd', 'f', 'l', 's', 'p', '\0' };
    char found[8] = { '0', '0', '0', '0', '0', '0', '0', '\0' };
    int index = 0;
    int len = strlen(arg);
    for (int i = 0; i < len; i += 2)
    {
        if (strchr(valid, arg[i]) == NULL)
            return 0;
        if (i + 1 < len && arg[i + 1] != ',')
            return 0;
        if (strchr(found, arg[i]))
        {
            return 0;
        }
        found[index] = arg[i];
        index++;
    }
    return 1;
}

static int is_valid_arg(const struct expr_op *exp, char *arg)
{
    if (strcmp(exp->name, "-type") == 0)
    {
        return is_valid_arg_type(arg);
    }

    if (strcmp(exp->name, "-newer") == 0)
    {
        return file_exist(arg, 0);
    }

    if (strcmp(exp->name, "-perm") == 0)
    {
        return is_valid_perm(arg);
    }

    if (strcmp(exp->name, "-user") == 0)
    {
        struct passwd *p = getpwnam(arg);
        if (p == NULL)
            return 0;
        return 1;
    }

    if (strcmp(exp->name, "-group") == 0)
    {
        struct group *g = NULL;
        g = getgrnam(arg);
        if (g == NULL)
            return 0;
        return 1;
    }
    return 1;
}

struct token *create_quick_token(char *arg)
{
    const struct expr_op *e = find_expr(arg);
    return tokenize(e, NULL);
}

struct token *tokenize(const struct expr_op *exp, char *arg)
{
    if (!is_valid_arg(exp, arg))
    {
        error_exit("Argument unvalid");
    }
    struct token *t = malloc(sizeof(struct token));
    t->exp = exp;
    t->arg = arg;
    return t;
}

int is_operator(struct token *t)
{
    return (t->exp->type == OP_OR || t->exp->type == OP_AND
            || t->exp->type == NOT);
}

/* == Token list == */

struct token_list *token_list_init(void)
{
    struct token_list *tl = malloc(sizeof(struct token));
    tl->capacity = 10;
    tl->size = 0;
    tl->tokens = malloc(sizeof(struct token *) * 10);
    return tl;
}

void token_list_add(struct token_list *tl, struct token *token)
{
    if (tl->capacity < tl->size * 2)
    {
        tl->capacity *= 2;
        tl->tokens = realloc(tl->tokens, tl->capacity * sizeof(struct token *));
    }

    tl->tokens[tl->size] = token;
    tl->size++;
}

void token_list_pop(struct token_list *tl)
{
    if (tl->size == 0)
        return;

    free_token(tl->tokens[tl->size - 1]);
    tl->size -= 1;
}

void token_list_free(struct token_list *tl)
{
    for (int i = 0; i < tl->size; i++)
    {
        free_token(tl->tokens[i]);
    }
    free(tl->tokens);
    free(tl);
}

void free_token(struct token *token)
{
    if (strcmp(token->exp->name, "-exec") == 0)
        free(token->arg);
    free(token);
}
