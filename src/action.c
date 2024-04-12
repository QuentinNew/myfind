#define _DEFAULT_SOURCE

#include <fnmatch.h>
#include <grp.h>
#include <pwd.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "error.h"
#include "options.h"
#include "tokens.h"

int print_op(const char *arg, const char *file)
{
    struct stat *s = NULL;
    if (get_stat(file, &s) < 0)
    {
        free(s);
        error_file(file);
        return -1;
    }
    free(s);
    if (!arg)
        printf("%s\n", file);
    return 1;
}

int delete_op(const char *arg, const char *file)
{
    if (arg)
    {
        return 0;
    }

    int res = remove(file);

    if (res != 0)
    {
        error_file(file);
        return -1;
    }

    return 1;
}

static void free_exec(char **args, const char *file)
{
    if (!file)
        return;
    // free(args[0]);
    free(args);
}

static char *do_replace(char *arg, const char *file, int i)
{
    char *res = malloc(strlen(arg) + strlen(file) + 1);
    int index = 0;
    for (; index < i; index++)
    {
        res[index] = arg[index];
    }
    for (int j = 0; file[j] != '\0'; j++, index++)
    {
        res[index] = file[j];
    }
    for (int j = i + 2; arg[j] != '\0'; index++, j++)
    {
        res[index] = arg[j];
    }
    res[index] = '\0';
    return res;
}

static char *replace_exec(char *arg, const char *file, struct namelist *nl)
{
    char *res = malloc(strlen(arg) + 1);
    res = strcpy(res, arg);
    name_list_add(nl, res);
    char prev = '}';
    for (int i = 0; res[i] != '\0'; i++)
    {
        if (prev == '{' && res[i] == '}')
        {
            res = do_replace(res, file, i - 1);
            name_list_add(nl, res);
            i = i + strlen(file) - 2;
        }
        if (res[i] != '\0')
            prev = res[i];
    }

    return res;
    /*
    if (strcmp(arg, "{}") == 0)
    {
        char *new = malloc(strlen(file) + 1);
        strcpy(new, file);
        name_list_add(nl, new);
        return new;
    }*/
}

static char **get_exec_args(const char *arg, const char *file,
                            struct namelist *nl)
{
    char **res = malloc(sizeof(char *));
    int init_size = strlen(arg);
    char delim[] = " ";
    char *cpy = malloc(init_size * (sizeof(char)) + 1);
    strcpy(cpy, arg);
    name_list_add(nl, cpy);
    char *ptr = strtok(cpy, delim);

    int i = 0;
    while (ptr != NULL)
    {
        res = realloc(res, (sizeof(char *)) * (i + 1));
        res[i] = replace_exec(ptr, file, nl);
        ptr = strtok(NULL, delim);
        i += 1;
    }
    // NULL TERMINATED
    res = realloc(res, sizeof(char *) * (i + 1));
    res[i] = NULL;
    free(ptr);
    return res;
}

int exec_op(const char *arg, const char *file)
{
    if (file == NULL)
        return -1;
    struct namelist *nl = name_list_init();
    char **args = get_exec_args(arg, file, nl);
    int res = 0;
    pid_t p = fork();
    if (0 == p)
    {
        exit(execvp(args[0], args));
    }
    else
    {
        int status;

        if (waitpid(p, &status, 0) == -1)
        {
            perror("waitpid() failed");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status))
        {
            int exit_code = WEXITSTATUS(status);
            if (exit_code == 0)
                res = 1;
            else
                res = 0;
        }
    }
    name_list_free(nl);
    free_exec(args, file);
    return res;
}
