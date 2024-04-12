#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "options.h"
#include "tokens.h"

static char *concat(char *s1, char *s2)
{
    char *res = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(res, s1);
    strcat(res, s2);
    return res;
}

static char *get_args_exec(int argc, char **argv, int *index)
{
    if (argv[*index][0] == ';')
        return NULL;

    char *res = concat("", argv[*index]);
    *index = *index + 1;
    for (; *index < argc && argv[*index][0] != ';'; *index = *index + 1)
    {
        char *new_res = concat(res, " ");
        free(res);
        res = concat(new_res, argv[*index]);
        free(new_res);
    }

    if (*index == argc) // no ; found
    {
        free(res);
        return NULL;
    }
    return res;
}

static struct token *to_token(int argc, char **argv, int *index)
{
    const struct expr_op *exp = find_expr(argv[*index]);
    struct token *t = NULL;

    if (exp == NULL)
    {
        error_arg("unknown predicate : ", argv[*index]);
    }

    if (exp->type == TEST)
    {
        if (argc < *index + 2)
        {
            error_exit("Missing argument for a test");
        }
        t = tokenize(exp, argv[*index + 1]);
        *index += 1;
    }
    else if (strcmp(exp->name, "-exec") == 0)
    {
        *index += 1;
        char *exec_arg = get_args_exec(argc, argv, index);
        if (exec_arg == NULL)
        {
            error_exit("exec doesn't end with ;");
        }
        t = tokenize(exp, exec_arg);
    }
    else
    {
        t = tokenize(exp, NULL);
    }

    return t;
}

static void test_before_adding(struct token_list *tl, struct token *t)
{
    int size = tl->size;
    if (is_operator(t) && t->exp->type != NOT)
    {
        if (size == 1 || is_operator(tl->tokens[size - 1])
            || (tl->tokens[size - 1]->exp->type == LEFT_PARENT))
        {
            token_list_free(tl);
            error_exit("Tokenization : invalid expression; "
                       "you have used a binary operator "
                       "with nothing before it.");
        }
    }
    else if (size > 0 && t->exp->type == NOT
             && tl->tokens[tl->size - 1]->exp->type == NOT)
    {
        token_list_pop(tl);
        free_token(t);
        return;
    }
    else if (size > 0 && !is_operator(tl->tokens[size - 1])
             && (tl->tokens[size - 1]->exp->type != LEFT_PARENT)
             && t->exp->type != RIGHT_PARENT)
    {
        token_list_add(tl, create_quick_token("-a"));
    }
    token_list_add(tl, t);
}

static int has_action(struct token_list *tl, struct options *o)
{
    for (int i = 0; i < tl->size; i++)
    {
        if (tl->tokens[i]->exp->type == ACTION)
        {
            if (strcmp(tl->tokens[i]->exp->name, "-delete") == 0)
                o->pre_order = 0;
            return 1;
        }
    }
    return 0;
}

static void good_parenthesis(struct token_list *tl)
{
    int parent = -1;
    enum my_type prev = TEST;
    for (int i = 0; i < tl->size; i++)
    {
        if (tl->tokens[i]->exp->type == LEFT_PARENT)
        {
            parent += 1;
        }
        else if (tl->tokens[i]->exp->type == RIGHT_PARENT)
        {
            if (prev == LEFT_PARENT)
            {
                token_list_free(tl);
                error_exit("Empty parenthesis");
            }
            if (prev == OP_OR || prev == OP_AND || prev == NOT)
            {
                token_list_free(tl);
                error_exit("Operator before parenthesis");
            }
            parent -= 1;
        }
        prev = tl->tokens[i]->exp->type;
    }
    if (parent != 0)
    {
        error_exit("Unmatched parenthesis");
    }
}

struct token_list *lexer(int argc, char **argv, int start, struct options *o)
{
    struct token_list *tl = token_list_init();
    token_list_add(tl, create_quick_token("("));
    for (int i = start; i < argc; i++)
    {
        struct token *t = to_token(argc, argv, &i);
        if (t != NULL)
        {
            test_before_adding(tl, t);
        }
        else
        {
            token_list_free(tl);
            error_exit("Invalid syntax");
        }
    }

    if (tl->size != 0 && is_operator(tl->tokens[tl->size - 1]))
    {
        token_list_free(tl);
        error_exit("expected an expression after the operator");
    }
    good_parenthesis(tl);
    token_list_add(tl, create_quick_token(")"));

    if (!has_action(tl, o))
    {
        token_list_add(tl, create_quick_token("-a"));
        token_list_add(tl, create_quick_token("-print"));
    }
    return tl;
}
