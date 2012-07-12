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
extern void eval_define();

extern void insert_trace();
extern void* insert_parameter();
extern void set_param_type();
extern void set_param_default();
extern void set_param_parse();
extern void set_param_message();
extern void finish_parameter();
extern void* make_codeMatch();
extern void* make_annot();
extern void* make_codeMatchQ();
extern void* make_empty();
extern void* make_any();
extern void* make_typelist2();
extern void* make_inputlist2();
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
#line 469 "poet_yacc.c"

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
# if defined YYENABLE_NLS && YYENABLE_NLS
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
#define YYLAST   1925

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  125
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  280
/* YYNRULES -- Number of rules.  */
#define YYNRULES  493
/* YYNRULES -- Number of states.  */
#define YYNSTATES  838

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   379

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
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    10,    17,    18,    25,
      26,    32,    37,    38,    39,    47,    48,    49,    56,    57,
      58,    65,    66,    73,    77,    78,    82,    83,    84,    89,
      93,    97,    98,   103,   107,   108,   113,   114,   119,   120,
     125,   126,   131,   132,   136,   137,   138,   143,   144,   149,
     150,   155,   156,   161,   165,   167,   168,   169,   173,   179,
     180,   185,   189,   190,   195,   196,   201,   202,   207,   211,
     212,   217,   218,   223,   225,   229,   232,   235,   236,   239,
     240,   243,   245,   246,   251,   255,   256,   257,   261,   262,
     267,   268,   273,   274,   279,   283,   284,   285,   294,   298,
     302,   307,   311,   317,   319,   321,   323,   325,   327,   329,
     331,   333,   335,   337,   340,   342,   345,   346,   351,   353,
     354,   359,   360,   364,   366,   369,   372,   373,   378,   379,
     384,   385,   390,   391,   396,   397,   402,   403,   408,   409,
     410,   414,   415,   420,   421,   426,   428,   431,   434,   436,
     437,   442,   443,   448,   451,   452,   457,   458,   463,   464,
     469,   470,   475,   476,   481,   482,   487,   488,   492,   494,
     495,   500,   501,   502,   506,   507,   512,   513,   518,   520,
     522,   523,   524,   532,   533,   534,   543,   544,   545,   554,
     556,   558,   560,   561,   566,   569,   572,   574,   576,   579,
     581,   582,   587,   588,   593,   594,   599,   600,   604,   606,
     607,   612,   613,   618,   619,   620,   624,   626,   628,   629,
     634,   636,   637,   642,   644,   645,   646,   650,   656,   657,
     664,   665,   670,   672,   673,   678,   682,   683,   688,   690,
     692,   693,   698,   700,   704,   705,   709,   711,   713,   714,
     715,   716,   726,   728,   729,   730,   734,   735,   736,   737,
     738,   752,   753,   754,   764,   765,   766,   767,   768,   782,
     783,   784,   785,   786,   800,   801,   806,   809,   812,   814,
     815,   816,   817,   825,   826,   827,   834,   835,   841,   843,
     845,   846,   851,   852,   857,   859,   861,   862,   866,   867,
     871,   872,   876,   878,   880,   881,   886,   887,   892,   893,
     898,   899,   904,   905,   910,   911,   916,   917,   922,   923,
     928,   929,   930,   938,   939,   944,   945,   950,   951,   955,
     956,   961,   962,   967,   968,   973,   974,   979,   980,   985,
     986,   991,   992,   997,   998,  1002,  1004,  1006,  1007,  1012,
    1013,  1018,  1019,  1024,  1025,  1030,  1031,  1036,  1037,  1042,
    1043,  1048,  1049,  1053,  1054,  1058,  1059,  1060,  1064,  1065,
    1066,  1067,  1068,  1078,  1079,  1080,  1087,  1088,  1089,  1098,
    1099,  1100,  1101,  1113,  1114,  1118,  1119,  1120,  1127,  1128,
    1134,  1135,  1136,  1145,  1146,  1153,  1154,  1160,  1161,  1165,
    1166,  1170,  1171,  1175,  1176,  1177,  1186,  1189,  1190,  1195,
    1202,  1204,  1206,  1207,  1212,  1213,  1218,  1220,  1221,  1225,
    1228,  1229,  1232,  1233,  1238,  1239,  1245,  1246,  1249,  1252,
    1255,  1258,  1259,  1260,  1266,  1268,  1269,  1270,  1271,  1279,
    1280,  1284,  1285,  1289,  1290,  1294,  1295,  1299,  1302,  1304,
    1305,  1310,  1312,  1314,  1316,  1318,  1319,  1323,  1324,  1329,
    1330,  1331,  1332,  1339,  1341,  1342,  1346,  1347,  1351,  1352,
    1353,  1358,  1359,  1363,  1364,  1368,  1369,  1370,  1375,  1376,
    1380,  1382,  1384,  1386,  1387,  1392,  1396,  1397,  1402,  1403,
    1408,  1409,  1414,  1415
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     126,     0,    -1,   127,    -1,    -1,   127,   128,    -1,    -1,
      25,     5,   115,   129,   170,    27,    -1,    -1,    25,     6,
     115,   130,   185,    27,    -1,    -1,    25,     7,   131,   253,
      27,    -1,    25,    90,   252,    27,    -1,    -1,    -1,    25,
      34,   115,   132,   156,   133,   237,    -1,    -1,    -1,    25,
      39,   134,   139,   135,   155,    -1,    -1,    -1,    25,    30,
     136,   148,   137,    27,    -1,    -1,    25,    29,   115,   138,
     241,   239,    -1,   364,   359,    11,    -1,    -1,   141,   140,
     139,    -1,    -1,    -1,    42,    61,   142,   376,    -1,    19,
      61,    22,    -1,    19,    61,   115,    -1,    -1,    23,    61,
     143,   376,    -1,    31,    61,    22,    -1,    -1,    31,    61,
     144,   223,    -1,    -1,    20,    61,   145,   376,    -1,    -1,
      18,    61,   146,   376,    -1,    -1,    17,    61,   147,   376,
      -1,    -1,   150,   149,   148,    -1,    -1,    -1,    20,    61,
     151,   376,    -1,    -1,    23,    61,   152,   376,    -1,    -1,
      19,    61,   153,   376,    -1,    -1,    17,    61,   154,   376,
      -1,    26,   359,    40,    -1,    27,    -1,    -1,    -1,   158,
     157,   156,    -1,    28,    61,   124,   168,   123,    -1,    -1,
     115,    61,   159,   183,    -1,   115,    61,    16,    -1,    -1,
      17,    61,   160,   376,    -1,    -1,   100,    61,   161,   376,
      -1,    -1,    31,    61,   162,   223,    -1,    32,    61,   114,
      -1,    -1,    33,    61,   163,   183,    -1,    -1,    30,    61,
     164,   376,    -1,   115,    -1,    35,   117,   115,    -1,    29,
     166,    -1,    34,   167,    -1,    -1,   117,   115,    -1,    -1,
     117,   115,    -1,   115,    -1,    -1,   115,   110,   169,   223,
      -1,   168,    63,   168,    -1,    -1,    -1,   170,   171,   172,
      -1,    -1,    21,    61,   173,   185,    -1,    -1,    51,    61,
     174,   376,    -1,    -1,    31,    61,   175,   223,    -1,    24,
      61,    36,    -1,    -1,    -1,    98,   124,   177,   183,    63,
     178,   183,   123,    -1,    98,   124,   123,    -1,    98,   122,
     121,    -1,    98,   122,   179,   121,    -1,   376,    69,   376,
      -1,   376,    69,   376,    63,   179,    -1,   215,    -1,   113,
      -1,    94,    -1,    93,    -1,    92,    -1,    91,    -1,    99,
      -1,    97,    -1,   176,    -1,   180,    -1,    76,   114,    -1,
     386,    -1,    83,   183,    -1,    -1,   124,   182,   185,   123,
      -1,   181,    -1,    -1,   183,    64,   184,   183,    -1,    -1,
     183,   186,   193,    -1,   195,    -1,   183,    49,    -1,   183,
      50,    -1,    -1,   181,   116,   187,   181,    -1,    -1,   183,
      77,   188,   183,    -1,    -1,   183,    81,   189,   183,    -1,
      -1,   183,    80,   190,   183,    -1,    -1,   183,    79,   191,
     183,    -1,    -1,   183,    82,   192,   183,    -1,    -1,    -1,
     183,   194,   193,    -1,    -1,   183,    63,   196,   183,    -1,
      -1,   195,    63,   197,   183,    -1,   180,    -1,    76,   114,
      -1,   107,   115,    -1,   391,    -1,    -1,   124,   199,   207,
     123,    -1,    -1,   198,    64,   200,   198,    -1,   198,    50,
      -1,    -1,   198,   116,   201,   198,    -1,    -1,   198,    77,
     202,   198,    -1,    -1,   198,    81,   203,   198,    -1,    -1,
     198,    80,   204,   198,    -1,    -1,   198,    79,   205,   198,
      -1,    -1,   198,    82,   206,   198,    -1,    -1,   198,   208,
     210,    -1,   212,    -1,    -1,   115,    61,   209,   198,    -1,
      -1,    -1,   198,   211,   210,    -1,    -1,   198,    63,   213,
     198,    -1,    -1,   212,    63,   214,   198,    -1,   114,    -1,
      36,    -1,    -1,    -1,    97,   124,   217,   225,   218,   234,
     123,    -1,    -1,    -1,    96,   124,   219,   225,    63,   220,
     236,   123,    -1,    -1,    -1,    95,   124,   221,   225,    63,
     222,   236,   123,    -1,    15,    -1,   216,    -1,   181,    -1,
      -1,   223,    64,   224,   223,    -1,   223,    49,    -1,   223,
      50,    -1,   180,    -1,   386,    -1,    83,   183,    -1,   216,
      -1,    -1,   115,    61,   226,   225,    -1,    -1,   225,    64,
     227,   225,    -1,    -1,   124,   228,   229,   123,    -1,    -1,
     225,   230,   234,    -1,   231,    -1,    -1,   225,    63,   232,
     225,    -1,    -1,   231,    63,   233,   225,    -1,    -1,    -1,
     225,   235,   234,    -1,   215,    -1,   165,    -1,    -1,    26,
     238,   253,    37,    -1,    27,    -1,    -1,    26,   240,   253,
      38,    -1,    38,    -1,    -1,    -1,   243,   242,   241,    -1,
      28,    61,   124,   246,   123,    -1,    -1,    30,    61,   124,
     244,   248,   123,    -1,    -1,   115,    61,   245,   183,    -1,
     115,    -1,    -1,   115,   110,   247,   183,    -1,   246,    63,
     246,    -1,    -1,   250,    63,   249,   248,    -1,   250,    -1,
     115,    -1,    -1,   115,   110,   251,   183,    -1,   115,    -1,
     115,    63,   252,    -1,    -1,   255,   254,   253,    -1,   255,
      -1,   284,    -1,    -1,    -1,    -1,    55,   124,   256,   292,
     123,   257,   261,   258,   259,    -1,   261,    -1,    -1,    -1,
      54,   260,   255,    -1,    -1,    -1,    -1,    -1,    48,   124,
     262,   277,    41,   263,   277,    41,   264,   277,   123,   265,
     261,    -1,    -1,    -1,    52,   124,   266,   253,   123,   122,
     267,   278,   121,    -1,    -1,    -1,    -1,    -1,    47,   124,
     268,   315,   110,   269,   198,   110,   270,   292,   123,   271,
     261,    -1,    -1,    -1,    -1,    -1,    46,   124,   272,   315,
     110,   273,   198,   110,   274,   292,   123,   275,   261,    -1,
      -1,   122,   276,   253,   121,    -1,   122,   121,    -1,   288,
      41,    -1,   292,    -1,    -1,    -1,    -1,   278,    53,   279,
     198,   110,   280,   253,    -1,    -1,    -1,    53,   281,   198,
     110,   282,   253,    -1,    -1,   278,    51,   110,   283,   253,
      -1,   285,    -1,   288,    -1,    -1,   285,    63,   286,   288,
      -1,    -1,   288,    63,   287,   288,    -1,    44,    -1,    45,
      -1,    -1,    43,   289,   376,    -1,    -1,     3,   290,   376,
      -1,    -1,    10,   291,   376,    -1,   292,    -1,   303,    -1,
      -1,   329,    61,   293,   292,    -1,    -1,   329,    69,   294,
     225,    -1,    -1,   329,    60,   295,   303,    -1,    -1,   329,
      59,   296,   303,    -1,    -1,   329,    58,   297,   303,    -1,
      -1,   329,    57,   298,   303,    -1,    -1,   329,    56,   299,
     303,    -1,    -1,   329,    68,   300,   225,    -1,    -1,    -1,
     329,    62,   301,   315,   110,   302,   292,    -1,    -1,   303,
      66,   304,   303,    -1,    -1,   303,    65,   305,   303,    -1,
      -1,    67,   306,   315,    -1,    -1,   315,    75,   307,   315,
      -1,    -1,   315,    74,   308,   315,    -1,    -1,   315,    73,
     309,   315,    -1,    -1,   315,    72,   310,   315,    -1,    -1,
     315,    71,   311,   315,    -1,    -1,   315,    70,   312,   315,
      -1,    -1,   315,   110,   313,   198,    -1,    -1,    76,   314,
     323,    -1,   315,    -1,   323,    -1,    -1,   315,    77,   316,
     315,    -1,    -1,   315,    76,   317,   315,    -1,    -1,   315,
      81,   318,   315,    -1,    -1,   315,    78,   319,   315,    -1,
      -1,   315,    80,   320,   315,    -1,    -1,   315,    79,   321,
     315,    -1,    -1,   315,    82,   322,   315,    -1,    -1,   329,
     324,   325,    -1,    -1,   329,   326,   327,    -1,    -1,    -1,
     329,   328,   327,    -1,    -1,    -1,    -1,    -1,   103,   124,
     330,   303,    63,   331,   303,   332,   354,    -1,    -1,    -1,
      90,   124,   333,   303,   334,   354,    -1,    -1,    -1,   104,
     124,   335,   303,    63,   336,   303,   123,    -1,    -1,    -1,
      -1,   101,   124,   337,   303,    63,   338,   303,    63,   339,
     303,   123,    -1,    -1,    88,   340,   376,    -1,    -1,    -1,
      89,   124,   341,   303,   342,   356,    -1,    -1,   109,   122,
     343,   253,   121,    -1,    -1,    -1,    85,   124,   344,   303,
      63,   345,   303,   123,    -1,    -1,    42,   358,   122,   346,
     253,   121,    -1,    -1,   108,   122,   347,   253,   121,    -1,
      -1,   100,   348,   376,    -1,    -1,   105,   349,   376,    -1,
      -1,   106,   350,   376,    -1,    -1,    -1,    87,   124,   351,
     303,    63,   352,   303,   123,    -1,   376,    50,    -1,    -1,
     376,   116,   353,   376,    -1,   102,   124,   376,    63,   376,
     123,    -1,   371,    -1,   123,    -1,    -1,    63,   355,   303,
     123,    -1,    -1,    63,   357,   303,   123,    -1,   123,    -1,
      -1,   120,   114,   119,    -1,   361,   360,    -1,    -1,   361,
     360,    -1,    -1,    12,   364,   362,   365,    -1,    -1,     9,
     124,   363,   225,   123,    -1,    -1,   114,   364,    -1,    36,
     364,    -1,    13,   364,    -1,   115,   364,    -1,    -1,    -1,
      69,   366,   225,   367,   368,    -1,   368,    -1,    -1,    -1,
      -1,     8,   124,   369,   225,   123,   370,   368,    -1,    -1,
     112,   372,   376,    -1,    -1,   111,   373,   376,    -1,    -1,
      86,   374,   376,    -1,    -1,    94,   375,   376,    -1,   107,
     115,    -1,   378,    -1,    -1,   124,   377,   253,   123,    -1,
     215,    -1,   113,    -1,   165,    -1,   176,    -1,    -1,   376,
     379,   381,    -1,    -1,   378,    64,   380,   378,    -1,    -1,
      -1,    -1,   120,   382,   396,   119,   383,   381,    -1,   384,
      -1,    -1,   118,   385,   376,    -1,    -1,   165,   387,   388,
      -1,    -1,    -1,   120,   389,   399,   119,    -1,    -1,   118,
     390,   183,    -1,    -1,   165,   392,   393,    -1,    -1,    -1,
     120,   394,   402,   119,    -1,    -1,   118,   395,   198,    -1,
     303,    -1,   399,    -1,   397,    -1,    -1,   397,    64,   398,
     397,    -1,   115,   117,   115,    -1,    -1,   115,    61,   400,
     303,    -1,    -1,   399,    41,   401,   399,    -1,    -1,   115,
      61,   403,   183,    -1,    -1,   402,    41,   404,   402,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   143,   143,   145,   146,   148,   148,   150,   150,   152,
     152,   153,   154,   154,   154,   155,   155,   155,   156,   156,
     156,   157,   157,   159,   162,   162,   163,   164,   164,   165,
     166,   167,   167,   168,   169,   169,   171,   171,   172,   172,
     173,   173,   175,   175,   176,   177,   177,   179,   179,   181,
     181,   183,   183,   185,   186,   188,   188,   188,   189,   190,
     190,   191,   192,   192,   194,   194,   196,   196,   198,   199,
     199,   201,   201,   204,   205,   206,   207,   209,   210,   212,
     213,   216,   217,   217,   218,   220,   220,   220,   221,   221,
     223,   223,   224,   224,   225,   228,   229,   228,   231,   232,
     233,   236,   238,   241,   242,   243,   244,   245,   246,   247,
     248,   249,   251,   252,   253,   254,   255,   255,   257,   258,
     258,   261,   261,   262,   263,   264,   265,   265,   267,   267,
     269,   269,   271,   271,   273,   273,   274,   274,   276,   277,
     277,   278,   278,   280,   280,   282,   283,   284,   285,   286,
     286,   287,   287,   289,   290,   290,   292,   292,   294,   294,
     296,   296,   298,   298,   300,   300,   302,   302,   303,   304,
     304,   306,   307,   307,   308,   308,   310,   310,   312,   313,
     315,   315,   315,   316,   316,   316,   317,   317,   317,   318,
     320,   321,   322,   322,   324,   325,   327,   328,   329,   330,
     331,   331,   332,   332,   334,   334,   336,   336,   337,   338,
     338,   340,   340,   341,   342,   342,   344,   345,   347,   347,
     348,   349,   349,   350,   351,   351,   351,   352,   353,   353,
     354,   354,   355,   356,   356,   357,   358,   358,   358,   359,
     360,   360,   361,   362,   365,   365,   366,   367,   368,   368,
     368,   368,   370,   371,   372,   372,   373,   374,   375,   375,
     373,   377,   377,   377,   379,   380,   381,   381,   379,   383,
     384,   385,   385,   383,   387,   387,   388,   389,   391,   392,
     394,   394,   394,   396,   396,   396,   397,   397,   400,   401,
     403,   403,   404,   404,   407,   408,   409,   409,   410,   410,
     411,   411,   412,   414,   415,   415,   416,   416,   417,   417,
     419,   419,   421,   421,   423,   423,   425,   425,   427,   427,
     428,   428,   428,   432,   432,   433,   433,   434,   434,   435,
     435,   436,   436,   437,   437,   438,   438,   440,   440,   442,
     442,   444,   444,   445,   445,   446,   448,   449,   449,   451,
     451,   453,   453,   455,   455,   457,   457,   459,   459,   461,
     461,   463,   463,   466,   466,   467,   468,   468,   469,   471,
     471,   471,   471,   474,   474,   474,   477,   477,   477,   479,
     479,   479,   479,   481,   481,   482,   483,   482,   485,   485,
     486,   486,   486,   488,   488,   490,   490,   491,   491,   492,
     492,   493,   493,   494,   494,   494,   496,   497,   497,   499,
     501,   502,   502,   502,   504,   504,   505,   507,   508,   510,
     512,   513,   516,   516,   517,   517,   520,   521,   522,   523,
     524,   527,   527,   527,   528,   530,   531,   531,   531,   535,
     535,   536,   536,   537,   537,   538,   538,   539,   540,   542,
     542,   543,   544,   545,   546,   548,   548,   549,   549,   550,
     551,   552,   551,   554,   556,   556,   559,   559,   560,   561,
     561,   563,   563,   566,   566,   567,   568,   568,   570,   570,
     573,   574,   575,   577,   577,   579,   582,   582,   584,   584,
     588,   588,   590,   590
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ERROR", "SYNERROR", "PARAMETER",
  "DEFINE", "EVAL", "LBEGIN", "RBEGIN", "PRINT", "INPUT_ENDFILE",
  "INPUT_ENDL", "INPUT_CONT", "FALSE", "TOKEN", "INHERIT", "COND", "ANNOT",
  "TO", "FROM", "TYPE", "POETTYPE", "SYNTAX", "MESSAGE", "LTAG", "RTAG",
  "ENDTAG", "PARS", "XFORM", "OUTPUT", "PARSE", "LOOKAHEAD", "MATCH",
  "CODE", "GLOBAL", "SOURCE", "ENDCODE", "ENDXFORM", "INPUT", "ENDINPUT",
  "SEMICOLON", "DEBUG", "RETURN", "CONTINUE", "BREAK", "FOREACH_R",
  "FOREACH", "FOR", "DOT4", "DOT3", "DEFAULT", "SWITCH", "CASE", "ELSE",
  "IF", "MOD_ASSIGN", "DIVIDE_ASSIGN", "MULT_ASSIGN", "MINUS_ASSIGN",
  "PLUS_ASSIGN", "ASSIGN", "QUESTION", "COMMA", "TOR", "OR", "AND", "NOT",
  "DARROW", "ARROW", "NE", "GE", "GT", "EQ", "LE", "LT", "MINUS", "PLUS",
  "STR_CONCAT", "MOD", "DIVIDE", "MULTIPLY", "DCOLON", "TILT", "UMINUS",
  "INSERT", "LEN", "SPLIT", "COPY", "ERASE", "TRACE", "EXP", "NAME",
  "STRING", "INT", "LIST1", "LIST", "TUPLE", "MAP", "VAR", "REBUILD",
  "DUPLICATE", "RANGE", "REPLACE", "PERMUTE", "RESTORE", "SAVE", "CLEAR",
  "APPLY", "DELAY", "COLON", "CDR", "CAR", "ANY", "ICONST", "ID", "DOT2",
  "DOT", "POND", "RBR", "LBR", "RB", "LB", "RP", "LP", "$accept", "poet",
  "sections", "section", "@1", "@2", "@3", "@4", "@5", "@6", "@7", "@8",
  "@9", "@10", "inputAttrs", "@11", "inputAttr", "@12", "@13", "@14",
  "@15", "@16", "@17", "outputAttrs", "@18", "outputAttr", "@19", "@20",
  "@21", "@22", "inputRHS", "codeAttrs", "@23", "codeAttr", "@24", "@25",
  "@26", "@27", "@28", "@29", "varRef", "xformRef", "codeRef", "codePars",
  "@30", "paramAttrs", "@31", "paramAttr", "@32", "@33", "@34", "map",
  "@35", "@36", "mapEntries", "singletype", "typeSpec1", "@37", "typeSpec",
  "@38", "typeMulti", "@39", "@40", "@41", "@42", "@43", "@44", "@45",
  "typeList", "@46", "typeTuple", "@47", "@48", "patternSpec", "@49",
  "@50", "@51", "@52", "@53", "@54", "@55", "@56", "patternMulti", "@57",
  "@58", "patternList", "@59", "patternTuple", "@60", "@61", "constant",
  "parseType1", "@62", "@63", "@64", "@65", "@66", "@67", "parseType",
  "@68", "parseElem", "@69", "@70", "@71", "parseElemMulti", "@72",
  "parseElemTuple", "@73", "@74", "parseElemList", "@75", "constantOrVar",
  "codeRHS", "@76", "xformRHS", "@77", "xformAttrs", "@78", "xformAttr",
  "@79", "@80", "xformPars", "@81", "outputPars", "@82", "outputPar",
  "@83", "traceVars", "code", "@84", "codeIf", "@85", "@86", "@87",
  "codeIfHelp", "@88", "code1", "@89", "@90", "@91", "@92", "@93", "@94",
  "@95", "@96", "@97", "@98", "@99", "@100", "@101", "@102", "@103",
  "e_code4", "switchHelp", "@104", "@105", "@106", "@107", "@108",
  "code23", "code2", "@109", "@110", "code3", "@111", "@112", "@113",
  "code4", "@114", "@115", "@116", "@117", "@118", "@119", "@120", "@121",
  "@122", "@123", "code5", "@124", "@125", "@126", "@127", "@128", "@129",
  "@130", "@131", "@132", "@133", "@134", "code6", "@135", "@136", "@137",
  "@138", "@139", "@140", "@141", "code7", "@142", "code7Help", "@143",
  "code7Help2", "@144", "code8", "@145", "@146", "@147", "@148", "@149",
  "@150", "@151", "@152", "@153", "@154", "@155", "@156", "@157", "@158",
  "@159", "@160", "@161", "@162", "@163", "@164", "@165", "@166", "@167",
  "@168", "paramRHS", "@169", "eraseRHS", "@170", "debugConfig",
  "inputCodeList", "inputCodeList2", "inputCode", "@171", "@172",
  "inputBase", "lineAnnot", "@173", "@174", "typeMatch", "@175", "@176",
  "code9", "@177", "@178", "@179", "@180", "codeUnit", "@181", "varInvoke",
  "@182", "@183", "varInvoke1", "@184", "@185", "pond", "@186",
  "varInvokeType", "@187", "varInvokeType1", "@188", "@189",
  "varInvokePattern", "@190", "varInvokePattern1", "@191", "@192",
  "configOrSubRef", "selectRef", "@193", "configRef", "@194", "@195",
  "xformConfig", "@196", "@197", 0
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
     375,   376,   377,   378,   379
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   125,   126,   127,   127,   129,   128,   130,   128,   131,
     128,   128,   132,   133,   128,   134,   135,   128,   136,   137,
     128,   138,   128,   128,   140,   139,   139,   142,   141,   141,
     141,   143,   141,   141,   144,   141,   145,   141,   146,   141,
     147,   141,   149,   148,   148,   151,   150,   152,   150,   153,
     150,   154,   150,   155,   155,   156,   157,   156,   158,   159,
     158,   158,   160,   158,   161,   158,   162,   158,   158,   163,
     158,   164,   158,   165,   165,   165,   165,   166,   166,   167,
     167,   168,   169,   168,   168,   170,   171,   170,   173,   172,
     174,   172,   175,   172,   172,   177,   178,   176,   176,   176,
     176,   179,   179,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   181,   181,   181,   181,   182,   181,   183,   184,
     183,   186,   185,   185,   185,   185,   187,   185,   188,   185,
     189,   185,   190,   185,   191,   185,   192,   185,   193,   194,
     193,   196,   195,   197,   195,   198,   198,   198,   198,   199,
     198,   200,   198,   198,   201,   198,   202,   198,   203,   198,
     204,   198,   205,   198,   206,   198,   208,   207,   207,   209,
     207,   210,   211,   210,   213,   212,   214,   212,   215,   215,
     217,   218,   216,   219,   220,   216,   221,   222,   216,   216,
     223,   223,   224,   223,   223,   223,   225,   225,   225,   225,
     226,   225,   227,   225,   228,   225,   230,   229,   229,   232,
     231,   233,   231,   234,   235,   234,   236,   236,   238,   237,
     237,   240,   239,   239,   241,   242,   241,   243,   244,   243,
     245,   243,   246,   247,   246,   246,   249,   248,   248,   250,
     251,   250,   252,   252,   254,   253,   253,   253,   256,   257,
     258,   255,   255,   259,   260,   259,   262,   263,   264,   265,
     261,   266,   267,   261,   268,   269,   270,   271,   261,   272,
     273,   274,   275,   261,   276,   261,   261,   261,   277,   277,
     279,   280,   278,   281,   282,   278,   283,   278,   284,   284,
     286,   285,   287,   285,   288,   288,   289,   288,   290,   288,
     291,   288,   288,   292,   293,   292,   294,   292,   295,   292,
     296,   292,   297,   292,   298,   292,   299,   292,   300,   292,
     301,   302,   292,   304,   303,   305,   303,   306,   303,   307,
     303,   308,   303,   309,   303,   310,   303,   311,   303,   312,
     303,   313,   303,   314,   303,   303,   315,   316,   315,   317,
     315,   318,   315,   319,   315,   320,   315,   321,   315,   322,
     315,   324,   323,   326,   325,   325,   328,   327,   327,   330,
     331,   332,   329,   333,   334,   329,   335,   336,   329,   337,
     338,   339,   329,   340,   329,   341,   342,   329,   343,   329,
     344,   345,   329,   346,   329,   347,   329,   348,   329,   349,
     329,   350,   329,   351,   352,   329,   329,   353,   329,   329,
     329,   354,   355,   354,   357,   356,   356,   358,   358,   359,
     360,   360,   362,   361,   363,   361,   364,   364,   364,   364,
     364,   366,   367,   365,   365,   368,   369,   370,   368,   372,
     371,   373,   371,   374,   371,   375,   371,   371,   371,   377,
     376,   376,   376,   376,   376,   379,   378,   380,   378,   381,
     382,   383,   381,   381,   385,   384,   387,   386,   388,   389,
     388,   390,   388,   392,   391,   393,   394,   393,   395,   393,
     396,   396,   396,   398,   397,   397,   400,   399,   401,   399,
     403,   402,   404,   402
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     0,     6,     0,     6,     0,
       5,     4,     0,     0,     7,     0,     0,     6,     0,     0,
       6,     0,     6,     3,     0,     3,     0,     0,     4,     3,
       3,     0,     4,     3,     0,     4,     0,     4,     0,     4,
       0,     4,     0,     3,     0,     0,     4,     0,     4,     0,
       4,     0,     4,     3,     1,     0,     0,     3,     5,     0,
       4,     3,     0,     4,     0,     4,     0,     4,     3,     0,
       4,     0,     4,     1,     3,     2,     2,     0,     2,     0,
       2,     1,     0,     4,     3,     0,     0,     3,     0,     4,
       0,     4,     0,     4,     3,     0,     0,     8,     3,     3,
       4,     3,     5,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     2,     0,     4,     1,     0,
       4,     0,     3,     1,     2,     2,     0,     4,     0,     4,
       0,     4,     0,     4,     0,     4,     0,     4,     0,     0,
       3,     0,     4,     0,     4,     1,     2,     2,     1,     0,
       4,     0,     4,     2,     0,     4,     0,     4,     0,     4,
       0,     4,     0,     4,     0,     4,     0,     3,     1,     0,
       4,     0,     0,     3,     0,     4,     0,     4,     1,     1,
       0,     0,     7,     0,     0,     8,     0,     0,     8,     1,
       1,     1,     0,     4,     2,     2,     1,     1,     2,     1,
       0,     4,     0,     4,     0,     4,     0,     3,     1,     0,
       4,     0,     4,     0,     0,     3,     1,     1,     0,     4,
       1,     0,     4,     1,     0,     0,     3,     5,     0,     6,
       0,     4,     1,     0,     4,     3,     0,     4,     1,     1,
       0,     4,     1,     3,     0,     3,     1,     1,     0,     0,
       0,     9,     1,     0,     0,     3,     0,     0,     0,     0,
      13,     0,     0,     9,     0,     0,     0,     0,    13,     0,
       0,     0,     0,    13,     0,     4,     2,     2,     1,     0,
       0,     0,     7,     0,     0,     6,     0,     5,     1,     1,
       0,     4,     0,     4,     1,     1,     0,     3,     0,     3,
       0,     3,     1,     1,     0,     4,     0,     4,     0,     4,
       0,     4,     0,     4,     0,     4,     0,     4,     0,     4,
       0,     0,     7,     0,     4,     0,     4,     0,     3,     0,
       4,     0,     4,     0,     4,     0,     4,     0,     4,     0,
       4,     0,     4,     0,     3,     1,     1,     0,     4,     0,
       4,     0,     4,     0,     4,     0,     4,     0,     4,     0,
       4,     0,     3,     0,     3,     0,     0,     3,     0,     0,
       0,     0,     9,     0,     0,     6,     0,     0,     8,     0,
       0,     0,    11,     0,     3,     0,     0,     6,     0,     5,
       0,     0,     8,     0,     6,     0,     5,     0,     3,     0,
       3,     0,     3,     0,     0,     8,     2,     0,     4,     6,
       1,     1,     0,     4,     0,     4,     1,     0,     3,     2,
       0,     2,     0,     4,     0,     5,     0,     2,     2,     2,
       2,     0,     0,     5,     1,     0,     0,     0,     7,     0,
       3,     0,     3,     0,     3,     0,     3,     2,     1,     0,
       4,     1,     1,     1,     1,     0,     3,     0,     4,     0,
       0,     0,     6,     1,     0,     3,     0,     3,     0,     0,
       4,     0,     3,     0,     3,     0,     0,     4,     0,     3,
       1,     1,     1,     0,     4,     3,     0,     4,     0,     4,
       0,     4,     0,     4
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       3,     0,   426,     1,   426,     0,   426,   426,   426,     4,
       0,   429,     0,     0,     9,     0,    18,     0,    15,     0,
     428,   427,   430,     0,   426,     0,   420,     5,     7,     0,
      21,    44,    12,    26,   242,     0,   424,   422,    23,   419,
     420,    85,     0,   298,   300,    77,    79,     0,   179,   417,
     296,   294,   295,     0,     0,     0,     0,     0,   327,   343,
       0,   443,     0,   383,     0,     0,   445,     0,   397,     0,
       0,     0,     0,   399,   401,     0,     0,     0,   441,   439,
     452,   178,    73,   274,   449,   453,   454,   451,     0,   244,
     252,   247,   288,   289,   302,   303,   345,   346,   361,   410,
     455,   448,   224,     0,     0,     0,     0,    19,    42,    55,
       0,     0,     0,     0,     0,     0,     0,    16,    24,     0,
      11,     0,   435,   421,    86,     0,     0,   108,   107,   106,
     105,   110,   109,   104,   116,   466,   111,   112,   118,   121,
       0,   123,   103,   114,     0,     0,     0,    75,     0,    76,
       0,     0,     0,     0,   269,   264,   256,   261,   248,     0,
       0,   390,     0,   403,     0,   385,   373,     0,     0,    95,
       0,   379,     0,   369,   376,     0,     0,   447,   395,   388,
       0,     0,   276,     0,     0,    10,     0,   290,   277,   292,
     325,   323,   339,   337,   335,   333,   331,   329,   349,   347,
     353,   357,   355,   351,   359,   341,   316,   314,   312,   310,
     308,   304,   320,   318,   306,   365,   406,   407,   459,   457,
       0,     0,     0,     0,   225,    51,    49,    45,    47,     0,
      44,     0,     0,     0,     0,     0,     0,     0,     0,    13,
      56,    40,    38,     0,    36,    31,    34,    27,     0,    26,
     243,   189,     0,     0,     0,   110,    73,   204,   196,   199,
       0,   197,     0,   431,   423,   434,     6,     0,   113,   118,
     115,     0,   468,   126,   124,   125,   141,   119,   128,   134,
     132,   130,   136,   138,     8,   143,   299,   301,    78,    80,
      74,     0,   393,   297,     0,     0,   279,     0,     0,   328,
     361,   344,     0,   444,     0,   384,     0,     0,   446,    99,
       0,     0,    98,     0,   398,     0,     0,     0,     0,   400,
     402,     0,     0,   442,   440,     0,     0,   245,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   362,   363,     0,   464,   460,
     456,   463,     0,     0,     0,   230,   221,   223,    22,   224,
       0,     0,     0,     0,    20,    43,    62,     0,    71,    66,
       0,    69,    64,    59,     0,    55,     0,     0,    29,    30,
       0,     0,    33,     0,     0,     0,    54,    17,    25,   198,
     186,   183,   180,   200,     0,   202,   425,   436,     0,     0,
       0,     0,     0,    87,     0,   471,   469,   467,     0,     0,
       0,     0,     0,     0,     0,     0,   139,   122,     0,   418,
       0,     0,     0,     0,   278,     0,     0,     0,     0,   386,
     374,   100,     0,     0,     0,     0,     0,     0,     0,     0,
     275,   450,   291,   293,   326,   324,   340,   338,   336,   334,
     332,   330,   350,   348,   354,   358,   356,   352,   360,     0,
       0,   149,   473,   145,   342,   148,   317,   315,   313,   311,
     309,   305,     0,   319,   307,   368,   408,     0,     0,   455,
     458,     0,   228,     0,     0,   226,    52,    50,    46,    48,
       0,     0,     0,     0,    68,     0,     0,    61,     0,   218,
     220,    14,    57,    41,    39,    37,    32,   191,   190,    35,
      28,     0,     0,     0,     0,     0,   206,     0,   208,     0,
       0,   432,    88,     0,    92,    90,   117,     0,     0,   127,
     142,   120,   129,   135,   133,   131,   137,   138,   144,     0,
     270,   265,   257,     0,   249,   391,   404,     0,     0,   101,
      96,   380,     0,   370,   377,   396,   389,   146,   147,     0,
     475,   153,   151,   156,   162,   160,   158,   164,   154,   321,
     364,   366,   465,    73,   480,     0,   482,   481,   232,     0,
       0,   231,     0,    63,    81,     0,    72,    67,    70,    65,
      60,     0,   194,   195,   192,    53,     0,     0,   181,   201,
     209,   213,   205,   211,   203,     0,   435,     0,    94,     0,
       0,   472,     0,     0,   140,   394,     0,     0,   279,   262,
       0,     0,     0,   414,   416,   387,   412,   411,   375,     0,
       0,     0,   409,     0,     0,    73,   166,     0,   168,   478,
     476,   474,     0,     0,     0,     0,     0,     0,     0,     0,
     368,   486,     0,   461,   483,   488,   233,     0,   227,   239,
       0,   238,   222,    82,     0,    58,     0,     0,   187,   184,
     213,     0,   214,   207,     0,   437,   433,    89,    93,    91,
     470,     0,     0,     0,     0,   250,     0,     0,     0,     0,
       0,   102,     0,     0,   371,     0,   169,   174,   171,   150,
     176,     0,     0,   152,   157,   163,   161,   159,   165,   155,
     322,   367,     0,   485,   459,     0,     0,     0,   235,   240,
     229,   236,     0,    84,   219,   193,     0,     0,     0,   210,
     213,   212,   435,   271,   266,   258,   283,     0,   253,   392,
     405,     0,     0,    97,   381,     0,   378,     0,     0,   172,
     167,     0,   479,     0,     0,   487,   462,     0,   484,   489,
     234,     0,     0,    83,   217,   216,     0,     0,   182,   215,
     438,     0,     0,   279,     0,     0,   280,   263,   254,   251,
     415,   413,     0,   372,   170,   175,   171,   177,   490,   492,
     477,   241,   237,   188,   185,     0,     0,     0,     0,   286,
       0,     0,     0,   173,     0,     0,   272,   267,   259,   284,
       0,     0,   255,   382,   491,   493,     0,     0,     0,     0,
     287,   281,   273,   268,   260,   285,     0,   282
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     9,    41,    42,    29,   109,   384,    33,
     248,    31,   229,   102,   117,   249,   118,   394,   391,   393,
     390,   387,   386,   107,   230,   108,   372,   373,   371,   370,
     397,   239,   385,   240,   508,   500,   506,   503,   505,   502,
      85,   147,   149,   595,   732,   124,   267,   413,   617,   620,
     619,    86,   313,   640,   310,   137,   269,   271,   139,   420,
     140,   283,   418,   421,   424,   423,   422,   425,   427,   547,
     141,   419,   428,   759,   569,   652,   658,   653,   656,   655,
     654,   657,   647,   708,   757,   760,   796,   648,   758,   761,
      87,   259,   524,   680,   523,   737,   522,   736,   519,   677,
     682,   525,   529,   404,   527,   611,   528,   681,   684,   683,
     740,   776,   511,   601,   368,   494,   223,   369,   224,   590,
     493,   589,   727,   670,   772,   671,   771,    35,    88,   186,
      89,   298,   630,   748,   789,   811,    90,   296,   628,   783,
     828,   297,   694,   295,   627,   782,   827,   294,   626,   781,
     826,   183,   433,   747,   810,   836,   784,   829,   820,    91,
      92,   328,   329,    93,   153,   144,   145,    94,   351,   354,
     350,   349,   348,   347,   346,   353,   352,   659,    95,   331,
     330,   159,   337,   336,   335,   334,   333,   332,   345,   160,
      96,   339,   338,   343,   340,   342,   341,   344,    97,   215,
     355,   485,   580,   660,   300,   317,   643,   755,   307,   558,
     318,   644,   315,   641,   792,   164,   306,   557,   322,   302,
     631,   430,   321,   170,   175,   176,   304,   632,   357,   638,
     700,   635,   699,   152,    25,    39,    26,   122,   121,    10,
     264,   408,   616,   265,   530,   742,    99,   181,   180,   162,
     167,   100,   184,   101,   218,   362,   360,   488,   724,   361,
     487,   143,   272,   417,   538,   537,   475,   570,   651,   712,
     711,   585,   586,   725,   587,   722,   726,   764,   814,   815
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -626
static const yytype_int16 yypact[] =
{
    -626,    30,    20,  -626,     3,   340,     3,     3,     3,  -626,
      85,  -626,   -67,   -62,  -626,   -57,  -626,   -52,  -626,   -41,
    -626,  -626,  -626,   -32,     3,    77,    85,  -626,  -626,  1354,
    -626,   259,  -626,   416,    56,    74,  -626,  -626,  -626,  -626,
      85,  -626,   982,  -626,  -626,    32,    36,    53,  -626,    45,
    -626,  -626,  -626,    91,   118,   140,   149,   159,  -626,  -626,
     161,  -626,   167,  -626,   174,   186,  -626,    14,  -626,   190,
     197,   216,   224,  -626,  -626,    65,   201,   210,  -626,  -626,
    -626,  -626,  -626,   235,  -626,  -626,  -626,  -626,   332,    27,
    -626,  -626,   303,     8,  -626,   145,   636,  -626,   770,  -626,
     -16,   307,    -7,   306,   312,   314,   315,  -626,  -626,    29,
     320,   329,   330,   339,   345,   346,   351,  -626,  -626,   -41,
    -626,   789,     4,  -626,   350,   299,   982,  -626,  -626,  -626,
    -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,   300,   256,
     388,   354,  -626,  -626,   188,   188,   313,  -626,   322,  -626,
     323,   304,   305,   188,  -626,  -626,  -626,  -626,  -626,  1801,
    1801,  -626,   188,  -626,   188,  -626,  -626,   188,     7,   308,
     188,  -626,   188,  -626,  -626,   188,   188,  -626,  -626,  -626,
     188,   188,  -626,  1354,  1354,  -626,  1354,  -626,  -626,  -626,
    -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,
    -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,
    -626,  -626,  -626,  -626,  -626,  1801,  -626,  -626,    25,  -626,
     361,   368,   379,    72,  -626,  -626,  -626,  -626,  -626,   414,
     259,   383,   384,   394,   398,   400,   401,   403,   405,  -626,
    -626,  -626,  -626,    -8,  -626,  -626,   448,  -626,   199,   416,
    -626,  -626,   982,   347,   349,   356,   413,  -626,  -626,  -626,
     -39,  -626,   357,  -626,  -626,  -626,  -626,   273,  -626,  -626,
    -626,   982,    87,  -626,  -626,  -626,  -626,  -626,  -626,  -626,
    -626,  -626,  -626,   982,  -626,  -626,  -626,  -626,  -626,  -626,
    -626,   366,  -626,  -626,  1801,  1801,  1049,  1354,  1049,   848,
    -626,  -626,  1049,  -626,  1049,  -626,  1049,  1049,  -626,  -626,
     369,   406,  -626,   982,  -626,  1049,   433,  1049,  1049,  -626,
    -626,  1354,  1354,  -626,  -626,   385,   382,  -626,  1627,  1627,
    1049,  1049,  1801,  1801,  1801,  1801,  1801,  1801,  1801,  1801,
    1801,  1801,  1801,  1801,  1801,  1388,  1049,  1049,  1049,  1049,
    1049,  1049,  1801,   789,   789,  -626,  -626,   188,  -626,  -626,
    -626,  -626,   188,   396,   402,  -626,  -626,  -626,  -626,    -7,
     188,   188,   188,   188,  -626,  -626,  -626,   407,  -626,  -626,
     415,  -626,  -626,   511,   222,    29,   188,   188,  -626,  -626,
     188,   188,  -626,    80,   188,    85,  -626,  -626,  -626,  -626,
    -626,  -626,  -626,  -626,   789,  -626,  -626,  -626,   789,   469,
     471,   474,   475,  -626,   417,  -626,  -626,  -626,   982,   982,
     982,   982,   982,   982,   982,   982,   473,  -626,   982,  -626,
    1354,   437,   478,   497,  -626,   419,   420,   122,   134,   145,
     145,  -626,   188,   208,   155,   188,   200,   250,   423,   424,
    -626,  -626,  -626,  -626,   482,  -626,   848,   848,   848,   848,
     848,   848,   370,   370,   467,   467,   467,   467,   467,   436,
     447,  -626,  -626,  -626,   489,  -626,   145,   145,   145,   145,
     145,  -626,   733,   487,   487,  1801,  -626,   188,  1714,  -626,
    -626,   452,  -626,   982,  1354,  -626,  -626,  -626,  -626,  -626,
     188,   458,   188,    80,  -626,   982,   188,  -626,   982,  -626,
    -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,   142,
    -626,   523,   789,   789,   789,   789,   226,   451,   513,   789,
     789,   487,  -626,   542,  -626,  -626,  -626,   982,   470,  -626,
     473,  -626,   473,   473,   473,   473,   473,   982,   473,   461,
    -626,  -626,  -626,   464,  -626,  -626,  -626,   -19,   -11,   524,
    -626,  -626,   466,  -626,  -626,  -626,  -626,  -626,  -626,  1748,
     112,  -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,
    -626,  -626,  -626,    -6,   145,   476,   532,   556,   488,    16,
     485,   473,   565,  -626,   492,    23,  -626,   142,   473,  -626,
     473,  1354,  -626,  -626,  -626,  -626,   232,   291,   487,   487,
    -626,   789,  -626,  -626,  -626,   -24,   600,   982,  -626,    80,
     188,  -626,   552,   -26,  -626,  -626,  1388,  1388,  1049,  -626,
    1512,  1049,  1049,  -626,  -626,  -626,  -626,  -626,  -626,   188,
     982,  1049,  -626,  1049,  1049,   553,   164,   500,   561,  -626,
    -626,  -626,  1388,  1388,  1388,  1388,  1388,  1388,  1388,  1049,
    1801,  -626,   512,  -626,  -626,  -626,  -626,   452,  -626,   505,
     503,   566,  -626,  -626,   458,  -626,   591,    80,  -626,  -626,
     789,   789,   487,  -626,   789,  -626,  -626,  -626,   142,  -626,
    -626,   344,   418,   595,   596,  -626,   601,    37,    59,  1049,
    1049,  -626,   -17,   278,   145,    61,  -626,  -626,  1388,  -626,
    -626,  1388,   535,   316,   158,    48,    48,    48,    48,  -626,
    -626,  -626,  1049,  -626,    25,   536,   470,   982,  -626,  -626,
    -626,  -626,    80,  -626,  -626,  -626,    47,    47,   529,   487,
     789,   487,   600,  -626,  -626,  -626,  -626,    38,   602,  -626,
    -626,    67,    75,  -626,  -626,   -11,  -626,  1388,  1388,   489,
    -626,  1388,  -626,   593,   -23,   145,  -626,   538,  -626,   556,
     473,   982,   485,   142,  -626,  -626,   550,   555,  -626,  -626,
    -626,  1049,  1049,  1049,  1388,   569,  -626,  -626,  -626,  -626,
    -626,  -626,  1049,  -626,   489,   489,  1388,   489,  -626,  -626,
    -626,   473,  -626,  -626,  -626,   557,   559,   560,   530,  -626,
    1388,  1354,    86,  -626,   982,   535,  -626,  -626,  -626,  -626,
    1354,   589,  -626,  -626,   473,   635,  1512,  1512,  1512,  1354,
    -626,  -626,  -626,  -626,  -626,  -626,  1354,  -626
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,
    -626,  -626,  -626,  -626,   435,  -626,  -626,  -626,  -626,  -626,
    -626,  -626,  -626,   456,  -626,  -626,  -626,  -626,  -626,  -626,
    -626,   302,  -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,
     -36,  -626,  -626,    15,  -626,  -626,  -626,  -626,  -626,  -626,
    -626,   564,  -626,  -626,    49,   598,   -40,  -626,   -94,  -626,
    -267,  -626,  -626,  -626,  -626,  -626,  -626,  -626,   146,  -626,
    -626,  -626,  -626,   280,  -626,  -626,  -626,  -626,  -626,  -626,
    -626,  -626,  -626,  -626,  -626,  -101,  -626,  -626,  -626,  -626,
     239,  -390,  -626,  -626,  -626,  -626,  -626,  -626,  -496,  -626,
    -120,  -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,  -593,
    -626,   -35,  -626,  -626,  -626,  -626,   328,  -626,  -626,  -626,
    -626,    31,  -626,   -69,  -626,  -626,  -626,   604,  -155,  -626,
     -87,  -626,  -626,  -626,  -626,  -626,  -625,  -626,  -626,  -626,
    -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,
    -626,  -626,  -615,  -626,  -626,  -626,  -626,  -626,  -626,  -626,
    -626,  -626,  -626,  -319,  -626,  -626,  -626,  -279,  -626,  -626,
    -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,  -280,  -626,
    -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,
     -82,  -626,  -626,  -626,  -626,  -626,  -626,  -626,   567,  -626,
    -626,  -626,    66,  -626,   -29,  -626,  -626,  -626,  -626,  -626,
    -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,
    -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,   -27,
    -626,  -626,  -626,  -626,   334,   690,   143,  -626,  -626,   517,
    -626,  -626,  -626,  -605,  -626,  -626,  -626,  -626,  -626,  -626,
    -626,  1148,  -626,   371,  -626,  -626,    10,  -626,  -626,  -626,
    -626,   -46,  -626,  -626,  -626,  -626,  -626,  -626,  -626,  -626,
    -626,  -626,     6,  -626,  -530,  -626,  -626,   -79,  -626,  -626
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -247
static const yytype_int16 yytable[] =
{
      98,   260,   138,   518,   414,   695,   135,   597,   623,   452,
     453,   686,   262,   693,   388,   665,     4,   434,   799,   436,
      -2,   220,   437,   221,   438,   405,   439,   440,   325,   326,
       3,   327,   270,     4,   216,   444,    45,   446,   447,     6,
     405,    46,    47,    48,   633,     5,   231,   277,    27,   188,
     454,   455,   636,    28,  -246,   661,     6,   232,    30,   233,
     234,   235,   236,    32,  -246,  -246,   476,   477,   478,   479,
     480,   189,   481,   263,    34,   261,    45,   299,  -246,   667,
    -246,    46,    47,    48,   406,   135,   674,   738,    38,   785,
     135,   786,    36,   690,    23,   251,   800,    24,   366,   685,
     217,   120,   190,   191,   634,    67,   753,   389,   222,    45,
     367,   662,   637,   518,    46,    47,    48,     7,     8,   119,
      80,    81,    82,   688,   190,   191,   190,   191,   309,   237,
     577,    84,   190,   191,     7,     8,   168,   780,   169,   668,
     190,   191,   435,   358,   238,   359,   675,   779,  -246,   146,
    -246,   190,   191,   148,    98,    98,   125,    98,   399,   787,
     749,    81,    82,   126,   578,   151,   448,   449,   807,    40,
     150,   127,   128,   129,   130,   253,   254,   255,    67,   132,
     177,   735,   750,    40,   756,   555,   356,   190,   191,   426,
     790,   602,   603,   133,    81,    82,   769,   556,   791,   190,
     191,   832,   833,   834,   134,   415,   604,   416,   584,   823,
     190,   191,   431,   432,   571,   154,   135,    45,   561,   443,
     190,   191,    46,    47,    48,   395,   396,   707,   572,   518,
     649,   138,   650,   483,   484,   135,   773,   574,   575,   576,
     577,   573,   155,   574,   575,   576,   577,   135,   509,   510,
     456,   457,   458,   459,   460,   461,   462,   463,   464,   465,
     466,   467,   468,   563,   156,   190,   191,    98,    98,    98,
     482,   560,   277,   157,   578,   549,   103,   135,   104,   105,
     578,   142,   106,   158,   526,   161,    67,   518,   531,   610,
     405,   163,    98,    98,   409,   678,   405,   410,   165,    98,
      98,    80,    81,    82,   411,   274,   275,   261,   261,   472,
     166,   696,    84,   564,   171,   190,   191,   135,   135,   276,
     277,   172,    98,   178,   412,   540,   541,   542,   543,   544,
     545,   546,   179,   278,   548,   279,   280,   281,   282,   592,
     173,   754,   518,   190,   191,    12,    13,    14,   174,   434,
     687,   697,   698,   517,   679,   405,   182,   135,   261,   185,
     142,   703,   261,   704,   705,   142,   187,   225,   135,    15,
      16,   219,   135,   226,    17,   227,   228,   266,   539,    18,
     720,   241,   135,   135,   135,   135,   135,   135,   135,   135,
     242,   243,   135,   573,   571,   574,   575,   576,   577,   591,
     244,    98,   606,   607,   608,   609,   245,   246,   572,   614,
     615,   598,   247,   268,   600,   284,   273,   285,   291,   751,
     752,   573,   363,   574,   575,   576,   577,   292,   288,   364,
      19,   312,   578,   110,   111,   112,   113,   289,   290,   114,
     365,   374,   765,   621,   376,   377,   676,   115,   200,   201,
     202,   203,   204,   426,   743,   378,   581,   135,   116,   379,
     578,   380,   381,   517,   382,    98,   383,   135,   571,   135,
     392,   400,   135,   401,   403,   442,   261,   261,   261,   261,
     402,   407,   572,   261,   261,   429,   135,   135,   135,   135,
     441,   142,   696,   135,   135,   573,   445,   574,   575,   576,
     577,   135,   805,   806,   434,   451,   450,   696,   696,   696,
     142,   135,   812,   198,   199,   200,   201,   202,   203,   204,
     491,    11,   142,    20,    21,    22,   492,   507,   744,   504,
     532,   501,   533,   472,   578,   534,   535,   277,   552,   571,
     536,    37,   553,   554,   565,   566,   702,   550,   191,   204,
     567,   405,   142,   572,   198,   199,   200,   201,   202,   203,
     204,   739,   568,   605,   741,   261,   573,   588,   574,   575,
     576,   577,    98,   594,   612,   135,   613,   138,   618,   517,
     571,   135,   625,   135,   142,   622,   629,   639,   551,   642,
     472,   472,   142,   142,   572,   663,   664,   665,   666,    98,
     669,    98,   673,   672,   135,   578,   136,   573,   262,   574,
     575,   576,   577,   661,   706,   729,   472,   472,   472,   472,
     472,   472,   472,   709,   710,   474,   730,   723,   734,   731,
      98,   581,   142,   770,   261,   261,   745,   517,   261,   571,
     819,   135,   188,   142,   135,   135,   578,   142,   135,   746,
     763,   767,   778,   572,   798,   662,   788,   142,   142,   142,
     142,   142,   142,   142,   142,   830,   573,   142,   574,   575,
     576,   577,   472,   803,   835,   472,   799,   801,   804,   809,
     816,   837,   817,   818,   398,   136,   375,   512,   701,   733,
     136,   135,   517,   624,   261,   813,   135,   495,   728,   831,
     774,   774,   777,   802,   135,   578,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   258,
     824,   472,   472,   250,   822,   472,   721,   301,   793,   521,
     123,   768,   142,   490,   766,   135,   825,     0,     0,     0,
       0,     0,   142,     0,   142,     0,   205,   142,   472,     0,
       0,     0,    98,    98,    98,     0,     0,     0,     0,     0,
     472,   142,   142,   142,   142,     0,     0,     0,   142,   142,
       0,     0,     0,     0,   472,     0,   142,     0,   135,     0,
       0,     0,    98,     0,     0,     0,   142,     0,     0,     0,
       0,    98,     0,     0,     0,     0,     0,    98,    98,    98,
      98,     0,     0,     0,   251,     0,     0,    98,   142,   198,
     199,   200,   201,   202,   203,   204,   136,     0,    45,     0,
       0,     0,     0,    46,    47,    48,   206,   207,   208,   209,
     210,   211,   212,     0,     0,   136,     0,     0,   213,   214,
       0,     0,     0,   579,     0,     0,     0,   136,     0,   646,
     142,     0,     0,     0,     0,     0,   142,     0,   142,     0,
       0,     0,     0,     0,     0,   142,   142,     0,     0,     0,
       0,     0,   252,     0,     0,     0,     0,   136,     0,   142,
     127,   128,   129,   130,   253,   254,   255,    67,   132,     0,
       0,   142,   142,   142,   142,   142,   142,   142,     0,     0,
       0,     0,   133,    81,   256,     0,   691,   692,     0,   136,
       0,     0,     0,   257,     0,     0,   142,   136,   136,   142,
     142,     0,     0,   142,   198,   199,   200,   201,   202,   203,
     204,     0,   713,   714,   715,   716,   717,   718,   719,     0,
       0,     0,     0,   473,     0,     0,     0,   142,     0,     0,
     142,   258,   258,     0,     0,     0,     0,   136,     0,     0,
       0,     0,     0,     0,     0,     0,   142,     0,   136,     0,
       0,   142,   136,     0,     0,   775,   775,     0,     0,   142,
       0,     0,   136,   136,   136,   136,   136,   136,   136,   136,
       0,   762,   136,     0,     0,     0,   142,   142,     0,     0,
     142,     0,   258,     0,     0,     0,   258,     0,     0,     0,
     142,    45,     0,     0,     0,     0,    46,    47,    48,     0,
       0,     0,     0,   142,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   142,     0,   794,   795,     0,
       0,   797,     0,     0,     0,     0,     0,     0,     0,   142,
       0,     0,     0,   142,     0,     0,     0,   136,   125,     0,
       0,     0,     0,     0,   808,   126,     0,   136,     0,   136,
       0,     0,   136,   127,   128,   129,   130,     0,    45,   131,
      67,   132,     0,    46,    47,    48,   136,   136,   136,   136,
     821,    49,     0,   136,   136,   133,    81,    82,     0,     0,
       0,   136,     0,     0,     0,     0,   134,     0,     0,     0,
       0,   136,     0,     0,     0,     0,    58,     0,     0,     0,
     258,   258,   258,   258,     0,    59,     0,   258,   258,     0,
       0,     0,     0,   136,    60,    61,    62,    63,    64,    65,
       0,     0,     0,    66,     0,     0,     0,    67,     0,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,     0,
      78,    79,    80,    81,    82,     0,     0,   473,     0,     0,
       0,     0,     0,    84,     0,   136,     0,     0,     0,     0,
       0,   136,     0,   136,     0,     0,     0,     0,     0,     0,
     136,   136,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   136,     0,     0,     0,     0,   258,
       0,     0,     0,     0,     0,     0,   136,   136,   136,   136,
     136,   136,   136,     0,   473,   473,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   136,     0,     0,   136,   136,     0,     0,   136,     0,
     473,   473,   473,   473,   473,   473,   473,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   136,     0,     0,   136,     0,     0,   258,   258,
       0,     0,   258,     0,     0,     0,     0,     0,     0,     0,
       0,   136,   286,   287,     0,     0,   136,     0,     0,     0,
       0,   293,     0,     0,   136,     0,   473,     0,     0,   473,
     303,     0,   305,     0,     0,   308,   311,     0,   314,     0,
     316,   136,   136,   319,   320,   136,     0,     0,   323,   324,
       0,     0,     0,     0,     0,   136,     0,     0,   258,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   136,     0,
       0,     0,     0,     0,     0,   473,   473,    43,     0,   473,
     136,     0,     0,     0,    44,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   136,     0,     0,     0,   136,     0,
       0,     0,   473,    45,     0,     0,     0,     0,    46,    47,
      48,     0,     0,     0,   473,     0,    49,    50,    51,    52,
      53,    54,    55,     0,     0,     0,    56,     0,   473,    57,
       0,     0,     0,     0,     0,     0,     0,    45,     0,     0,
       0,    58,    46,    47,    48,     0,     0,     0,     0,     0,
      59,     0,     0,     0,     0,     0,     0,     0,     0,    60,
      61,    62,    63,    64,    65,     0,     0,     0,    66,     0,
       0,     0,    67,     0,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,   469,    78,    79,    80,    81,    82,
       0,     0,     0,     0,     0,     0,    83,     0,    84,   127,
     128,   129,   130,     0,     0,   131,    67,   132,     0,     0,
       0,     0,     0,     0,     0,   470,     0,     0,     0,     0,
       0,   133,    81,    82,     0,   486,     0,     0,     0,     0,
     489,     0,   471,     0,     0,    43,     0,     0,   496,   497,
     498,   499,    44,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   513,   514,     0,     0,   515,   516,
       0,    45,   520,     0,     0,     0,    46,    47,    48,     0,
       0,     0,     0,     0,    49,    50,    51,    52,    53,    54,
      55,     0,     0,     0,    56,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    58,
       0,     0,     0,     0,     0,     0,     0,     0,    59,     0,
     559,     0,     0,   562,     0,     0,     0,    60,    61,    62,
      63,    64,    65,     0,     0,     0,    66,     0,     0,     0,
      67,     0,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,     0,    78,    79,    80,    81,    82,     0,     0,
      43,     0,     0,     0,    83,   582,    84,    44,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   593,     0,
     596,     0,     0,     0,   599,     0,    45,     0,     0,     0,
       0,    46,    47,    48,     0,     0,     0,     0,     0,    49,
      50,    51,    52,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    58,     0,     0,     0,     0,     0,
       0,     0,     0,    59,     0,     0,     0,     0,     0,     0,
       0,     0,    60,    61,    62,    63,    64,    65,     0,     0,
       0,    66,     0,     0,     0,    67,     0,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,     0,    78,    79,
      80,    81,    82,    45,     0,     0,     0,     0,    46,    47,
      48,    84,     0,     0,     0,     0,    49,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   689,     0,
       0,     0,     0,     0,     0,     0,     0,    45,     0,     0,
       0,    58,    46,    47,    48,     0,     0,   311,     0,     0,
      59,     0,     0,     0,     0,     0,     0,     0,     0,    60,
      61,    62,    63,    64,    65,     0,     0,     0,    66,     0,
       0,     0,    67,     0,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,   469,    78,    79,    80,    81,   583,
      45,     0,     0,     0,     0,    46,    47,    48,    84,   127,
     128,   129,   130,    49,     0,   131,    67,   132,     0,     0,
       0,     0,     0,     0,     0,   470,     0,     0,     0,     0,
       0,   133,    81,   645,     0,     0,     0,     0,     0,     0,
       0,     0,   471,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    60,    61,    62,    63,
      64,    65,     0,     0,     0,    66,     0,     0,     0,    67,
       0,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,     0,    78,    79,    80,    81,    82,     0,     0,     0,
       0,     0,     0,     0,     0,    84
};

static const yytype_int16 yycheck[] =
{
      29,   121,    42,   393,   271,   630,    42,   503,   538,   328,
     329,   616,     8,   628,    22,    41,    13,   296,    41,   298,
       0,    28,   302,    30,   304,    64,   306,   307,   183,   184,
       0,   186,   126,    13,    50,   315,    29,   317,   318,    36,
      64,    34,    35,    36,    63,    25,    17,    64,   115,    41,
     330,   331,    63,   115,    27,    61,    36,    28,   115,    30,
      31,    32,    33,   115,    37,    38,   346,   347,   348,   349,
     350,    63,   351,    69,   115,   121,    29,   159,    51,    63,
      53,    34,    35,    36,   123,   121,    63,   680,    11,    51,
     126,    53,   124,   119,     9,    15,   119,    12,    26,   123,
     116,    27,    65,    66,   123,    98,   123,   115,   115,    29,
      38,   117,   123,   503,    34,    35,    36,   114,   115,    63,
     113,   114,   115,   619,    65,    66,    65,    66,   121,   100,
      82,   124,    65,    66,   114,   115,   122,   742,   124,   123,
      65,    66,   297,   118,   115,   120,   123,   740,   121,   117,
     123,    65,    66,   117,   183,   184,    76,   186,   252,   121,
     123,   114,   115,    83,   116,   120,   321,   322,   783,    26,
     117,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     115,   677,   123,    40,   123,    63,   215,    65,    66,   283,
     123,    49,    50,   113,   114,   115,   726,    63,   123,    65,
      66,   826,   827,   828,   124,   118,    64,   120,   488,   123,
      65,    66,   294,   295,    50,   124,   252,    29,    63,   313,
      65,    66,    34,    35,    36,    26,    27,    63,    64,   619,
     118,   271,   120,   353,   354,   271,   732,    79,    80,    81,
      82,    77,   124,    79,    80,    81,    82,   283,    26,    27,
     332,   333,   334,   335,   336,   337,   338,   339,   340,   341,
     342,   343,   344,    63,   124,    65,    66,   296,   297,   298,
     352,    63,    64,   124,   116,   430,    17,   313,    19,    20,
     116,    42,    23,   124,   404,   124,    98,   677,   408,    63,
      64,   124,   321,   322,    21,    63,    64,    24,   124,   328,
     329,   113,   114,   115,    31,    49,    50,   353,   354,   345,
     124,   630,   124,    63,   124,    65,    66,   353,   354,    63,
      64,   124,   351,   122,    51,   419,   420,   421,   422,   423,
     424,   425,   122,    77,   428,    79,    80,    81,    82,   494,
     124,    63,   732,    65,    66,     5,     6,     7,   124,   628,
     617,   631,   632,   393,    63,    64,   121,   393,   404,    27,
     121,   641,   408,   643,   644,   126,    63,    61,   404,    29,
      30,    64,   408,    61,    34,    61,    61,    27,   418,    39,
     659,    61,   418,   419,   420,   421,   422,   423,   424,   425,
      61,    61,   428,    77,    50,    79,    80,    81,    82,   493,
      61,   430,   522,   523,   524,   525,    61,    61,    64,   529,
     530,   505,    61,   114,   508,    27,   116,    63,   114,   699,
     700,    77,    61,    79,    80,    81,    82,   122,   115,    61,
      90,   123,   116,    17,    18,    19,    20,   115,   115,    23,
      61,    27,   722,   537,    61,    61,   601,    31,    78,    79,
      80,    81,    82,   547,   110,    61,   485,   493,    42,    61,
     116,    61,    61,   503,    61,   494,    61,   503,    50,   505,
      22,   124,   508,   124,    61,    69,   522,   523,   524,   525,
     124,   124,    64,   529,   530,   119,   522,   523,   524,   525,
     121,   252,   811,   529,   530,    77,    63,    79,    80,    81,
      82,   537,   781,   782,   783,   123,   121,   826,   827,   828,
     271,   547,   792,    76,    77,    78,    79,    80,    81,    82,
     124,     4,   283,     6,     7,     8,   124,    16,   110,   114,
      61,   124,    61,   569,   116,    61,    61,    64,    41,    50,
     123,    24,   123,   123,   121,   121,   640,   110,    66,    82,
     114,    64,   313,    64,    76,    77,    78,    79,    80,    81,
      82,   681,   115,    40,   684,   611,    77,   115,    79,    80,
      81,    82,   601,   115,   123,   611,    63,   617,    36,   619,
      50,   617,   121,   619,   345,   115,   122,    63,   110,   123,
     626,   627,   353,   354,    64,   119,    64,    41,   110,   628,
     115,   630,   110,    38,   640,   116,    42,    77,     8,    79,
      80,    81,    82,    61,    61,   110,   652,   653,   654,   655,
     656,   657,   658,   123,    63,   345,   123,   115,    37,    63,
     659,   660,   393,   727,   680,   681,    41,   677,   684,    50,
     110,   677,    41,   404,   680,   681,   116,   408,   684,    53,
     115,   115,   123,    64,    61,   117,    54,   418,   419,   420,
     421,   422,   423,   424,   425,   820,    77,   428,    79,    80,
      81,    82,   708,   123,   829,   711,    41,   771,   123,   110,
     123,   836,   123,   123,   249,   121,   230,   385,   639,   674,
     126,   727,   732,   547,   740,   796,   732,   369,   667,   110,
     736,   737,   737,   772,   740,   116,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,   121,
     814,   757,   758,   119,   811,   761,   660,   160,   755,   395,
      40,   725,   493,   362,   724,   771,   815,    -1,    -1,    -1,
      -1,    -1,   503,    -1,   505,    -1,   110,   508,   784,    -1,
      -1,    -1,   781,   782,   783,    -1,    -1,    -1,    -1,    -1,
     796,   522,   523,   524,   525,    -1,    -1,    -1,   529,   530,
      -1,    -1,    -1,    -1,   810,    -1,   537,    -1,   814,    -1,
      -1,    -1,   811,    -1,    -1,    -1,   547,    -1,    -1,    -1,
      -1,   820,    -1,    -1,    -1,    -1,    -1,   826,   827,   828,
     829,    -1,    -1,    -1,    15,    -1,    -1,   836,   569,    76,
      77,    78,    79,    80,    81,    82,   252,    -1,    29,    -1,
      -1,    -1,    -1,    34,    35,    36,    56,    57,    58,    59,
      60,    61,    62,    -1,    -1,   271,    -1,    -1,    68,    69,
      -1,    -1,    -1,   110,    -1,    -1,    -1,   283,    -1,   569,
     611,    -1,    -1,    -1,    -1,    -1,   617,    -1,   619,    -1,
      -1,    -1,    -1,    -1,    -1,   626,   627,    -1,    -1,    -1,
      -1,    -1,    83,    -1,    -1,    -1,    -1,   313,    -1,   640,
      91,    92,    93,    94,    95,    96,    97,    98,    99,    -1,
      -1,   652,   653,   654,   655,   656,   657,   658,    -1,    -1,
      -1,    -1,   113,   114,   115,    -1,   626,   627,    -1,   345,
      -1,    -1,    -1,   124,    -1,    -1,   677,   353,   354,   680,
     681,    -1,    -1,   684,    76,    77,    78,    79,    80,    81,
      82,    -1,   652,   653,   654,   655,   656,   657,   658,    -1,
      -1,    -1,    -1,   345,    -1,    -1,    -1,   708,    -1,    -1,
     711,   353,   354,    -1,    -1,    -1,    -1,   393,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   727,    -1,   404,    -1,
      -1,   732,   408,    -1,    -1,   736,   737,    -1,    -1,   740,
      -1,    -1,   418,   419,   420,   421,   422,   423,   424,   425,
      -1,   711,   428,    -1,    -1,    -1,   757,   758,    -1,    -1,
     761,    -1,   404,    -1,    -1,    -1,   408,    -1,    -1,    -1,
     771,    29,    -1,    -1,    -1,    -1,    34,    35,    36,    -1,
      -1,    -1,    -1,   784,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   796,    -1,   757,   758,    -1,
      -1,   761,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   810,
      -1,    -1,    -1,   814,    -1,    -1,    -1,   493,    76,    -1,
      -1,    -1,    -1,    -1,   784,    83,    -1,   503,    -1,   505,
      -1,    -1,   508,    91,    92,    93,    94,    -1,    29,    97,
      98,    99,    -1,    34,    35,    36,   522,   523,   524,   525,
     810,    42,    -1,   529,   530,   113,   114,   115,    -1,    -1,
      -1,   537,    -1,    -1,    -1,    -1,   124,    -1,    -1,    -1,
      -1,   547,    -1,    -1,    -1,    -1,    67,    -1,    -1,    -1,
     522,   523,   524,   525,    -1,    76,    -1,   529,   530,    -1,
      -1,    -1,    -1,   569,    85,    86,    87,    88,    89,    90,
      -1,    -1,    -1,    94,    -1,    -1,    -1,    98,    -1,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,    -1,
     111,   112,   113,   114,   115,    -1,    -1,   569,    -1,    -1,
      -1,    -1,    -1,   124,    -1,   611,    -1,    -1,    -1,    -1,
      -1,   617,    -1,   619,    -1,    -1,    -1,    -1,    -1,    -1,
     626,   627,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   640,    -1,    -1,    -1,    -1,   611,
      -1,    -1,    -1,    -1,    -1,    -1,   652,   653,   654,   655,
     656,   657,   658,    -1,   626,   627,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   677,    -1,    -1,   680,   681,    -1,    -1,   684,    -1,
     652,   653,   654,   655,   656,   657,   658,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   708,    -1,    -1,   711,    -1,    -1,   680,   681,
      -1,    -1,   684,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   727,   144,   145,    -1,    -1,   732,    -1,    -1,    -1,
      -1,   153,    -1,    -1,   740,    -1,   708,    -1,    -1,   711,
     162,    -1,   164,    -1,    -1,   167,   168,    -1,   170,    -1,
     172,   757,   758,   175,   176,   761,    -1,    -1,   180,   181,
      -1,    -1,    -1,    -1,    -1,   771,    -1,    -1,   740,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   784,    -1,
      -1,    -1,    -1,    -1,    -1,   757,   758,     3,    -1,   761,
     796,    -1,    -1,    -1,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   810,    -1,    -1,    -1,   814,    -1,
      -1,    -1,   784,    29,    -1,    -1,    -1,    -1,    34,    35,
      36,    -1,    -1,    -1,   796,    -1,    42,    43,    44,    45,
      46,    47,    48,    -1,    -1,    -1,    52,    -1,   810,    55,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,    -1,    -1,
      -1,    67,    34,    35,    36,    -1,    -1,    -1,    -1,    -1,
      76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      86,    87,    88,    89,    90,    -1,    -1,    -1,    94,    -1,
      -1,    -1,    98,    -1,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,    76,   111,   112,   113,   114,   115,
      -1,    -1,    -1,    -1,    -1,    -1,   122,    -1,   124,    91,
      92,    93,    94,    -1,    -1,    97,    98,    99,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   107,    -1,    -1,    -1,    -1,
      -1,   113,   114,   115,    -1,   357,    -1,    -1,    -1,    -1,
     362,    -1,   124,    -1,    -1,     3,    -1,    -1,   370,   371,
     372,   373,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   386,   387,    -1,    -1,   390,   391,
      -1,    29,   394,    -1,    -1,    -1,    34,    35,    36,    -1,
      -1,    -1,    -1,    -1,    42,    43,    44,    45,    46,    47,
      48,    -1,    -1,    -1,    52,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,    -1,
     442,    -1,    -1,   445,    -1,    -1,    -1,    85,    86,    87,
      88,    89,    90,    -1,    -1,    -1,    94,    -1,    -1,    -1,
      98,    -1,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,    -1,   111,   112,   113,   114,   115,    -1,    -1,
       3,    -1,    -1,    -1,   122,   487,   124,    10,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   500,    -1,
     502,    -1,    -1,    -1,   506,    -1,    29,    -1,    -1,    -1,
      -1,    34,    35,    36,    -1,    -1,    -1,    -1,    -1,    42,
      43,    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    85,    86,    87,    88,    89,    90,    -1,    -1,
      -1,    94,    -1,    -1,    -1,    98,    -1,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,    -1,   111,   112,
     113,   114,   115,    29,    -1,    -1,    -1,    -1,    34,    35,
      36,   124,    -1,    -1,    -1,    -1,    42,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   620,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,    -1,    -1,
      -1,    67,    34,    35,    36,    -1,    -1,   639,    -1,    -1,
      76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      86,    87,    88,    89,    90,    -1,    -1,    -1,    94,    -1,
      -1,    -1,    98,    -1,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,    76,   111,   112,   113,   114,   115,
      29,    -1,    -1,    -1,    -1,    34,    35,    36,   124,    91,
      92,    93,    94,    42,    -1,    97,    98,    99,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   107,    -1,    -1,    -1,    -1,
      -1,   113,   114,   115,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   124,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    85,    86,    87,    88,
      89,    90,    -1,    -1,    -1,    94,    -1,    -1,    -1,    98,
      -1,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,    -1,   111,   112,   113,   114,   115,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   124
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   126,   127,     0,    13,    25,    36,   114,   115,   128,
     364,   364,     5,     6,     7,    29,    30,    34,    39,    90,
     364,   364,   364,     9,    12,   359,   361,   115,   115,   131,
     115,   136,   115,   134,   115,   252,   124,   364,    11,   360,
     361,   129,   130,     3,    10,    29,    34,    35,    36,    42,
      43,    44,    45,    46,    47,    48,    52,    55,    67,    76,
      85,    86,    87,    88,    89,    90,    94,    98,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   111,   112,
     113,   114,   115,   122,   124,   165,   176,   215,   253,   255,
     261,   284,   285,   288,   292,   303,   315,   323,   329,   371,
     376,   378,   138,    17,    19,    20,    23,   148,   150,   132,
      17,    18,    19,    20,    23,    31,    42,   139,   141,    63,
      27,   363,   362,   360,   170,    76,    83,    91,    92,    93,
      94,    97,    99,   113,   124,   165,   176,   180,   181,   183,
     185,   195,   215,   386,   290,   291,   117,   166,   117,   167,
     117,   120,   358,   289,   124,   124,   124,   124,   124,   306,
     314,   124,   374,   124,   340,   124,   124,   375,   122,   124,
     348,   124,   124,   124,   124,   349,   350,   115,   122,   122,
     373,   372,   121,   276,   377,    27,   254,    63,    41,    63,
      65,    66,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,   110,    56,    57,    58,    59,
      60,    61,    62,    68,    69,   324,    50,   116,   379,    64,
      28,    30,   115,   241,   243,    61,    61,    61,    61,   137,
     149,    17,    28,    30,    31,    32,    33,   100,   115,   156,
     158,    61,    61,    61,    61,    61,    61,    61,   135,   140,
     252,    15,    83,    95,    96,    97,   115,   124,   180,   216,
     225,   386,     8,    69,   365,   368,    27,   171,   114,   181,
     183,   182,   387,   116,    49,    50,    63,    64,    77,    79,
      80,    81,    82,   186,    27,    63,   376,   376,   115,   115,
     115,   114,   122,   376,   272,   268,   262,   266,   256,   315,
     329,   323,   344,   376,   351,   376,   341,   333,   376,   121,
     179,   376,   123,   177,   376,   337,   376,   330,   335,   376,
     376,   347,   343,   376,   376,   253,   253,   253,   286,   287,
     305,   304,   312,   311,   310,   309,   308,   307,   317,   316,
     319,   321,   320,   318,   322,   313,   299,   298,   297,   296,
     295,   293,   301,   300,   294,   325,   329,   353,   118,   120,
     381,   384,   380,    61,    61,    61,    26,    38,   239,   242,
     154,   153,   151,   152,    27,   148,    61,    61,    61,    61,
      61,    61,    61,    61,   133,   157,   147,   146,    22,   115,
     145,   143,    22,   144,   142,    26,    27,   155,   139,   183,
     124,   124,   124,    61,   228,    64,   123,   124,   366,    21,
      24,    31,    51,   172,   185,   118,   120,   388,   187,   196,
     184,   188,   191,   190,   189,   192,   183,   193,   197,   119,
     346,   315,   315,   277,   292,   253,   292,   303,   303,   303,
     303,   121,    69,   183,   303,    63,   303,   303,   253,   253,
     121,   123,   288,   288,   303,   303,   315,   315,   315,   315,
     315,   315,   315,   315,   315,   315,   315,   315,   315,    76,
     107,   124,   165,   180,   198,   391,   303,   303,   303,   303,
     303,   292,   315,   225,   225,   326,   376,   385,   382,   376,
     378,   124,   124,   245,   240,   241,   376,   376,   376,   376,
     160,   124,   164,   162,   114,   163,   161,    16,   159,    26,
      27,   237,   156,   376,   376,   376,   376,   181,   216,   223,
     376,   359,   221,   219,   217,   226,   225,   229,   231,   227,
     369,   225,    61,    61,    61,    61,   123,   390,   389,   181,
     183,   183,   183,   183,   183,   183,   183,   194,   183,   253,
     110,   110,    41,   123,   123,    63,    63,   342,   334,   376,
      63,    63,   376,    63,    63,   121,   121,   114,   115,   199,
     392,    50,    64,    77,    79,    80,    81,    82,   116,   110,
     327,   329,   376,   115,   303,   396,   397,   399,   115,   246,
     244,   183,   253,   376,   115,   168,   376,   223,   183,   376,
     183,   238,    49,    50,    64,    40,   225,   225,   225,   225,
      63,   230,   123,    63,   225,   225,   367,   173,    36,   175,
     174,   183,   115,   399,   193,   121,   273,   269,   263,   122,
     257,   345,   352,    63,   123,   356,    63,   123,   354,    63,
     178,   338,   123,   331,   336,   115,   198,   207,   212,   118,
     120,   393,   200,   202,   205,   204,   203,   206,   201,   302,
     328,    61,   117,   119,    64,    41,   110,    63,   123,   115,
     248,   250,    38,   110,    63,   123,   253,   224,    63,    63,
     218,   232,   225,   234,   233,   123,   368,   185,   223,   376,
     119,   198,   198,   277,   267,   261,   288,   303,   303,   357,
     355,   179,   183,   303,   303,   303,    61,    63,   208,   123,
      63,   395,   394,   198,   198,   198,   198,   198,   198,   198,
     292,   327,   400,   115,   383,   398,   401,   247,   246,   110,
     123,    63,   169,   168,    37,   223,   222,   220,   234,   225,
     235,   225,   370,   110,   110,    41,    53,   278,   258,   123,
     123,   303,   303,   123,    63,   332,   123,   209,   213,   198,
     210,   214,   198,   115,   402,   303,   381,   115,   397,   399,
     183,   251,   249,   223,   165,   215,   236,   236,   123,   234,
     368,   274,   270,   264,   281,    51,    53,   121,    54,   259,
     123,   123,   339,   354,   198,   198,   211,   198,    61,    41,
     119,   183,   248,   123,   123,   292,   292,   277,   198,   110,
     279,   260,   303,   210,   403,   404,   123,   123,   123,   110,
     283,   198,   255,   123,   183,   402,   275,   271,   265,   282,
     253,   110,   261,   261,   261,   253,   280,   253
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
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
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
#line 148 "poet_yacc.y"
    {(yyval).ptr=insert_parameter((yyvsp[(3) - (3)]).ptr);}
    break;

  case 6:
#line 149 "poet_yacc.y"
    {finish_parameter((yyvsp[(4) - (6)])); }
    break;

  case 7:
#line 150 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE; }
    break;

  case 8:
#line 151 "poet_yacc.y"
    {eval_define(make_macroVar((yyvsp[(3) - (6)]).ptr),(yyvsp[(5) - (6)])); }
    break;

  case 9:
#line 152 "poet_yacc.y"
    {(yyval).config=GLOBAL_VAR; }
    break;

  case 10:
#line 152 "poet_yacc.y"
    { insert_eval((yyvsp[(4) - (5)]).ptr); }
    break;

  case 11:
#line 153 "poet_yacc.y"
    { insert_trace((yyvsp[(3) - (4)])); }
    break;

  case 12:
#line 154 "poet_yacc.y"
    {(yyval).ptr=insert_code((yyvsp[(3) - (3)]).ptr);(yyval).config=0; }
    break;

  case 13:
#line 154 "poet_yacc.y"
    {(yyval).ptr = (yyvsp[(4) - (5)]).ptr; }
    break;

  case 15:
#line 155 "poet_yacc.y"
    {(yyval).ptr=insert_input(); }
    break;

  case 16:
#line 155 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(3) - (4)]).ptr; }
    break;

  case 18:
#line 156 "poet_yacc.y"
    {(yyval).ptr=insert_output(); }
    break;

  case 19:
#line 156 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(3) - (4)]).ptr; }
    break;

  case 21:
#line 157 "poet_yacc.y"
    { (yyval).ptr=insert_xform((yyvsp[(3) - (3)]).ptr); }
    break;

  case 22:
#line 158 "poet_yacc.y"
    { set_xform_def((yyvsp[(4) - (6)]).ptr,(yyvsp[(6) - (6)]).ptr); }
    break;

  case 23:
#line 160 "poet_yacc.y"
    { if ((yyvsp[(2) - (3)]).ptr==0) insert_source((yyvsp[(1) - (3)]).ptr); else insert_source(make_inputlist2((yyvsp[(1) - (3)]).ptr,(yyvsp[(2) - (3)]).ptr)); }
    break;

  case 24:
#line 162 "poet_yacc.y"
    {(yyval).ptr = (yyvsp[(0) - (1)]).ptr; }
    break;

  case 27:
#line 164 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 28:
#line 164 "poet_yacc.y"
    { set_input_debug((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 29:
#line 165 "poet_yacc.y"
    { set_input_target((yyvsp[(0) - (3)]).ptr, (yyvsp[(0) - (3)]).ptr); }
    break;

  case 30:
#line 166 "poet_yacc.y"
    { set_input_target((yyvsp[(0) - (3)]).ptr,make_varRef((yyvsp[(3) - (3)]).ptr,GLOBAL_VAR)); }
    break;

  case 31:
#line 167 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 32:
#line 167 "poet_yacc.y"
    {set_input_syntax((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr);}
    break;

  case 33:
#line 168 "poet_yacc.y"
    { set_input_codeType((yyvsp[(0) - (3)]).ptr, (yyvsp[(0) - (3)]).ptr); }
    break;

  case 34:
#line 169 "poet_yacc.y"
    {(yyval).config=CODE_OR_XFORM_VAR; }
    break;

  case 35:
#line 170 "poet_yacc.y"
    { set_input_codeType((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 36:
#line 171 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 37:
#line 171 "poet_yacc.y"
    {set_input_files((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 38:
#line 172 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 39:
#line 172 "poet_yacc.y"
    { set_input_annot((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 40:
#line 173 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 41:
#line 173 "poet_yacc.y"
    { set_input_cond((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 42:
#line 175 "poet_yacc.y"
    {(yyval).ptr = (yyvsp[(0) - (1)]).ptr; }
    break;

  case 45:
#line 177 "poet_yacc.y"
    {(yyval).config=GLOBAL_VAR;}
    break;

  case 46:
#line 178 "poet_yacc.y"
    { set_output_target((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 47:
#line 179 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 48:
#line 180 "poet_yacc.y"
    { set_output_syntax((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 49:
#line 181 "poet_yacc.y"
    {(yyval).config=GLOBAL_VAR;}
    break;

  case 50:
#line 182 "poet_yacc.y"
    { set_output_file((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 51:
#line 183 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 52:
#line 184 "poet_yacc.y"
    { set_output_cond((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 53:
#line 185 "poet_yacc.y"
    { set_input_inline((yyvsp[(0) - (3)]).ptr,(yyvsp[(2) - (3)]).ptr);  }
    break;

  case 56:
#line 188 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(0) - (1)]).ptr; (yyval).config=(yyvsp[(1) - (1)]).config;}
    break;

  case 58:
#line 189 "poet_yacc.y"
    { set_code_params((yyvsp[(0) - (5)]).ptr,(yyvsp[(4) - (5)]).ptr); (yyval).config=(yyvsp[(0) - (5)]).config; }
    break;

  case 59:
#line 190 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 60:
#line 190 "poet_yacc.y"
    { set_code_attr((yyvsp[(0) - (4)]).ptr, (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 61:
#line 191 "poet_yacc.y"
    { set_code_inherit((yyvsp[(0) - (3)]).ptr, (yyvsp[(1) - (3)]).ptr); }
    break;

  case 62:
#line 192 "poet_yacc.y"
    {(yyval).config=ID_DEFAULT;}
    break;

  case 63:
#line 193 "poet_yacc.y"
    { set_local_static(make_sourceString("cond",4), (yyvsp[(4) - (4)]).ptr,LVAR_ATTR,(yyvsp[(4) - (4)]).ptr,1); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 64:
#line 194 "poet_yacc.y"
    {(yyval).config=ID_DEFAULT; }
    break;

  case 65:
#line 195 "poet_yacc.y"
    { set_local_static(make_sourceString("rebuild",7), (yyvsp[(4) - (4)]).ptr,LVAR_ATTR,(yyvsp[(4) - (4)]).ptr,1); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 66:
#line 196 "poet_yacc.y"
    {(yyval).config=ID_DEFAULT; }
    break;

  case 67:
#line 197 "poet_yacc.y"
    { set_code_parse((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 68:
#line 198 "poet_yacc.y"
    { set_code_lookahead((yyvsp[(0) - (3)]).ptr, (yyvsp[(3) - (3)]).ptr); (yyval).config=(yyvsp[(0) - (3)]).config; }
    break;

  case 69:
#line 199 "poet_yacc.y"
    {(yyval).config=ID_DEFAULT; }
    break;

  case 70:
#line 200 "poet_yacc.y"
    { set_local_static(make_sourceString("match",5), (yyvsp[(4) - (4)]).ptr,LVAR_ATTR,(yyvsp[(4) - (4)]).ptr,1); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 71:
#line 201 "poet_yacc.y"
    {(yyval).config=ID_DEFAULT; }
    break;

  case 72:
#line 202 "poet_yacc.y"
    { set_local_static(make_sourceString("output",6), (yyvsp[(4) - (4)]).ptr,LVAR_ATTR,(yyvsp[(4) - (4)]).ptr,1); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 73:
#line 204 "poet_yacc.y"
    { (yyval).ptr= make_varRef((yyvsp[(1) - (1)]).ptr,(yyvsp[(0) - (1)]).config); (yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 74:
#line 205 "poet_yacc.y"
    { (yyval).ptr=make_varRef((yyvsp[(3) - (3)]).ptr,GLOBAL_SCOPE); (yyval).config=(yyvsp[(0) - (3)]).config; }
    break;

  case 75:
#line 206 "poet_yacc.y"
    {(yyval)=(yyvsp[(2) - (2)]); }
    break;

  case 76:
#line 207 "poet_yacc.y"
    {(yyval)=(yyvsp[(2) - (2)]); }
    break;

  case 77:
#line 209 "poet_yacc.y"
    { (yyval).ptr = make_dummyOperator(POET_OP_XFORM); (yyval).config=(yyvsp[(0) - (0)]).config; }
    break;

  case 78:
#line 210 "poet_yacc.y"
    { (yyval).ptr=make_varRef((yyvsp[(2) - (2)]).ptr,XFORM_VAR); (yyval).config=(yyvsp[(0) - (2)]).config; }
    break;

  case 79:
#line 212 "poet_yacc.y"
    { (yyval).ptr = make_dummyOperator(POET_OP_CODE); }
    break;

  case 80:
#line 213 "poet_yacc.y"
    { (yyval).ptr=make_varRef((yyvsp[(2) - (2)]).ptr,CODE_VAR); }
    break;

  case 81:
#line 216 "poet_yacc.y"
    { (yyval).ptr=make_localPar((yyvsp[(1) - (1)]).ptr,0,LVAR_CODEPAR); }
    break;

  case 82:
#line 217 "poet_yacc.y"
    {(yyval).config=CODE_OR_XFORM_VAR; }
    break;

  case 83:
#line 217 "poet_yacc.y"
    { (yyval).ptr = make_localPar((yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr,LVAR_CODEPAR); }
    break;

  case 84:
#line 218 "poet_yacc.y"
    {(yyval).ptr = make_sourceVector((yyvsp[(1) - (3)]).ptr, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 86:
#line 220 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(0) - (1)]).ptr; }
    break;

  case 88:
#line 221 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 89:
#line 222 "poet_yacc.y"
    {set_param_type((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 90:
#line 223 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 91:
#line 223 "poet_yacc.y"
    { set_param_default((yyvsp[(0) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 92:
#line 224 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE; }
    break;

  case 93:
#line 224 "poet_yacc.y"
    { set_param_parse((yyvsp[(0) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 94:
#line 225 "poet_yacc.y"
    { set_param_message((yyvsp[(0) - (3)]).ptr, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 95:
#line 228 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config; }
    break;

  case 96:
#line 229 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config; }
    break;

  case 97:
#line 230 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MAP, (yyvsp[(4) - (8)]).ptr, (yyvsp[(7) - (8)]).ptr); }
    break;

  case 98:
#line 231 "poet_yacc.y"
    { (yyval).ptr=make_sourceUop(POET_OP_MAP,make_empty()); }
    break;

  case 99:
#line 232 "poet_yacc.y"
    { (yyval).ptr=make_sourceUop(POET_OP_MAP,make_empty()); }
    break;

  case 100:
#line 234 "poet_yacc.y"
    {(yyval).ptr=make_sourceUop(POET_OP_MAP, (yyvsp[(3) - (4)]).ptr); }
    break;

  case 101:
#line 237 "poet_yacc.y"
    { (yyval).ptr = make_inputlist(make_inputlist((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr),0); }
    break;

  case 102:
#line 239 "poet_yacc.y"
    { (yyval).ptr = make_inputlist2(make_inputlist((yyvsp[(1) - (5)]).ptr,(yyvsp[(3) - (5)]).ptr),(yyvsp[(5) - (5)])); }
    break;

  case 103:
#line 241 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 104:
#line 242 "poet_yacc.y"
    { (yyval).ptr = make_any(); }
    break;

  case 105:
#line 243 "poet_yacc.y"
    { (yyval).ptr = make_type(TYPE_INT); }
    break;

  case 106:
#line 244 "poet_yacc.y"
    { (yyval).ptr = make_type(TYPE_STRING); }
    break;

  case 107:
#line 245 "poet_yacc.y"
    { (yyval).ptr = make_type(TYPE_ID); }
    break;

  case 108:
#line 246 "poet_yacc.y"
    { (yyval).ptr = make_dummyOperator(POET_OP_EXP); }
    break;

  case 109:
#line 247 "poet_yacc.y"
    {  (yyval).ptr = make_dummyOperator(POET_OP_VAR); }
    break;

  case 110:
#line 248 "poet_yacc.y"
    { (yyval).ptr = make_dummyOperator(POET_OP_TUPLE); }
    break;

  case 111:
#line 249 "poet_yacc.y"
    { (yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 112:
#line 251 "poet_yacc.y"
    {(yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 113:
#line 252 "poet_yacc.y"
    { (yyval).ptr = negate_Iconst((yyvsp[(2) - (2)]).ptr); }
    break;

  case 114:
#line 253 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 115:
#line 254 "poet_yacc.y"
    { (yyval).ptr = make_typeNot((yyvsp[(2) - (2)])); }
    break;

  case 116:
#line 255 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 117:
#line 255 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(3) - (4)]).ptr; }
    break;

  case 118:
#line 257 "poet_yacc.y"
    {(yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 119:
#line 258 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 120:
#line 259 "poet_yacc.y"
    { (yyval).ptr = make_typeTor((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 121:
#line 261 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 122:
#line 261 "poet_yacc.y"
    { (yyval).ptr = ((yyvsp[(3) - (3)]).ptr==0)? (yyvsp[(1) - (3)]).ptr : make_typelist2((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 123:
#line 262 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 124:
#line 263 "poet_yacc.y"
    {(yyval).ptr = make_typeUop(TYPE_LIST1, (yyvsp[(1) - (2)]).ptr); }
    break;

  case 125:
#line 264 "poet_yacc.y"
    {(yyval).ptr = make_typeUop(TYPE_LIST, (yyvsp[(1) - (2)]).ptr); }
    break;

  case 126:
#line 265 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 127:
#line 266 "poet_yacc.y"
    {(yyval).ptr = make_sourceBop(POET_OP_RANGE, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr);}
    break;

  case 128:
#line 267 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 129:
#line 268 "poet_yacc.y"
    { (yyval).ptr=make_sourceBop(POET_OP_PLUS, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 130:
#line 269 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 131:
#line 270 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MULT,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 132:
#line 271 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 133:
#line 272 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_DIVIDE,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 134:
#line 273 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 135:
#line 273 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MOD,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 136:
#line 274 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 137:
#line 274 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_CONS,(yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 138:
#line 276 "poet_yacc.y"
    {(yyval).ptr=0; }
    break;

  case 139:
#line 277 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 140:
#line 277 "poet_yacc.y"
    { (yyval).ptr = make_typelist2((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 141:
#line 278 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 142:
#line 279 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector2((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 143:
#line 280 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 144:
#line 280 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 145:
#line 282 "poet_yacc.y"
    {(yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 146:
#line 283 "poet_yacc.y"
    { (yyval).ptr = negate_Iconst((yyvsp[(2) - (2)]).ptr); }
    break;

  case 147:
#line 284 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_CLEAR, make_varRef((yyvsp[(2) - (2)]).ptr,ASSIGN_VAR)); }
    break;

  case 148:
#line 285 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 149:
#line 286 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 150:
#line 286 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(3) - (4)]).ptr; }
    break;

  case 151:
#line 287 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 152:
#line 288 "poet_yacc.y"
    { (yyval).ptr = make_typeTor((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 153:
#line 289 "poet_yacc.y"
    {(yyval).ptr = make_typeUop(TYPE_LIST, (yyvsp[(1) - (2)]).ptr); }
    break;

  case 154:
#line 290 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 155:
#line 291 "poet_yacc.y"
    {(yyval).ptr = make_sourceBop(POET_OP_RANGE, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr);}
    break;

  case 156:
#line 292 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 157:
#line 293 "poet_yacc.y"
    { (yyval).ptr=make_sourceBop(POET_OP_PLUS, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 158:
#line 294 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 159:
#line 295 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MULT,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 160:
#line 296 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 161:
#line 297 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_DIVIDE,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 162:
#line 298 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 163:
#line 299 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MOD,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 164:
#line 300 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 165:
#line 300 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_CONS,(yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 166:
#line 302 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 167:
#line 302 "poet_yacc.y"
    { (yyval).ptr = ((yyvsp[(3) - (3)]).ptr==0)? (yyvsp[(1) - (3)]).ptr : make_typelist2((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 168:
#line 303 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 169:
#line 304 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 170:
#line 304 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign(make_varRef((yyvsp[(1) - (4)]).ptr,ASSIGN_VAR), (yyvsp[(4) - (4)]).ptr); }
    break;

  case 171:
#line 306 "poet_yacc.y"
    {(yyval).ptr=0; }
    break;

  case 172:
#line 307 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 173:
#line 307 "poet_yacc.y"
    { (yyval).ptr = make_typelist2((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 174:
#line 308 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 175:
#line 309 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector2((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 176:
#line 310 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 177:
#line 310 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 178:
#line 312 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 179:
#line 313 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 180:
#line 315 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config; }
    break;

  case 181:
#line 315 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (4)]).config;}
    break;

  case 182:
#line 315 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_TUPLE, make_typelist2((yyvsp[(4) - (7)]).ptr,(yyvsp[(6) - (7)]).ptr)); }
    break;

  case 183:
#line 316 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config; }
    break;

  case 184:
#line 316 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 185:
#line 316 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_LIST,(yyvsp[(4) - (8)]).ptr,(yyvsp[(7) - (8)]).ptr); }
    break;

  case 186:
#line 317 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config; }
    break;

  case 187:
#line 317 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 188:
#line 317 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_LIST1,(yyvsp[(4) - (8)]).ptr,(yyvsp[(7) - (8)]).ptr); }
    break;

  case 189:
#line 318 "poet_yacc.y"
    { (yyval).ptr = make_empty(); }
    break;

  case 190:
#line 320 "poet_yacc.y"
    {(yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 191:
#line 321 "poet_yacc.y"
    { (yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 192:
#line 322 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 193:
#line 323 "poet_yacc.y"
    { (yyval).ptr = make_typeTor((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 194:
#line 324 "poet_yacc.y"
    {(yyval).ptr = make_typeUop(TYPE_LIST1, (yyvsp[(1) - (2)]).ptr); }
    break;

  case 195:
#line 325 "poet_yacc.y"
    {(yyval).ptr = make_typeUop(TYPE_LIST, (yyvsp[(1) - (2)]).ptr); }
    break;

  case 196:
#line 327 "poet_yacc.y"
    { (yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 197:
#line 328 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 198:
#line 329 "poet_yacc.y"
    { (yyval).ptr = make_typeNot((yyvsp[(2) - (2)])); }
    break;

  case 199:
#line 330 "poet_yacc.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 200:
#line 331 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 201:
#line 331 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign(make_varRef((yyvsp[(1) - (4)]).ptr,ASSIGN_VAR), (yyvsp[(4) - (4)]).ptr); }
    break;

  case 202:
#line 332 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 203:
#line 333 "poet_yacc.y"
    { (yyval).ptr = make_typeTor((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 204:
#line 334 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 205:
#line 334 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(3) - (4)]).ptr; }
    break;

  case 206:
#line 336 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 207:
#line 336 "poet_yacc.y"
    { (yyval).ptr = ((yyvsp[(3) - (3)]).ptr==0)? (yyvsp[(1) - (3)]).ptr : make_typelist2((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 208:
#line 337 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 209:
#line 338 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 210:
#line 339 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector2((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 211:
#line 340 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 212:
#line 340 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 213:
#line 341 "poet_yacc.y"
    {(yyval).ptr=0; }
    break;

  case 214:
#line 342 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 215:
#line 342 "poet_yacc.y"
    { (yyval).ptr = make_typelist2((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 216:
#line 344 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 217:
#line 345 "poet_yacc.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 218:
#line 347 "poet_yacc.y"
    {yaccState=YACC_CODE; (yyval).config=ID_DEFAULT;}
    break;

  case 219:
#line 347 "poet_yacc.y"
    { yaccState=YACC_DEFAULT; set_code_def((yyvsp[(0) - (4)]).ptr,(yyvsp[(3) - (4)]).ptr); }
    break;

  case 220:
#line 348 "poet_yacc.y"
    { set_code_def((yyvsp[(0) - (1)]).ptr, 0); }
    break;

  case 221:
#line 349 "poet_yacc.y"
    {(yyval).config=ID_DEFAULT; }
    break;

  case 222:
#line 349 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(3) - (4)]).ptr; }
    break;

  case 223:
#line 350 "poet_yacc.y"
    {(yyval).ptr = 0; }
    break;

  case 225:
#line 351 "poet_yacc.y"
    {(yyval)=(yyvsp[(0) - (1)]); }
    break;

  case 227:
#line 352 "poet_yacc.y"
    {set_xform_params((yyvsp[(0) - (5)]).ptr,(yyvsp[(4) - (5)]).ptr); }
    break;

  case 228:
#line 353 "poet_yacc.y"
    {(yyval).ptr=0;}
    break;

  case 230:
#line 354 "poet_yacc.y"
    {(yyval).config=CODE_VAR;}
    break;

  case 231:
#line 354 "poet_yacc.y"
    { (yyval).ptr = set_local_static((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr,LVAR_TUNE,0,1); }
    break;

  case 232:
#line 355 "poet_yacc.y"
    { (yyval).ptr=make_localPar((yyvsp[(1) - (1)]).ptr,0,LVAR_XFORMPAR); }
    break;

  case 233:
#line 356 "poet_yacc.y"
    {(yyval).config=CODE_VAR;}
    break;

  case 234:
#line 356 "poet_yacc.y"
    { (yyval).ptr = make_localPar((yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr,LVAR_XFORMPAR); }
    break;

  case 235:
#line 357 "poet_yacc.y"
    {(yyval).ptr = make_sourceVector((yyvsp[(1) - (3)]).ptr, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 236:
#line 358 "poet_yacc.y"
    {(yyval).ptr = (yyvsp[(0) - (2)]).ptr + 1; }
    break;

  case 239:
#line 359 "poet_yacc.y"
    { set_local_static((yyvsp[(1) - (1)]).ptr,make_Iconst1((yyvsp[(0) - (1)]).ptr), LVAR_OUTPUT,0,1); }
    break;

  case 240:
#line 360 "poet_yacc.y"
    {(yyval).config=CODE_VAR;}
    break;

  case 241:
#line 360 "poet_yacc.y"
    { set_local_static((yyvsp[(1) - (4)]).ptr,make_Iconst1((yyvsp[(0) - (4)]).ptr), LVAR_OUTPUT,(yyvsp[(4) - (4)]).ptr,1); }
    break;

  case 242:
#line 361 "poet_yacc.y"
    { (yyval).ptr = make_traceVar((yyvsp[(1) - (1)]).ptr,0); }
    break;

  case 243:
#line 363 "poet_yacc.y"
    { (yyval).ptr = make_traceVar((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr);  }
    break;

  case 244:
#line 365 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 245:
#line 365 "poet_yacc.y"
    { (yyval).ptr=make_sourceBop(POET_OP_SEQ,(yyvsp[(1) - (3)]).ptr, (yyvsp[(3) - (3)]).ptr);}
    break;

  case 246:
#line 366 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 247:
#line 367 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 248:
#line 368 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 249:
#line 368 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 250:
#line 368 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (7)]).config;}
    break;

  case 251:
#line 369 "poet_yacc.y"
    { (yyval).ptr = make_ifElse((yyvsp[(4) - (9)]).ptr, (yyvsp[(7) - (9)]).ptr, (yyvsp[(9) - (9)]).ptr); }
    break;

  case 252:
#line 370 "poet_yacc.y"
    { (yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 253:
#line 371 "poet_yacc.y"
    { (yyval).ptr=make_empty(); }
    break;

  case 254:
#line 372 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 255:
#line 372 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(3) - (3)]).ptr; }
    break;

  case 256:
#line 373 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 257:
#line 374 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 258:
#line 375 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (8)]).config;}
    break;

  case 259:
#line 375 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (11)]).config;}
    break;

  case 260:
#line 376 "poet_yacc.y"
    { (yyval).ptr = make_sourceQop(POET_OP_FOR, (yyvsp[(4) - (13)]).ptr,(yyvsp[(7) - (13)]).ptr,(yyvsp[(10) - (13)]).ptr,(yyvsp[(13) - (13)]).ptr); }
    break;

  case 261:
#line 377 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 262:
#line 377 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (6)]).config;}
    break;

  case 263:
#line 378 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_CASE, (yyvsp[(4) - (9)]).ptr,(yyvsp[(8) - (9)]).ptr); }
    break;

  case 264:
#line 379 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 265:
#line 380 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 266:
#line 381 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (8)]).config;}
    break;

  case 267:
#line 381 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (11)]).config;}
    break;

  case 268:
#line 382 "poet_yacc.y"
    { (yyval).ptr = make_sourceQop(POET_OP_FOREACH, (yyvsp[(4) - (13)]).ptr,(yyvsp[(7) - (13)]).ptr,(yyvsp[(10) - (13)]).ptr,(yyvsp[(13) - (13)]).ptr); }
    break;

  case 269:
#line 383 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 270:
#line 384 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 271:
#line 385 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (8)]).config;}
    break;

  case 272:
#line 385 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (11)]).config;}
    break;

  case 273:
#line 386 "poet_yacc.y"
    { (yyval).ptr = make_sourceQop(POET_OP_FOREACHR, (yyvsp[(4) - (13)]).ptr,(yyvsp[(7) - (13)]).ptr,(yyvsp[(10) - (13)]).ptr,(yyvsp[(13) - (13)]).ptr); }
    break;

  case 274:
#line 387 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 275:
#line 387 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(3) - (4)]).ptr; }
    break;

  case 276:
#line 388 "poet_yacc.y"
    { (yyval).ptr=make_empty(); }
    break;

  case 277:
#line 389 "poet_yacc.y"
    { (yyval).ptr = make_sourceStmt((yyvsp[(1) - (2)]).ptr); }
    break;

  case 278:
#line 391 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 279:
#line 392 "poet_yacc.y"
    { (yyval).ptr = make_empty(); }
    break;

  case 280:
#line 394 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 281:
#line 394 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 282:
#line 395 "poet_yacc.y"
    {(yyval).ptr = make_sourceVector( (yyvsp[(1) - (7)]).ptr, make_inputlist2((yyvsp[(4) - (7)]).ptr,(yyvsp[(7) - (7)]).ptr)); }
    break;

  case 283:
#line 396 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 284:
#line 396 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (4)]).config;}
    break;

  case 285:
#line 396 "poet_yacc.y"
    { (yyval).ptr = make_inputlist2((yyvsp[(3) - (6)]).ptr,(yyvsp[(6) - (6)]).ptr); }
    break;

  case 286:
#line 397 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (3)]).config;}
    break;

  case 287:
#line 398 "poet_yacc.y"
    {(yyval).ptr = make_sourceVector( (yyvsp[(1) - (5)]).ptr, make_inputlist2(make_any(),(yyvsp[(5) - (5)]).ptr)); }
    break;

  case 288:
#line 400 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 289:
#line 401 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 290:
#line 403 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 291:
#line 403 "poet_yacc.y"
    {(yyval).ptr = make_sourceVector((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 292:
#line 404 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 293:
#line 404 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector2((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 294:
#line 407 "poet_yacc.y"
    { (yyval).ptr=make_dummyOperator(POET_OP_CONTINUE); }
    break;

  case 295:
#line 408 "poet_yacc.y"
    { (yyval).ptr=make_dummyOperator(POET_OP_BREAK); }
    break;

  case 296:
#line 409 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 297:
#line 409 "poet_yacc.y"
    { (yyval).ptr=make_sourceUop(POET_OP_RETURN, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 298:
#line 410 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 299:
#line 410 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_ERROR,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 300:
#line 411 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 301:
#line 411 "poet_yacc.y"
    { (yyval).ptr=make_sourceUop(POET_OP_PRINT,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 302:
#line 412 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 303:
#line 414 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 304:
#line 415 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 305:
#line 415 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign((yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 306:
#line 416 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 307:
#line 416 "poet_yacc.y"
    { (yyval).ptr = make_codeMatch((yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 308:
#line 417 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 309:
#line 418 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign((yyvsp[(1) - (4)]).ptr, make_sourceBop(POET_OP_PLUS, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr)); }
    break;

  case 310:
#line 419 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 311:
#line 420 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign((yyvsp[(1) - (4)]).ptr, make_sourceBop(POET_OP_MINUS, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr)); }
    break;

  case 312:
#line 421 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 313:
#line 422 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign((yyvsp[(1) - (4)]).ptr, make_sourceBop(POET_OP_MULT, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr)); }
    break;

  case 314:
#line 423 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 315:
#line 424 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign((yyvsp[(1) - (4)]).ptr, make_sourceBop(POET_OP_DIVIDE, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr)); }
    break;

  case 316:
#line 425 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 317:
#line 426 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign((yyvsp[(1) - (4)]).ptr, make_sourceBop(POET_OP_MOD, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr)); }
    break;

  case 318:
#line 427 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 319:
#line 427 "poet_yacc.y"
    { (yyval).ptr = make_codeMatchQ((yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 320:
#line 428 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 321:
#line 428 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 322:
#line 429 "poet_yacc.y"
    { (yyval).ptr = make_ifElse( (yyvsp[(1) - (7)]).ptr, (yyvsp[(4) - (7)]).ptr, (yyvsp[(7) - (7)]).ptr); }
    break;

  case 323:
#line 432 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 324:
#line 432 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_AND,  (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 325:
#line 433 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 326:
#line 433 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_OR,   (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 327:
#line 434 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 328:
#line 434 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_NOT, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 329:
#line 435 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 330:
#line 435 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_LT, (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 331:
#line 436 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 332:
#line 436 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_LE, (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 333:
#line 437 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 334:
#line 437 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_EQ, (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 335:
#line 438 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 336:
#line 439 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_GT,   (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 337:
#line 440 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 338:
#line 441 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_GE,   (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 339:
#line 442 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 340:
#line 443 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_NE,   (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 341:
#line 444 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 342:
#line 444 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_ASTMATCH, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 343:
#line 445 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 344:
#line 445 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_UMINUS,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 345:
#line 446 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 346:
#line 448 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 347:
#line 449 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 348:
#line 450 "poet_yacc.y"
    { (yyval).ptr=make_sourceBop(POET_OP_PLUS, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 349:
#line 451 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 350:
#line 452 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MINUS,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 351:
#line 453 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 352:
#line 454 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MULT,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 353:
#line 455 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 354:
#line 456 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_CONCAT, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 355:
#line 457 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 356:
#line 458 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_DIVIDE,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 357:
#line 459 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 358:
#line 460 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MOD,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 359:
#line 461 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 360:
#line 461 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_CONS,(yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 361:
#line 463 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 362:
#line 464 "poet_yacc.y"
    { if ((yyvsp[(3) - (3)]).ptr==0) (yyval).ptr=(yyvsp[(1) - (3)]).ptr; else (yyval).ptr = make_xformList((yyvsp[(1) - (3)]).ptr, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 363:
#line 466 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 364:
#line 466 "poet_yacc.y"
    { (yyval).ptr =make_xformList((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr);}
    break;

  case 365:
#line 467 "poet_yacc.y"
    { (yyval).ptr=0; }
    break;

  case 366:
#line 468 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 367:
#line 468 "poet_yacc.y"
    { (yyval).ptr =make_xformList((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr);}
    break;

  case 368:
#line 469 "poet_yacc.y"
    { (yyval).ptr=0; }
    break;

  case 369:
#line 471 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 370:
#line 471 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 371:
#line 471 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (7)]).config;}
    break;

  case 372:
#line 472 "poet_yacc.y"
    { (yyval).ptr=((yyvsp[(9) - (9)]).ptr==0)? make_sourceBop(POET_OP_REPLACE,(yyvsp[(4) - (9)]).ptr,(yyvsp[(7) - (9)]).ptr) 
                       : make_sourceTop(POET_OP_REPLACE,(yyvsp[(4) - (9)]).ptr,(yyvsp[(7) - (9)]).ptr,(yyvsp[(9) - (9)]).ptr); }
    break;

  case 373:
#line 474 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 374:
#line 474 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (4)]).config;}
    break;

  case 375:
#line 475 "poet_yacc.y"
    { (yyval).ptr=((yyvsp[(6) - (6)]).ptr==0)? make_sourceUop(POET_OP_TRACE,(yyvsp[(4) - (6)]).ptr)
                      : make_sourceBop(POET_OP_TRACE,(yyvsp[(4) - (6)]).ptr,(yyvsp[(6) - (6)]).ptr); }
    break;

  case 376:
#line 477 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 377:
#line 477 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 378:
#line 478 "poet_yacc.y"
    { (yyval).ptr=make_sourceBop(POET_OP_PERMUTE,(yyvsp[(4) - (8)]).ptr,(yyvsp[(7) - (8)]).ptr); }
    break;

  case 379:
#line 479 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 380:
#line 479 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 381:
#line 479 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (8)]).config;}
    break;

  case 382:
#line 480 "poet_yacc.y"
    { (yyval).ptr=make_sourceTop(POET_OP_DUPLICATE,(yyvsp[(4) - (11)]).ptr,(yyvsp[(7) - (11)]).ptr,(yyvsp[(10) - (11)]).ptr); }
    break;

  case 383:
#line 481 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 384:
#line 481 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_COPY, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 385:
#line 482 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 386:
#line 483 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (4)]).config;(yyval).ptr=(yyvsp[(4) - (4)]).ptr; }
    break;

  case 387:
#line 484 "poet_yacc.y"
    {(yyval).ptr = make_sourceBop(POET_OP_ERASE,(yyvsp[(4) - (6)]).ptr,(yyvsp[(6) - (6)]).ptr); }
    break;

  case 388:
#line 485 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 389:
#line 485 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_DELAY, (yyvsp[(4) - (5)]).ptr); }
    break;

  case 390:
#line 486 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 391:
#line 486 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 392:
#line 487 "poet_yacc.y"
    {  (yyval).ptr=make_sourceBop(POET_OP_INSERT, (yyvsp[(4) - (8)]).ptr,(yyvsp[(7) - (8)]).ptr); }
    break;

  case 393:
#line 488 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (3)]).config;}
    break;

  case 394:
#line 489 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_DEBUG,(yyvsp[(2) - (6)]).ptr,(yyvsp[(5) - (6)]).ptr); }
    break;

  case 395:
#line 490 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 396:
#line 490 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_APPLY, (yyvsp[(4) - (5)]).ptr); }
    break;

  case 397:
#line 491 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 398:
#line 491 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_REBUILD,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 399:
#line 492 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 400:
#line 492 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_RESTORE,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 401:
#line 493 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 402:
#line 493 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_SAVE,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 403:
#line 494 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 404:
#line 494 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 405:
#line 495 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_SPLIT, (yyvsp[(4) - (8)]).ptr,(yyvsp[(7) - (8)]).ptr); }
    break;

  case 406:
#line 496 "poet_yacc.y"
    {(yyval).ptr = make_typeUop(TYPE_LIST, (yyvsp[(1) - (2)]).ptr); }
    break;

  case 407:
#line 497 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 408:
#line 498 "poet_yacc.y"
    {(yyval).ptr = make_sourceBop(POET_OP_RANGE, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr);}
    break;

  case 409:
#line 500 "poet_yacc.y"
    {(yyval).ptr = make_sourceBop(POET_OP_RANGE, (yyvsp[(3) - (6)]).ptr,(yyvsp[(6) - (6)]).ptr);}
    break;

  case 410:
#line 501 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 411:
#line 502 "poet_yacc.y"
    { (yyval).ptr = 0; }
    break;

  case 412:
#line 502 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 413:
#line 502 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(3) - (4)]).ptr; }
    break;

  case 414:
#line 504 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 415:
#line 504 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(3) - (4)]).ptr; }
    break;

  case 416:
#line 505 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(0) - (1)]).ptr; }
    break;

  case 417:
#line 507 "poet_yacc.y"
    {(yyval).ptr = make_Iconst1(1); }
    break;

  case 418:
#line 508 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(2) - (3)]).ptr; }
    break;

  case 419:
#line 510 "poet_yacc.y"
    { (yyval).ptr = make_inputlist2((yyvsp[(1) - (2)]).ptr,(yyvsp[(2) - (2)]).ptr); }
    break;

  case 420:
#line 512 "poet_yacc.y"
    { (yyval).ptr = 0; }
    break;

  case 421:
#line 513 "poet_yacc.y"
    { (yyval).ptr = make_inputlist2((yyvsp[(1) - (2)]).ptr,(yyvsp[(2) - (2)]).ptr); }
    break;

  case 422:
#line 516 "poet_yacc.y"
    { (yyval).ptr = ((yyvsp[(2) - (2)]).ptr==0)? (yyvsp[(1) - (2)]).ptr : make_inputlist2((yyvsp[(1) - (2)]).ptr,(yyvsp[(2) - (2)]).ptr); }
    break;

  case 423:
#line 516 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(4) - (4)]).ptr; }
    break;

  case 424:
#line 517 "poet_yacc.y"
    {(yyval).config=CODE_OR_XFORM_VAR; }
    break;

  case 425:
#line 518 "poet_yacc.y"
    { (yyval).ptr = make_annot((yyvsp[(4) - (5)]).ptr); }
    break;

  case 426:
#line 520 "poet_yacc.y"
    { (yyval).ptr = 0; }
    break;

  case 427:
#line 521 "poet_yacc.y"
    { (yyval).ptr = make_inputlist2((yyvsp[(1) - (2)]).ptr, (yyvsp[(2) - (2)]).ptr); }
    break;

  case 428:
#line 522 "poet_yacc.y"
    { (yyval).ptr = make_inputlist2((yyvsp[(1) - (2)]).ptr, (yyvsp[(2) - (2)]).ptr); }
    break;

  case 429:
#line 523 "poet_yacc.y"
    { (yyval).ptr = make_inputlist2((yyvsp[(1) - (2)]).ptr, (yyvsp[(2) - (2)]).ptr); }
    break;

  case 430:
#line 524 "poet_yacc.y"
    { (yyval).ptr = make_inputlist2(make_varRef((yyvsp[(1) - (2)]).ptr,GLOBAL_VAR), (yyvsp[(2) - (2)]).ptr); }
    break;

  case 431:
#line 527 "poet_yacc.y"
    {(yyval).config=CODE_OR_XFORM_VAR;}
    break;

  case 432:
#line 527 "poet_yacc.y"
    { (yyval).ptr = make_codeMatch((yyvsp[(0) - (3)]).ptr, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 433:
#line 527 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(5) - (5)]).ptr; }
    break;

  case 434:
#line 528 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 435:
#line 530 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(0) - (0)]).ptr; }
    break;

  case 436:
#line 531 "poet_yacc.y"
    {(yyval).config=CODE_OR_XFORM_VAR; }
    break;

  case 437:
#line 531 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(0) - (5)]).ptr; }
    break;

  case 438:
#line 532 "poet_yacc.y"
    { (yyval).ptr = make_inputlist2(make_annot((yyvsp[(4) - (7)]).ptr), (yyvsp[(7) - (7)]).ptr); }
    break;

  case 439:
#line 535 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 440:
#line 535 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_CAR, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 441:
#line 536 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 442:
#line 536 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_CDR, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 443:
#line 537 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 444:
#line 537 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_LEN, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 445:
#line 538 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 446:
#line 538 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(TYPE_INT, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 447:
#line 539 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_CLEAR, make_varRef((yyvsp[(2) - (2)]).ptr,ASSIGN_VAR)); }
    break;

  case 448:
#line 540 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 449:
#line 542 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 450:
#line 542 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(3) - (4)]).ptr; }
    break;

  case 451:
#line 543 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 452:
#line 544 "poet_yacc.y"
    { (yyval).ptr = make_any(); }
    break;

  case 453:
#line 545 "poet_yacc.y"
    { (yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 454:
#line 546 "poet_yacc.y"
    {(yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 455:
#line 548 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(1) - (1)]).ptr; (yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 456:
#line 548 "poet_yacc.y"
    { (yyval) = (yyvsp[(3) - (3)]); }
    break;

  case 457:
#line 549 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config; }
    break;

  case 458:
#line 549 "poet_yacc.y"
    { (yyval).ptr = make_typeTor((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 459:
#line 550 "poet_yacc.y"
    {(yyval)=(yyvsp[(0) - (0)]); }
    break;

  case 460:
#line 551 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; (yyval).ptr=(yyvsp[(0) - (1)]).ptr; }
    break;

  case 461:
#line 552 "poet_yacc.y"
    { (yyval).ptr = make_tupleAccess((yyvsp[(0) - (4)]).ptr, (yyvsp[(3) - (4)]).ptr); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 462:
#line 553 "poet_yacc.y"
    { (yyval)=(yyvsp[(6) - (6)]); }
    break;

  case 463:
#line 554 "poet_yacc.y"
    { (yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 464:
#line 556 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 465:
#line 557 "poet_yacc.y"
    { (yyval).ptr = make_codeRef((yyvsp[(0) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr,(yyvsp[(0) - (3)]).config); (yyval).config=(yyvsp[(0) - (3)]).config; }
    break;

  case 466:
#line 559 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(1) - (1)]).ptr; (yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 467:
#line 559 "poet_yacc.y"
    { (yyval) = (yyvsp[(3) - (3)]); }
    break;

  case 468:
#line 560 "poet_yacc.y"
    {(yyval)=(yyvsp[(0) - (0)]); }
    break;

  case 469:
#line 561 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; (yyval).ptr=(yyvsp[(0) - (1)]).ptr; }
    break;

  case 470:
#line 562 "poet_yacc.y"
    { (yyval).ptr = make_tupleAccess((yyvsp[(0) - (4)]).ptr, (yyvsp[(3) - (4)]).ptr); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 471:
#line 563 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 472:
#line 564 "poet_yacc.y"
    { (yyval).ptr = make_codeRef((yyvsp[(0) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr,(yyvsp[(0) - (3)]).config);(yyval).config=(yyvsp[(0) - (3)]).config; }
    break;

  case 473:
#line 566 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(1) - (1)]).ptr; (yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 474:
#line 566 "poet_yacc.y"
    { (yyval) = (yyvsp[(3) - (3)]); }
    break;

  case 475:
#line 567 "poet_yacc.y"
    {(yyval)=(yyvsp[(0) - (0)]); }
    break;

  case 476:
#line 568 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; (yyval).ptr=(yyvsp[(0) - (1)]).ptr; }
    break;

  case 477:
#line 569 "poet_yacc.y"
    { (yyval).ptr = make_tupleAccess((yyvsp[(0) - (4)]).ptr, (yyvsp[(3) - (4)]).ptr); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 478:
#line 570 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 479:
#line 571 "poet_yacc.y"
    { (yyval).ptr = make_codeRef((yyvsp[(0) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr,(yyvsp[(0) - (3)]).config);(yyval).config=(yyvsp[(0) - (3)]).config; }
    break;

  case 480:
#line 573 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(1) - (1)]).ptr;}
    break;

  case 481:
#line 574 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 482:
#line 575 "poet_yacc.y"
    {(yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 483:
#line 577 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 484:
#line 578 "poet_yacc.y"
    { (yyval).ptr = make_typeTor((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 485:
#line 579 "poet_yacc.y"
    { (yyval).ptr = make_attrAccess(make_varRef((yyvsp[(1) - (3)]).ptr,CODE_OR_XFORM_VAR), (yyvsp[(3) - (3)]).ptr); }
    break;

  case 486:
#line 582 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 487:
#line 583 "poet_yacc.y"
    { (yyval).ptr = make_sourceAssign( make_attrAccess((yyvsp[(0) - (4)]).ptr,(yyvsp[(1) - (4)]).ptr),(yyvsp[(4) - (4)]).ptr); }
    break;

  case 488:
#line 584 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(0) - (2)]).ptr; (yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 489:
#line 585 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_SEQ,(yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr);}
    break;

  case 490:
#line 588 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 491:
#line 589 "poet_yacc.y"
    { (yyval).ptr = make_sourceAssign( make_attrAccess((yyvsp[(0) - (4)]).ptr,(yyvsp[(1) - (4)]).ptr),(yyvsp[(4) - (4)]).ptr); }
    break;

  case 492:
#line 590 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(0) - (2)]).ptr; (yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 493:
#line 591 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_SEQ,(yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr);}
    break;


/* Line 1267 of yacc.c.  */
#line 5110 "poet_yacc.c"
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


#line 592 "poet_yacc.y"


