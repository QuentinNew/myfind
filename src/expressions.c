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

struct options *options = NULL;

int get_stat(char const *file, struct stat **s)
{
    *s = malloc(sizeof(struct stat));
    if (options->type == FOLLOW_SLINK)
    {
        if (stat(file, *s) == -1)
        {
            if (lstat(file, *s) == -1)
            {
                return -1;
            }
            else
            {
                return lstat(file, *s);
            }
        }
        else
        {
            return stat(file, *s);
        }
    }
    if (lstat(file, *s) == -1)
        return -1;

    return lstat(file, *s);
}

static const struct expr_op *expressions[NB_EXPR] = {
    &name_expr,  &type_expr,        &newer_expr,        &perm_expr, &user_expr,
    &group_expr, &print_expr,       &delete_expr,       &exec_expr, &or_expr,
    &and_expr,   &left_parent_expr, &right_parent_expr, &not_expr
};

static char *basename(char const *path)
{
    char *s = strrchr(path, '/');
    char *res = NULL;
    if (!s)
    {
        res = malloc(strlen(path) + 1);
        res = strcpy(res, path);
        return res;
    }
    else
    {
        res = malloc(strlen(s + 1) + 1);
        res = strcpy(res, s + 1);
        return res;
    }
}

int name_op(const char *arg, const char *file)
{
    struct stat *s = NULL;
    if (get_stat(file, &s) < 0)
    {
        free(s);
        error_file(file);
        return -1;
    }
    free(s);
    char *bsname = basename(file);
    int res = (fnmatch(arg, bsname, 0) == 0);
    free(bsname);
    return res;
}
static int good_type(char arg, struct stat *buf)
{
    if (arg == 'b')
    {
        return S_ISBLK(buf->st_mode);
    }
    else if (arg == 'c')
    {
        return (S_ISCHR(buf->st_mode));
    }
    else if (arg == 'd')
    {
        return (S_ISDIR(buf->st_mode));
    }
    else if (arg == 'f')
    {
        return (S_ISREG(buf->st_mode));
    }
    else if (arg == 'l')
    {
        return (S_ISLNK(buf->st_mode));
    }
    else if (arg == 'p')
    {
        return (S_ISFIFO(buf->st_mode));
    }
    else if (arg == 's')
    {
        return (S_ISSOCK(buf->st_mode));
    }
    return 0;
}

int type_op(const char *arg, const char *file)
{
    struct stat *buf = NULL;
    if (get_stat(file, &buf) < 0)
    {
        free(buf);
        error_file(file);
        return -1;
    }
    int res = 0;
    int len = strlen(arg);
    for (int i = 0; i < len && !res; i += 2)
    {
        res = good_type(arg[i], buf);
    }
    free(buf);
    return res;
}

int newer_op(const char *arg, const char *file)
{
    struct stat *buf_file = NULL;
    if (get_stat(file, &buf_file) < 0)
    {
        free(buf_file);
        error_file(file);
        return -1;
    }
    struct stat *buf_arg = malloc(sizeof(struct stat));
    if (options->type != NO_FOLLOW_SLINK)
    {
        if (stat(arg, buf_arg) == -1)
        {
            error_file(file);
            return -1;
        }
    }
    else
    {
        if (lstat(arg, buf_arg) == -1)
        {
            error_file(file);
            return -1;
        }
    }

    int res = 0;
    if (buf_file->st_mtime == buf_arg->st_mtime)
    {
        res = buf_file->st_mtim.tv_nsec > buf_arg->st_mtim.tv_nsec;
    }
    else
    {
        res = buf_file->st_mtime > buf_arg->st_mtime;
    }

    free(buf_arg);
    free(buf_file);

    return res;
}

// -000
static int perm_all_bit(int fperm, int aperm)
{
    for (; fperm != 0; fperm >>= 1, aperm >>= 1)
    {
        int fbit = fperm & 01;
        int abit = aperm & 01;

        if (abit == 1 && fbit != 1)
            return 0;
    }
    return 1;
}

// /000
static int perm_one_bit(int fperm, int aperm)
{
    for (; fperm != 0; fperm >>= 1, aperm >>= 1)
    {
        int fbit = fperm & 01;
        int abit = aperm & 01;

        if (abit == 1 && fbit == 1)
            return 1;
    }
    return 0;
}

int perm_op(const char *arg, const char *file)
{
    struct stat *fstat = NULL;
    if (get_stat(file, &fstat) < 0)
    {
        free(fstat);
        error_file(file);
        return -1;
    }

    char pref = arg[0];

    int fperm = fstat->st_mode & 0777;
    int aperm = 0;

    int res = 0;

    if (pref != '/' && pref != '-')
    {
        aperm = strtol(arg, NULL, 8);
        res = fperm == aperm;
    }
    else
    {
        aperm = strtol(arg + 1, NULL, 8);
        if (pref == '/')
            res = perm_one_bit(fperm, aperm);
        else if (pref == '-')
            res = perm_all_bit(fperm, aperm);
    }
    free(fstat);
    return res;
}

int user_op(const char *arg, const char *file)
{
    struct stat *fstat = NULL;
    if (get_stat(file, &fstat) < 0)
    {
        free(fstat);
        error_file(file);
        return -1;
    }

    struct passwd *user = getpwuid(fstat->st_uid);
    if (user == NULL)
    {
        free(fstat);
        return 0;
    }
    if (strcmp(arg, user->pw_name) == 0)
    {
        free(fstat);
        return 1;
    }
    free(fstat);
    return 0;
}

int group_op(const char *arg, const char *file)
{
    struct stat *fstat = NULL;
    if (get_stat(file, &fstat) < 0)
    {
        free(fstat);
        error_file(file);
        return -1;
    }

    struct group *group = getgrgid(fstat->st_gid);
    if (group == NULL)
    {
        free(fstat);
        return 0;
    }
    if (strcmp(arg, group->gr_name) == 0)
    {
        free(fstat);
        return 1;
    }
    free(fstat);
    return 0;
}
const struct expr_op *find_expr(char *arg)
{
    for (int i = 0; i < NB_EXPR; i++)
    {
        if (strcmp(arg, expressions[i]->name) == 0)
            return expressions[i];
    }
    return NULL; // not found
}

void set_options(struct options *opts)
{
    options = opts;
}
