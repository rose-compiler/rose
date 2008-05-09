// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: ast_support_funcs.h,v 1.2 2008-05-09 13:56:02 gergo Exp $

#ifndef H_AST_SUPPORT_FUNCS
#define H_AST_SUPPORT_FUNCS

#include <rose.h>

SgType *typenum_to_type(unsigned int n);
unsigned int type_to_typenum(SgType *type);
std::string typenum_to_str(unsigned int n);
SgExpression *exprnum_to_expr(unsigned int n);
unsigned int expr_to_exprnum(SgExpression *expr);
std::string exprnum_to_str(unsigned int n);
SgType *expr_type(SgExpression *expr);
unsigned int exprnum_typenum(unsigned int n);
bool is_subtype_of(SgClassType *a, SgClassType *b);
bool is_subtypenum_of(unsigned int a, unsigned int b);

#endif
