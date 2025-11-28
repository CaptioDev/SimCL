/*
 * Symbol table implementation for SimCL
 * Simple linked-list based symbol table
 */

#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>

/* create new table with parent */
SymbolTable *symtab_new(SymbolTable *parent)
{
    SymbolTable *t = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!t) return NULL;
    t->head = NULL;
    t->parent = parent;
    return t;
}

/* add symbol */
int symtab_add(SymbolTable *table, const char *name, SimCLType type)
{
    Symbol *s = (Symbol*)malloc(sizeof(Symbol));
    if (!s) return 0;
    s->name = strdup(name);
    s->type = type;
    s->next = table->head;
    table->head = s;
    return 1;
}

/* lookup symbol recursively */
Symbol *symtab_lookup(SymbolTable *table, const char *name)
{
    Symbol *s;
    if (!table) return NULL;
    for (s = table->head; s; s = s->next) {
        if (strcmp(s->name, name) == 0) return s;
    }
    return symtab_lookup(table->parent, name);
}

/* free table and all symbols */
void symtab_free(SymbolTable *table)
{
    Symbol *s, *tmp;
    if (!table) return;
    s = table->head;
    while (s) {
        tmp = s->next;
        free(s->name);
        free(s);
        s = tmp;
    }
    free(table);
}
