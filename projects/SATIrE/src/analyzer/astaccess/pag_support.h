// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: pag_support.h,v 1.4 2007-10-31 10:52:08 gergo Exp $

#ifndef H_PAG_SUPPORT
#define H_PAG_SUPPORT

unsigned long synttype_hash(void *);
int syntax_eq(void *, void *);
void syntax_mcopy(void *, void *);
void syntaxdummy(void *);
#ifdef __cplusplus
extern "C"
#endif
void syntax_init(void);

// GB (2007-10-31): Made these extern "C" to fix an error reported by
// Stefan Stattelmann.
#ifdef __cplusplus
extern "C" {
#endif
extern int syntaxtype;
extern int e_syntaxtype;
#ifdef __cplusplus
}
#endif

#include "syn_typedefs.h"

#ifdef __cplusplus
extern "C"
#endif
char *basic_type_name(const void *);

#endif
