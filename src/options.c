#include "options.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

static int is_exp(char *arg)
{
    return (arg == NULL || arg[0] == '-' || arg[0] == '(' || arg[0] == ')'
            || arg[0] == '!');
}

int get_starting_point(int argc, char *argv[], struct namelist *nl, int start)
{
    char *copy = NULL;
    if (argc == start || is_exp(argv[start]))
    {
        copy = malloc(strlen(".") + 1);
        copy = strcpy(copy, ".");
        name_list_add(nl, copy);
        return start;
    }
    int i = start;
    for (; i < argc && !is_exp(argv[i]); i++)
    {
        if (file_exist(argv[i], 0))
        {
            copy = malloc(strlen(argv[i]) + 1);
            copy = strcpy(copy, argv[i]);
            name_list_add(nl, copy);
        }
    }

    if (nl->size == 0)
    {
        error_exit("No such file or directory");
    }
    return i;
}

struct options *get_options(int argc, char **argv, int *i)
{
    struct options *options = malloc(sizeof(struct options));

    options->pre_order = 1;
    options->type = NO_FOLLOW_SLINK;

    for (; *i < argc; *i = *i + 1)
    {
        if (strcmp(argv[*i], "-d") == 0)
            options->pre_order = 0;
        else if (strcmp(argv[*i], "-H") == 0)
            options->type = FOLLOW_SLINK_ARGS;
        else if (strcmp(argv[*i], "-L") == 0)
            options->type = FOLLOW_SLINK;
        else if (strcmp(argv[*i], "-P") == 0)
            options->type = NO_FOLLOW_SLINK;
        else
            break;
    }
    return options;
}
