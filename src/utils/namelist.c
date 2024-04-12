#include <stdlib.h>

#include "../options.h"

struct namelist *name_list_init(void)
{
    struct namelist *nl = malloc(sizeof(struct namelist));
    nl->capacity = 10;
    nl->size = 0;
    nl->names = malloc(sizeof(char *) * 10);
    return nl;
}

void name_list_add(struct namelist *nl, char *name)
{
    if (nl->capacity < nl->size * 2)
    {
        nl->capacity *= 2;
        nl->names = realloc(nl->names, nl->capacity * sizeof(char *));
    }

    nl->names[nl->size] = name;
    nl->size++;
}

void name_list_free(struct namelist *nl)
{
    for (int i = 0; i < nl->size; i++)
    {
        free(nl->names[i]);
    }
    free(nl->names);
    free(nl);
}
