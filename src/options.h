#ifndef OPTIONS_H
#define OPTIONS_H

#include <dirent.h>

#include "utils/file_management.h"

struct namelist
{
    int size;
    int capacity;
    char **names;
};

struct namelist *name_list_init(void);

void name_list_add(struct namelist *nl, char *name);

void name_list_free(struct namelist *nl);

enum options_type
{
    NO_FOLLOW_SLINK,
    FOLLOW_SLINK,
    FOLLOW_SLINK_ARGS
};

struct options
{
    int pre_order;
    enum options_type type;
};

struct options *get_options(int argc, char **args, int *i);

int get_starting_point(int argc, char *argv[], struct namelist *nl, int i);
struct namelist *get_all_files(char *path, struct options *options);

#endif /* !OPTIONS_H */
