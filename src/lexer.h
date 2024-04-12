#ifndef LEXER_H
#define LEXER_H

#include "options.h"

struct token_list *lexer(int argc, char **argv, int start, struct options *o);

#endif /* !LEXER_H */
