/* A Bison parser, made by GNU Bison 1.875c.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

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
     AUTO = 268,
     SECTIONS = 269,
     SINGLE = 270,
     NOWAIT = 271,
     SECTION = 272,
     FOR = 273,
     MASTER = 274,
     CRITICAL = 275,
     BARRIER = 276,
     ATOMIC = 277,
     FLUSH = 278,
     THREADPRIVATE = 279,
     PRIVATE = 280,
     COPYPRIVATE = 281,
     FIRSTPRIVATE = 282,
     LASTPRIVATE = 283,
     SHARED = 284,
     DEFAULT = 285,
     NONE = 286,
     REDUCTION = 287,
     COPYIN = 288,
     LOGAND = 289,
     LOGOR = 290,
     EXPRESSION = 291,
     ID_EXPRESSION = 292,
     IDENTIFIER = 293,
     NEWLINE = 294,
     LEXICALERROR = 295
   };
#endif
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
#define AUTO 268
#define SECTIONS 269
#define SINGLE 270
#define NOWAIT 271
#define SECTION 272
#define FOR 273
#define MASTER 274
#define CRITICAL 275
#define BARRIER 276
#define ATOMIC 277
#define FLUSH 278
#define THREADPRIVATE 279
#define PRIVATE 280
#define COPYPRIVATE 281
#define FIRSTPRIVATE 282
#define LASTPRIVATE 283
#define SHARED 284
#define DEFAULT 285
#define NONE 286
#define REDUCTION 287
#define COPYIN 288
#define LOGAND 289
#define LOGOR 290
#define EXPRESSION 291
#define ID_EXPRESSION 292
#define IDENTIFIER 293
#define NEWLINE 294
#define LEXICALERROR 295




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE omp_lval;



