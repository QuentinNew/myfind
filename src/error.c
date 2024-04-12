#define _DEFAULT_SOURCE
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int is_symbolic_link(const char *path)
{
    struct stat *fstat = malloc(sizeof(struct stat));
    if (lstat(path, fstat) < 0)
    {
        free(fstat);
        return -1;
    }
    int res = S_ISLNK(fstat->st_mode);
    free(fstat);
    return res;
}

int file_exist(const char *path, int follow_link)
{
    struct stat *fstat = malloc(sizeof(struct stat));
    if (follow_link)
    {
        if (stat(path, fstat) < 0)
        {
            free(fstat);
            return 0;
        }
    }
    else
    {
        if (lstat(path, fstat) < 0)
        {
            free(fstat);
            return 0;
        }
    }
    free(fstat);
    return 1;
}

void error_file(const char *path)
{
    fprintf(stderr, "my find: '%s': No such file or directory\n", path);
}

void error_arg(char msg[], char *text)
{
    char filename[] = "myfind: ";
    char *error_msg = malloc(strlen(filename) + strlen(text) + strlen(msg) + 1);
    strcpy(error_msg, filename);
    strcat(error_msg, msg);
    strcat(error_msg, text);
    fprintf(stderr, error_msg);
    free(error_msg);
    exit(1);
}

void error_exit(char msg[])
{
    char filename[] = "myfind: ";
    char *error_msg = malloc(strlen(filename) + strlen(msg) + 1);

    strcpy(error_msg, filename);
    strcat(error_msg, msg);
    fprintf(stderr, error_msg);
    free(error_msg);
    exit(1);
}
