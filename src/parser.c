#include "parser.h"

void parser_init(Parser *p, Lexer *lex)
{
    p->lex = lex;
}

ASTNode *parser_parse(Parser *p)
{
    (void)p;
    return ast_new(AST_NOP);
}
