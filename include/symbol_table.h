#ifndef SIMCL_SYMBOL_TABLE_H
#define SIMCL_SYMBOL_TABLE_H

#include "type_system.h"
#include "ast.h"

/* Symbol table entry */
typedef struct Symbol {
    char *name;
    SimCLType type;
    struct Symbol *next;
} Symbol;

/* Symbol table - simple linked list per scope */
typedef struct SymbolTable {
    Symbol *head;
    struct SymbolTable *parent;
} SymbolTable;

/* Create a new symbol table with optional parent */
SymbolTable *symtab_new(SymbolTable *parent);

/* Add symbol to table */
int symtab_add(SymbolTable *table, const char *name, SimCLType type);

/* Lookup symbol in table hierarchy */
Symbol *symtab_lookup(SymbolTable *table, const char *name);

/* Free table recursively */
void symtab_free(SymbolTable *table);

#endif
