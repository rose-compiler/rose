// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: pag_support.h,v 1.4 2008-08-23 13:46:48 gergo Exp $

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
const char *basic_type_name(const void *);

// GB (2008-06-04): If an analyzer prefix is set, include the header that
// redefines prefixed support function names to the actual unprefixed names.
#if PREFIX_SET
    #include "prefixed_support_funcs.h"
#endif

#endif
