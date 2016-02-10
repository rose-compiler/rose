/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 26 "poet_yacc.y"

/*
   POET : Parameterized Optimizations for Empirical Tuning
   Copyright (c)  2008,  Qing Yi.  All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
 3. Neither the name of UTSA nor the names of its contributors may be used to
    endorse or promote products derived from this software without specific
    prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISEDOF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*********************************************************************/
/*  Yacc parser for reading POET specefications and building AST     */
/*********************************************************************/
#define YYDEBUG 1
#define YYERROR_VERBOSE
#include <poet_config.h>
#include <assert.h>

YaccState yaccState;

extern void* insert_input();
extern void set_input_debug();
extern void set_input_cond();
extern void set_input_annot();
extern void set_input_target();
extern void set_input_codeType();
extern void set_input_syntax();
extern void set_input_files();
extern void set_input_inline();

extern void* insert_output();
extern void set_output_target();
extern void set_output_syntax();
extern void set_output_file();
extern void set_output_cond();

extern void* insert_code();
extern void set_code_params();
extern void set_code_def();
extern void set_code_parse();
extern void set_code_lookahead();
extern void* set_code_attr();
extern void set_code_inherit();

extern void insert_source(void* code);
extern void* insert_xform();
extern void set_xform_params();
extern void set_xform_def();
extern void* set_local_static(void* id, void* code, LocalVarType t, void* restr, int insert);


extern void insert_eval();
extern void insert_cond();
extern void eval_define();

extern void insert_trace();
extern void* insert_parameter();
extern void set_param_type();
extern void set_param_default();
extern void set_param_parse();
extern void set_param_message();
extern void finish_parameter();
extern void* make_codeMatch();
extern void* make_annot_single();
extern void* make_annot_lbegin();
extern void* make_codeMatchQ();
extern void* make_empty_list();
extern void* make_empty();
extern void* make_any();
extern void* make_seq(void*, void*);
extern void* make_typelist2();
extern void* make_inputlist();
extern void* make_xformList();
extern void* make_sourceString();
extern void* make_sourceAssign();
extern void* make_codeRef();
extern void* make_traceVar();
extern void* negate_Iconst();
extern void* make_Iconst();
extern void* make_Iconst1();
extern void* make_tupleAccess();
extern void* make_type();
extern void* make_typeNot();
extern void* make_typeUop();
extern void* make_typeTor();
extern void* make_sourceQop();
extern void* make_sourceTop();
extern void* make_ifElse();
extern void* make_sourceBop();
extern void* make_sourceUop();
extern void* make_sourceStmt();
extern void* make_dummyOperator();
extern void* make_macroVar();
extern void* make_localPar();
extern void* make_varRef();
extern void* make_attrAccess();
extern void* make_sourceVector();
extern void* make_sourceVector2();
extern void* make_parseOperator();


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 484 "poet_yacc.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2158

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  131
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  284
/* YYNRULES -- Number of rules.  */
#define YYNRULES  512
/* YYNRULES -- Number of states.  */
#define YYNSTATES  869

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   385

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    10,    17,    18,    25,
      26,    32,    33,    39,    44,    45,    46,    54,    55,    56,
      63,    64,    65,    72,    73,    80,    83,    84,    88,    89,
      90,    95,    99,   103,   104,   109,   113,   114,   119,   120,
     125,   126,   131,   132,   137,   138,   142,   143,   144,   149,
     150,   155,   156,   161,   162,   167,   171,   173,   174,   175,
     179,   185,   186,   191,   192,   197,   198,   203,   204,   209,
     213,   214,   219,   220,   225,   227,   229,   231,   235,   238,
     241,   242,   245,   246,   249,   251,   252,   257,   261,   262,
     263,   267,   268,   273,   274,   279,   280,   285,   289,   290,
     291,   300,   304,   308,   310,   315,   319,   325,   327,   329,
     331,   333,   335,   337,   339,   341,   343,   345,   347,   350,
     355,   357,   358,   363,   366,   367,   372,   374,   375,   380,
     381,   385,   387,   390,   393,   394,   399,   400,   405,   406,
     411,   412,   417,   418,   423,   424,   429,   430,   431,   435,
     436,   441,   442,   447,   449,   452,   455,   457,   458,   463,
     464,   469,   472,   473,   478,   479,   484,   485,   490,   491,
     496,   497,   502,   503,   508,   509,   514,   515,   519,   521,
     522,   523,   527,   528,   533,   534,   539,   541,   543,   545,
     547,   548,   549,   557,   558,   559,   568,   569,   570,   579,
     581,   583,   584,   589,   592,   595,   597,   599,   602,   604,
     605,   610,   611,   616,   617,   622,   623,   627,   629,   630,
     635,   636,   641,   642,   643,   647,   649,   651,   652,   657,
     659,   660,   665,   667,   668,   669,   673,   679,   680,   687,
     688,   693,   695,   696,   701,   705,   706,   711,   713,   715,
     716,   721,   723,   727,   731,   732,   736,   738,   740,   741,
     742,   743,   753,   755,   756,   757,   761,   762,   763,   764,
     765,   779,   780,   781,   791,   792,   793,   794,   806,   807,
     808,   809,   810,   824,   825,   826,   827,   828,   842,   843,
     848,   851,   853,   856,   858,   863,   865,   866,   867,   872,
     874,   875,   876,   880,   881,   882,   890,   891,   892,   899,
     900,   906,   908,   910,   911,   916,   917,   922,   924,   926,
     927,   931,   932,   936,   937,   941,   942,   946,   948,   950,
     951,   956,   957,   962,   963,   968,   969,   974,   975,   980,
     981,   986,   987,   992,   993,   998,   999,  1000,  1008,  1009,
    1014,  1015,  1020,  1021,  1025,  1026,  1031,  1032,  1037,  1038,
    1043,  1044,  1049,  1050,  1055,  1056,  1061,  1062,  1067,  1069,
    1071,  1072,  1077,  1078,  1083,  1084,  1089,  1090,  1095,  1096,
    1101,  1102,  1107,  1108,  1113,  1114,  1118,  1119,  1123,  1124,
    1128,  1129,  1130,  1134,  1135,  1136,  1137,  1138,  1148,  1149,
    1150,  1157,  1158,  1159,  1168,  1169,  1170,  1171,  1183,  1184,
    1188,  1189,  1190,  1197,  1198,  1204,  1205,  1206,  1215,  1216,
    1223,  1224,  1230,  1231,  1235,  1236,  1240,  1241,  1245,  1246,
    1247,  1256,  1259,  1260,  1265,  1272,  1274,  1276,  1277,  1282,
    1283,  1288,  1290,  1291,  1295,  1296,  1299,  1300,  1305,  1306,
    1313,  1314,  1321,  1324,  1327,  1330,  1333,  1336,  1337,  1341,
    1342,  1346,  1347,  1351,  1352,  1356,  1359,  1361,  1362,  1367,
    1370,  1372,  1374,  1376,  1378,  1379,  1383,  1384,  1389,  1390,
    1391,  1392,  1399,  1401,  1402,  1406,  1407,  1411,  1412,  1413,
    1418,  1419,  1423,  1424,  1428,  1429,  1430,  1435,  1436,  1440,
    1442,  1444,  1446,  1447,  1452,  1456,  1457,  1462,  1463,  1468,
    1469,  1474,  1475
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     132,     0,    -1,   133,    -1,    -1,   133,   134,    -1,    -1,
      23,     6,   121,   135,   178,    25,    -1,    -1,    23,     7,
     121,   136,   232,    25,    -1,    -1,    23,     8,   137,   262,
      25,    -1,    -1,    23,    15,   138,   262,    25,    -1,    23,
      92,   261,    25,    -1,    -1,    -1,    23,    32,   121,   139,
     163,   140,   246,    -1,    -1,    -1,    23,    37,   141,   146,
     142,   162,    -1,    -1,    -1,    23,    28,   143,   155,   144,
      25,    -1,    -1,    23,    27,   121,   145,   250,   248,    -1,
     377,    12,    -1,    -1,   148,   147,   146,    -1,    -1,    -1,
      40,    63,   149,   386,    -1,    17,    63,    20,    -1,    17,
      63,   121,    -1,    -1,    21,    63,   150,   386,    -1,    29,
      63,    20,    -1,    -1,    29,    63,   151,   232,    -1,    -1,
      18,    63,   152,   386,    -1,    -1,    16,    63,   153,   386,
      -1,    -1,    15,    63,   154,   386,    -1,    -1,   157,   156,
     155,    -1,    -1,    -1,    18,    63,   158,   386,    -1,    -1,
      21,    63,   159,   386,    -1,    -1,    17,    63,   160,   386,
      -1,    -1,    15,    63,   161,   386,    -1,    24,   377,    38,
      -1,    25,    -1,    -1,    -1,   165,   164,   163,    -1,    26,
      63,   130,   176,   129,    -1,    -1,   121,    63,   166,   386,
      -1,    -1,    15,    63,   167,   386,    -1,    -1,   103,    63,
     168,   386,    -1,    -1,    29,    63,   169,   232,    -1,    30,
      63,   120,    -1,    -1,    31,    63,   170,   172,    -1,    -1,
      28,    63,   171,   386,    -1,   388,    -1,    29,    -1,   121,
      -1,    33,   123,   121,    -1,    27,   174,    -1,    32,   175,
      -1,    -1,   123,   121,    -1,    -1,   123,   121,    -1,   121,
      -1,    -1,   121,   115,   177,   232,    -1,   176,    65,   176,
      -1,    -1,    -1,   178,   179,   180,    -1,    -1,    19,    63,
     181,   194,    -1,    -1,    53,    63,   182,   386,    -1,    -1,
      29,    63,   183,   232,    -1,    22,    63,    34,    -1,    -1,
      -1,   101,   130,   185,   192,    65,   186,   192,   129,    -1,
     101,   130,   129,    -1,   101,   128,   127,    -1,   101,    -1,
     101,   128,   187,   127,    -1,   386,    71,   386,    -1,   187,
      65,   386,    71,   386,    -1,   224,    -1,   118,    -1,    97,
      -1,    95,    -1,    96,    -1,    94,    -1,    93,    -1,   102,
      -1,   100,    -1,   184,    -1,   188,    -1,    78,   120,    -1,
       8,   130,   262,   129,    -1,   396,    -1,    -1,   121,    63,
     190,   189,    -1,    85,   192,    -1,    -1,   130,   191,   194,
     129,    -1,   189,    -1,    -1,   192,    66,   193,   192,    -1,
      -1,   192,   195,   202,    -1,   204,    -1,   192,    51,    -1,
     192,    52,    -1,    -1,   189,   122,   196,   189,    -1,    -1,
     192,    79,   197,   192,    -1,    -1,   192,    83,   198,   192,
      -1,    -1,   192,    82,   199,   192,    -1,    -1,   192,    81,
     200,   192,    -1,    -1,   192,    84,   201,   192,    -1,    -1,
      -1,   192,   203,   202,    -1,    -1,   192,    65,   205,   192,
      -1,    -1,   204,    65,   206,   192,    -1,   188,    -1,    78,
     120,    -1,   112,   121,    -1,   401,    -1,    -1,   130,   208,
     217,   129,    -1,    -1,   207,    66,   209,   207,    -1,   207,
      52,    -1,    -1,   207,   122,   210,   207,    -1,    -1,   207,
      79,   211,   207,    -1,    -1,   207,    83,   212,   207,    -1,
      -1,   207,    82,   213,   207,    -1,    -1,   207,    81,   214,
     207,    -1,    -1,   207,    84,   215,   207,    -1,    -1,   121,
      63,   216,   207,    -1,    -1,   207,   218,   219,    -1,   221,
      -1,    -1,    -1,   207,   220,   219,    -1,    -1,   207,    65,
     222,   207,    -1,    -1,   221,    65,   223,   207,    -1,   120,
      -1,   119,    -1,    34,    -1,     3,    -1,    -1,    -1,   100,
     130,   226,   234,   227,   243,   129,    -1,    -1,    -1,    99,
     130,   228,   234,    65,   229,   245,   129,    -1,    -1,    -1,
      98,   130,   230,   234,    65,   231,   245,   129,    -1,   225,
      -1,   189,    -1,    -1,   232,    66,   233,   232,    -1,   232,
      51,    -1,   232,    52,    -1,   188,    -1,   396,    -1,    85,
     192,    -1,   225,    -1,    -1,   121,    63,   235,   234,    -1,
      -1,   234,    66,   236,   234,    -1,    -1,   130,   237,   238,
     129,    -1,    -1,   234,   239,   243,    -1,   240,    -1,    -1,
     234,    65,   241,   234,    -1,    -1,   240,    65,   242,   234,
      -1,    -1,    -1,   234,   244,   243,    -1,   224,    -1,   173,
      -1,    -1,    24,   247,   262,    35,    -1,    25,    -1,    -1,
      24,   249,   262,    36,    -1,    36,    -1,    -1,    -1,   252,
     251,   250,    -1,    26,    63,   130,   255,   129,    -1,    -1,
      28,    63,   130,   253,   257,   129,    -1,    -1,   121,    63,
     254,   192,    -1,   121,    -1,    -1,   121,   115,   256,   192,
      -1,   255,    65,   255,    -1,    -1,   259,    65,   258,   257,
      -1,   259,    -1,   121,    -1,    -1,   121,   115,   260,   192,
      -1,   121,    -1,   121,    63,   386,    -1,   121,    65,   261,
      -1,    -1,   264,   263,   262,    -1,   264,    -1,   301,    -1,
      -1,    -1,    -1,    57,   130,   265,   310,   129,   266,   270,
     267,   268,    -1,   270,    -1,    -1,    -1,    56,   269,   264,
      -1,    -1,    -1,    -1,    -1,    48,   130,   271,   290,    39,
     272,   290,    39,   273,   290,   129,   274,   270,    -1,    -1,
      -1,    54,   130,   275,   262,   129,   128,   276,   295,   127,
      -1,    -1,    -1,    -1,    47,   277,   291,    50,   278,   289,
     293,    45,   279,   262,    44,    -1,    -1,    -1,    -1,    -1,
      47,   130,   280,   332,   115,   281,   207,   115,   282,   310,
     129,   283,   270,    -1,    -1,    -1,    -1,    -1,    46,   130,
     284,   332,   115,   285,   207,   115,   286,   310,   129,   287,
     270,    -1,    -1,   128,   288,   262,   127,    -1,   128,   127,
      -1,    39,    -1,   305,    39,    -1,   332,    -1,   109,   130,
     332,   129,    -1,   310,    -1,    -1,    -1,   121,    63,   292,
     207,    -1,   401,    -1,    -1,    -1,    49,   294,   310,    -1,
      -1,    -1,   295,    55,   296,   207,   115,   297,   262,    -1,
      -1,    -1,    55,   298,   207,   115,   299,   262,    -1,    -1,
     295,    53,   115,   300,   262,    -1,   302,    -1,   305,    -1,
      -1,   302,    65,   303,   305,    -1,    -1,   305,    65,   304,
     305,    -1,    42,    -1,    43,    -1,    -1,    41,   306,   310,
      -1,    -1,     4,   307,   386,    -1,    -1,   106,   308,   386,
      -1,    -1,    11,   309,   386,    -1,   310,    -1,   321,    -1,
      -1,   347,    63,   311,   310,    -1,    -1,   347,    71,   312,
     234,    -1,    -1,   347,    62,   313,   321,    -1,    -1,   347,
      61,   314,   321,    -1,    -1,   347,    60,   315,   321,    -1,
      -1,   347,    59,   316,   321,    -1,    -1,   347,    58,   317,
     321,    -1,    -1,   347,    70,   318,   234,    -1,    -1,    -1,
     321,    64,   319,   332,   115,   320,   310,    -1,    -1,   321,
      68,   322,   321,    -1,    -1,   321,    67,   323,   321,    -1,
      -1,    69,   324,   332,    -1,    -1,   332,    77,   325,   332,
      -1,    -1,   332,    76,   326,   332,    -1,    -1,   332,    75,
     327,   332,    -1,    -1,   332,    74,   328,   332,    -1,    -1,
     332,    73,   329,   332,    -1,    -1,   332,    72,   330,   332,
      -1,    -1,   332,   115,   331,   207,    -1,   332,    -1,   341,
      -1,    -1,   332,    79,   333,   332,    -1,    -1,   332,    78,
     334,   332,    -1,    -1,   332,    83,   335,   332,    -1,    -1,
     332,    80,   336,   332,    -1,    -1,   332,    82,   337,   332,
      -1,    -1,   332,    81,   338,   332,    -1,    -1,   332,    84,
     339,   332,    -1,    -1,    78,   340,   332,    -1,    -1,   347,
     342,   343,    -1,    -1,   347,   344,   345,    -1,    -1,    -1,
     347,   346,   345,    -1,    -1,    -1,    -1,    -1,   107,   130,
     348,   321,    65,   349,   321,   350,   372,    -1,    -1,    -1,
      92,   130,   351,   321,   352,   372,    -1,    -1,    -1,   108,
     130,   353,   321,    65,   354,   321,   129,    -1,    -1,    -1,
      -1,   104,   130,   355,   321,    65,   356,   321,    65,   357,
     321,   129,    -1,    -1,    90,   358,   386,    -1,    -1,    -1,
      91,   130,   359,   321,   360,   374,    -1,    -1,   114,   128,
     361,   262,   127,    -1,    -1,    -1,    87,   130,   362,   321,
      65,   363,   321,   129,    -1,    -1,    40,   376,   128,   364,
     262,   127,    -1,    -1,   113,   128,   365,   262,   127,    -1,
      -1,   103,   366,   386,    -1,    -1,   110,   367,   386,    -1,
      -1,   111,   368,   386,    -1,    -1,    -1,    89,   130,   369,
     321,    65,   370,   321,   129,    -1,   386,    52,    -1,    -1,
     386,   122,   371,   386,    -1,   105,   130,   386,    65,   386,
     129,    -1,   381,    -1,   129,    -1,    -1,    65,   373,   321,
     129,    -1,    -1,    65,   375,   321,   129,    -1,   129,    -1,
      -1,   126,   120,   125,    -1,    -1,   377,    13,    -1,    -1,
     377,    71,   378,   234,    -1,    -1,   377,     9,   130,   379,
     234,   129,    -1,    -1,   377,    10,   130,   380,   234,   129,
      -1,   377,   120,    -1,   377,   119,    -1,   377,    34,    -1,
     377,    14,    -1,   377,   121,    -1,    -1,   117,   382,   386,
      -1,    -1,   116,   383,   386,    -1,    -1,    88,   384,   386,
      -1,    -1,    97,   385,   386,    -1,   112,   121,    -1,   388,
      -1,    -1,   130,   387,   262,   129,    -1,   130,   129,    -1,
     224,    -1,   118,    -1,   173,    -1,   184,    -1,    -1,   386,
     389,   391,    -1,    -1,   388,    66,   390,   388,    -1,    -1,
      -1,    -1,   126,   392,   406,   125,   393,   391,    -1,   394,
      -1,    -1,   124,   395,   386,    -1,    -1,   173,   397,   398,
      -1,    -1,    -1,   126,   399,   409,   125,    -1,    -1,   124,
     400,   192,    -1,    -1,   173,   402,   403,    -1,    -1,    -1,
     126,   404,   412,   125,    -1,    -1,   124,   405,   207,    -1,
     321,    -1,   409,    -1,   407,    -1,    -1,   407,    66,   408,
     407,    -1,   121,   123,   121,    -1,    -1,   121,    63,   410,
     321,    -1,    -1,   409,    39,   411,   409,    -1,    -1,   121,
      63,   413,   192,    -1,    -1,   412,    39,   414,   412,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   146,   146,   148,   149,   151,   151,   153,   153,   155,
     155,   156,   156,   157,   158,   158,   158,   159,   159,   159,
     160,   160,   160,   161,   161,   163,   165,   165,   166,   167,
     167,   168,   169,   170,   170,   171,   172,   172,   174,   174,
     175,   175,   176,   176,   178,   178,   179,   180,   180,   182,
     182,   184,   184,   186,   186,   188,   189,   191,   191,   191,
     192,   193,   193,   194,   194,   196,   196,   198,   198,   200,
     201,   201,   203,   203,   206,   207,   209,   210,   211,   212,
     214,   215,   217,   218,   221,   222,   222,   223,   225,   225,
     225,   226,   226,   228,   228,   229,   229,   230,   233,   234,
     233,   236,   237,   238,   239,   242,   244,   247,   248,   249,
     250,   251,   252,   253,   254,   255,   256,   258,   259,   260,
     261,   262,   262,   264,   265,   265,   267,   268,   268,   271,
     271,   272,   273,   274,   275,   275,   277,   277,   279,   279,
     281,   281,   283,   283,   284,   284,   286,   287,   287,   288,
     288,   290,   290,   292,   293,   294,   295,   296,   296,   297,
     297,   299,   300,   300,   302,   302,   304,   304,   306,   306,
     308,   308,   310,   310,   311,   311,   313,   313,   314,   316,
     317,   317,   318,   318,   320,   320,   322,   323,   324,   325,
     327,   327,   327,   328,   328,   328,   329,   329,   329,   331,
     332,   333,   333,   335,   336,   338,   339,   340,   341,   342,
     342,   344,   344,   346,   346,   348,   348,   349,   350,   350,
     352,   352,   353,   354,   354,   356,   357,   359,   359,   360,
     361,   361,   362,   363,   363,   363,   364,   365,   365,   366,
     366,   367,   368,   368,   369,   370,   370,   370,   371,   372,
     372,   373,   374,   375,   378,   378,   379,   380,   381,   381,
     381,   381,   383,   384,   385,   385,   386,   387,   388,   388,
     386,   390,   390,   390,   392,   392,   392,   392,   394,   395,
     396,   396,   394,   398,   399,   400,   400,   398,   402,   402,
     403,   404,   405,   407,   408,   410,   411,   414,   414,   416,
     418,   419,   419,   421,   421,   421,   423,   423,   423,   424,
     424,   427,   428,   430,   430,   431,   431,   434,   435,   436,
     436,   437,   437,   438,   438,   439,   439,   440,   442,   443,
     443,   444,   444,   445,   445,   447,   447,   449,   449,   451,
     451,   453,   453,   455,   455,   456,   456,   456,   460,   460,
     461,   461,   462,   462,   463,   463,   464,   464,   465,   465,
     466,   466,   468,   468,   470,   470,   472,   472,   473,   475,
     476,   476,   478,   478,   480,   480,   482,   482,   484,   484,
     486,   486,   488,   488,   489,   489,   491,   491,   494,   494,
     495,   496,   496,   497,   499,   499,   499,   499,   502,   502,
     502,   505,   505,   505,   507,   507,   507,   507,   509,   509,
     510,   511,   510,   513,   513,   514,   514,   514,   516,   516,
     518,   518,   519,   519,   520,   520,   521,   521,   522,   522,
     522,   524,   525,   525,   527,   529,   530,   530,   530,   532,
     532,   533,   535,   536,   538,   539,   540,   540,   541,   541,
     543,   543,   545,   546,   547,   548,   549,   552,   552,   553,
     553,   554,   554,   555,   555,   556,   557,   560,   560,   561,
     562,   563,   564,   565,   567,   567,   568,   568,   569,   570,
     571,   570,   573,   575,   575,   578,   578,   580,   581,   581,
     583,   583,   586,   586,   587,   588,   588,   590,   590,   593,
     594,   595,   597,   597,   599,   602,   602,   604,   604,   608,
     608,   610,   610
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ENDLIST", "ERROR", "SYNERROR",
  "PARAMETER", "DEFINE", "EVAL", "LBEGIN", "RBEGIN", "PRINT",
  "INPUT_ENDFILE", "INPUT_ENDL", "INPUT_CONT", "COND", "ANNOT", "TO",
  "FROM", "TYPE", "POETTYPE", "SYNTAX", "MESSAGE", "LTAG", "RTAG",
  "ENDTAG", "PARS", "XFORM", "OUTPUT", "PARSE", "LOOKAHEAD", "MATCH",
  "CODE", "GLOBAL", "SOURCE", "ENDCODE", "ENDXFORM", "INPUT", "ENDINPUT",
  "SEMICOLON", "DEBUG", "RETURN", "CONTINUE", "BREAK", "ENDDO", "DO",
  "FOREACH_R", "FOREACH", "FOR", "SUCH_THAT", "IN", "DOT4", "DOT3",
  "DEFAULT", "SWITCH", "CASE", "ELSE", "IF", "MOD_ASSIGN", "DIVIDE_ASSIGN",
  "MULT_ASSIGN", "MINUS_ASSIGN", "PLUS_ASSIGN", "ASSIGN", "QUESTION",
  "COMMA", "TOR", "OR", "AND", "NOT", "DARROW", "ARROW", "NE", "GE", "GT",
  "EQ", "LE", "LT", "MINUS", "PLUS", "STR_CONCAT", "MOD", "DIVIDE",
  "MULTIPLY", "DCOLON", "TILT", "UMINUS", "INSERT", "LEN", "SPLIT", "COPY",
  "ERASE", "TRACE", "EXP", "NAME", "FLOAT", "STRING", "INT", "LIST1",
  "LIST", "TUPLE", "MAP", "VAR", "REBUILD", "DUPLICATE", "RANGE", "ASSERT",
  "REPLACE", "PERMUTE", "REVERSE", "RESTORE", "SAVE", "CLEAR", "APPLY",
  "DELAY", "COLON", "CDR", "CAR", "ANY", "FCONST", "ICONST", "ID", "DOT2",
  "DOT", "POND", "RBR", "LBR", "RB", "LB", "RP", "LP", "$accept", "poet",
  "sections", "section", "@1", "@2", "@3", "@4", "@5", "@6", "@7", "@8",
  "@9", "@10", "@11", "inputAttrs", "@12", "inputAttr", "@13", "@14",
  "@15", "@16", "@17", "@18", "outputAttrs", "@19", "outputAttr", "@20",
  "@21", "@22", "@23", "inputRHS", "codeAttrs", "@24", "codeAttr", "@25",
  "@26", "@27", "@28", "@29", "@30", "matchSpec", "varRef", "xformRef",
  "codeRef", "codePars", "@31", "paramAttrs", "@32", "paramAttr", "@33",
  "@34", "@35", "map", "@36", "@37", "mapEntries", "singletype",
  "typeSpec1", "@38", "@39", "typeSpec", "@40", "typeMulti", "@41", "@42",
  "@43", "@44", "@45", "@46", "@47", "typeList", "@48", "typeTuple", "@49",
  "@50", "patternSpec", "@51", "@52", "@53", "@54", "@55", "@56", "@57",
  "@58", "@59", "patternMulti", "@60", "patternList", "@61",
  "patternTuple", "@62", "@63", "constant", "parseType1", "@64", "@65",
  "@66", "@67", "@68", "@69", "parseType", "@70", "parseElem", "@71",
  "@72", "@73", "parseElemMulti", "@74", "parseElemTuple", "@75", "@76",
  "parseElemList", "@77", "constantOrVar", "codeRHS", "@78", "xformRHS",
  "@79", "xformAttrs", "@80", "xformAttr", "@81", "@82", "xformPars",
  "@83", "outputPars", "@84", "outputPar", "@85", "traceVars", "code",
  "@86", "codeIf", "@87", "@88", "@89", "codeIfHelp", "@90", "code1",
  "@91", "@92", "@93", "@94", "@95", "@96", "@97", "@98", "@99", "@100",
  "@101", "@102", "@103", "@104", "@105", "@106", "@107", "@108",
  "foreach_body", "e_code4", "patternAssign", "@109", "suchThat", "@110",
  "switchHelp", "@111", "@112", "@113", "@114", "@115", "code23", "code2",
  "@116", "@117", "code3", "@118", "@119", "@120", "@121", "code4", "@122",
  "@123", "@124", "@125", "@126", "@127", "@128", "@129", "@130", "@131",
  "code5", "@132", "@133", "@134", "@135", "@136", "@137", "@138", "@139",
  "@140", "@141", "code6", "@142", "@143", "@144", "@145", "@146", "@147",
  "@148", "@149", "code7", "@150", "code7Help", "@151", "code7Help2",
  "@152", "code8", "@153", "@154", "@155", "@156", "@157", "@158", "@159",
  "@160", "@161", "@162", "@163", "@164", "@165", "@166", "@167", "@168",
  "@169", "@170", "@171", "@172", "@173", "@174", "@175", "@176",
  "paramRHS", "@177", "eraseRHS", "@178", "debugConfig", "inputBase",
  "@179", "@180", "@181", "code9", "@182", "@183", "@184", "@185",
  "codeUnit", "@186", "varInvoke", "@187", "@188", "varInvoke1", "@189",
  "@190", "pond", "@191", "varInvokeType", "@192", "varInvokeType1",
  "@193", "@194", "varInvokePattern", "@195", "varInvokePattern1", "@196",
  "@197", "configOrSubRef", "selectRef", "@198", "configRef", "@199",
  "@200", "xformConfig", "@201", "@202", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   131,   132,   133,   133,   135,   134,   136,   134,   137,
     134,   138,   134,   134,   139,   140,   134,   141,   142,   134,
     143,   144,   134,   145,   134,   134,   147,   146,   146,   149,
     148,   148,   148,   150,   148,   148,   151,   148,   152,   148,
     153,   148,   154,   148,   156,   155,   155,   158,   157,   159,
     157,   160,   157,   161,   157,   162,   162,   163,   164,   163,
     165,   166,   165,   167,   165,   168,   165,   169,   165,   165,
     170,   165,   171,   165,   172,   172,   173,   173,   173,   173,
     174,   174,   175,   175,   176,   177,   176,   176,   178,   179,
     178,   181,   180,   182,   180,   183,   180,   180,   185,   186,
     184,   184,   184,   184,   184,   187,   187,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   189,   189,   189,
     189,   190,   189,   189,   191,   189,   192,   193,   192,   195,
     194,   194,   194,   194,   196,   194,   197,   194,   198,   194,
     199,   194,   200,   194,   201,   194,   202,   203,   202,   205,
     204,   206,   204,   207,   207,   207,   207,   208,   207,   209,
     207,   207,   210,   207,   211,   207,   212,   207,   213,   207,
     214,   207,   215,   207,   216,   207,   218,   217,   217,   219,
     220,   219,   222,   221,   223,   221,   224,   224,   224,   224,
     226,   227,   225,   228,   229,   225,   230,   231,   225,   232,
     232,   233,   232,   232,   232,   234,   234,   234,   234,   235,
     234,   236,   234,   237,   234,   239,   238,   238,   241,   240,
     242,   240,   243,   244,   243,   245,   245,   247,   246,   246,
     249,   248,   248,   250,   251,   250,   252,   253,   252,   254,
     252,   255,   256,   255,   255,   258,   257,   257,   259,   260,
     259,   261,   261,   261,   263,   262,   262,   262,   265,   266,
     267,   264,   264,   268,   269,   268,   271,   272,   273,   274,
     270,   275,   276,   270,   277,   278,   279,   270,   280,   281,
     282,   283,   270,   284,   285,   286,   287,   270,   288,   270,
     270,   270,   270,   289,   289,   290,   290,   292,   291,   291,
     293,   294,   293,   296,   297,   295,   298,   299,   295,   300,
     295,   301,   301,   303,   302,   304,   302,   305,   305,   306,
     305,   307,   305,   308,   305,   309,   305,   305,   310,   311,
     310,   312,   310,   313,   310,   314,   310,   315,   310,   316,
     310,   317,   310,   318,   310,   319,   320,   310,   322,   321,
     323,   321,   324,   321,   325,   321,   326,   321,   327,   321,
     328,   321,   329,   321,   330,   321,   331,   321,   321,   332,
     333,   332,   334,   332,   335,   332,   336,   332,   337,   332,
     338,   332,   339,   332,   340,   332,   342,   341,   344,   343,
     343,   346,   345,   345,   348,   349,   350,   347,   351,   352,
     347,   353,   354,   347,   355,   356,   357,   347,   358,   347,
     359,   360,   347,   361,   347,   362,   363,   347,   364,   347,
     365,   347,   366,   347,   367,   347,   368,   347,   369,   370,
     347,   347,   371,   347,   347,   347,   372,   373,   372,   375,
     374,   374,   376,   376,   377,   377,   378,   377,   379,   377,
     380,   377,   377,   377,   377,   377,   377,   382,   381,   383,
     381,   384,   381,   385,   381,   381,   381,   387,   386,   386,
     386,   386,   386,   386,   389,   388,   390,   388,   391,   392,
     393,   391,   391,   395,   394,   397,   396,   398,   399,   398,
     400,   398,   402,   401,   403,   404,   403,   405,   403,   406,
     406,   406,   408,   407,   407,   410,   409,   411,   409,   413,
     412,   414,   412
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     0,     6,     0,     6,     0,
       5,     0,     5,     4,     0,     0,     7,     0,     0,     6,
       0,     0,     6,     0,     6,     2,     0,     3,     0,     0,
       4,     3,     3,     0,     4,     3,     0,     4,     0,     4,
       0,     4,     0,     4,     0,     3,     0,     0,     4,     0,
       4,     0,     4,     0,     4,     3,     1,     0,     0,     3,
       5,     0,     4,     0,     4,     0,     4,     0,     4,     3,
       0,     4,     0,     4,     1,     1,     1,     3,     2,     2,
       0,     2,     0,     2,     1,     0,     4,     3,     0,     0,
       3,     0,     4,     0,     4,     0,     4,     3,     0,     0,
       8,     3,     3,     1,     4,     3,     5,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     4,
       1,     0,     4,     2,     0,     4,     1,     0,     4,     0,
       3,     1,     2,     2,     0,     4,     0,     4,     0,     4,
       0,     4,     0,     4,     0,     4,     0,     0,     3,     0,
       4,     0,     4,     1,     2,     2,     1,     0,     4,     0,
       4,     2,     0,     4,     0,     4,     0,     4,     0,     4,
       0,     4,     0,     4,     0,     4,     0,     3,     1,     0,
       0,     3,     0,     4,     0,     4,     1,     1,     1,     1,
       0,     0,     7,     0,     0,     8,     0,     0,     8,     1,
       1,     0,     4,     2,     2,     1,     1,     2,     1,     0,
       4,     0,     4,     0,     4,     0,     3,     1,     0,     4,
       0,     4,     0,     0,     3,     1,     1,     0,     4,     1,
       0,     4,     1,     0,     0,     3,     5,     0,     6,     0,
       4,     1,     0,     4,     3,     0,     4,     1,     1,     0,
       4,     1,     3,     3,     0,     3,     1,     1,     0,     0,
       0,     9,     1,     0,     0,     3,     0,     0,     0,     0,
      13,     0,     0,     9,     0,     0,     0,    11,     0,     0,
       0,     0,    13,     0,     0,     0,     0,    13,     0,     4,
       2,     1,     2,     1,     4,     1,     0,     0,     4,     1,
       0,     0,     3,     0,     0,     7,     0,     0,     6,     0,
       5,     1,     1,     0,     4,     0,     4,     1,     1,     0,
       3,     0,     3,     0,     3,     0,     3,     1,     1,     0,
       4,     0,     4,     0,     4,     0,     4,     0,     4,     0,
       4,     0,     4,     0,     4,     0,     0,     7,     0,     4,
       0,     4,     0,     3,     0,     4,     0,     4,     0,     4,
       0,     4,     0,     4,     0,     4,     0,     4,     1,     1,
       0,     4,     0,     4,     0,     4,     0,     4,     0,     4,
       0,     4,     0,     4,     0,     3,     0,     3,     0,     3,
       0,     0,     3,     0,     0,     0,     0,     9,     0,     0,
       6,     0,     0,     8,     0,     0,     0,    11,     0,     3,
       0,     0,     6,     0,     5,     0,     0,     8,     0,     6,
       0,     5,     0,     3,     0,     3,     0,     3,     0,     0,
       8,     2,     0,     4,     6,     1,     1,     0,     4,     0,
       4,     1,     0,     3,     0,     2,     0,     4,     0,     6,
       0,     6,     2,     2,     2,     2,     2,     0,     3,     0,
       3,     0,     3,     0,     3,     2,     1,     0,     4,     2,
       1,     1,     1,     1,     0,     3,     0,     4,     0,     0,
       0,     6,     1,     0,     3,     0,     3,     0,     0,     4,
       0,     3,     0,     3,     0,     0,     4,     0,     3,     1,
       1,     1,     0,     4,     3,     0,     4,     0,     4,     0,
       4,     0,     4
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       3,     0,   444,     1,     0,     4,     0,     0,     0,     9,
      11,     0,    20,     0,    17,     0,     0,     0,    25,   445,
     455,   454,   446,   453,   452,   456,     5,     7,     0,     0,
      23,    46,    14,    28,   251,     0,   448,   450,     0,    88,
       0,   189,   321,   325,    80,    82,     0,   188,   291,   442,
     319,   317,   318,     0,   274,     0,     0,     0,   352,   384,
       0,   461,     0,   408,     0,     0,   463,   103,   422,     0,
       0,   323,     0,     0,   424,   426,     0,     0,     0,   459,
     457,   471,   187,   186,    76,   288,   467,   472,   473,   470,
       0,   254,   262,   257,   311,   312,   327,   328,   368,   369,
     386,   435,   474,   466,     0,   233,     0,     0,     0,     0,
      21,    44,    57,     0,     0,     0,     0,     0,     0,     0,
      18,    26,     0,     0,    13,     0,     0,     0,   113,   112,
     110,   111,   109,     0,     0,   115,   114,   108,    76,   213,
     485,   116,   205,   107,   208,   447,   206,    89,     0,     0,
       0,    76,   124,   117,   200,   199,     0,   120,     0,     0,
       0,    78,     0,    79,     0,     0,     0,     0,   283,   278,
       0,   266,   271,   258,     0,     0,   415,     0,   428,     0,
     410,   398,     0,     0,    98,     0,   404,     0,     0,   394,
     401,     0,     0,   465,   420,   413,     0,     0,   290,     0,
     469,     0,    10,     0,   313,   292,   315,   345,   350,   348,
     364,   362,   360,   358,   356,   354,   372,   370,   376,   380,
     378,   374,   382,   366,   341,   339,   337,   335,   333,   329,
     343,   331,   390,   431,   432,   478,   476,    12,     0,     0,
       0,     0,   234,    53,    51,    47,    49,     0,    46,     0,
       0,     0,     0,     0,     0,     0,     0,    15,    58,    42,
      40,     0,    38,    33,    36,    29,     0,    28,   252,   253,
       0,     0,   115,   126,   207,   196,   193,   190,   209,     0,
     487,   211,     6,     0,     0,   118,   123,   121,     0,     8,
     203,   204,   201,   322,   326,    81,    83,    77,     0,   418,
     320,     0,     0,    76,   492,     0,   299,   296,     0,     0,
     353,   386,   385,     0,   462,     0,   409,     0,     0,   464,
     102,     0,     0,   101,     0,   423,     0,     0,   324,     0,
       0,   425,   427,     0,     0,   460,   458,     0,     0,   255,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   387,   388,     0,
     483,   479,   475,   482,     0,     0,     0,   239,   230,   232,
      24,   233,     0,     0,     0,     0,    22,    45,    63,     0,
      72,    67,     0,    70,    65,    61,     0,    57,     0,     0,
      31,    32,     0,     0,    35,     0,     0,   444,    56,    19,
      27,   449,   451,   127,     0,     0,     0,     0,   215,     0,
     217,   490,   488,   486,     0,     0,     0,     0,     0,    90,
       0,     0,   126,   129,     0,   131,     0,   443,     0,     0,
       0,   297,   494,   275,     0,   295,     0,     0,     0,     0,
     411,   399,     0,   104,     0,     0,     0,     0,     0,     0,
       0,     0,   289,   468,   314,   316,     0,   351,   349,   365,
     363,   361,   359,   357,   355,   373,   371,   377,   381,   379,
     375,   383,     0,     0,    76,   157,   153,   367,   156,   342,
     340,   338,   336,   334,   330,   344,   332,   393,   433,     0,
       0,   474,   477,     0,   237,     0,     0,   235,    54,    52,
      48,    50,     0,     0,     0,     0,    69,     0,     0,     0,
     227,   229,    16,    59,    43,    41,    39,    34,    37,    30,
       0,     0,     0,     0,   191,   210,   218,   222,   214,   220,
       0,     0,   212,    91,     0,    95,    93,   119,   122,   134,
     132,   133,   149,   136,   142,   140,   138,   144,   146,   125,
     151,   202,     0,   284,   279,     0,   497,   495,   493,     0,
     267,     0,   259,   416,   429,     0,     0,     0,   105,    99,
     405,     0,   395,   402,   421,   414,   346,   154,   155,   174,
       0,   161,   159,   164,   170,   168,   166,   172,   162,   389,
     391,   484,    76,   499,     0,   501,   500,   241,     0,     0,
     240,     0,    64,    84,     0,    73,    68,    75,    71,    74,
      66,    62,     0,    55,   128,   197,   194,   222,     0,   223,
     216,     0,   491,     0,     0,     0,    97,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   147,   130,     0,   419,
       0,     0,   298,     0,     0,     0,   300,   293,   296,   272,
       0,     0,     0,   439,   441,   412,   437,   436,   400,     0,
       0,     0,   434,     0,     0,     0,     0,   176,     0,   178,
       0,     0,     0,     0,     0,     0,     0,   393,   505,     0,
     480,   502,   507,   242,     0,   236,   248,     0,   247,   231,
      85,     0,    60,     0,     0,     0,     0,   219,   222,   221,
     489,    92,    96,    94,   135,   150,   137,   143,   141,   139,
     145,   146,   152,     0,     0,   498,     0,     0,     0,   301,
       0,     0,     0,   260,     0,     0,     0,     0,     0,   106,
       0,     0,   396,     0,   347,   175,   182,   179,   158,   184,
     160,   165,   171,   169,   167,   173,   163,   392,     0,   504,
     478,     0,     0,     0,   244,   249,   238,   245,     0,    87,
     228,   226,   225,     0,     0,   192,   224,   148,   285,   280,
     509,   511,   496,     0,     0,   276,   268,   306,     0,   263,
     417,   430,     0,     0,   100,   406,     0,   403,     0,   180,
     177,     0,   506,   481,     0,   503,   508,   243,     0,     0,
      86,   198,   195,     0,     0,     0,     0,   294,   302,     0,
     296,     0,     0,   303,   273,   264,   261,   440,   438,     0,
     397,   183,   179,   185,   250,   246,     0,     0,   510,   512,
       0,     0,     0,   309,     0,     0,     0,   181,   286,   281,
     277,   269,   307,     0,     0,   265,   407,     0,     0,     0,
       0,   310,   304,   287,   282,   270,   308,     0,   305
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     5,    39,    40,    28,    29,   112,   396,
      33,   266,    31,   247,   105,   120,   267,   121,   406,   403,
     405,   402,   399,   398,   110,   248,   111,   384,   385,   383,
     382,   409,   257,   397,   258,   519,   512,   518,   515,   517,
     514,   618,    87,   161,   163,   614,   768,   147,   283,   429,
     635,   638,   637,    88,   324,   670,   321,   153,   273,   431,
     288,   433,   531,   434,   558,   639,   641,   644,   643,   642,
     645,   647,   721,   435,   640,   648,   799,   590,   680,   686,
     681,   684,   683,   682,   685,   676,   678,   747,   800,   832,
     679,   798,   801,    89,   144,   416,   627,   415,   705,   414,
     704,   156,   436,   629,   417,   424,   279,   419,   537,   420,
     628,   631,   630,   708,   773,   522,   622,   380,   506,   241,
     381,   242,   609,   505,   608,   763,   697,   809,   698,   808,
      35,    90,   203,    91,   309,   660,   789,   826,   845,    92,
     307,   658,   820,   859,   308,   732,   170,   569,   819,   302,
     651,   814,   858,   301,   650,   813,   857,   199,   656,   444,
     305,   565,   730,   784,   788,   844,   867,   821,   860,   853,
      93,    94,   340,   341,    95,   167,   158,   188,   159,    96,
     364,   366,   363,   362,   361,   360,   359,   365,   342,   675,
      97,   344,   343,   174,   350,   349,   348,   347,   346,   345,
     358,    98,   352,   351,   356,   353,   355,   354,   357,   175,
      99,   232,   367,   497,   599,   687,   311,   329,   673,   796,
     318,   576,   330,   674,   326,   671,   829,   179,   317,   575,
     334,   313,   661,   438,   333,   185,   191,   192,   315,   662,
     369,   668,   738,   665,   737,   166,     6,    38,   125,   126,
     101,   197,   196,   177,   182,   102,   201,   103,   235,   374,
     372,   500,   760,   373,   499,   157,   280,   423,   541,   540,
     488,   442,   568,   654,   653,   604,   605,   761,   606,   758,
     762,   727,   815,   816
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -659
static const yytype_int16 yypact[] =
{
    -659,    50,    76,  -659,   259,  -659,   239,   -67,   -42,  -659,
    -659,   -34,  -659,   -17,  -659,     0,   -45,     8,  -659,  -659,
    -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  1468,  1468,
    -659,   199,  -659,   560,   132,   125,  -659,  -659,  1220,  -659,
    1755,  -659,  -659,  -659,    30,    54,    63,  -659,  -659,    33,
    -659,  -659,  -659,    72,    75,    85,    88,    95,  -659,  -659,
     115,  -659,   124,  -659,   127,   159,  -659,   -79,  -659,   173,
     177,  -659,   181,   212,  -659,  -659,    45,    66,   119,  -659,
    -659,  -659,  -659,  -659,  -659,   135,   200,  -659,  -659,  -659,
     336,    36,  -659,  -659,   298,    62,  -659,   123,  1251,  -659,
     980,  -659,   -26,   312,   359,   -11,   327,   343,   348,   358,
    -659,  -659,     9,   362,   365,   367,   381,   386,   391,   399,
    -659,  -659,   261,     0,  -659,  1220,  1220,   518,  -659,  -659,
    -659,  -659,  -659,   317,   334,   338,  -659,  -659,   403,  -659,
    -659,  -659,  -659,  -659,  -659,   407,  -659,   450,   357,   368,
     518,   426,  -659,  -659,  -659,  -659,   156,  -659,   261,   261,
     375,  -659,   376,  -659,   385,   371,   379,  1803,  -659,  -659,
      -2,  -659,  -659,  -659,  1993,  1993,  -659,   261,  -659,   261,
    -659,  -659,   261,    25,   369,   261,  -659,   261,   261,  -659,
    -659,   261,   261,  -659,  -659,  -659,   261,   261,  -659,  1468,
    -659,  1468,  -659,  1468,  -659,  -659,  -659,  -659,  -659,  -659,
    -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,
    -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,
    -659,  -659,  2028,  -659,  -659,   166,  -659,  -659,   445,   449,
     451,    64,  -659,  -659,  -659,  -659,  -659,   480,   199,   453,
     455,   456,   457,   459,   460,   461,   462,  -659,  -659,  -659,
    -659,   -14,  -659,  -659,   510,  -659,    68,   560,  -659,  -659,
     -39,    11,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  1220,
     178,  -659,  -659,   370,  1468,  -659,  -659,  -659,   518,  -659,
    -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,   406,  -659,
    -659,  1993,  1993,   478,  -659,   494,  -659,  1803,  1468,  1803,
     237,  -659,   313,  1803,  -659,  1803,  -659,  1803,  1803,  -659,
    -659,     5,   475,  -659,   518,  -659,  1803,   482,  -659,  1803,
    1803,  -659,  -659,  1468,  1468,  -659,  -659,   422,   424,  -659,
    1708,  1708,  1993,  1803,  1803,  1993,  1993,  1993,  1993,  1993,
    1993,  1993,  1993,  1993,  1993,  1993,  1993,  1993,   816,  1803,
    1803,  1803,  1803,  1803,  1803,  1220,  1220,  -659,  -659,   261,
    -659,  -659,  -659,  -659,   261,   425,   427,  -659,  -659,  -659,
    -659,   -11,   261,   261,   261,   261,  -659,  -659,  -659,   430,
    -659,  -659,   434,  -659,  -659,  -659,   247,     9,   261,   261,
    -659,  -659,   261,   261,  -659,  1755,   261,  -659,  -659,  -659,
    -659,  -659,  -659,  -659,  1220,  1220,  1220,  1220,   219,   435,
     491,  -659,  -659,  -659,  1220,   516,   525,   527,   528,  -659,
     444,   518,   470,   914,   464,   530,  1755,  -659,  1468,   697,
     799,  -659,   281,  -659,   559,  -659,   481,   492,   175,   193,
     303,   303,   261,  -659,   261,   377,   210,   261,   267,   318,
     472,   495,  -659,  -659,  -659,  -659,   905,   539,  -659,   237,
     237,   237,   237,   237,   237,   313,   313,   540,   540,   540,
     540,   540,   507,   502,   566,  -659,  -659,   411,  -659,   303,
     303,   303,   303,   303,  -659,   407,   407,  2028,  -659,   261,
    1898,  -659,  -659,   514,  -659,   518,  1468,  -659,  -659,  -659,
    -659,  -659,   261,   519,   261,  1755,  -659,    91,   261,   261,
    -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,   106,  -659,
     442,   518,   405,   419,   407,   407,  -659,  1220,  -659,  -659,
     518,   520,  -659,  -659,   608,  -659,  -659,  -659,  -659,  -659,
    -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,   518,  -659,
    -659,  -659,   517,  -659,  -659,   816,  -659,  -659,  -659,   236,
    -659,   515,  -659,  -659,  -659,   -23,   -12,   574,  -659,  -659,
    -659,   522,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,
     816,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,
    -659,  -659,    19,   303,   521,   588,   618,   545,     2,   543,
     595,   629,  -659,   551,     4,  -659,   106,  -659,  -659,   312,
    -659,  -659,  1468,  -659,  -659,  -659,  -659,  1220,  1220,   407,
    -659,  1220,  -659,   604,   -20,   518,  -659,  1755,   261,   518,
     518,   518,   518,   518,   518,   518,   595,  -659,   518,  -659,
     816,   816,   411,   816,   547,   546,   620,   237,  1803,  -659,
    1588,  1803,  1803,  -659,  -659,  -659,  -659,  -659,  -659,   261,
     518,  1803,  -659,  1803,  1803,  1803,   816,    32,   544,   610,
     816,   816,   816,   816,   816,   816,   816,  2028,  -659,   558,
    -659,  -659,  -659,  -659,   514,  -659,   575,   552,   624,  -659,
    -659,   519,  -659,   657,    41,    41,   568,   407,  1220,   407,
    -659,  -659,   106,  -659,  -659,   595,   595,   595,   595,   595,
     595,   518,   595,   350,   993,  -659,   632,   -16,  1993,  -659,
     653,   660,   645,  -659,   662,    35,    67,  1803,  1803,  -659,
      12,   347,   303,    69,  -659,   420,  -659,   816,  -659,  -659,
     503,   216,   -36,   -36,   -36,   -36,  -659,  -659,  1803,  -659,
     166,   581,   520,   518,  -659,  -659,  -659,  -659,  1755,  -659,
    -659,  -659,  -659,   576,   577,  -659,  -659,  -659,  -659,  -659,
    -659,  -659,  -659,   488,  1803,  -659,  -659,  -659,   -19,   648,
    -659,  -659,   112,   117,  -659,  -659,   -12,  -659,   816,   411,
    -659,   816,   303,  -659,   584,  -659,   618,   595,   518,   543,
     106,  -659,  -659,  1803,  1803,   518,   547,  -659,  -659,  1468,
    1803,   816,   594,  -659,  -659,  -659,  -659,  -659,  -659,  1803,
    -659,   411,   816,   411,   595,  -659,   582,   583,   595,   675,
     673,   589,  1028,  -659,   816,  1468,   121,  -659,  -659,  -659,
    -659,  -659,  -659,  1468,  1038,  -659,  -659,  1588,  1588,  1588,
    1468,  -659,  -659,  -659,  -659,  -659,  -659,  1468,  -659
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,
    -659,  -659,  -659,  -659,  -659,   454,  -659,  -659,  -659,  -659,
    -659,  -659,  -659,  -659,   474,  -659,  -659,  -659,  -659,  -659,
    -659,  -659,   323,  -659,  -659,  -659,  -659,  -659,  -659,  -659,
    -659,  -659,    43,  -659,  -659,    31,  -659,  -659,  -659,  -659,
    -659,  -659,  -659,   720,  -659,  -659,  -659,   590,   -33,  -659,
    -659,  -105,  -659,    96,  -659,  -659,  -659,  -659,  -659,  -659,
    -659,    13,  -659,  -659,  -659,  -659,    87,  -659,  -659,  -659,
    -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,   -99,  -659,
    -659,  -659,  -659,   384,   -32,  -659,  -659,  -659,  -659,  -659,
    -659,  -393,  -659,     3,  -659,  -659,  -659,  -659,  -659,  -659,
    -659,  -659,  -613,  -659,    34,  -659,  -659,  -659,  -659,   360,
    -659,  -659,  -659,  -659,    49,  -659,   -65,  -659,  -659,  -659,
     622,   -25,  -659,   -93,  -659,  -659,  -659,  -659,  -659,  -658,
    -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,
    -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -653,
    -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,
    -659,  -659,  -659,  -659,  -330,  -659,  -659,  -659,  -659,  -158,
    -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,
    -297,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,
    -659,  -119,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,
    -659,  -659,  -659,  -659,    59,  -659,   -28,  -659,  -659,  -659,
    -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,
    -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,  -659,
    -659,   -43,  -659,  -659,  -659,  -659,   352,  -659,  -659,  -659,
    -659,  -659,  -659,  -659,  -659,   785,  -659,  -361,  -659,  -659,
      -6,  -659,  -659,  -659,  -659,    22,  -659,  -659,  -659,  -659,
     585,  -659,  -659,  -659,  -659,  -659,    -4,  -659,  -538,  -659,
    -659,   -55,  -659,  -659
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -257
static const yytype_int16 yytable[] =
{
     100,   100,   733,   634,   104,   731,   400,   154,   155,   300,
     464,   465,   528,   502,   706,   238,   448,   239,   449,   692,
     450,   451,   274,   781,   249,    44,   233,   281,    41,   456,
      45,    46,   458,   459,   822,   250,   823,   251,   252,   253,
     254,   145,   663,   561,    41,   286,   467,   468,   597,   183,
       3,   184,    44,   666,    26,   310,   312,    45,    46,    47,
     146,  -256,   489,   490,   491,   492,   493,   694,    44,   701,
     452,  -256,  -256,    45,    46,    47,    -2,   281,   413,    27,
    -256,   140,   688,   140,   591,    36,   598,    30,   378,  -256,
     411,  -256,   407,   408,    41,   776,   234,   746,   592,     4,
     379,   205,   208,   209,    32,   710,   664,   401,   824,   782,
     240,   593,   255,   594,   595,   596,   597,   667,    44,   303,
     617,    34,   616,    45,    46,    47,    67,   206,   270,   271,
     256,   695,   453,   702,   208,   209,   208,   209,    37,   100,
     412,   794,   689,    81,    82,    83,    84,   146,   146,   445,
     124,   447,   320,   160,   598,    86,   619,   290,   291,   165,
      82,    83,    84,  -256,   790,  -256,   193,   841,   140,   140,
     140,   100,   292,   100,   337,   100,   338,   162,   339,   208,
     209,   289,   439,   440,   208,   209,   164,   207,   208,   209,
     208,   209,    67,   140,   194,   122,   791,   123,   797,   863,
     864,   865,   168,   603,   368,   169,   494,   290,   291,    81,
      82,    83,    84,   304,   106,   171,   107,   108,   172,   455,
     109,    86,   292,   466,   806,   173,   469,   470,   471,   472,
     473,   474,   475,   476,   477,   478,   479,   480,   481,    41,
     573,   827,   208,   209,   712,   176,   828,   195,    16,    17,
     856,    18,    19,    20,   178,   432,   100,   180,   574,   430,
     208,   209,   198,    44,    41,     7,     8,     9,    45,    46,
      47,   520,   521,    21,    10,   580,    49,   208,   209,   100,
     100,   100,   418,   446,   536,   281,    11,    12,    44,   181,
     370,    13,   371,    45,    46,    47,    14,   594,   595,   596,
     597,   146,   421,   186,   422,   100,   100,   187,   460,   461,
      22,   189,   100,   100,    59,   216,   217,   218,   219,   220,
     221,   222,   140,    60,    61,    62,    63,    64,    65,   200,
     734,   140,   582,    66,   208,   209,   100,    67,   598,    68,
      69,    70,   190,    72,    73,   655,    74,    75,    76,    77,
      78,    15,    79,    80,    81,    82,    83,    84,    23,    24,
      25,   202,    67,   204,   735,   736,    86,   140,   495,   496,
     208,   209,   154,   155,   741,   810,   742,   743,   236,    81,
      82,    83,    84,   583,   237,   208,   209,   146,   146,   425,
     243,    86,   426,   218,   219,   220,   221,   222,   548,   427,
     610,   304,   591,   154,   155,   566,   244,   567,   140,   140,
     100,   245,   795,   562,   208,   209,   592,   532,   533,   534,
     535,   246,   143,   428,   143,   259,   624,   542,   260,   593,
     261,   594,   595,   596,   597,   632,   146,   146,   146,   146,
     792,   793,   579,   413,   262,   487,   146,   275,   140,   263,
     657,    16,    17,   646,   264,    19,    20,   140,   140,   140,
     140,   802,   265,   591,   276,   778,   278,   140,   277,   600,
     625,   281,   598,   281,   140,   282,    21,   592,   100,   140,
     623,   611,   154,   155,   626,   281,   592,   284,   285,   287,
     593,   298,   594,   595,   596,   597,   295,   296,   323,   593,
     445,   594,   595,   596,   597,   386,   297,   299,   375,   143,
     143,   143,   376,    22,   377,   734,   388,   744,   389,   390,
     391,    41,   392,   393,   394,   395,   148,   734,   734,   734,
     404,   437,   846,   598,   143,   715,   716,   717,   718,   719,
     720,   441,   598,   722,   443,    44,   454,   457,   140,   462,
      45,    46,    47,   463,   516,   503,   539,   504,   140,   146,
     513,    23,    24,    25,   538,   740,   216,   217,   218,   219,
     220,   221,   222,   547,   140,   113,   114,   115,   116,   543,
     140,   117,   593,   140,   594,   595,   596,   597,   544,   118,
     545,   546,   549,   559,   100,   560,   149,   703,   570,   584,
     119,   140,   432,   150,   154,   155,   714,   209,   304,   783,
     571,   128,   129,   130,   131,   132,   646,   817,   272,    67,
     136,   572,   585,   588,   222,   598,   818,   587,   142,   589,
     100,   707,   100,   304,   709,   607,   137,    82,    83,   151,
     613,   633,   636,   659,   649,   669,   690,   100,   152,   146,
     146,   672,   652,   146,   691,   836,   837,   692,   807,   600,
     693,   413,   445,   143,   696,   699,   700,   688,   726,   729,
     140,   140,   143,   748,   140,   749,   728,   677,   140,   759,
     140,   766,   140,   140,   140,   140,   140,   140,   140,   767,
     765,   140,   770,   304,   304,   780,   304,   775,   785,   786,
     787,   205,   804,   834,   825,   811,   812,   689,   143,   843,
     838,   848,   849,   140,   781,   142,   142,   850,   851,   304,
     523,   410,   387,   304,   304,   304,   304,   304,   304,   304,
     146,   711,   769,   847,   777,   154,   155,   723,   724,   774,
     725,   507,   143,   764,   835,   269,   757,   771,   771,   143,
     143,   140,   855,   830,   803,   306,   100,   805,   141,   530,
     141,   839,     0,   745,   140,     0,     0,   750,   751,   752,
     753,   754,   755,   756,     0,   216,   217,   218,   219,   220,
     221,   222,     0,     0,     0,   100,   100,     0,     0,   143,
     304,   100,   100,     0,   840,     0,     0,     0,   143,   143,
     143,   143,     0,     0,     0,     0,   140,     0,   143,     0,
       0,   140,   563,     0,     0,   143,     0,   100,     0,    41,
     143,     0,     0,     0,     0,   100,     0,     0,   861,   100,
     100,   100,   100,     0,     0,   866,     0,     0,     0,   100,
       0,   304,   868,    44,   304,   141,   141,   141,    45,    46,
      47,   140,     0,     0,     0,     0,     0,     0,   140,     0,
       0,     0,     0,     0,   304,     0,     0,     0,     0,   142,
     141,     0,     0,     0,     0,   304,     0,   216,   217,   218,
     219,   220,   221,   222,     0,   831,     0,   304,   833,   143,
       0,     0,     0,     0,   482,     0,     0,     0,     0,   143,
       0,     0,     0,     0,     0,     0,     0,   268,   842,   128,
     129,   130,   131,   132,   564,   143,   272,    67,   136,     0,
       0,   143,     0,     0,   143,     0,     0,     0,   483,     0,
       0,   854,     0,     0,   137,    82,    83,   484,     0,     0,
       0,     0,   143,   293,   294,     0,   485,     0,   486,   143,
       0,     0,     0,     0,     0,   142,   142,     0,     0,     0,
       0,     0,   314,     0,   316,   550,   551,   319,   322,     0,
     325,     0,   327,   328,   143,     0,   331,   332,     0,   552,
     413,   335,   336,   216,   217,   218,   219,   220,   221,   222,
       0,     0,     0,   553,     0,   554,   555,   556,   557,   141,
       0,     0,     0,     0,   142,   142,   142,   142,   141,     0,
       0,   143,   143,     0,   142,   143,     0,     0,     0,   143,
     586,   143,     0,   143,   143,   143,   143,   143,   143,   143,
       0,     0,   143,     0,   143,   143,     0,   143,   224,   225,
     226,   227,   228,   229,   141,   591,     0,     0,     0,     0,
     230,   231,     0,     0,   143,     0,     0,     0,     0,   592,
     143,     0,     0,     0,   143,   143,   143,   143,   143,   143,
     143,     0,   593,     0,   594,   595,   596,   597,   141,     0,
     591,     0,     0,     0,     0,   141,   141,     0,   772,   772,
     591,     0,   143,     0,   592,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   592,   143,     0,   593,   779,   594,
     595,   596,   597,     0,     0,   598,     0,   593,     0,   594,
     595,   596,   597,     0,     0,   141,     0,   142,     0,     0,
       0,   143,     0,     0,   141,   141,   141,   141,     0,     0,
       0,     0,     0,   852,   141,     0,     0,   143,     0,     0,
     598,   141,   143,   862,   498,   486,   141,     0,     0,   501,
     598,     0,     0,     0,     0,     0,     0,   508,   509,   510,
     511,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     486,     0,   143,   524,   525,   143,     0,   526,   527,     0,
       0,   529,   143,     0,     0,     0,     0,     0,     0,   143,
       0,     0,     0,     0,     0,   143,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   143,   142,   142,     0,
       0,   142,     0,    41,     0,   141,     0,     0,   143,     0,
       0,     0,     0,     0,     0,   141,     0,   577,     0,   578,
     486,   486,   581,   486,     0,     0,     0,    44,     0,     0,
       0,   141,    45,    46,    47,     0,     0,   141,     0,     0,
     141,     0,     0,     0,     0,     0,   486,     0,     0,     0,
     486,   486,   486,   486,   486,   486,   486,     0,   141,     0,
       0,     0,     0,     0,   601,   141,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   612,   142,   615,
       0,     0,   501,   620,   621,   127,     0,     0,     0,     0,
     141,     0,     0,   128,   129,   130,   131,   132,   133,   134,
     135,    67,   136,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,     0,   486,   137,    82,
      83,   138,     0,     0,     0,     0,     0,   141,   141,     0,
     139,   141,     0,     0,     0,   141,     0,   141,     0,   141,
     141,   141,   141,   141,   141,   141,   223,     0,   141,     0,
     141,   141,     0,   141,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   486,     0,
     141,   486,     0,     0,     0,     0,   141,     0,     0,     0,
     141,   141,   141,   141,   141,   141,   141,     0,     0,     0,
       0,   486,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   486,   713,     0,     0,     0,     0,   141,     0,
       0,     0,     0,     0,   486,     0,     0,     0,     0,     0,
       0,   141,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   739,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   141,     0,     0,
       0,    41,    42,     0,     0,     0,     0,     0,     0,    43,
       0,     0,     0,   141,     0,     0,     0,     0,   141,     0,
       0,     0,     0,     0,     0,    44,     0,     0,     0,     0,
      45,    46,    47,     0,     0,     0,     0,    48,    49,    50,
      51,    52,     0,     0,    53,    54,    55,     0,   141,     0,
       0,   141,    56,     0,     0,    57,     0,     0,   141,     0,
       0,     0,     0,     0,     0,   141,     0,    58,     0,     0,
       0,   141,     0,     0,     0,     0,    59,     0,     0,     0,
       0,     0,   141,     0,     0,    60,    61,    62,    63,    64,
      65,     0,     0,     0,   141,    66,     0,     0,     0,    67,
       0,    68,    69,    70,    71,    72,    73,     0,    74,    75,
      76,    77,    78,     0,    79,    80,    81,    82,    83,    84,
       0,    41,    42,     0,     0,     0,    85,     0,    86,    43,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    44,     0,     0,     0,     0,
      45,    46,    47,     0,     0,     0,     0,    48,    49,    50,
      51,    52,     0,     0,    53,    54,    55,     0,     0,     0,
       0,     0,    56,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    58,     0,     0,
       0,     0,     0,     0,     0,     0,    59,     0,     0,     0,
       0,     0,     0,     0,     0,    60,    61,    62,    63,    64,
      65,     0,     0,     0,     0,    66,     0,     0,     0,    67,
       0,    68,    69,    70,    71,    72,    73,     0,    74,    75,
      76,    77,    78,     0,    79,    80,    81,    82,    83,    84,
       0,    41,    42,     0,     0,     0,    85,     0,    86,    43,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    44,     0,     0,     0,     0,
      45,    46,    47,     0,     0,     0,     0,     0,    49,    50,
      51,    52,     0,     0,     0,     0,     0,     0,    41,     0,
       0,     0,     0,   148,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    58,     0,     0,
       0,     0,    44,     0,     0,     0,    59,    45,    46,    47,
       0,     0,     0,     0,     0,    60,    61,    62,    63,    64,
      65,     0,     0,     0,     0,    66,    41,     0,     0,    67,
       0,    68,    69,    70,    71,    72,    73,     0,    74,    75,
      76,    77,    78,     0,    79,    80,    81,    82,    83,    84,
      44,     0,     0,   149,     0,    45,    46,    47,    86,     0,
     150,     0,     0,    49,     0,     0,     0,     0,   128,   129,
     130,   131,   132,   133,   134,   135,    67,   136,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    58,   137,    82,    83,   151,     0,     0,     0,
       0,    59,     0,     0,     0,   152,     0,     0,     0,     0,
      60,    61,    62,    63,    64,    65,     0,     0,     0,     0,
      66,    41,     0,     0,    67,     0,    68,    69,    70,     0,
      72,    73,     0,    74,    75,    76,    77,    78,     0,    79,
      80,    81,    82,    83,    84,    44,     0,     0,     0,     0,
      45,    46,    47,    86,     0,     0,     0,     0,    49,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    58,     0,     0,
       0,     0,     0,     0,     0,     0,    59,     0,     0,     0,
       0,     0,     0,     0,     0,    60,    61,    62,    63,    64,
      65,     0,     0,     0,     0,    66,    41,     0,     0,    67,
       0,    68,    69,    70,     0,    72,    73,     0,    74,    75,
      76,    77,    78,     0,    79,    80,    81,    82,    83,   602,
      44,     0,     0,     0,     0,    45,    46,    47,    86,     0,
       0,    41,     0,    49,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    44,     0,     0,     0,     0,
      45,    46,    47,     0,     0,     0,     0,     0,    49,     0,
       0,    59,     0,     0,     0,     0,     0,     0,     0,     0,
      60,    61,    62,    63,    64,    65,     0,     0,     0,     0,
      66,     0,     0,     0,    67,     0,    68,    69,    70,     0,
      72,    73,     0,    74,    75,    76,    77,    78,     0,    79,
      80,    81,    82,    83,    84,    60,    61,    62,    63,    64,
      65,     0,     0,    86,     0,    66,     0,     0,     0,    67,
       0,    68,    69,    70,     0,    72,    73,     0,    74,    75,
      76,    77,    78,     0,    79,    80,    81,    82,    83,    84,
       0,     0,     0,     0,     0,     0,     0,     0,    86
};

static const yytype_int16 yycheck[] =
{
      28,    29,   660,   541,    29,   658,    20,    40,    40,   167,
     340,   341,   405,   374,   627,    26,   313,    28,   315,    39,
     317,   318,   127,    39,    15,    27,    52,    66,     3,   326,
      32,    33,   329,   330,    53,    26,    55,    28,    29,    30,
      31,    38,    65,   436,     3,   150,   343,   344,    84,   128,
       0,   130,    27,    65,   121,   174,   175,    32,    33,    34,
      38,    25,   359,   360,   361,   362,   363,    65,    27,    65,
      65,    35,    36,    32,    33,    34,     0,    66,    66,   121,
      44,    38,    63,    40,    52,   130,   122,   121,    24,    53,
     129,    55,    24,    25,     3,   708,   122,    65,    66,    23,
      36,    39,    67,    68,   121,   125,   129,   121,   127,   125,
     121,    79,   103,    81,    82,    83,    84,   129,    27,   121,
      29,   121,   515,    32,    33,    34,   101,    65,   125,   126,
     121,   129,   127,   129,    67,    68,    67,    68,   130,   167,
     129,   129,   123,   118,   119,   120,   121,   125,   126,   307,
      25,   309,   127,   123,   122,   130,   517,    51,    52,   126,
     119,   120,   121,   127,   129,   129,   121,   820,   125,   126,
     127,   199,    66,   201,   199,   203,   201,   123,   203,    67,
      68,    25,   301,   302,    67,    68,   123,    64,    67,    68,
      67,    68,   101,   150,   128,    63,   129,    65,   129,   857,
     858,   859,   130,   500,   232,   130,   364,    51,    52,   118,
     119,   120,   121,   170,    15,   130,    17,    18,   130,   324,
      21,   130,    66,   342,   762,   130,   345,   346,   347,   348,
     349,   350,   351,   352,   353,   354,   355,   356,   357,     3,
      65,   129,    67,    68,   637,   130,   129,   128,     9,    10,
     129,    12,    13,    14,   130,   288,   284,   130,    65,   284,
      67,    68,   127,    27,     3,     6,     7,     8,    32,    33,
      34,    24,    25,    34,    15,    65,    40,    67,    68,   307,
     308,   309,   279,   308,    65,    66,    27,    28,    27,   130,
     124,    32,   126,    32,    33,    34,    37,    81,    82,    83,
      84,   279,   124,   130,   126,   333,   334,   130,   333,   334,
      71,   130,   340,   341,    78,    78,    79,    80,    81,    82,
      83,    84,   279,    87,    88,    89,    90,    91,    92,   129,
     660,   288,    65,    97,    67,    68,   364,   101,   122,   103,
     104,   105,   130,   107,   108,   109,   110,   111,   112,   113,
     114,    92,   116,   117,   118,   119,   120,   121,   119,   120,
     121,    25,   101,    65,   661,   662,   130,   324,   365,   366,
      67,    68,   405,   405,   671,   768,   673,   674,    66,   118,
     119,   120,   121,    65,    25,    67,    68,   365,   366,    19,
      63,   130,    22,    80,    81,    82,    83,    84,   431,    29,
     505,   358,    52,   436,   436,   124,    63,   126,   365,   366,
     438,    63,    65,   438,    67,    68,    66,   414,   415,   416,
     417,    63,    38,    53,    40,    63,   531,   424,    63,    79,
      63,    81,    82,    83,    84,   540,   414,   415,   416,   417,
     737,   738,    65,    66,    63,   358,   424,   130,   405,    63,
     569,     9,    10,   558,    63,    13,    14,   414,   415,   416,
     417,   758,    63,    52,   130,   115,    63,   424,   130,   497,
      65,    66,   122,    66,   431,    25,    34,    66,   506,   436,
      38,   506,   515,   515,    65,    66,    66,   130,   120,    63,
      79,   120,    81,    82,    83,    84,   121,   121,   129,    79,
     658,    81,    82,    83,    84,    25,   121,   128,    63,   125,
     126,   127,    63,    71,    63,   845,    63,   675,    63,    63,
      63,     3,    63,    63,    63,    63,     8,   857,   858,   859,
      20,   125,   829,   122,   150,   640,   641,   642,   643,   644,
     645,    63,   122,   648,    50,    27,    71,    65,   505,   127,
      32,    33,    34,   129,   120,   130,    65,   130,   515,   537,
     130,   119,   120,   121,   129,   670,    78,    79,    80,    81,
      82,    83,    84,   129,   531,    15,    16,    17,    18,    63,
     537,    21,    79,   540,    81,    82,    83,    84,    63,    29,
      63,    63,   122,   129,   622,    65,    78,   622,    39,   127,
      40,   558,   635,    85,   637,   637,   639,    68,   565,   728,
     129,    93,    94,    95,    96,    97,   721,   129,   100,   101,
     102,   129,   127,   121,    84,   122,   784,   120,    38,    63,
     658,   628,   660,   590,   631,   121,   118,   119,   120,   121,
     121,   121,    34,   128,   127,    71,   125,   675,   130,   627,
     628,   129,   565,   631,    66,   813,   814,    39,   763,   687,
     115,    66,   820,   279,   121,    36,   115,    63,   121,    49,
     627,   628,   288,   129,   631,    65,   130,   590,   635,   121,
     637,   129,   639,   640,   641,   642,   643,   644,   645,    65,
     115,   648,    35,   650,   651,    63,   653,   129,    45,    39,
      55,    39,   121,   808,    56,   129,   129,   123,   324,   115,
     815,   129,   129,   670,    39,   125,   126,    44,   129,   676,
     397,   267,   248,   680,   681,   682,   683,   684,   685,   686,
     708,   635,   701,   832,   721,   768,   768,   650,   651,   705,
     653,   381,   358,   694,   809,   123,   687,   704,   705,   365,
     366,   708,   845,   796,   760,   170,   784,   761,    38,   407,
      40,   816,    -1,   676,   721,    -1,    -1,   680,   681,   682,
     683,   684,   685,   686,    -1,    78,    79,    80,    81,    82,
      83,    84,    -1,    -1,    -1,   813,   814,    -1,    -1,   405,
     747,   819,   820,    -1,   819,    -1,    -1,    -1,   414,   415,
     416,   417,    -1,    -1,    -1,    -1,   763,    -1,   424,    -1,
      -1,   768,   115,    -1,    -1,   431,    -1,   845,    -1,     3,
     436,    -1,    -1,    -1,    -1,   853,    -1,    -1,   853,   857,
     858,   859,   860,    -1,    -1,   860,    -1,    -1,    -1,   867,
      -1,   798,   867,    27,   801,   125,   126,   127,    32,    33,
      34,   808,    -1,    -1,    -1,    -1,    -1,    -1,   815,    -1,
      -1,    -1,    -1,    -1,   821,    -1,    -1,    -1,    -1,   279,
     150,    -1,    -1,    -1,    -1,   832,    -1,    78,    79,    80,
      81,    82,    83,    84,    -1,   798,    -1,   844,   801,   505,
      -1,    -1,    -1,    -1,    78,    -1,    -1,    -1,    -1,   515,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   122,   821,    93,
      94,    95,    96,    97,   115,   531,   100,   101,   102,    -1,
      -1,   537,    -1,    -1,   540,    -1,    -1,    -1,   112,    -1,
      -1,   844,    -1,    -1,   118,   119,   120,   121,    -1,    -1,
      -1,    -1,   558,   158,   159,    -1,   130,    -1,   358,   565,
      -1,    -1,    -1,    -1,    -1,   365,   366,    -1,    -1,    -1,
      -1,    -1,   177,    -1,   179,    51,    52,   182,   183,    -1,
     185,    -1,   187,   188,   590,    -1,   191,   192,    -1,    65,
      66,   196,   197,    78,    79,    80,    81,    82,    83,    84,
      -1,    -1,    -1,    79,    -1,    81,    82,    83,    84,   279,
      -1,    -1,    -1,    -1,   414,   415,   416,   417,   288,    -1,
      -1,   627,   628,    -1,   424,   631,    -1,    -1,    -1,   635,
     115,   637,    -1,   639,   640,   641,   642,   643,   644,   645,
      -1,    -1,   648,    -1,   650,   651,    -1,   653,    58,    59,
      60,    61,    62,    63,   324,    52,    -1,    -1,    -1,    -1,
      70,    71,    -1,    -1,   670,    -1,    -1,    -1,    -1,    66,
     676,    -1,    -1,    -1,   680,   681,   682,   683,   684,   685,
     686,    -1,    79,    -1,    81,    82,    83,    84,   358,    -1,
      52,    -1,    -1,    -1,    -1,   365,   366,    -1,   704,   705,
      52,    -1,   708,    -1,    66,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    66,   721,    -1,    79,   115,    81,
      82,    83,    84,    -1,    -1,   122,    -1,    79,    -1,    81,
      82,    83,    84,    -1,    -1,   405,    -1,   537,    -1,    -1,
      -1,   747,    -1,    -1,   414,   415,   416,   417,    -1,    -1,
      -1,    -1,    -1,   115,   424,    -1,    -1,   763,    -1,    -1,
     122,   431,   768,   115,   369,   565,   436,    -1,    -1,   374,
     122,    -1,    -1,    -1,    -1,    -1,    -1,   382,   383,   384,
     385,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     590,    -1,   798,   398,   399,   801,    -1,   402,   403,    -1,
      -1,   406,   808,    -1,    -1,    -1,    -1,    -1,    -1,   815,
      -1,    -1,    -1,    -1,    -1,   821,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   832,   627,   628,    -1,
      -1,   631,    -1,     3,    -1,   505,    -1,    -1,   844,    -1,
      -1,    -1,    -1,    -1,    -1,   515,    -1,   452,    -1,   454,
     650,   651,   457,   653,    -1,    -1,    -1,    27,    -1,    -1,
      -1,   531,    32,    33,    34,    -1,    -1,   537,    -1,    -1,
     540,    -1,    -1,    -1,    -1,    -1,   676,    -1,    -1,    -1,
     680,   681,   682,   683,   684,   685,   686,    -1,   558,    -1,
      -1,    -1,    -1,    -1,   499,   565,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   512,   708,   514,
      -1,    -1,   517,   518,   519,    85,    -1,    -1,    -1,    -1,
     590,    -1,    -1,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    -1,   747,   118,   119,
     120,   121,    -1,    -1,    -1,    -1,    -1,   627,   628,    -1,
     130,   631,    -1,    -1,    -1,   635,    -1,   637,    -1,   639,
     640,   641,   642,   643,   644,   645,   115,    -1,   648,    -1,
     650,   651,    -1,   653,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   798,    -1,
     670,   801,    -1,    -1,    -1,    -1,   676,    -1,    -1,    -1,
     680,   681,   682,   683,   684,   685,   686,    -1,    -1,    -1,
      -1,   821,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   832,   638,    -1,    -1,    -1,    -1,   708,    -1,
      -1,    -1,    -1,    -1,   844,    -1,    -1,    -1,    -1,    -1,
      -1,   721,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   669,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   747,    -1,    -1,
      -1,     3,     4,    -1,    -1,    -1,    -1,    -1,    -1,    11,
      -1,    -1,    -1,   763,    -1,    -1,    -1,    -1,   768,    -1,
      -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,
      32,    33,    34,    -1,    -1,    -1,    -1,    39,    40,    41,
      42,    43,    -1,    -1,    46,    47,    48,    -1,   798,    -1,
      -1,   801,    54,    -1,    -1,    57,    -1,    -1,   808,    -1,
      -1,    -1,    -1,    -1,    -1,   815,    -1,    69,    -1,    -1,
      -1,   821,    -1,    -1,    -1,    -1,    78,    -1,    -1,    -1,
      -1,    -1,   832,    -1,    -1,    87,    88,    89,    90,    91,
      92,    -1,    -1,    -1,   844,    97,    -1,    -1,    -1,   101,
      -1,   103,   104,   105,   106,   107,   108,    -1,   110,   111,
     112,   113,   114,    -1,   116,   117,   118,   119,   120,   121,
      -1,     3,     4,    -1,    -1,    -1,   128,    -1,   130,    11,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,
      32,    33,    34,    -1,    -1,    -1,    -1,    39,    40,    41,
      42,    43,    -1,    -1,    46,    47,    48,    -1,    -1,    -1,
      -1,    -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    78,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    87,    88,    89,    90,    91,
      92,    -1,    -1,    -1,    -1,    97,    -1,    -1,    -1,   101,
      -1,   103,   104,   105,   106,   107,   108,    -1,   110,   111,
     112,   113,   114,    -1,   116,   117,   118,   119,   120,   121,
      -1,     3,     4,    -1,    -1,    -1,   128,    -1,   130,    11,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,
      32,    33,    34,    -1,    -1,    -1,    -1,    -1,    40,    41,
      42,    43,    -1,    -1,    -1,    -1,    -1,    -1,     3,    -1,
      -1,    -1,    -1,     8,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,    -1,    -1,
      -1,    -1,    27,    -1,    -1,    -1,    78,    32,    33,    34,
      -1,    -1,    -1,    -1,    -1,    87,    88,    89,    90,    91,
      92,    -1,    -1,    -1,    -1,    97,     3,    -1,    -1,   101,
      -1,   103,   104,   105,   106,   107,   108,    -1,   110,   111,
     112,   113,   114,    -1,   116,   117,   118,   119,   120,   121,
      27,    -1,    -1,    78,    -1,    32,    33,    34,   130,    -1,
      85,    -1,    -1,    40,    -1,    -1,    -1,    -1,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    69,   118,   119,   120,   121,    -1,    -1,    -1,
      -1,    78,    -1,    -1,    -1,   130,    -1,    -1,    -1,    -1,
      87,    88,    89,    90,    91,    92,    -1,    -1,    -1,    -1,
      97,     3,    -1,    -1,   101,    -1,   103,   104,   105,    -1,
     107,   108,    -1,   110,   111,   112,   113,   114,    -1,   116,
     117,   118,   119,   120,   121,    27,    -1,    -1,    -1,    -1,
      32,    33,    34,   130,    -1,    -1,    -1,    -1,    40,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    78,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    87,    88,    89,    90,    91,
      92,    -1,    -1,    -1,    -1,    97,     3,    -1,    -1,   101,
      -1,   103,   104,   105,    -1,   107,   108,    -1,   110,   111,
     112,   113,   114,    -1,   116,   117,   118,   119,   120,   121,
      27,    -1,    -1,    -1,    -1,    32,    33,    34,   130,    -1,
      -1,     3,    -1,    40,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,
      32,    33,    34,    -1,    -1,    -1,    -1,    -1,    40,    -1,
      -1,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      87,    88,    89,    90,    91,    92,    -1,    -1,    -1,    -1,
      97,    -1,    -1,    -1,   101,    -1,   103,   104,   105,    -1,
     107,   108,    -1,   110,   111,   112,   113,   114,    -1,   116,
     117,   118,   119,   120,   121,    87,    88,    89,    90,    91,
      92,    -1,    -1,   130,    -1,    97,    -1,    -1,    -1,   101,
      -1,   103,   104,   105,    -1,   107,   108,    -1,   110,   111,
     112,   113,   114,    -1,   116,   117,   118,   119,   120,   121,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   130
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   132,   133,     0,    23,   134,   377,     6,     7,     8,
      15,    27,    28,    32,    37,    92,     9,    10,    12,    13,
      14,    34,    71,   119,   120,   121,   121,   121,   137,   138,
     121,   143,   121,   141,   121,   261,   130,   130,   378,   135,
     136,     3,     4,    11,    27,    32,    33,    34,    39,    40,
      41,    42,    43,    46,    47,    48,    54,    57,    69,    78,
      87,    88,    89,    90,    91,    92,    97,   101,   103,   104,
     105,   106,   107,   108,   110,   111,   112,   113,   114,   116,
     117,   118,   119,   120,   121,   128,   130,   173,   184,   224,
     262,   264,   270,   301,   302,   305,   310,   321,   332,   341,
     347,   381,   386,   388,   262,   145,    15,    17,    18,    21,
     155,   157,   139,    15,    16,    17,    18,    21,    29,    40,
     146,   148,    63,    65,    25,   379,   380,    85,    93,    94,
      95,    96,    97,    98,    99,   100,   102,   118,   121,   130,
     173,   184,   188,   224,   225,   234,   396,   178,     8,    78,
      85,   121,   130,   188,   189,   225,   232,   396,   307,   309,
     123,   174,   123,   175,   123,   126,   376,   306,   130,   130,
     277,   130,   130,   130,   324,   340,   130,   384,   130,   358,
     130,   130,   385,   128,   130,   366,   130,   130,   308,   130,
     130,   367,   368,   121,   128,   128,   383,   382,   127,   288,
     129,   387,    25,   263,    65,    39,    65,    64,    67,    68,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,   115,    58,    59,    60,    61,    62,    63,
      70,    71,   342,    52,   122,   389,    66,    25,    26,    28,
     121,   250,   252,    63,    63,    63,    63,   144,   156,    15,
      26,    28,    29,    30,    31,   103,   121,   163,   165,    63,
      63,    63,    63,    63,    63,    63,   142,   147,   386,   261,
     234,   234,   100,   189,   192,   130,   130,   130,    63,   237,
     397,    66,    25,   179,   130,   120,   192,    63,   191,    25,
      51,    52,    66,   386,   386,   121,   121,   121,   120,   128,
     310,   284,   280,   121,   173,   291,   401,   271,   275,   265,
     332,   347,   332,   362,   386,   369,   386,   359,   351,   386,
     127,   187,   386,   129,   185,   386,   355,   386,   386,   348,
     353,   386,   386,   365,   361,   386,   386,   262,   262,   262,
     303,   304,   319,   323,   322,   330,   329,   328,   327,   326,
     325,   334,   333,   336,   338,   337,   335,   339,   331,   317,
     316,   315,   314,   313,   311,   318,   312,   343,   347,   371,
     124,   126,   391,   394,   390,    63,    63,    63,    24,    36,
     248,   251,   161,   160,   158,   159,    25,   155,    63,    63,
      63,    63,    63,    63,    63,    63,   140,   164,   154,   153,
      20,   121,   152,   150,    20,   151,   149,    24,    25,   162,
     146,   129,   129,    66,   230,   228,   226,   235,   234,   238,
     240,   124,   126,   398,   236,    19,    22,    29,    53,   180,
     262,   190,   189,   192,   194,   204,   233,   125,   364,   332,
     332,    63,   402,    50,   290,   310,   262,   310,   321,   321,
     321,   321,    65,   127,    71,   192,   321,    65,   321,   321,
     262,   262,   127,   129,   305,   305,   332,   321,   321,   332,
     332,   332,   332,   332,   332,   332,   332,   332,   332,   332,
     332,   332,    78,   112,   121,   130,   188,   207,   401,   321,
     321,   321,   321,   321,   310,   234,   234,   344,   386,   395,
     392,   386,   388,   130,   130,   254,   249,   250,   386,   386,
     386,   386,   167,   130,   171,   169,   120,   170,   168,   166,
      24,    25,   246,   163,   386,   386,   386,   386,   232,   386,
     377,   193,   234,   234,   234,   234,    65,   239,   129,    65,
     400,   399,   234,    63,    63,    63,    63,   129,   189,   122,
      51,    52,    65,    79,    81,    82,    83,    84,   195,   129,
      65,   232,   262,   115,   115,   292,   124,   126,   403,   278,
      39,   129,   129,    65,    65,   360,   352,   386,   386,    65,
      65,   386,    65,    65,   127,   127,   115,   120,   121,    63,
     208,    52,    66,    79,    81,    82,    83,    84,   122,   345,
     347,   386,   121,   321,   406,   407,   409,   121,   255,   253,
     192,   262,   386,   121,   176,   386,   232,    29,   172,   388,
     386,   386,   247,    38,   192,    65,    65,   227,   241,   234,
     243,   242,   192,   121,   409,   181,    34,   183,   182,   196,
     205,   197,   200,   199,   198,   201,   192,   202,   206,   127,
     285,   281,   207,   405,   404,   109,   289,   332,   272,   128,
     266,   363,   370,    65,   129,   374,    65,   129,   372,    71,
     186,   356,   129,   349,   354,   320,   216,   207,   217,   221,
     209,   211,   214,   213,   212,   215,   210,   346,    63,   123,
     125,    66,    39,   115,    65,   129,   121,   257,   259,    36,
     115,    65,   129,   262,   231,   229,   243,   234,   244,   234,
     125,   194,   232,   386,   189,   192,   192,   192,   192,   192,
     192,   203,   192,   207,   207,   207,   121,   412,   130,    49,
     293,   290,   276,   270,   305,   321,   321,   375,   373,   386,
     192,   321,   321,   321,   310,   207,    65,   218,   129,    65,
     207,   207,   207,   207,   207,   207,   207,   345,   410,   121,
     393,   408,   411,   256,   255,   115,   129,    65,   177,   176,
      35,   173,   224,   245,   245,   129,   243,   202,   115,   115,
      63,    39,   125,   332,   294,    45,    39,    55,   295,   267,
     129,   129,   321,   321,   129,    65,   350,   129,   222,   207,
     219,   223,   321,   391,   121,   407,   409,   192,   260,   258,
     232,   129,   129,   286,   282,   413,   414,   129,   310,   279,
     273,   298,    53,    55,   127,    56,   268,   129,   129,   357,
     372,   207,   220,   207,   192,   257,   310,   310,   192,   412,
     262,   290,   207,   115,   296,   269,   321,   219,   129,   129,
      44,   129,   115,   300,   207,   264,   129,   287,   283,   274,
     299,   262,   115,   270,   270,   270,   262,   297,   262
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 5:
#line 151 "poet_yacc.y"
    {(yyval).ptr=insert_parameter((yyvsp[(3) - (3)]).ptr);}
    break;

  case 6:
#line 152 "poet_yacc.y"
    {finish_parameter((yyvsp[(4) - (6)])); }
    break;

  case 7:
#line 153 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE; }
    break;

  case 8:
#line 154 "poet_yacc.y"
    {eval_define(make_macroVar((yyvsp[(3) - (6)]).ptr),(yyvsp[(5) - (6)]).ptr); }
    break;

  case 9:
#line 155 "poet_yacc.y"
    {(yyval).config=GLOBAL_VAR; }
    break;

  case 10:
#line 155 "poet_yacc.y"
    { insert_eval((yyvsp[(4) - (5)]).ptr); }
    break;

  case 11:
#line 156 "poet_yacc.y"
    {(yyval).config=GLOBAL_VAR; }
    break;

  case 12:
#line 156 "poet_yacc.y"
    { insert_cond((yyvsp[(4) - (5)]).ptr); }
    break;

  case 13:
#line 157 "poet_yacc.y"
    { insert_trace((yyvsp[(3) - (4)])); }
    break;

  case 14:
#line 158 "poet_yacc.y"
    {(yyval).ptr=insert_code((yyvsp[(3) - (3)]).ptr);(yyval).config=0; }
    break;

  case 15:
#line 158 "poet_yacc.y"
    {(yyval).ptr = (yyvsp[(4) - (5)]).ptr; }
    break;

  case 17:
#line 159 "poet_yacc.y"
    {(yyval).ptr=insert_input(); }
    break;

  case 18:
#line 159 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(3) - (4)]).ptr; }
    break;

  case 20:
#line 160 "poet_yacc.y"
    {(yyval).ptr=insert_output(); }
    break;

  case 21:
#line 160 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(3) - (4)]).ptr; }
    break;

  case 23:
#line 161 "poet_yacc.y"
    { (yyval).ptr=insert_xform((yyvsp[(3) - (3)]).ptr); }
    break;

  case 24:
#line 162 "poet_yacc.y"
    { set_xform_def((yyvsp[(4) - (6)]).ptr,(yyvsp[(6) - (6)]).ptr); }
    break;

  case 25:
#line 163 "poet_yacc.y"
    { insert_source((yyvsp[(1) - (2)]).ptr); }
    break;

  case 26:
#line 165 "poet_yacc.y"
    {(yyval).ptr = (yyvsp[(0) - (1)]).ptr; }
    break;

  case 29:
#line 167 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 30:
#line 167 "poet_yacc.y"
    { set_input_debug((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 31:
#line 168 "poet_yacc.y"
    { set_input_target((yyvsp[(0) - (3)]).ptr, (yyvsp[(0) - (3)]).ptr); }
    break;

  case 32:
#line 169 "poet_yacc.y"
    { set_input_target((yyvsp[(0) - (3)]).ptr,make_varRef((yyvsp[(3) - (3)]).ptr,GLOBAL_VAR)); }
    break;

  case 33:
#line 170 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 34:
#line 170 "poet_yacc.y"
    {set_input_syntax((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr);}
    break;

  case 35:
#line 171 "poet_yacc.y"
    { set_input_codeType((yyvsp[(0) - (3)]).ptr, (yyvsp[(0) - (3)]).ptr); }
    break;

  case 36:
#line 172 "poet_yacc.y"
    {(yyval).config=CODE_OR_XFORM_VAR; }
    break;

  case 37:
#line 173 "poet_yacc.y"
    { set_input_codeType((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 38:
#line 174 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 39:
#line 174 "poet_yacc.y"
    {set_input_files((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 40:
#line 175 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 41:
#line 175 "poet_yacc.y"
    { set_input_annot((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 42:
#line 176 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 43:
#line 176 "poet_yacc.y"
    { set_input_cond((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 44:
#line 178 "poet_yacc.y"
    {(yyval).ptr = (yyvsp[(0) - (1)]).ptr; }
    break;

  case 47:
#line 180 "poet_yacc.y"
    {(yyval).config=GLOBAL_VAR;}
    break;

  case 48:
#line 181 "poet_yacc.y"
    { set_output_target((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 49:
#line 182 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 50:
#line 183 "poet_yacc.y"
    { set_output_syntax((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 51:
#line 184 "poet_yacc.y"
    {(yyval).config=GLOBAL_VAR;}
    break;

  case 52:
#line 185 "poet_yacc.y"
    { set_output_file((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 53:
#line 186 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 54:
#line 187 "poet_yacc.y"
    { set_output_cond((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 55:
#line 188 "poet_yacc.y"
    { set_input_inline((yyvsp[(0) - (3)]).ptr,(yyvsp[(2) - (3)]).ptr);  }
    break;

  case 58:
#line 191 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(0) - (1)]).ptr; (yyval).config=(yyvsp[(1) - (1)]).config;}
    break;

  case 60:
#line 192 "poet_yacc.y"
    { set_code_params((yyvsp[(0) - (5)]).ptr,(yyvsp[(4) - (5)]).ptr); (yyval).config=(yyvsp[(0) - (5)]).config; }
    break;

  case 61:
#line 193 "poet_yacc.y"
    {(yyval).config=ID_DEFAULT;}
    break;

  case 62:
#line 193 "poet_yacc.y"
    { set_code_attr((yyvsp[(0) - (4)]).ptr, (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 63:
#line 194 "poet_yacc.y"
    {(yyval).config=ID_DEFAULT;}
    break;

  case 64:
#line 195 "poet_yacc.y"
    { set_local_static(make_sourceString("cond",4), (yyvsp[(4) - (4)]).ptr,LVAR_ATTR,(yyvsp[(4) - (4)]).ptr,1); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 65:
#line 196 "poet_yacc.y"
    {(yyval).config=ID_DEFAULT; }
    break;

  case 66:
#line 197 "poet_yacc.y"
    { set_local_static(make_sourceString("rebuild",7), (yyvsp[(4) - (4)]).ptr,LVAR_ATTR,(yyvsp[(4) - (4)]).ptr,1); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 67:
#line 198 "poet_yacc.y"
    {(yyval).config=ID_DEFAULT; }
    break;

  case 68:
#line 199 "poet_yacc.y"
    { set_code_parse((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 69:
#line 200 "poet_yacc.y"
    { set_code_lookahead((yyvsp[(0) - (3)]).ptr, (yyvsp[(3) - (3)]).ptr); (yyval).config=(yyvsp[(0) - (3)]).config; }
    break;

  case 70:
#line 201 "poet_yacc.y"
    {(yyval).config=ID_DEFAULT; }
    break;

  case 71:
#line 202 "poet_yacc.y"
    { set_local_static(make_sourceString("match",5), (yyvsp[(4) - (4)]).ptr,LVAR_ATTR,(yyvsp[(4) - (4)]).ptr,1); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 72:
#line 203 "poet_yacc.y"
    {(yyval).config=ID_DEFAULT; }
    break;

  case 73:
#line 204 "poet_yacc.y"
    { set_local_static(make_sourceString("output",6), (yyvsp[(4) - (4)]).ptr,LVAR_ATTR,(yyvsp[(4) - (4)]).ptr,1); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 74:
#line 206 "poet_yacc.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 75:
#line 207 "poet_yacc.y"
    { (yyval).ptr = make_sourceString("PARSE",5); }
    break;

  case 76:
#line 209 "poet_yacc.y"
    { (yyval).ptr= make_varRef((yyvsp[(1) - (1)]).ptr,(yyvsp[(0) - (1)]).config); (yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 77:
#line 210 "poet_yacc.y"
    { (yyval).ptr=make_varRef((yyvsp[(3) - (3)]).ptr,GLOBAL_SCOPE); (yyval).config=(yyvsp[(0) - (3)]).config; }
    break;

  case 78:
#line 211 "poet_yacc.y"
    {(yyval)=(yyvsp[(2) - (2)]); }
    break;

  case 79:
#line 212 "poet_yacc.y"
    {(yyval)=(yyvsp[(2) - (2)]); }
    break;

  case 80:
#line 214 "poet_yacc.y"
    { (yyval).ptr = make_dummyOperator(POET_OP_XFORM); (yyval).config=(yyvsp[(0) - (0)]).config; }
    break;

  case 81:
#line 215 "poet_yacc.y"
    { (yyval).ptr=make_varRef((yyvsp[(2) - (2)]).ptr,XFORM_VAR); (yyval).config=(yyvsp[(0) - (2)]).config; }
    break;

  case 82:
#line 217 "poet_yacc.y"
    { (yyval).ptr = make_dummyOperator(POET_OP_CODE); }
    break;

  case 83:
#line 218 "poet_yacc.y"
    { (yyval).ptr=make_varRef((yyvsp[(2) - (2)]).ptr,CODE_VAR); }
    break;

  case 84:
#line 221 "poet_yacc.y"
    { (yyval).ptr=make_localPar((yyvsp[(1) - (1)]).ptr,0,LVAR_CODEPAR); }
    break;

  case 85:
#line 222 "poet_yacc.y"
    {(yyval).config=CODE_OR_XFORM_VAR; }
    break;

  case 86:
#line 222 "poet_yacc.y"
    { (yyval).ptr = make_localPar((yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr,LVAR_CODEPAR); }
    break;

  case 87:
#line 223 "poet_yacc.y"
    {(yyval).ptr = make_sourceVector((yyvsp[(1) - (3)]).ptr, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 89:
#line 225 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(0) - (1)]).ptr; }
    break;

  case 91:
#line 226 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 92:
#line 227 "poet_yacc.y"
    {set_param_type((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 93:
#line 228 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 94:
#line 228 "poet_yacc.y"
    { set_param_default((yyvsp[(0) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 95:
#line 229 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE; }
    break;

  case 96:
#line 229 "poet_yacc.y"
    { set_param_parse((yyvsp[(0) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 97:
#line 230 "poet_yacc.y"
    { set_param_message((yyvsp[(0) - (3)]).ptr, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 98:
#line 233 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config; }
    break;

  case 99:
#line 234 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config; }
    break;

  case 100:
#line 235 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MAP, (yyvsp[(4) - (8)]).ptr, (yyvsp[(7) - (8)]).ptr); }
    break;

  case 101:
#line 236 "poet_yacc.y"
    { (yyval).ptr=make_dummyOperator(POET_OP_MAP); }
    break;

  case 102:
#line 237 "poet_yacc.y"
    { (yyval).ptr=make_dummyOperator(POET_OP_MAP); }
    break;

  case 103:
#line 238 "poet_yacc.y"
    { (yyval).ptr=make_dummyOperator(POET_OP_MAP); }
    break;

  case 104:
#line 240 "poet_yacc.y"
    {(yyval).ptr=make_sourceUop(POET_OP_MAP, (yyvsp[(3) - (4)]).ptr); }
    break;

  case 105:
#line 243 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector2((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 106:
#line 245 "poet_yacc.y"
    { (yyval).ptr = make_inputlist((yyvsp[(1) - (5)]).ptr,make_sourceVector2((yyvsp[(3) - (5)]).ptr,(yyvsp[(5) - (5)]).ptr)); }
    break;

  case 107:
#line 247 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 108:
#line 248 "poet_yacc.y"
    { (yyval).ptr = make_any(); }
    break;

  case 109:
#line 249 "poet_yacc.y"
    { (yyval).ptr = make_type(TYPE_INT); }
    break;

  case 110:
#line 250 "poet_yacc.y"
    { (yyval).ptr = make_type(TYPE_FLOAT); }
    break;

  case 111:
#line 251 "poet_yacc.y"
    { (yyval).ptr = make_type(TYPE_STRING); }
    break;

  case 112:
#line 252 "poet_yacc.y"
    { (yyval).ptr = make_type(TYPE_ID); }
    break;

  case 113:
#line 253 "poet_yacc.y"
    { (yyval).ptr = make_dummyOperator(POET_OP_EXP); }
    break;

  case 114:
#line 254 "poet_yacc.y"
    {  (yyval).ptr = make_dummyOperator(POET_OP_VAR); }
    break;

  case 115:
#line 255 "poet_yacc.y"
    { (yyval).ptr = make_dummyOperator(POET_OP_TUPLE); }
    break;

  case 116:
#line 256 "poet_yacc.y"
    { (yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 117:
#line 258 "poet_yacc.y"
    {(yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 118:
#line 259 "poet_yacc.y"
    { (yyval).ptr = negate_Iconst((yyvsp[(2) - (2)]).ptr); }
    break;

  case 119:
#line 260 "poet_yacc.y"
    { (yyval).ptr=make_sourceUop(POET_OP_APPLY, (yyvsp[(3) - (4)]).ptr); }
    break;

  case 120:
#line 261 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 121:
#line 262 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config; }
    break;

  case 122:
#line 263 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign(make_varRef((yyvsp[(1) - (4)]).ptr,ASSIGN_VAR), (yyvsp[(4) - (4)]).ptr); }
    break;

  case 123:
#line 264 "poet_yacc.y"
    { (yyval).ptr = make_typeNot((yyvsp[(2) - (2)])); }
    break;

  case 124:
#line 265 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 125:
#line 265 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(3) - (4)]).ptr; }
    break;

  case 126:
#line 267 "poet_yacc.y"
    {(yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 127:
#line 268 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 128:
#line 269 "poet_yacc.y"
    { (yyval).ptr = make_typeTor((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 129:
#line 271 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 130:
#line 271 "poet_yacc.y"
    { (yyval).ptr = ((yyvsp[(3) - (3)]).ptr==0)? (yyvsp[(1) - (3)]).ptr : make_typelist2((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 131:
#line 272 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 132:
#line 273 "poet_yacc.y"
    {(yyval).ptr = make_typeUop(TYPE_LIST1, (yyvsp[(1) - (2)]).ptr); }
    break;

  case 133:
#line 274 "poet_yacc.y"
    {(yyval).ptr = make_typeUop(TYPE_LIST, (yyvsp[(1) - (2)]).ptr); }
    break;

  case 134:
#line 275 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 135:
#line 276 "poet_yacc.y"
    {(yyval).ptr = make_sourceBop(POET_OP_RANGE, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr);}
    break;

  case 136:
#line 277 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 137:
#line 278 "poet_yacc.y"
    { (yyval).ptr=make_sourceBop(POET_OP_PLUS, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 138:
#line 279 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 139:
#line 280 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MULT,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 140:
#line 281 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 141:
#line 282 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_DIVIDE,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 142:
#line 283 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 143:
#line 283 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MOD,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 144:
#line 284 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 145:
#line 284 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_CONS,(yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 146:
#line 286 "poet_yacc.y"
    {(yyval).ptr=0; }
    break;

  case 147:
#line 287 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 148:
#line 287 "poet_yacc.y"
    { (yyval).ptr = make_typelist2((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 149:
#line 288 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 150:
#line 289 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector2((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 151:
#line 290 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 152:
#line 290 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 153:
#line 292 "poet_yacc.y"
    {(yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 154:
#line 293 "poet_yacc.y"
    { (yyval).ptr = negate_Iconst((yyvsp[(2) - (2)]).ptr); }
    break;

  case 155:
#line 294 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_CLEAR, make_varRef((yyvsp[(2) - (2)]).ptr,ASSIGN_VAR)); }
    break;

  case 156:
#line 295 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 157:
#line 296 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 158:
#line 296 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(3) - (4)]).ptr; }
    break;

  case 159:
#line 297 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 160:
#line 298 "poet_yacc.y"
    { (yyval).ptr = make_typeTor((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 161:
#line 299 "poet_yacc.y"
    {(yyval).ptr = make_typeUop(TYPE_LIST, (yyvsp[(1) - (2)]).ptr); }
    break;

  case 162:
#line 300 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 163:
#line 301 "poet_yacc.y"
    {(yyval).ptr = make_sourceBop(POET_OP_RANGE, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr);}
    break;

  case 164:
#line 302 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 165:
#line 303 "poet_yacc.y"
    { (yyval).ptr=make_sourceBop(POET_OP_PLUS, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 166:
#line 304 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 167:
#line 305 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MULT,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 168:
#line 306 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 169:
#line 307 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_DIVIDE,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 170:
#line 308 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 171:
#line 309 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MOD,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 172:
#line 310 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 173:
#line 310 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_CONS,(yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 174:
#line 311 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 175:
#line 311 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign(make_varRef((yyvsp[(1) - (4)]).ptr,ASSIGN_VAR), (yyvsp[(4) - (4)]).ptr); }
    break;

  case 176:
#line 313 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 177:
#line 313 "poet_yacc.y"
    { (yyval).ptr = ((yyvsp[(3) - (3)]).ptr==0)? (yyvsp[(1) - (3)]).ptr : make_typelist2((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 178:
#line 314 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 179:
#line 316 "poet_yacc.y"
    {(yyval).ptr=0; }
    break;

  case 180:
#line 317 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 181:
#line 317 "poet_yacc.y"
    { (yyval).ptr = make_typelist2((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 182:
#line 318 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 183:
#line 319 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector2((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 184:
#line 320 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 185:
#line 320 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 186:
#line 322 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 187:
#line 323 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 188:
#line 324 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 189:
#line 325 "poet_yacc.y"
    { (yyval).ptr=make_empty_list(); }
    break;

  case 190:
#line 327 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config; }
    break;

  case 191:
#line 327 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (4)]).config;}
    break;

  case 192:
#line 327 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_TUPLE, make_typelist2((yyvsp[(4) - (7)]).ptr,(yyvsp[(6) - (7)]).ptr)); }
    break;

  case 193:
#line 328 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config; }
    break;

  case 194:
#line 328 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 195:
#line 328 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_LIST,(yyvsp[(4) - (8)]).ptr,(yyvsp[(7) - (8)]).ptr); }
    break;

  case 196:
#line 329 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config; }
    break;

  case 197:
#line 329 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 198:
#line 329 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_LIST1,(yyvsp[(4) - (8)]).ptr,(yyvsp[(7) - (8)]).ptr); }
    break;

  case 199:
#line 331 "poet_yacc.y"
    {(yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 200:
#line 332 "poet_yacc.y"
    { (yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 201:
#line 333 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 202:
#line 334 "poet_yacc.y"
    { (yyval).ptr = make_typeTor((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 203:
#line 335 "poet_yacc.y"
    {(yyval).ptr = make_typeUop(TYPE_LIST1, (yyvsp[(1) - (2)]).ptr); }
    break;

  case 204:
#line 336 "poet_yacc.y"
    {(yyval).ptr = make_typeUop(TYPE_LIST, (yyvsp[(1) - (2)]).ptr); }
    break;

  case 205:
#line 338 "poet_yacc.y"
    { (yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 206:
#line 339 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 207:
#line 340 "poet_yacc.y"
    { (yyval).ptr = make_typeNot((yyvsp[(2) - (2)])); }
    break;

  case 208:
#line 341 "poet_yacc.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 209:
#line 342 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config; }
    break;

  case 210:
#line 343 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign(make_varRef((yyvsp[(1) - (4)]).ptr,ASSIGN_VAR), (yyvsp[(4) - (4)]).ptr); }
    break;

  case 211:
#line 344 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 212:
#line 345 "poet_yacc.y"
    { (yyval).ptr = make_typeTor((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 213:
#line 346 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 214:
#line 346 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(3) - (4)]).ptr; }
    break;

  case 215:
#line 348 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 216:
#line 348 "poet_yacc.y"
    { (yyval).ptr = ((yyvsp[(3) - (3)]).ptr==0)? (yyvsp[(1) - (3)]).ptr : make_typelist2((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 217:
#line 349 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 218:
#line 350 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 219:
#line 351 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector2((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 220:
#line 352 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 221:
#line 352 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 222:
#line 353 "poet_yacc.y"
    {(yyval).ptr=0; }
    break;

  case 223:
#line 354 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 224:
#line 354 "poet_yacc.y"
    { (yyval).ptr = make_typelist2((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 225:
#line 356 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 226:
#line 357 "poet_yacc.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 227:
#line 359 "poet_yacc.y"
    {yaccState=YACC_CODE; (yyval).config=ID_DEFAULT;}
    break;

  case 228:
#line 359 "poet_yacc.y"
    { yaccState=YACC_DEFAULT; set_code_def((yyvsp[(0) - (4)]).ptr,(yyvsp[(3) - (4)]).ptr); }
    break;

  case 229:
#line 360 "poet_yacc.y"
    { set_code_def((yyvsp[(0) - (1)]).ptr, 0); }
    break;

  case 230:
#line 361 "poet_yacc.y"
    {(yyval).config=ID_DEFAULT; }
    break;

  case 231:
#line 361 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(3) - (4)]).ptr; }
    break;

  case 232:
#line 362 "poet_yacc.y"
    {(yyval).ptr = 0; }
    break;

  case 234:
#line 363 "poet_yacc.y"
    {(yyval)=(yyvsp[(0) - (1)]); }
    break;

  case 236:
#line 364 "poet_yacc.y"
    {set_xform_params((yyvsp[(0) - (5)]).ptr,(yyvsp[(4) - (5)]).ptr); }
    break;

  case 237:
#line 365 "poet_yacc.y"
    {(yyval).ptr=0;}
    break;

  case 239:
#line 366 "poet_yacc.y"
    {(yyval).config=CODE_VAR;}
    break;

  case 240:
#line 366 "poet_yacc.y"
    { (yyval).ptr = set_local_static((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr,LVAR_TUNE,0,1); }
    break;

  case 241:
#line 367 "poet_yacc.y"
    { (yyval).ptr=make_localPar((yyvsp[(1) - (1)]).ptr,0,LVAR_XFORMPAR); }
    break;

  case 242:
#line 368 "poet_yacc.y"
    {(yyval).config=CODE_VAR;}
    break;

  case 243:
#line 368 "poet_yacc.y"
    { (yyval).ptr = make_localPar((yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr,LVAR_XFORMPAR); }
    break;

  case 244:
#line 369 "poet_yacc.y"
    {(yyval).ptr = make_sourceVector((yyvsp[(1) - (3)]).ptr, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 245:
#line 370 "poet_yacc.y"
    {(yyval).ptr = (yyvsp[(0) - (2)]).ptr + 1; }
    break;

  case 248:
#line 371 "poet_yacc.y"
    { set_local_static((yyvsp[(1) - (1)]).ptr,make_Iconst1((yyvsp[(0) - (1)]).ptr), LVAR_OUTPUT,0,1); }
    break;

  case 249:
#line 372 "poet_yacc.y"
    {(yyval).config=CODE_VAR;}
    break;

  case 250:
#line 372 "poet_yacc.y"
    { set_local_static((yyvsp[(1) - (4)]).ptr,make_Iconst1((yyvsp[(0) - (4)]).ptr), LVAR_OUTPUT,(yyvsp[(4) - (4)]).ptr,1); }
    break;

  case 251:
#line 373 "poet_yacc.y"
    { (yyval).ptr = make_traceVar((yyvsp[(1) - (1)]).ptr,0); }
    break;

  case 252:
#line 374 "poet_yacc.y"
    { (yyval).ptr = make_traceVar((yyvsp[(1) - (3)]).ptr,0); eval_define((yyval).ptr,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 253:
#line 376 "poet_yacc.y"
    { (yyval).ptr = make_traceVar((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr);  }
    break;

  case 254:
#line 378 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 255:
#line 378 "poet_yacc.y"
    { (yyval).ptr=make_seq((yyvsp[(1) - (3)]).ptr, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 256:
#line 379 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 257:
#line 380 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 258:
#line 381 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 259:
#line 381 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 260:
#line 381 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (7)]).config;}
    break;

  case 261:
#line 382 "poet_yacc.y"
    { (yyval).ptr = make_ifElse((yyvsp[(4) - (9)]).ptr, (yyvsp[(7) - (9)]).ptr, (yyvsp[(9) - (9)]).ptr); }
    break;

  case 262:
#line 383 "poet_yacc.y"
    { (yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 263:
#line 384 "poet_yacc.y"
    { (yyval).ptr=make_empty(); }
    break;

  case 264:
#line 385 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 265:
#line 385 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(3) - (3)]).ptr; }
    break;

  case 266:
#line 386 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 267:
#line 387 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 268:
#line 388 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (8)]).config;}
    break;

  case 269:
#line 388 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (11)]).config;}
    break;

  case 270:
#line 389 "poet_yacc.y"
    { (yyval).ptr = make_sourceQop(POET_OP_FOR, (yyvsp[(4) - (13)]).ptr,(yyvsp[(7) - (13)]).ptr,(yyvsp[(10) - (13)]).ptr,(yyvsp[(13) - (13)]).ptr); }
    break;

  case 271:
#line 390 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 272:
#line 390 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (6)]).config;}
    break;

  case 273:
#line 391 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_CASE, (yyvsp[(4) - (9)]).ptr,(yyvsp[(8) - (9)]).ptr); }
    break;

  case 274:
#line 392 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 275:
#line 392 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (4)]).config;}
    break;

  case 276:
#line 392 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (8)]).config;}
    break;

  case 277:
#line 393 "poet_yacc.y"
    { (yyval).ptr=make_sourceQop(POET_OP_FOREACH,(yyvsp[(6) - (11)]).ptr,(yyvsp[(3) - (11)]).ptr,(yyvsp[(7) - (11)]).ptr,(yyvsp[(10) - (11)]).ptr); }
    break;

  case 278:
#line 394 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 279:
#line 395 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 280:
#line 396 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (8)]).config;}
    break;

  case 281:
#line 396 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (11)]).config;}
    break;

  case 282:
#line 397 "poet_yacc.y"
    { (yyval).ptr = make_sourceQop(POET_OP_FOREACH, (yyvsp[(4) - (13)]).ptr,(yyvsp[(7) - (13)]).ptr,(yyvsp[(10) - (13)]).ptr,(yyvsp[(13) - (13)]).ptr); }
    break;

  case 283:
#line 398 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 284:
#line 399 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 285:
#line 400 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (8)]).config;}
    break;

  case 286:
#line 400 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (11)]).config;}
    break;

  case 287:
#line 401 "poet_yacc.y"
    { (yyval).ptr = make_sourceQop(POET_OP_FOREACH, make_sourceUop(POET_OP_REVERSE,(yyvsp[(4) - (13)]).ptr),(yyvsp[(7) - (13)]).ptr,(yyvsp[(10) - (13)]).ptr,(yyvsp[(13) - (13)]).ptr); }
    break;

  case 288:
#line 402 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 289:
#line 402 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(3) - (4)]).ptr; }
    break;

  case 290:
#line 403 "poet_yacc.y"
    { (yyval).ptr=make_empty(); }
    break;

  case 291:
#line 404 "poet_yacc.y"
    { (yyval).ptr = make_empty(); }
    break;

  case 292:
#line 405 "poet_yacc.y"
    { (yyval).ptr = make_sourceStmt((yyvsp[(1) - (2)]).ptr); }
    break;

  case 293:
#line 407 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 294:
#line 408 "poet_yacc.y"
    { (yyval).ptr=make_sourceUop(POET_OP_REVERSE,(yyvsp[(3) - (4)]).ptr); }
    break;

  case 295:
#line 410 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 296:
#line 411 "poet_yacc.y"
    { (yyval).ptr = make_empty(); }
    break;

  case 297:
#line 414 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config; }
    break;

  case 298:
#line 415 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign(make_varRef((yyvsp[(1) - (4)]).ptr,ASSIGN_VAR), (yyvsp[(4) - (4)]).ptr); }
    break;

  case 299:
#line 416 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(1) - (1)]).ptr;}
    break;

  case 300:
#line 418 "poet_yacc.y"
    { (yyval).ptr=make_Iconst1(1); }
    break;

  case 301:
#line 419 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 302:
#line 419 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(3) - (3)]).ptr; }
    break;

  case 303:
#line 421 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 304:
#line 421 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 305:
#line 422 "poet_yacc.y"
    {(yyval).ptr = make_sourceVector( (yyvsp[(1) - (7)]).ptr, make_inputlist((yyvsp[(4) - (7)]).ptr,(yyvsp[(7) - (7)]).ptr)); }
    break;

  case 306:
#line 423 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 307:
#line 423 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (4)]).config;}
    break;

  case 308:
#line 423 "poet_yacc.y"
    { (yyval).ptr = make_inputlist((yyvsp[(3) - (6)]).ptr,(yyvsp[(6) - (6)]).ptr); }
    break;

  case 309:
#line 424 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (3)]).config;}
    break;

  case 310:
#line 425 "poet_yacc.y"
    {(yyval).ptr = make_sourceVector( (yyvsp[(1) - (5)]).ptr, make_inputlist(make_any(),(yyvsp[(5) - (5)]).ptr)); }
    break;

  case 311:
#line 427 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 312:
#line 428 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 313:
#line 430 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 314:
#line 430 "poet_yacc.y"
    {(yyval).ptr = make_sourceVector((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 315:
#line 431 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 316:
#line 431 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector2((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 317:
#line 434 "poet_yacc.y"
    { (yyval).ptr=make_dummyOperator(POET_OP_CONTINUE); }
    break;

  case 318:
#line 435 "poet_yacc.y"
    { (yyval).ptr=make_dummyOperator(POET_OP_BREAK); }
    break;

  case 319:
#line 436 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 320:
#line 436 "poet_yacc.y"
    { (yyval).ptr=make_sourceUop(POET_OP_RETURN, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 321:
#line 437 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 322:
#line 437 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_ERROR,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 323:
#line 438 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 324:
#line 438 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_ASSERT,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 325:
#line 439 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 326:
#line 439 "poet_yacc.y"
    { (yyval).ptr=make_sourceUop(POET_OP_PRINT,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 327:
#line 440 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 328:
#line 442 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 329:
#line 443 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 330:
#line 443 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign((yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 331:
#line 444 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 332:
#line 444 "poet_yacc.y"
    { (yyval).ptr = make_codeMatch((yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 333:
#line 445 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 334:
#line 446 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign((yyvsp[(1) - (4)]).ptr, make_sourceBop(POET_OP_PLUS, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr)); }
    break;

  case 335:
#line 447 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 336:
#line 448 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign((yyvsp[(1) - (4)]).ptr, make_sourceBop(POET_OP_MINUS, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr)); }
    break;

  case 337:
#line 449 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 338:
#line 450 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign((yyvsp[(1) - (4)]).ptr, make_sourceBop(POET_OP_MULT, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr)); }
    break;

  case 339:
#line 451 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 340:
#line 452 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign((yyvsp[(1) - (4)]).ptr, make_sourceBop(POET_OP_DIVIDE, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr)); }
    break;

  case 341:
#line 453 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 342:
#line 454 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign((yyvsp[(1) - (4)]).ptr, make_sourceBop(POET_OP_MOD, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr)); }
    break;

  case 343:
#line 455 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 344:
#line 455 "poet_yacc.y"
    { (yyval).ptr = make_codeMatchQ((yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 345:
#line 456 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 346:
#line 456 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 347:
#line 457 "poet_yacc.y"
    { (yyval).ptr = make_ifElse( (yyvsp[(1) - (7)]).ptr, (yyvsp[(4) - (7)]).ptr, (yyvsp[(7) - (7)]).ptr); }
    break;

  case 348:
#line 460 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 349:
#line 460 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_AND,  (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 350:
#line 461 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 351:
#line 461 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_OR,   (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 352:
#line 462 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 353:
#line 462 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_NOT, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 354:
#line 463 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 355:
#line 463 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_LT, (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 356:
#line 464 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 357:
#line 464 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_LE, (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 358:
#line 465 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 359:
#line 465 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_EQ, (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 360:
#line 466 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 361:
#line 467 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_GT,   (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 362:
#line 468 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 363:
#line 469 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_GE,   (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 364:
#line 470 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 365:
#line 471 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_NE,   (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 366:
#line 472 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 367:
#line 472 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_ASTMATCH, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 368:
#line 473 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 369:
#line 475 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 370:
#line 476 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 371:
#line 477 "poet_yacc.y"
    { (yyval).ptr=make_sourceBop(POET_OP_PLUS, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 372:
#line 478 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 373:
#line 479 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MINUS,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 374:
#line 480 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 375:
#line 481 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MULT,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 376:
#line 482 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 377:
#line 483 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_CONCAT, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 378:
#line 484 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 379:
#line 485 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_DIVIDE,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 380:
#line 486 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 381:
#line 487 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MOD,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 382:
#line 488 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 383:
#line 488 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_CONS,(yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 384:
#line 489 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 385:
#line 489 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_UMINUS,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 386:
#line 491 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 387:
#line 492 "poet_yacc.y"
    { if ((yyvsp[(3) - (3)]).ptr==0) (yyval).ptr=(yyvsp[(1) - (3)]).ptr; else (yyval).ptr = make_xformList((yyvsp[(1) - (3)]).ptr, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 388:
#line 494 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 389:
#line 494 "poet_yacc.y"
    { (yyval).ptr =make_xformList((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr);}
    break;

  case 390:
#line 495 "poet_yacc.y"
    { (yyval).ptr=0; }
    break;

  case 391:
#line 496 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 392:
#line 496 "poet_yacc.y"
    { (yyval).ptr =make_xformList((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr);}
    break;

  case 393:
#line 497 "poet_yacc.y"
    { (yyval).ptr=0; }
    break;

  case 394:
#line 499 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 395:
#line 499 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 396:
#line 499 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (7)]).config;}
    break;

  case 397:
#line 500 "poet_yacc.y"
    { (yyval).ptr=((yyvsp[(9) - (9)]).ptr==0)? make_sourceBop(POET_OP_REPLACE,(yyvsp[(4) - (9)]).ptr,(yyvsp[(7) - (9)]).ptr) 
                       : make_sourceTop(POET_OP_REPLACE,(yyvsp[(4) - (9)]).ptr,(yyvsp[(7) - (9)]).ptr,(yyvsp[(9) - (9)]).ptr); }
    break;

  case 398:
#line 502 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 399:
#line 502 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (4)]).config;}
    break;

  case 400:
#line 503 "poet_yacc.y"
    { (yyval).ptr=((yyvsp[(6) - (6)]).ptr==0)? make_sourceUop(POET_OP_TRACE,(yyvsp[(4) - (6)]).ptr)
                      : make_sourceBop(POET_OP_TRACE,(yyvsp[(4) - (6)]).ptr,(yyvsp[(6) - (6)]).ptr); }
    break;

  case 401:
#line 505 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 402:
#line 505 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 403:
#line 506 "poet_yacc.y"
    { (yyval).ptr=make_sourceBop(POET_OP_PERMUTE,(yyvsp[(4) - (8)]).ptr,(yyvsp[(7) - (8)]).ptr); }
    break;

  case 404:
#line 507 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 405:
#line 507 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 406:
#line 507 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (8)]).config;}
    break;

  case 407:
#line 508 "poet_yacc.y"
    { (yyval).ptr=make_sourceTop(POET_OP_DUPLICATE,(yyvsp[(4) - (11)]).ptr,(yyvsp[(7) - (11)]).ptr,(yyvsp[(10) - (11)]).ptr); }
    break;

  case 408:
#line 509 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 409:
#line 509 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_COPY, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 410:
#line 510 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 411:
#line 511 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (4)]).config;(yyval).ptr=(yyvsp[(4) - (4)]).ptr; }
    break;

  case 412:
#line 512 "poet_yacc.y"
    {(yyval).ptr = make_sourceBop(POET_OP_ERASE,(yyvsp[(4) - (6)]).ptr,(yyvsp[(6) - (6)]).ptr); }
    break;

  case 413:
#line 513 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 414:
#line 513 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_DELAY, (yyvsp[(4) - (5)]).ptr); }
    break;

  case 415:
#line 514 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 416:
#line 514 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 417:
#line 515 "poet_yacc.y"
    {  (yyval).ptr=make_sourceBop(POET_OP_INSERT, (yyvsp[(4) - (8)]).ptr,(yyvsp[(7) - (8)]).ptr); }
    break;

  case 418:
#line 516 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (3)]).config;}
    break;

  case 419:
#line 517 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_DEBUG,(yyvsp[(2) - (6)]).ptr,(yyvsp[(5) - (6)]).ptr); }
    break;

  case 420:
#line 518 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 421:
#line 518 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_APPLY, (yyvsp[(4) - (5)]).ptr); }
    break;

  case 422:
#line 519 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 423:
#line 519 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_REBUILD,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 424:
#line 520 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 425:
#line 520 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_RESTORE,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 426:
#line 521 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 427:
#line 521 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_SAVE,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 428:
#line 522 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 429:
#line 522 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 430:
#line 523 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_SPLIT, (yyvsp[(4) - (8)]).ptr,(yyvsp[(7) - (8)]).ptr); }
    break;

  case 431:
#line 524 "poet_yacc.y"
    {(yyval).ptr = make_typeUop(TYPE_LIST, (yyvsp[(1) - (2)]).ptr); }
    break;

  case 432:
#line 525 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 433:
#line 526 "poet_yacc.y"
    {(yyval).ptr = make_sourceBop(POET_OP_RANGE, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr);}
    break;

  case 434:
#line 528 "poet_yacc.y"
    {(yyval).ptr = make_sourceBop(POET_OP_RANGE, (yyvsp[(3) - (6)]).ptr,(yyvsp[(6) - (6)]).ptr);}
    break;

  case 435:
#line 529 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 436:
#line 530 "poet_yacc.y"
    { (yyval).ptr = 0; }
    break;

  case 437:
#line 530 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 438:
#line 530 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(3) - (4)]).ptr; }
    break;

  case 439:
#line 532 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 440:
#line 532 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(3) - (4)]).ptr; }
    break;

  case 441:
#line 533 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(0) - (1)]).ptr; }
    break;

  case 442:
#line 535 "poet_yacc.y"
    {(yyval).ptr = make_Iconst1(1); }
    break;

  case 443:
#line 536 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(2) - (3)]).ptr; }
    break;

  case 444:
#line 538 "poet_yacc.y"
    { (yyval).ptr = 0; }
    break;

  case 445:
#line 539 "poet_yacc.y"
    { (yyval).ptr=make_inputlist((yyvsp[(1) - (2)]).ptr,(yyvsp[(2) - (2)]).ptr); }
    break;

  case 446:
#line 540 "poet_yacc.y"
    {(yyval).config=CODE_OR_XFORM_VAR;}
    break;

  case 447:
#line 540 "poet_yacc.y"
    { (yyval).ptr = make_annot_single((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 448:
#line 541 "poet_yacc.y"
    {(yyval).config=CODE_OR_XFORM_VAR; }
    break;

  case 449:
#line 542 "poet_yacc.y"
    { (yyval).ptr = make_annot_lbegin((yyvsp[(1) - (6)]).ptr,(yyvsp[(5) - (6)]).ptr); }
    break;

  case 450:
#line 543 "poet_yacc.y"
    {(yyval).config=CODE_OR_XFORM_VAR; }
    break;

  case 451:
#line 544 "poet_yacc.y"
    { (yyval).ptr = make_inputlist((yyvsp[(1) - (6)]).ptr,make_sourceUop(POET_OP_ANNOT,(yyvsp[(5) - (6)]).ptr)); }
    break;

  case 452:
#line 545 "poet_yacc.y"
    { (yyval).ptr = make_inputlist((yyvsp[(1) - (2)]).ptr, (yyvsp[(2) - (2)]).ptr); }
    break;

  case 453:
#line 546 "poet_yacc.y"
    { (yyval).ptr = make_inputlist((yyvsp[(1) - (2)]).ptr, (yyvsp[(2) - (2)]).ptr); }
    break;

  case 454:
#line 547 "poet_yacc.y"
    { (yyval).ptr = make_inputlist((yyvsp[(1) - (2)]).ptr, (yyvsp[(2) - (2)]).ptr); }
    break;

  case 455:
#line 548 "poet_yacc.y"
    { (yyval).ptr = make_inputlist((yyvsp[(1) - (2)]).ptr, (yyvsp[(2) - (2)]).ptr); }
    break;

  case 456:
#line 549 "poet_yacc.y"
    { (yyval).ptr = make_inputlist((yyvsp[(1) - (2)]).ptr,make_varRef((yyvsp[(2) - (2)]).ptr,GLOBAL_VAR)); }
    break;

  case 457:
#line 552 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 458:
#line 552 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_CAR, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 459:
#line 553 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 460:
#line 553 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_CDR, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 461:
#line 554 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 462:
#line 554 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_LEN, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 463:
#line 555 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 464:
#line 555 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(TYPE_INT, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 465:
#line 556 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_CLEAR, make_varRef((yyvsp[(2) - (2)]).ptr,ASSIGN_VAR)); }
    break;

  case 466:
#line 557 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 467:
#line 560 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 468:
#line 560 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(3) - (4)]).ptr; }
    break;

  case 469:
#line 561 "poet_yacc.y"
    { (yyval).ptr=make_empty(); }
    break;

  case 470:
#line 562 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 471:
#line 563 "poet_yacc.y"
    { (yyval).ptr = make_any(); }
    break;

  case 472:
#line 564 "poet_yacc.y"
    { (yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 473:
#line 565 "poet_yacc.y"
    {(yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 474:
#line 567 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(1) - (1)]).ptr; (yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 475:
#line 567 "poet_yacc.y"
    { (yyval) = (yyvsp[(3) - (3)]); }
    break;

  case 476:
#line 568 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config; }
    break;

  case 477:
#line 568 "poet_yacc.y"
    { (yyval).ptr = make_typeTor((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 478:
#line 569 "poet_yacc.y"
    {(yyval)=(yyvsp[(0) - (0)]); }
    break;

  case 479:
#line 570 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; (yyval).ptr=(yyvsp[(0) - (1)]).ptr; }
    break;

  case 480:
#line 571 "poet_yacc.y"
    { (yyval).ptr = make_tupleAccess((yyvsp[(0) - (4)]).ptr, (yyvsp[(3) - (4)]).ptr); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 481:
#line 572 "poet_yacc.y"
    { (yyval)=(yyvsp[(6) - (6)]); }
    break;

  case 482:
#line 573 "poet_yacc.y"
    { (yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 483:
#line 575 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 484:
#line 576 "poet_yacc.y"
    { (yyval).ptr = make_codeRef((yyvsp[(0) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr,(yyvsp[(0) - (3)]).config); (yyval).config=(yyvsp[(0) - (3)]).config; }
    break;

  case 485:
#line 578 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(1) - (1)]).ptr; (yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 486:
#line 578 "poet_yacc.y"
    { (yyval) = (yyvsp[(3) - (3)]); }
    break;

  case 487:
#line 580 "poet_yacc.y"
    {(yyval)=(yyvsp[(0) - (0)]); }
    break;

  case 488:
#line 581 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; (yyval).ptr=(yyvsp[(0) - (1)]).ptr; }
    break;

  case 489:
#line 582 "poet_yacc.y"
    { (yyval).ptr = make_tupleAccess((yyvsp[(0) - (4)]).ptr, (yyvsp[(3) - (4)]).ptr); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 490:
#line 583 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 491:
#line 584 "poet_yacc.y"
    { (yyval).ptr = make_codeRef((yyvsp[(0) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr,(yyvsp[(0) - (3)]).config);(yyval).config=(yyvsp[(0) - (3)]).config; }
    break;

  case 492:
#line 586 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(1) - (1)]).ptr; (yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 493:
#line 586 "poet_yacc.y"
    { (yyval) = (yyvsp[(3) - (3)]); }
    break;

  case 494:
#line 587 "poet_yacc.y"
    {(yyval)=(yyvsp[(0) - (0)]); }
    break;

  case 495:
#line 588 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; (yyval).ptr=(yyvsp[(0) - (1)]).ptr; }
    break;

  case 496:
#line 589 "poet_yacc.y"
    { (yyval).ptr = make_tupleAccess((yyvsp[(0) - (4)]).ptr, (yyvsp[(3) - (4)]).ptr); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 497:
#line 590 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 498:
#line 591 "poet_yacc.y"
    { (yyval).ptr = make_codeRef((yyvsp[(0) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr,(yyvsp[(0) - (3)]).config);(yyval).config=(yyvsp[(0) - (3)]).config; }
    break;

  case 499:
#line 593 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(1) - (1)]).ptr;}
    break;

  case 500:
#line 594 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 501:
#line 595 "poet_yacc.y"
    {(yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 502:
#line 597 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 503:
#line 598 "poet_yacc.y"
    { (yyval).ptr = make_typeTor((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 504:
#line 599 "poet_yacc.y"
    { (yyval).ptr = make_attrAccess(make_varRef((yyvsp[(1) - (3)]).ptr,CODE_OR_XFORM_VAR), (yyvsp[(3) - (3)]).ptr); }
    break;

  case 505:
#line 602 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 506:
#line 603 "poet_yacc.y"
    { (yyval).ptr = make_sourceAssign( make_attrAccess((yyvsp[(0) - (4)]).ptr,(yyvsp[(1) - (4)]).ptr),(yyvsp[(4) - (4)]).ptr); }
    break;

  case 507:
#line 604 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(0) - (2)]).ptr; (yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 508:
#line 605 "poet_yacc.y"
    { (yyval).ptr = make_seq((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 509:
#line 608 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 510:
#line 609 "poet_yacc.y"
    { (yyval).ptr = make_sourceAssign( make_attrAccess((yyvsp[(0) - (4)]).ptr,(yyvsp[(1) - (4)]).ptr),(yyvsp[(4) - (4)]).ptr); }
    break;

  case 511:
#line 610 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(0) - (2)]).ptr; (yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 512:
#line 611 "poet_yacc.y"
    { (yyval).ptr = make_seq((yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr);}
    break;


/* Line 1267 of yacc.c.  */
#line 5294 "poet_yacc.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 612 "poet_yacc.y"


