/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */

#ifdef _MSC_VER
    #undef TRUE
    #undef FALSE
#endif

#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ALTERNATION = 258,
     NOT = 259,
     AND = 260,
     XOR = 261,
     OR = 262,
     NEQ = 263,
     EQ = 264,
     C_NEQ = 265,
     C_EQ = 266,
     WHERE = 267,
     TRUE = 268,
     FALSE = 269,
     IDENT = 270,
     VARIABLE = 271,
     INTEGER = 272,
     SQ_STRING = 273,
     NULL_NODE = 274,
     DOTDOT = 275
   };
#endif
/* Tokens.  */
#define ALTERNATION 258
#define NOT 259
#define AND 260
#define XOR 261
#define OR 262
#define NEQ 263
#define EQ 264
#define C_NEQ 265
#define C_EQ 266
#define WHERE 267
#define TRUE 268
#define FALSE 269
#define IDENT 270
#define VARIABLE 271
#define INTEGER 272
#define SQ_STRING 273
#define NULL_NODE 274
#define DOTDOT 275




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 34 "/export/tmp.schordan1/development/rosework/mainbranch/rose/src/midend/astMatching/matcherparser.yy"
{
char* idstring; /* IDENT */
char* varstring; /* VARIABLE */
char* sq_string; /* single quoted STRING */
MatchOperationListList* matchoperationlistlist; /* list of (MatchOperation or subclasses) */
MatchOperation* matchoperation; /* MatchOperation or subclasses */
MatchOperationList* matchoperationlist; /* list of (MatchOperation or subclasses) */
long intval; /* INTEGER */
int length; /* #elements */
}
/* Line 1529 of yacc.c.  */
#line 100 "y.tab.h"
        YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE matcherparserlval;

