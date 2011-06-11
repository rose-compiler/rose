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
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     OMP = 258,
     PARALLEL = 259,
     IF = 260,
     NUM_THREADS = 261,
     ORDERED = 262,
     SCHEDULE = 263,
     STATIC = 264,
     DYNAMIC = 265,
     GUIDED = 266,
     RUNTIME = 267,
     SECTIONS = 268,
     SINGLE = 269,
     NOWAIT = 270,
     SECTION = 271,
     FOR = 272,
     MASTER = 273,
     CRITICAL = 274,
     BARRIER = 275,
     ATOMIC = 276,
     FLUSH = 277,
     THREADPRIVATE = 278,
     PRIVATE = 279,
     COPYPRIVATE = 280,
     FIRSTPRIVATE = 281,
     LASTPRIVATE = 282,
     SHARED = 283,
     DEFAULT = 284,
     NONE = 285,
     REDUCTION = 286,
     COPYIN = 287,
     TASK = 288,
     TASKWAIT = 289,
     UNTIED = 290,
     COLLAPSE = 291,
     AUTO = 292,
     LOGAND = 293,
     LOGOR = 294,
     LE_OP2 = 295,
     GE_OP2 = 296,
     EQ_OP2 = 297,
     NE_OP2 = 298,
     RIGHT_ASSIGN2 = 299,
     LEFT_ASSIGN2 = 300,
     ADD_ASSIGN2 = 301,
     SUB_ASSIGN2 = 302,
     MUL_ASSIGN2 = 303,
     DIV_ASSIGN2 = 304,
     MOD_ASSIGN2 = 305,
     AND_ASSIGN2 = 306,
     XOR_ASSIGN2 = 307,
     OR_ASSIGN2 = 308,
     LEXICALERROR = 309,
     IDENTIFIER = 310,
     ICONSTANT = 311,
     EXPRESSION = 312,
     ID_EXPRESSION = 313
   };
#endif
/* Tokens.  */
#define OMP 258
#define PARALLEL 259
#define IF 260
#define NUM_THREADS 261
#define ORDERED 262
#define SCHEDULE 263
#define STATIC 264
#define DYNAMIC 265
#define GUIDED 266
#define RUNTIME 267
#define SECTIONS 268
#define SINGLE 269
#define NOWAIT 270
#define SECTION 271
#define FOR 272
#define MASTER 273
#define CRITICAL 274
#define BARRIER 275
#define ATOMIC 276
#define FLUSH 277
#define THREADPRIVATE 278
#define PRIVATE 279
#define COPYPRIVATE 280
#define FIRSTPRIVATE 281
#define LASTPRIVATE 282
#define SHARED 283
#define DEFAULT 284
#define NONE 285
#define REDUCTION 286
#define COPYIN 287
#define TASK 288
#define TASKWAIT 289
#define UNTIED 290
#define COLLAPSE 291
#define AUTO 292
#define LOGAND 293
#define LOGOR 294
#define LE_OP2 295
#define GE_OP2 296
#define EQ_OP2 297
#define NE_OP2 298
#define RIGHT_ASSIGN2 299
#define LEFT_ASSIGN2 300
#define ADD_ASSIGN2 301
#define SUB_ASSIGN2 302
#define MUL_ASSIGN2 303
#define DIV_ASSIGN2 304
#define MOD_ASSIGN2 305
#define AND_ASSIGN2 306
#define XOR_ASSIGN2 307
#define OR_ASSIGN2 308
#define LEXICALERROR 309
#define IDENTIFIER 310
#define ICONSTANT 311
#define EXPRESSION 312
#define ID_EXPRESSION 313




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 73 "../../../../src/frontend/SageIII/ompparser.yy"
{  int itype;
          double ftype;
          const char* stype;
          void* ptype; /* For expressions */
        }
/* Line 1529 of yacc.c.  */
#line 171 "ompparser.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE omp_lval;

