#ifndef TOKENS_H
#define TOKENS_H

#include <stddef.h>
#include <sys/stat.h>

#include "options.h"

typedef int (*operation)(const char *, const char *);

enum my_type
{
    TEST,
    ACTION,
    OP_OR,
    OP_AND,
    LEFT_PARENT,
    RIGHT_PARENT,
    NOT
};
/* == EXPRESSIONS == */

#define NB_EXPR 14

struct expr_op
{
    enum my_type type;
    char name[50];
    operation op;
};

int name_op(const char *arg, const char *file);
int print_op(const char *arg, const char *file);
int type_op(const char *arg, const char *file);
int newer_op(const char *arg, const char *file);
int perm_op(const char *arg, const char *file);
int user_op(const char *arg, const char *file);
int group_op(const char *arg, const char *file);
int delete_op(const char *arg, const char *file);
int exec_op(const char *arg, const char *file);

/* === TEST === */

static const struct expr_op name_expr = { .type = TEST,
                                          .name = "-name",
                                          .op = name_op };

static const struct expr_op type_expr = { .type = TEST,
                                          .name = "-type",
                                          .op = type_op };

static const struct expr_op newer_expr = { .type = TEST,
                                           .name = "-newer",
                                           .op = newer_op };

static const struct expr_op perm_expr = { .type = TEST,
                                          .name = "-perm",
                                          .op = perm_op };

static const struct expr_op user_expr = { .type = TEST,
                                          .name = "-user",
                                          .op = user_op };

static const struct expr_op group_expr = { .type = TEST,
                                           .name = "-group",
                                           .op = group_op };

/* === ACTION == */

static const struct expr_op print_expr = { .type = ACTION,
                                           .name = "-print",
                                           .op = print_op };

static const struct expr_op delete_expr = { .type = ACTION,
                                            .name = "-delete",
                                            .op = delete_op };

static const struct expr_op exec_expr = { .type = ACTION,
                                          .name = "-exec",
                                          .op = exec_op };
/* === OTHER === */

static const struct expr_op or_expr = { .type = OP_OR,
                                        .name = "-o",
                                        .op = NULL };

static const struct expr_op and_expr = { .type = OP_AND,
                                         .name = "-a",
                                         .op = NULL };

static const struct expr_op left_parent_expr = { .type = LEFT_PARENT,
                                                 .name = "(",
                                                 .op = NULL };

static const struct expr_op right_parent_expr = { .type = RIGHT_PARENT,
                                                  .name = ")",
                                                  .op = NULL };

static const struct expr_op not_expr = { .type = NOT, .name = "!", .op = NULL };

const struct expr_op *find_expr(char *arg);

void set_options(struct options *opts);

/* == TOKENS == */

struct token
{
    const struct expr_op *exp;
    char *arg;
};

struct token_list
{
    int size;
    int capacity;
    struct token **tokens;
};

int is_operator(struct token *t);

struct token *create_quick_token(char *arg);

struct token *tokenize(const struct expr_op *exp, char *arg);

struct token_list *token_list_init(void);

void token_list_add(struct token_list *tl, struct token *token);

void token_list_pop(struct token_list *tl);

void token_list_free(struct token_list *tl);

void free_token(struct token *token);

int get_stat(char const *file, struct stat **s);
#endif /* !TOKENS_H */
