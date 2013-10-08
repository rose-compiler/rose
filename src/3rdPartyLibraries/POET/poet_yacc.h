/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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
     INHERIT = 269,
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
     FOREACH_R = 299,
     FOREACH = 300,
     FOR = 301,
     DOT4 = 302,
     DOT3 = 303,
     DEFAULT = 304,
     SWITCH = 305,
     CASE = 306,
     ELSE = 307,
     IF = 308,
     MOD_ASSIGN = 309,
     DIVIDE_ASSIGN = 310,
     MULT_ASSIGN = 311,
     MINUS_ASSIGN = 312,
     PLUS_ASSIGN = 313,
     ASSIGN = 314,
     QUESTION = 315,
     COMMA = 316,
     TOR = 317,
     OR = 318,
     AND = 319,
     NOT = 320,
     DARROW = 321,
     ARROW = 322,
     NE = 323,
     GE = 324,
     GT = 325,
     EQ = 326,
     LE = 327,
     LT = 328,
     MINUS = 329,
     PLUS = 330,
     STR_CONCAT = 331,
     MOD = 332,
     DIVIDE = 333,
     MULTIPLY = 334,
     DCOLON = 335,
     TILT = 336,
     UMINUS = 337,
     INSERT = 338,
     LEN = 339,
     SPLIT = 340,
     COPY = 341,
     ERASE = 342,
     TRACE = 343,
     EXP = 344,
     NAME = 345,
     STRING = 346,
     INT = 347,
     LIST1 = 348,
     LIST = 349,
     TUPLE = 350,
     MAP = 351,
     VAR = 352,
     REBUILD = 353,
     DUPLICATE = 354,
     RANGE = 355,
     REPLACE = 356,
     PERMUTE = 357,
     RESTORE = 358,
     SAVE = 359,
     CLEAR = 360,
     APPLY = 361,
     DELAY = 362,
     COLON = 363,
     CDR = 364,
     CAR = 365,
     EMPTY = 366,
     ANY = 367,
     ICONST = 368,
     ID = 369,
     DOT2 = 370,
     DOT = 371,
     POND = 372,
     RBR = 373,
     LBR = 374,
     RB = 375,
     LB = 376,
     RP = 377,
     LP = 378
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
#define INHERIT 269
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
#define FOREACH_R 299
#define FOREACH 300
#define FOR 301
#define DOT4 302
#define DOT3 303
#define DEFAULT 304
#define SWITCH 305
#define CASE 306
#define ELSE 307
#define IF 308
#define MOD_ASSIGN 309
#define DIVIDE_ASSIGN 310
#define MULT_ASSIGN 311
#define MINUS_ASSIGN 312
#define PLUS_ASSIGN 313
#define ASSIGN 314
#define QUESTION 315
#define COMMA 316
#define TOR 317
#define OR 318
#define AND 319
#define NOT 320
#define DARROW 321
#define ARROW 322
#define NE 323
#define GE 324
#define GT 325
#define EQ 326
#define LE 327
#define LT 328
#define MINUS 329
#define PLUS 330
#define STR_CONCAT 331
#define MOD 332
#define DIVIDE 333
#define MULTIPLY 334
#define DCOLON 335
#define TILT 336
#define UMINUS 337
#define INSERT 338
#define LEN 339
#define SPLIT 340
#define COPY 341
#define ERASE 342
#define TRACE 343
#define EXP 344
#define NAME 345
#define STRING 346
#define INT 347
#define LIST1 348
#define LIST 349
#define TUPLE 350
#define MAP 351
#define VAR 352
#define REBUILD 353
#define DUPLICATE 354
#define RANGE 355
#define REPLACE 356
#define PERMUTE 357
#define RESTORE 358
#define SAVE 359
#define CLEAR 360
#define APPLY 361
#define DELAY 362
#define COLON 363
#define CDR 364
#define CAR 365
#define EMPTY 366
#define ANY 367
#define ICONST 368
#define ID 369
#define DOT2 370
#define DOT 371
#define POND 372
#define RBR 373
#define LBR 374
#define RB 375
#define LB 376
#define RP 377
#define LP 378




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


