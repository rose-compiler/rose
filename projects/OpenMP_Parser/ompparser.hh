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
     EXPRESSION = 295,
     ID_EXPRESSION = 296,
     IDENTIFIER = 297,
     NEWLINE = 298,
     LEXICALERROR = 299
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
#define EXPRESSION 295
#define ID_EXPRESSION 296
#define IDENTIFIER 297
#define NEWLINE 298
#define LEXICALERROR 299




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE omp_lval;



