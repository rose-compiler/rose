#ifndef AFUN_H
#define AFUN_H

#include "atypes.h"
#include "encoding.h"

#ifdef __cplusplus
extern "C"
{
#endif/* __cplusplus */

#define Symbol AFun

#define AS_INT          0
#define AS_REAL         1
#define AS_BLOB         2
#define AS_PLACEHOLDER  3
#define AS_LIST         4
#define AS_EMPTY_LIST   5
#define AS_ANNOTATION   6

typedef MachineWord AFun;

/* The Symbol type */
typedef struct _SymEntry
{
  header_type header;
  struct _SymEntry *next;
  Symbol  id;
  char   *name;
  int     count;  /* used in bafio.c */
  int     index;  /* used in bafio.c */
} *SymEntry;

/* defined on SymEntry */
#define SYM_IS_FREE(sym)          (((MachineWord)(sym) & 1) == 1)

#define AT_markSymbol(s)       (at_lookup_table[(s)]->header |= MASK_AGE_MARK)
#define AT_markSymbol_young(s) if(!IS_OLD(at_lookup_table[(s)]->header)) AT_markSymbol(s)

/* void AT_unmarkSymbol(Symbol sym); */
#define AT_unmarkSymbol(s) (at_lookup_table[(s)]->header &= ~MASK_MARK)

union _ATerm;
extern union _ATerm **at_lookup_table_alias;
extern SymEntry *at_lookup_table;

unsigned int AT_symbolTableSize();
void AT_initSymbol(int argc, char *argv[]);
int AT_printSymbol(Symbol sym, FILE *f);
/* ATbool AT_isValidSymbol(Symbol sym); */
#define AT_isValidSymbol(sym) (((Symbol)sym >= 0 && (unsigned int)(Symbol)sym < AT_symbolTableSize() \
                                 && !SYM_IS_FREE(at_lookup_table[(Symbol)sym])) ?  ATtrue : ATfalse)

/* ATbool AT_isMarkedSymbol(Symbol sym); */
#define AT_isMarkedSymbol(sym) IS_MARKED(at_lookup_table[(Symbol)sym]->header)

void  AT_freeSymbol(SymEntry sym);
void AT_markProtectedSymbols();
void AT_markProtectedSymbols_young();

unsigned int AT_hashSymbol(const char *name, int arity);
ATbool AT_findSymbol(char *name, int arity, ATbool quoted);
void AT_unmarkAllAFuns();

#ifdef __cplusplus
}
#endif/* __cplusplus */ 

#endif
