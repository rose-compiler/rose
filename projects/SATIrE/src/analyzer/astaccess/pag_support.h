#ifndef H_PAG_SUPPORT
#define H_PAG_SUPPORT

unsigned int synttype_hash(void *);
int syntax_eq(void *, void *);
void syntax_mcopy(void *, void *);
void syntaxdummy(void *);
#ifdef __cplusplus
extern "C"
#endif
void syntax_init(void);

extern int syntaxtype;
extern int e_syntaxtype;

#include "syn_typedefs.h"

#ifdef __cplusplus
extern "C"
#endif
char *basic_type_name(const void *);

#endif
