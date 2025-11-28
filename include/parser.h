#ifndef SIMCL_PARSER_H
#define SIMCL_PARSER_H

#include "ast.h"
#include "lexer.h"

/* Parser - recursive descent for SimCL grammar (Phase 3)
 *
 * Public API:
 *   parser_init(&p, &lex);
 *   ASTNode *root = parser_parse(&p);
 *   ast_free(root);
 *
 */

typedef struct {
    Lexer *lex;
    /* current token cached for convenience */
} Parser;

void parser_init(Parser *p, Lexer *lex);
ASTNode *parser_parse(Parser *p);

#endif
