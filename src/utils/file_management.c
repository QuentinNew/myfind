#include "file_management.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../options.h"

/*
static int hidden_file(struct dirent *f)
{
    if (f->d_name[0] == '.')
        return 1;
    return 0;
}*/

int is_folder(char *path)
{
    struct stat buf;
    return (stat(path, &buf) == 0 && S_ISDIR(buf.st_mode));
}

static char *concat(char *s1, char *s2)
{
    char *res = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(res, s1);
    strcat(res, s2);
    return res;
}

static void get_all_files_rec(char *path, struct namelist *nl,
                              struct options *options)
{
    // printf("opening :%s\n", path);
    DIR *dir = opendir(path);

    if (!dir)
        return;

    struct dirent *file = NULL;

    while ((file = readdir(dir)) != NULL)
    {
        if (strcmp(file->d_name, "..") == 0 || strcmp(file->d_name, ".") == 0)
            continue;

        char *name = concat(path, file->d_name);

        if (options->pre_order)
            name_list_add(nl, name);
        // printf("%s\n", name);
        if (is_folder(name))
        {
            char *new_name = concat(name, "/");
            get_all_files_rec(new_name, nl, options);
            free(new_name);
        }

        if (!options->pre_order)
            name_list_add(nl, name);
    }
    closedir(dir);
}

static int has_slash_at_end(char *s)
{
    int i = 0;
    for (; s[i] != '\0'; i++)
    {
        continue;
    }
    return i > 0 && s[i - 1] == '/';
}

static struct namelist *
get_all_files_folder(char *path, struct options *options, struct namelist *nl)
{
    int has_slash = has_slash_at_end(path);

    char *folder = malloc(strlen(path) + 1);
    strcpy(folder, path);

    if (options->pre_order)
        name_list_add(nl, folder);
    if (!has_slash)
    {
        char *real_path = concat(path, "/");
        get_all_files_rec(real_path, nl, options);
        free(real_path);
    }
    else
    {
        get_all_files_rec(path, nl, options);
    }
    if (!options->pre_order)
    {
        name_list_add(nl, folder);
    }
    return nl;
}

struct namelist *get_all_files(char *path, struct options *options)
{
    struct namelist *nl = name_list_init();

    char *real_path = NULL;
    if (!is_folder(path))
    {
        real_path = malloc(strlen(path) + 1);
        real_path = strcpy(real_path, path);
        name_list_add(nl, real_path);
        return nl;
    }
    return get_all_files_folder(path, options, nl);
}
