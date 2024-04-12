#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "compute.h"
#include "lexer.h"
#include "options.h"
#include "tokens.h"

int main(int argc, char *argv[])
{
    struct namelist *nl = name_list_init();
    int i = 1;

    struct options *options = get_options(argc, argv, &i);
    set_options(options);
    int start_tok = get_starting_point(argc, argv, nl, i);
    struct token_list *tl = lexer(argc, argv, start_tok, options);

    struct node *ast = build_ast(tl);

    int res = 0;
    for (int i = 0; i < nl->size; i++)
    {
        if (compute_all(nl->names[i], ast, options) == 1)
            res = 1;
    }

    free(tl->tokens);
    free(tl);
    free_nodes(ast);
    free(nl->names);
    free(nl);
    free(options);
    return res;
}
