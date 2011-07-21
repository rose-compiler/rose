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
     ERROR = 258,
     SYNERROR = 259,
     PARAMETER = 260,
     DEFINE = 261,
     EVAL = 262,
     LBEGIN = 263,
     RBEGIN = 264,
     PRINT = 265,
     INPUT_ENDFILE = 266,
     INPUT_ENDL = 267,
     INPUT_CONT = 268,
     FALSE = 269,
     TOKEN = 270,
     INHERIT = 271,
     COND = 272,
     ANNOT = 273,
     TO = 274,
     FROM = 275,
     TYPE = 276,
     POETTYPE = 277,
     SYNTAX = 278,
     MESSAGE = 279,
     LTAG = 280,
     RTAG = 281,
     ENDTAG = 282,
     PARS = 283,
     XFORM = 284,
     OUTPUT = 285,
     PARSE = 286,
     LOOKAHEAD = 287,
     MATCH = 288,
     CODE = 289,
     GLOBAL = 290,
     SOURCE = 291,
     ENDCODE = 292,
     ENDXFORM = 293,
     INPUT = 294,
     ENDINPUT = 295,
     SEMICOLON = 296,
     DEBUG = 297,
     RETURN = 298,
     CONTINUE = 299,
     BREAK = 300,
     FOREACH_R = 301,
     FOREACH = 302,
     FOR = 303,
     DOT4 = 304,
     DOT3 = 305,
     DEFAULT = 306,
     SWITCH = 307,
     CASE = 308,
     ELSE = 309,
     IF = 310,
     MOD_ASSIGN = 311,
     DIVIDE_ASSIGN = 312,
     MULT_ASSIGN = 313,
     MINUS_ASSIGN = 314,
     PLUS_ASSIGN = 315,
     ASSIGN = 316,
     QUESTION = 317,
     COMMA = 318,
     TOR = 319,
     OR = 320,
     AND = 321,
     NOT = 322,
     DARROW = 323,
     ARROW = 324,
     NE = 325,
     GE = 326,
     GT = 327,
     EQ = 328,
     LE = 329,
     LT = 330,
     MINUS = 331,
     PLUS = 332,
     STR_CONCAT = 333,
     MOD = 334,
     DIVIDE = 335,
     MULTIPLY = 336,
     DCOLON = 337,
     TILT = 338,
     UMINUS = 339,
     INSERT = 340,
     LEN = 341,
     SPLIT = 342,
     COPY = 343,
     ERASE = 344,
     TRACE = 345,
     EXP = 346,
     NAME = 347,
     STRING = 348,
     INT = 349,
     LIST1 = 350,
     LIST = 351,
     TUPLE = 352,
     MAP = 353,
     VAR = 354,
     REBUILD = 355,
     DUPLICATE = 356,
     RANGE = 357,
     REPLACE = 358,
     PERMUTE = 359,
     RESTORE = 360,
     SAVE = 361,
     CLEAR = 362,
     APPLY = 363,
     DELAY = 364,
     COLON = 365,
     CDR = 366,
     CAR = 367,
     ANY = 368,
     ICONST = 369,
     ID = 370,
     DOT2 = 371,
     DOT = 372,
     POND = 373,
     RBR = 374,
     LBR = 375,
     RB = 376,
     LB = 377,
     RP = 378,
     LP = 379
   };
#endif
/* Tokens.  */
#define ERROR 258
#define SYNERROR 259
#define PARAMETER 260
#define DEFINE 261
#define EVAL 262
#define LBEGIN 263
#define RBEGIN 264
#define PRINT 265
#define INPUT_ENDFILE 266
#define INPUT_ENDL 267
#define INPUT_CONT 268
#define FALSE 269
#define TOKEN 270
#define INHERIT 271
#define COND 272
#define ANNOT 273
#define TO 274
#define FROM 275
#define TYPE 276
#define POETTYPE 277
#define SYNTAX 278
#define MESSAGE 279
#define LTAG 280
#define RTAG 281
#define ENDTAG 282
#define PARS 283
#define XFORM 284
#define OUTPUT 285
#define PARSE 286
#define LOOKAHEAD 287
#define MATCH 288
#define CODE 289
#define GLOBAL 290
#define SOURCE 291
#define ENDCODE 292
#define ENDXFORM 293
#define INPUT 294
#define ENDINPUT 295
#define SEMICOLON 296
#define DEBUG 297
#define RETURN 298
#define CONTINUE 299
#define BREAK 300
#define FOREACH_R 301
#define FOREACH 302
#define FOR 303
#define DOT4 304
#define DOT3 305
#define DEFAULT 306
#define SWITCH 307
#define CASE 308
#define ELSE 309
#define IF 310
#define MOD_ASSIGN 311
#define DIVIDE_ASSIGN 312
#define MULT_ASSIGN 313
#define MINUS_ASSIGN 314
#define PLUS_ASSIGN 315
#define ASSIGN 316
#define QUESTION 317
#define COMMA 318
#define TOR 319
#define OR 320
#define AND 321
#define NOT 322
#define DARROW 323
#define ARROW 324
#define NE 325
#define GE 326
#define GT 327
#define EQ 328
#define LE 329
#define LT 330
#define MINUS 331
#define PLUS 332
#define STR_CONCAT 333
#define MOD 334
#define DIVIDE 335
#define MULTIPLY 336
#define DCOLON 337
#define TILT 338
#define UMINUS 339
#define INSERT 340
#define LEN 341
#define SPLIT 342
#define COPY 343
#define ERASE 344
#define TRACE 345
#define EXP 346
#define NAME 347
#define STRING 348
#define INT 349
#define LIST1 350
#define LIST 351
#define TUPLE 352
#define MAP 353
#define VAR 354
#define REBUILD 355
#define DUPLICATE 356
#define RANGE 357
#define REPLACE 358
#define PERMUTE 359
#define RESTORE 360
#define SAVE 361
#define CLEAR 362
#define APPLY 363
#define DELAY 364
#define COLON 365
#define CDR 366
#define CAR 367
#define ANY 368
#define ICONST 369
#define ID 370
#define DOT2 371
#define DOT 372
#define POND 373
#define RBR 374
#define LBR 375
#define RB 376
#define LB 377
#define RP 378
#define LP 379




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

