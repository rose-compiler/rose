// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: ast_support_funcs.h,v 1.1 2007-03-13 10:11:35 pr009 Exp $

#ifndef H_AST_SUPPORT_FUNCS
#define H_AST_SUPPORT_FUNCS

#include <rose.h>

SgType *typenum_to_type(int n);
int type_to_typenum(SgType *type);
std::string typenum_to_str(int n);
SgExpression *exprnum_to_expr(int n);
int expr_to_exprnum(SgExpression *expr);
std::string exprnum_to_str(int n);
SgType *expr_type(SgExpression *expr);
int exprnum_typenum(int n);
bool is_subtype_of(SgClassType *a, SgClassType *b);
bool is_subtypenum_of(int a, int b);

#endif
