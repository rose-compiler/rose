<<<<<<< HEAD
/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
=======
/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.
>>>>>>> 21644f3277badc2c97102315e9b3e454283ff430

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

<<<<<<< HEAD
#ifndef YY_YY_POET_YACC_H_INCLUDED
# define YY_YY_POET_YACC_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
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
    IN = 295,
    SUCH_THAT = 296,
    FOR = 297,
    FOREACH = 298,
    FOREACH_R = 299,
    DO = 300,
    ENDDO = 301,
    BREAK = 302,
    CONTINUE = 303,
    RETURN = 304,
    DEBUG = 305,
    IF = 306,
    ELSE = 307,
    CASE = 308,
    SWITCH = 309,
    DEFAULT = 310,
    DOT3 = 311,
    DOT4 = 312,
    ASSIGN = 313,
    PLUS_ASSIGN = 314,
    MINUS_ASSIGN = 315,
    MULT_ASSIGN = 316,
    DIVIDE_ASSIGN = 317,
    MOD_ASSIGN = 318,
    COMMA = 319,
    QUESTION = 320,
    OR = 321,
    TOR = 322,
    AND = 323,
    NOT = 324,
    LT = 325,
    LE = 326,
    EQ = 327,
    GT = 328,
    GE = 329,
    NE = 330,
    ARROW = 331,
    DARROW = 332,
    PLUS = 333,
    MINUS = 334,
    MULTIPLY = 335,
    DIVIDE = 336,
    MOD = 337,
    STR_CONCAT = 338,
    DCOLON = 339,
    UMINUS = 340,
    TILT = 341,
    DELAY = 342,
    APPLY = 343,
    CLEAR = 344,
    SAVE = 345,
    RESTORE = 346,
    REVERSE = 347,
    PERMUTE = 348,
    REPLACE = 349,
    ASSERT = 350,
    RANGE = 351,
    DUPLICATE = 352,
    REBUILD = 353,
    VAR = 354,
    MAP = 355,
    TUPLE = 356,
    LIST = 357,
    LIST1 = 358,
    INT = 359,
    STRING = 360,
    FLOAT = 361,
    NAME = 362,
    EXP = 363,
    TRACE = 364,
    ERASE = 365,
    COPY = 366,
    SPLIT = 367,
    LEN = 368,
    INSERT = 369,
    CAR = 370,
    CDR = 371,
    COLON = 372,
    ID = 373,
    ICONST = 374,
    FCONST = 375,
    ANY = 376,
    DOT = 377,
    DOT2 = 378,
    POND = 379,
    LB = 380,
    RB = 381,
    LBR = 382,
    RBR = 383,
    LP = 384,
    RP = 385
  };
=======
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
>>>>>>> 21644f3277badc2c97102315e9b3e454283ff430
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
<<<<<<< HEAD
#define IN 295
#define SUCH_THAT 296
#define FOR 297
#define FOREACH 298
#define FOREACH_R 299
#define DO 300
#define ENDDO 301
#define BREAK 302
#define CONTINUE 303
#define RETURN 304
#define DEBUG 305
#define IF 306
#define ELSE 307
#define CASE 308
#define SWITCH 309
#define DEFAULT 310
#define DOT3 311
#define DOT4 312
#define ASSIGN 313
#define PLUS_ASSIGN 314
#define MINUS_ASSIGN 315
#define MULT_ASSIGN 316
#define DIVIDE_ASSIGN 317
#define MOD_ASSIGN 318
#define COMMA 319
#define QUESTION 320
#define OR 321
#define TOR 322
#define AND 323
#define NOT 324
#define LT 325
#define LE 326
#define EQ 327
#define GT 328
#define GE 329
#define NE 330
#define ARROW 331
#define DARROW 332
#define PLUS 333
#define MINUS 334
#define MULTIPLY 335
#define DIVIDE 336
#define MOD 337
#define STR_CONCAT 338
#define DCOLON 339
#define UMINUS 340
#define TILT 341
#define DELAY 342
#define APPLY 343
#define CLEAR 344
#define SAVE 345
#define RESTORE 346
#define REVERSE 347
#define PERMUTE 348
#define REPLACE 349
#define ASSERT 350
#define RANGE 351
#define DUPLICATE 352
#define REBUILD 353
#define VAR 354
#define MAP 355
#define TUPLE 356
#define LIST 357
#define LIST1 358
#define INT 359
#define STRING 360
#define FLOAT 361
#define NAME 362
#define EXP 363
#define TRACE 364
#define ERASE 365
#define COPY 366
#define SPLIT 367
#define LEN 368
#define INSERT 369
#define CAR 370
#define CDR 371
#define COLON 372
#define ID 373
#define ICONST 374
#define FCONST 375
#define ANY 376
#define DOT 377
#define DOT2 378
#define POND 379
#define LB 380
#define RB 381
#define LBR 382
#define RBR 383
#define LP 384
#define RP 385
=======
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



>>>>>>> 21644f3277badc2c97102315e9b3e454283ff430

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
<<<<<<< HEAD
# define YYSTYPE_IS_TRIVIAL 1
=======
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
>>>>>>> 21644f3277badc2c97102315e9b3e454283ff430
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;

<<<<<<< HEAD
int yyparse (void);

#endif /* !YY_YY_POET_YACC_H_INCLUDED  */
=======
>>>>>>> 21644f3277badc2c97102315e9b3e454283ff430
