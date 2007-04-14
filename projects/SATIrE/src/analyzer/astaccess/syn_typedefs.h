// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: syn_typedefs.h,v 1.3 2007-04-14 13:47:16 pr009 Exp $

#ifndef H_SYN_TYPEDEFS
#define H_SYN_TYPEDEFS

typedef void *Statement;
typedef void *DeclarationStatement;
typedef void *ScopeStatementNT;
typedef void *Expression;
typedef void *Initializer;
typedef void *ValueExpNT;
typedef void *ExpressionRootNT;
typedef void *InitializedNameNT;
typedef void *UnsignedLongValNT;
typedef void *NamespaceDefinitionStatementNT;
typedef void *PragmaNT;
typedef void *ForInitStatementNT;
typedef void *VariableDeclarationNT;
typedef void *VariableSymbolNT;
typedef void *ExprListExpNT;
typedef void *ConstructorInitializerNT;
typedef void *Type;

typedef long double astldouble;
typedef unsigned long long astullong;
typedef long long astllong;
typedef unsigned char astuchar;
typedef char *aststring;
typedef int astint;
typedef short astshort;
typedef char astchar;
typedef unsigned short astushort;
typedef unsigned int astuint;
typedef long astlong;
typedef unsigned long astulong;
typedef float astfloat;
typedef double astdouble;

typedef void *LIST_DeclarationStatement;
typedef void *LIST_Expression;
typedef void *LIST_InitializedNameNT;
typedef void *LIST_Statement;
typedef void *LIST_Type;
typedef void *LIST_VariableSymbolNT;

#endif
