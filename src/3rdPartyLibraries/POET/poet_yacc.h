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
     ENDLIST = 258,
     ERROR = 259,
     SYNERROR = 260,
     PARAMETER = 261,
     DEFINE = 262,
     EVAL = 263,
     LBEGIN = 264,
     RBEGIN = 265,
     PRINT = 266,
     INPUT_ENDFILE = 267,
     INPUT_ENDL = 268,
     INPUT_CONT = 269,
     COND = 270,
     ANNOT = 271,
     TO = 272,
     FROM = 273,
     TYPE = 274,
     POETTYPE = 275,
     SYNTAX = 276,
     MESSAGE = 277,
     LTAG = 278,
     RTAG = 279,
     ENDTAG = 280,
     PARS = 281,
     XFORM = 282,
     OUTPUT = 283,
     PARSE = 284,
     LOOKAHEAD = 285,
     MATCH = 286,
     CODE = 287,
     GLOBAL = 288,
     SOURCE = 289,
     ENDCODE = 290,
     ENDXFORM = 291,
     INPUT = 292,
     ENDINPUT = 293,
     SEMICOLON = 294,
     DEBUG = 295,
     RETURN = 296,
     CONTINUE = 297,
     BREAK = 298,
     ENDDO = 299,
     DO = 300,
     FOREACH_R = 301,
     FOREACH = 302,
     FOR = 303,
     SUCH_THAT = 304,
     IN = 305,
     DOT4 = 306,
     DOT3 = 307,
     DEFAULT = 308,
     SWITCH = 309,
     CASE = 310,
     ELSE = 311,
     IF = 312,
     MOD_ASSIGN = 313,
     DIVIDE_ASSIGN = 314,
     MULT_ASSIGN = 315,
     MINUS_ASSIGN = 316,
     PLUS_ASSIGN = 317,
     ASSIGN = 318,
     QUESTION = 319,
     COMMA = 320,
     TOR = 321,
     OR = 322,
     AND = 323,
     NOT = 324,
     DARROW = 325,
     ARROW = 326,
     NE = 327,
     GE = 328,
     GT = 329,
     EQ = 330,
     LE = 331,
     LT = 332,
     MINUS = 333,
     PLUS = 334,
     STR_CONCAT = 335,
     MOD = 336,
     DIVIDE = 337,
     MULTIPLY = 338,
     DCOLON = 339,
     TILT = 340,
     UMINUS = 341,
     INSERT = 342,
     LEN = 343,
     SPLIT = 344,
     COPY = 345,
     ERASE = 346,
     TRACE = 347,
     EXP = 348,
     NAME = 349,
     FLOAT = 350,
     STRING = 351,
     INT = 352,
     LIST1 = 353,
     LIST = 354,
     TUPLE = 355,
     MAP = 356,
     VAR = 357,
     REBUILD = 358,
     DUPLICATE = 359,
     RANGE = 360,
     ASSERT = 361,
     REPLACE = 362,
     PERMUTE = 363,
     REVERSE = 364,
     RESTORE = 365,
     SAVE = 366,
     CLEAR = 367,
     APPLY = 368,
     DELAY = 369,
     COLON = 370,
     CDR = 371,
     CAR = 372,
     ANY = 373,
     FCONST = 374,
     ICONST = 375,
     ID = 376,
     DOT2 = 377,
     DOT = 378,
     POND = 379,
     RBR = 380,
     LBR = 381,
     RB = 382,
     LB = 383,
     RP = 384,
     LP = 385
   };
#endif
/* Tokens.  */
#define ENDLIST 258
#define ERROR 259
#define SYNERROR 260
#define PARAMETER 261
#define DEFINE 262
#define EVAL 263
#define LBEGIN 264
#define RBEGIN 265
#define PRINT 266
#define INPUT_ENDFILE 267
#define INPUT_ENDL 268
#define INPUT_CONT 269
#define COND 270
#define ANNOT 271
#define TO 272
#define FROM 273
#define TYPE 274
#define POETTYPE 275
#define SYNTAX 276
#define MESSAGE 277
#define LTAG 278
#define RTAG 279
#define ENDTAG 280
#define PARS 281
#define XFORM 282
#define OUTPUT 283
#define PARSE 284
#define LOOKAHEAD 285
#define MATCH 286
#define CODE 287
#define GLOBAL 288
#define SOURCE 289
#define ENDCODE 290
#define ENDXFORM 291
#define INPUT 292
#define ENDINPUT 293
#define SEMICOLON 294
#define DEBUG 295
#define RETURN 296
#define CONTINUE 297
#define BREAK 298
#define ENDDO 299
#define DO 300
#define FOREACH_R 301
#define FOREACH 302
#define FOR 303
#define SUCH_THAT 304
#define IN 305
#define DOT4 306
#define DOT3 307
#define DEFAULT 308
#define SWITCH 309
#define CASE 310
#define ELSE 311
#define IF 312
#define MOD_ASSIGN 313
#define DIVIDE_ASSIGN 314
#define MULT_ASSIGN 315
#define MINUS_ASSIGN 316
#define PLUS_ASSIGN 317
#define ASSIGN 318
#define QUESTION 319
#define COMMA 320
#define TOR 321
#define OR 322
#define AND 323
#define NOT 324
#define DARROW 325
#define ARROW 326
#define NE 327
#define GE 328
#define GT 329
#define EQ 330
#define LE 331
#define LT 332
#define MINUS 333
#define PLUS 334
#define STR_CONCAT 335
#define MOD 336
#define DIVIDE 337
#define MULTIPLY 338
#define DCOLON 339
#define TILT 340
#define UMINUS 341
#define INSERT 342
#define LEN 343
#define SPLIT 344
#define COPY 345
#define ERASE 346
#define TRACE 347
#define EXP 348
#define NAME 349
#define FLOAT 350
#define STRING 351
#define INT 352
#define LIST1 353
#define LIST 354
#define TUPLE 355
#define MAP 356
#define VAR 357
#define REBUILD 358
#define DUPLICATE 359
#define RANGE 360
#define ASSERT 361
#define REPLACE 362
#define PERMUTE 363
#define REVERSE 364
#define RESTORE 365
#define SAVE 366
#define CLEAR 367
#define APPLY 368
#define DELAY 369
#define COLON 370
#define CDR 371
#define CAR 372
#define ANY 373
#define FCONST 374
#define ICONST 375
#define ID 376
#define DOT2 377
#define DOT 378
#define POND 379
#define RBR 380
#define LBR 381
#define RB 382
#define LB 383
#define RP 384
#define LP 385




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

