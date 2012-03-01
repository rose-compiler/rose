// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: ast_support_funcs.h,v 1.4 2008-05-19 12:37:09 gergo Exp $

#ifndef H_AST_SUPPORT_FUNCS
#define H_AST_SUPPORT_FUNCS

#include <satire_rose.h>

SgType *typenum_to_type(unsigned long n);
unsigned long type_to_typenum(SgType *type);
std::string typenum_to_str(unsigned long n);
SgExpression *exprnum_to_expr(unsigned long n);
unsigned long expr_to_exprnum(SgExpression *expr);
std::string exprnum_to_str(unsigned long n);
SgType *expr_type(SgExpression *expr);
unsigned long exprnum_typenum(unsigned long n);
bool is_subtype_of(SgClassType *a, SgClassType *b);
bool is_subtypenum_of(unsigned long a, unsigned long b);

#endif
