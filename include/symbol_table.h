#ifndef SIMCL_SYMBOL_TABLE_H
#define SIMCL_SYMBOL_TABLE_H

typedef struct Symbol {
    char name[64];
    int type;
    struct Symbol *next;
} Symbol;

Symbol *symbol_table_create(void);
void symbol_table_free(Symbol *s);

#endif
