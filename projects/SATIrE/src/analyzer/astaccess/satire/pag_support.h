// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: pag_support.h,v 1.1 2007-09-20 09:25:31 adrian Exp $

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

extern int syntaxtype;
extern int e_syntaxtype;

#include "syn_typedefs.h"

#ifdef __cplusplus
extern "C"
#endif
char *basic_type_name(const void *);

#endif
