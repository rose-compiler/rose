/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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
#define YYBISON_VERSION "2.4.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
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
extern void* make_annot();
extern void* make_codeMatchQ();
extern void* make_empty();
extern void* make_any();
extern void* make_seq(void*, void*);
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


/* Line 189 of yacc.c  */
#line 190 "poet_yacc.c"

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


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 478 "poet_yacc.c"

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
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
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
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2066

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  124
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  281
/* YYNRULES -- Number of rules.  */
#define YYNRULES  497
/* YYNRULES -- Number of states.  */
#define YYNSTATES  844

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   378

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
     115,   116,   117,   118,   119,   120,   121,   122,   123
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    10,    17,    18,    25,
      26,    32,    33,    39,    44,    45,    46,    54,    55,    56,
      63,    64,    65,    72,    73,    80,    84,    85,    89,    90,
      91,    96,   100,   104,   105,   110,   114,   115,   120,   121,
     126,   127,   132,   133,   138,   139,   143,   144,   145,   150,
     151,   156,   157,   162,   163,   168,   172,   174,   175,   176,
     180,   186,   187,   192,   196,   197,   202,   203,   208,   209,
     214,   218,   219,   224,   225,   230,   232,   236,   239,   242,
     243,   246,   247,   250,   252,   253,   258,   262,   263,   264,
     268,   269,   274,   275,   280,   281,   286,   290,   291,   292,
     301,   305,   309,   311,   316,   320,   326,   328,   330,   332,
     334,   336,   338,   340,   342,   344,   346,   349,   351,   354,
     355,   360,   362,   363,   368,   369,   373,   375,   378,   381,
     382,   387,   388,   393,   394,   399,   400,   405,   406,   411,
     412,   417,   418,   419,   423,   424,   429,   430,   435,   437,
     440,   443,   445,   446,   451,   452,   457,   460,   461,   466,
     467,   472,   473,   478,   479,   484,   485,   490,   491,   496,
     497,   501,   503,   504,   509,   510,   511,   515,   516,   521,
     522,   527,   529,   531,   533,   534,   535,   543,   544,   545,
     554,   555,   556,   565,   567,   569,   570,   575,   578,   581,
     583,   585,   588,   590,   591,   596,   597,   602,   603,   608,
     609,   613,   615,   616,   621,   622,   627,   628,   629,   633,
     635,   637,   638,   643,   645,   646,   651,   653,   654,   655,
     659,   665,   666,   673,   674,   679,   681,   682,   687,   691,
     692,   697,   699,   701,   702,   707,   709,   713,   717,   718,
     722,   724,   726,   727,   728,   729,   739,   741,   742,   743,
     747,   748,   749,   750,   751,   765,   766,   767,   777,   778,
     779,   780,   781,   795,   796,   797,   798,   799,   813,   814,
     819,   822,   825,   827,   828,   829,   830,   838,   839,   840,
     847,   848,   854,   856,   858,   859,   864,   865,   870,   872,
     874,   875,   879,   880,   884,   885,   889,   891,   893,   894,
     899,   900,   905,   906,   911,   912,   917,   918,   923,   924,
     929,   930,   935,   936,   941,   942,   943,   951,   952,   957,
     958,   963,   964,   968,   969,   974,   975,   980,   981,   986,
     987,   992,   993,   998,   999,  1004,  1005,  1010,  1012,  1014,
    1015,  1020,  1021,  1026,  1027,  1032,  1033,  1038,  1039,  1044,
    1045,  1050,  1051,  1056,  1057,  1061,  1062,  1066,  1067,  1071,
    1072,  1073,  1077,  1078,  1079,  1080,  1081,  1091,  1092,  1093,
    1100,  1101,  1102,  1111,  1112,  1113,  1114,  1126,  1127,  1131,
    1132,  1133,  1140,  1141,  1147,  1148,  1149,  1158,  1159,  1166,
    1167,  1173,  1174,  1178,  1179,  1183,  1184,  1188,  1189,  1190,
    1199,  1202,  1203,  1208,  1215,  1217,  1219,  1220,  1225,  1226,
    1231,  1233,  1234,  1238,  1241,  1242,  1245,  1246,  1251,  1252,
    1258,  1259,  1262,  1265,  1268,  1271,  1272,  1273,  1279,  1281,
    1282,  1283,  1284,  1292,  1293,  1297,  1298,  1302,  1303,  1307,
    1308,  1312,  1315,  1317,  1318,  1323,  1325,  1327,  1329,  1331,
    1332,  1336,  1337,  1342,  1343,  1344,  1345,  1352,  1354,  1355,
    1359,  1360,  1364,  1365,  1366,  1371,  1372,  1376,  1377,  1381,
    1382,  1383,  1388,  1389,  1393,  1395,  1397,  1399,  1400,  1405,
    1409,  1410,  1415,  1416,  1421,  1422,  1427,  1428
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     125,     0,    -1,   126,    -1,    -1,   126,   127,    -1,    -1,
      23,     5,   114,   128,   170,    25,    -1,    -1,    23,     6,
     114,   129,   223,    25,    -1,    -1,    23,     7,   130,   253,
      25,    -1,    -1,    23,    15,   131,   253,    25,    -1,    23,
      88,   252,    25,    -1,    -1,    -1,    23,    32,   114,   132,
     156,   133,   237,    -1,    -1,    -1,    23,    37,   134,   139,
     135,   155,    -1,    -1,    -1,    23,    28,   136,   148,   137,
      25,    -1,    -1,    23,    27,   114,   138,   241,   239,    -1,
     364,   359,    11,    -1,    -1,   141,   140,   139,    -1,    -1,
      -1,    40,    59,   142,   376,    -1,    17,    59,    20,    -1,
      17,    59,   114,    -1,    -1,    21,    59,   143,   376,    -1,
      29,    59,    20,    -1,    -1,    29,    59,   144,   223,    -1,
      -1,    18,    59,   145,   376,    -1,    -1,    16,    59,   146,
     376,    -1,    -1,    15,    59,   147,   376,    -1,    -1,   150,
     149,   148,    -1,    -1,    -1,    18,    59,   151,   376,    -1,
      -1,    21,    59,   152,   376,    -1,    -1,    17,    59,   153,
     376,    -1,    -1,    15,    59,   154,   376,    -1,    24,   359,
      38,    -1,    25,    -1,    -1,    -1,   158,   157,   156,    -1,
      26,    59,   123,   168,   122,    -1,    -1,   114,    59,   159,
     376,    -1,   114,    59,    14,    -1,    -1,    15,    59,   160,
     376,    -1,    -1,    98,    59,   161,   376,    -1,    -1,    29,
      59,   162,   223,    -1,    30,    59,   113,    -1,    -1,    31,
      59,   163,   183,    -1,    -1,    28,    59,   164,   376,    -1,
     114,    -1,    33,   116,   114,    -1,    27,   166,    -1,    32,
     167,    -1,    -1,   116,   114,    -1,    -1,   116,   114,    -1,
     114,    -1,    -1,   114,   108,   169,   223,    -1,   168,    61,
     168,    -1,    -1,    -1,   170,   171,   172,    -1,    -1,    19,
      59,   173,   185,    -1,    -1,    49,    59,   174,   376,    -1,
      -1,    29,    59,   175,   223,    -1,    22,    59,    34,    -1,
      -1,    -1,    96,   123,   177,   183,    61,   178,   183,   122,
      -1,    96,   123,   122,    -1,    96,   121,   120,    -1,    96,
      -1,    96,   121,   179,   120,    -1,   376,    67,   376,    -1,
     376,    67,   376,    61,   179,    -1,   215,    -1,   112,    -1,
      92,    -1,    91,    -1,    90,    -1,    89,    -1,    97,    -1,
      95,    -1,   176,    -1,   180,    -1,    74,   113,    -1,   386,
      -1,    81,   183,    -1,    -1,   123,   182,   185,   122,    -1,
     181,    -1,    -1,   183,    62,   184,   183,    -1,    -1,   183,
     186,   193,    -1,   195,    -1,   183,    47,    -1,   183,    48,
      -1,    -1,   181,   115,   187,   181,    -1,    -1,   183,    75,
     188,   183,    -1,    -1,   183,    79,   189,   183,    -1,    -1,
     183,    78,   190,   183,    -1,    -1,   183,    77,   191,   183,
      -1,    -1,   183,    80,   192,   183,    -1,    -1,    -1,   183,
     194,   193,    -1,    -1,   183,    61,   196,   183,    -1,    -1,
     195,    61,   197,   183,    -1,   180,    -1,    74,   113,    -1,
     105,   114,    -1,   391,    -1,    -1,   123,   199,   207,   122,
      -1,    -1,   198,    62,   200,   198,    -1,   198,    48,    -1,
      -1,   198,   115,   201,   198,    -1,    -1,   198,    75,   202,
     198,    -1,    -1,   198,    79,   203,   198,    -1,    -1,   198,
      78,   204,   198,    -1,    -1,   198,    77,   205,   198,    -1,
      -1,   198,    80,   206,   198,    -1,    -1,   198,   208,   210,
      -1,   212,    -1,    -1,   114,    59,   209,   198,    -1,    -1,
      -1,   198,   211,   210,    -1,    -1,   198,    61,   213,   198,
      -1,    -1,   212,    61,   214,   198,    -1,   113,    -1,    34,
      -1,   111,    -1,    -1,    -1,    95,   123,   217,   225,   218,
     234,   122,    -1,    -1,    -1,    94,   123,   219,   225,    61,
     220,   236,   122,    -1,    -1,    -1,    93,   123,   221,   225,
      61,   222,   236,   122,    -1,   216,    -1,   181,    -1,    -1,
     223,    62,   224,   223,    -1,   223,    47,    -1,   223,    48,
      -1,   180,    -1,   386,    -1,    81,   183,    -1,   216,    -1,
      -1,   114,    59,   226,   225,    -1,    -1,   225,    62,   227,
     225,    -1,    -1,   123,   228,   229,   122,    -1,    -1,   225,
     230,   234,    -1,   231,    -1,    -1,   225,    61,   232,   225,
      -1,    -1,   231,    61,   233,   225,    -1,    -1,    -1,   225,
     235,   234,    -1,   215,    -1,   165,    -1,    -1,    24,   238,
     253,    35,    -1,    25,    -1,    -1,    24,   240,   253,    36,
      -1,    36,    -1,    -1,    -1,   243,   242,   241,    -1,    26,
      59,   123,   246,   122,    -1,    -1,    28,    59,   123,   244,
     248,   122,    -1,    -1,   114,    59,   245,   183,    -1,   114,
      -1,    -1,   114,   108,   247,   183,    -1,   246,    61,   246,
      -1,    -1,   250,    61,   249,   248,    -1,   250,    -1,   114,
      -1,    -1,   114,   108,   251,   183,    -1,   114,    -1,   114,
      59,   376,    -1,   114,    61,   252,    -1,    -1,   255,   254,
     253,    -1,   255,    -1,   284,    -1,    -1,    -1,    -1,    53,
     123,   256,   292,   122,   257,   261,   258,   259,    -1,   261,
      -1,    -1,    -1,    52,   260,   255,    -1,    -1,    -1,    -1,
      -1,    46,   123,   262,   277,    39,   263,   277,    39,   264,
     277,   122,   265,   261,    -1,    -1,    -1,    50,   123,   266,
     253,   122,   121,   267,   278,   120,    -1,    -1,    -1,    -1,
      -1,    45,   123,   268,   314,   108,   269,   198,   108,   270,
     292,   122,   271,   261,    -1,    -1,    -1,    -1,    -1,    44,
     123,   272,   314,   108,   273,   198,   108,   274,   292,   122,
     275,   261,    -1,    -1,   121,   276,   253,   120,    -1,   121,
     120,    -1,   288,    39,    -1,   292,    -1,    -1,    -1,    -1,
     278,    51,   279,   198,   108,   280,   253,    -1,    -1,    -1,
      51,   281,   198,   108,   282,   253,    -1,    -1,   278,    49,
     108,   283,   253,    -1,   285,    -1,   288,    -1,    -1,   285,
      61,   286,   288,    -1,    -1,   288,    61,   287,   288,    -1,
      42,    -1,    43,    -1,    -1,    41,   289,   376,    -1,    -1,
       3,   290,   376,    -1,    -1,    10,   291,   376,    -1,   292,
      -1,   303,    -1,    -1,   329,    59,   293,   292,    -1,    -1,
     329,    67,   294,   225,    -1,    -1,   329,    58,   295,   303,
      -1,    -1,   329,    57,   296,   303,    -1,    -1,   329,    56,
     297,   303,    -1,    -1,   329,    55,   298,   303,    -1,    -1,
     329,    54,   299,   303,    -1,    -1,   329,    66,   300,   225,
      -1,    -1,    -1,   329,    60,   301,   314,   108,   302,   292,
      -1,    -1,   303,    64,   304,   303,    -1,    -1,   303,    63,
     305,   303,    -1,    -1,    65,   306,   314,    -1,    -1,   314,
      73,   307,   314,    -1,    -1,   314,    72,   308,   314,    -1,
      -1,   314,    71,   309,   314,    -1,    -1,   314,    70,   310,
     314,    -1,    -1,   314,    69,   311,   314,    -1,    -1,   314,
      68,   312,   314,    -1,    -1,   314,   108,   313,   198,    -1,
     314,    -1,   323,    -1,    -1,   314,    75,   315,   314,    -1,
      -1,   314,    74,   316,   314,    -1,    -1,   314,    79,   317,
     314,    -1,    -1,   314,    76,   318,   314,    -1,    -1,   314,
      78,   319,   314,    -1,    -1,   314,    77,   320,   314,    -1,
      -1,   314,    80,   321,   314,    -1,    -1,    74,   322,   314,
      -1,    -1,   329,   324,   325,    -1,    -1,   329,   326,   327,
      -1,    -1,    -1,   329,   328,   327,    -1,    -1,    -1,    -1,
      -1,   101,   123,   330,   303,    61,   331,   303,   332,   354,
      -1,    -1,    -1,    88,   123,   333,   303,   334,   354,    -1,
      -1,    -1,   102,   123,   335,   303,    61,   336,   303,   122,
      -1,    -1,    -1,    -1,    99,   123,   337,   303,    61,   338,
     303,    61,   339,   303,   122,    -1,    -1,    86,   340,   376,
      -1,    -1,    -1,    87,   123,   341,   303,   342,   356,    -1,
      -1,   107,   121,   343,   253,   120,    -1,    -1,    -1,    83,
     123,   344,   303,    61,   345,   303,   122,    -1,    -1,    40,
     358,   121,   346,   253,   120,    -1,    -1,   106,   121,   347,
     253,   120,    -1,    -1,    98,   348,   376,    -1,    -1,   103,
     349,   376,    -1,    -1,   104,   350,   376,    -1,    -1,    -1,
      85,   123,   351,   303,    61,   352,   303,   122,    -1,   376,
      48,    -1,    -1,   376,   115,   353,   376,    -1,   100,   123,
     376,    61,   376,   122,    -1,   371,    -1,   122,    -1,    -1,
      61,   355,   303,   122,    -1,    -1,    61,   357,   303,   122,
      -1,   122,    -1,    -1,   119,   113,   118,    -1,   361,   360,
      -1,    -1,   361,   360,    -1,    -1,    12,   364,   362,   365,
      -1,    -1,     9,   123,   363,   225,   122,    -1,    -1,   113,
     364,    -1,    34,   364,    -1,    13,   364,    -1,   114,   364,
      -1,    -1,    -1,    67,   366,   225,   367,   368,    -1,   368,
      -1,    -1,    -1,    -1,     8,   123,   369,   225,   122,   370,
     368,    -1,    -1,   110,   372,   376,    -1,    -1,   109,   373,
     376,    -1,    -1,    84,   374,   376,    -1,    -1,    92,   375,
     376,    -1,   105,   114,    -1,   378,    -1,    -1,   123,   377,
     253,   122,    -1,   215,    -1,   112,    -1,   165,    -1,   176,
      -1,    -1,   376,   379,   381,    -1,    -1,   378,    62,   380,
     378,    -1,    -1,    -1,    -1,   119,   382,   396,   118,   383,
     381,    -1,   384,    -1,    -1,   117,   385,   376,    -1,    -1,
     165,   387,   388,    -1,    -1,    -1,   119,   389,   399,   118,
      -1,    -1,   117,   390,   183,    -1,    -1,   165,   392,   393,
      -1,    -1,    -1,   119,   394,   402,   118,    -1,    -1,   117,
     395,   198,    -1,   303,    -1,   399,    -1,   397,    -1,    -1,
     397,    62,   398,   397,    -1,   114,   116,   114,    -1,    -1,
     114,    59,   400,   303,    -1,    -1,   399,    39,   401,   399,
      -1,    -1,   114,    59,   403,   183,    -1,    -1,   402,    39,
     404,   402,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   145,   145,   147,   148,   150,   150,   152,   152,   154,
     154,   155,   155,   156,   157,   157,   157,   158,   158,   158,
     159,   159,   159,   160,   160,   162,   165,   165,   166,   167,
     167,   168,   169,   170,   170,   171,   172,   172,   174,   174,
     175,   175,   176,   176,   178,   178,   179,   180,   180,   182,
     182,   184,   184,   186,   186,   188,   189,   191,   191,   191,
     192,   193,   193,   194,   195,   195,   197,   197,   199,   199,
     201,   202,   202,   204,   204,   207,   208,   209,   210,   212,
     213,   215,   216,   219,   220,   220,   221,   223,   223,   223,
     224,   224,   226,   226,   227,   227,   228,   231,   232,   231,
     234,   235,   236,   237,   240,   242,   245,   246,   247,   248,
     249,   250,   251,   252,   253,   255,   256,   257,   258,   259,
     259,   261,   262,   262,   265,   265,   266,   267,   268,   269,
     269,   271,   271,   273,   273,   275,   275,   277,   277,   278,
     278,   280,   281,   281,   282,   282,   284,   284,   286,   287,
     288,   289,   290,   290,   291,   291,   293,   294,   294,   296,
     296,   298,   298,   300,   300,   302,   302,   304,   304,   306,
     306,   307,   308,   308,   310,   311,   311,   312,   312,   314,
     314,   316,   317,   318,   320,   320,   320,   321,   321,   321,
     322,   322,   322,   324,   325,   326,   326,   328,   329,   331,
     332,   333,   334,   335,   335,   336,   336,   338,   338,   340,
     340,   341,   342,   342,   344,   344,   345,   346,   346,   348,
     349,   351,   351,   352,   353,   353,   354,   355,   355,   355,
     356,   357,   357,   358,   358,   359,   360,   360,   361,   362,
     362,   362,   363,   364,   364,   365,   366,   367,   370,   370,
     371,   372,   373,   373,   373,   373,   375,   376,   377,   377,
     378,   379,   380,   380,   378,   382,   382,   382,   384,   385,
     386,   386,   384,   388,   389,   390,   390,   388,   392,   392,
     393,   394,   396,   397,   399,   399,   399,   401,   401,   401,
     402,   402,   405,   406,   408,   408,   409,   409,   412,   413,
     414,   414,   415,   415,   416,   416,   417,   419,   420,   420,
     421,   421,   422,   422,   424,   424,   426,   426,   428,   428,
     430,   430,   432,   432,   433,   433,   433,   437,   437,   438,
     438,   439,   439,   440,   440,   441,   441,   442,   442,   443,
     443,   445,   445,   447,   447,   449,   449,   450,   452,   453,
     453,   455,   455,   457,   457,   459,   459,   461,   461,   463,
     463,   465,   465,   466,   466,   468,   468,   471,   471,   472,
     473,   473,   474,   476,   476,   476,   476,   479,   479,   479,
     482,   482,   482,   484,   484,   484,   484,   486,   486,   487,
     488,   487,   490,   490,   491,   491,   491,   493,   493,   495,
     495,   496,   496,   497,   497,   498,   498,   499,   499,   499,
     501,   502,   502,   504,   506,   507,   507,   507,   509,   509,
     510,   512,   513,   515,   517,   518,   521,   521,   522,   522,
     525,   526,   527,   528,   529,   532,   532,   532,   533,   535,
     536,   536,   536,   540,   540,   541,   541,   542,   542,   543,
     543,   544,   545,   547,   547,   548,   549,   550,   551,   553,
     553,   554,   554,   555,   556,   557,   556,   559,   561,   561,
     564,   564,   565,   566,   566,   568,   568,   571,   571,   572,
     573,   573,   575,   575,   578,   579,   580,   582,   582,   584,
     587,   587,   589,   589,   593,   593,   595,   595
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ERROR", "SYNERROR", "PARAMETER",
  "DEFINE", "EVAL", "LBEGIN", "RBEGIN", "PRINT", "INPUT_ENDFILE",
  "INPUT_ENDL", "INPUT_CONT", "INHERIT", "COND", "ANNOT", "TO", "FROM",
  "TYPE", "POETTYPE", "SYNTAX", "MESSAGE", "LTAG", "RTAG", "ENDTAG",
  "PARS", "XFORM", "OUTPUT", "PARSE", "LOOKAHEAD", "MATCH", "CODE",
  "GLOBAL", "SOURCE", "ENDCODE", "ENDXFORM", "INPUT", "ENDINPUT",
  "SEMICOLON", "DEBUG", "RETURN", "CONTINUE", "BREAK", "FOREACH_R",
  "FOREACH", "FOR", "DOT4", "DOT3", "DEFAULT", "SWITCH", "CASE", "ELSE",
  "IF", "MOD_ASSIGN", "DIVIDE_ASSIGN", "MULT_ASSIGN", "MINUS_ASSIGN",
  "PLUS_ASSIGN", "ASSIGN", "QUESTION", "COMMA", "TOR", "OR", "AND", "NOT",
  "DARROW", "ARROW", "NE", "GE", "GT", "EQ", "LE", "LT", "MINUS", "PLUS",
  "STR_CONCAT", "MOD", "DIVIDE", "MULTIPLY", "DCOLON", "TILT", "UMINUS",
  "INSERT", "LEN", "SPLIT", "COPY", "ERASE", "TRACE", "EXP", "NAME",
  "STRING", "INT", "LIST1", "LIST", "TUPLE", "MAP", "VAR", "REBUILD",
  "DUPLICATE", "RANGE", "REPLACE", "PERMUTE", "RESTORE", "SAVE", "CLEAR",
  "APPLY", "DELAY", "COLON", "CDR", "CAR", "EMPTY", "ANY", "ICONST", "ID",
  "DOT2", "DOT", "POND", "RBR", "LBR", "RB", "LB", "RP", "LP", "$accept",
  "poet", "sections", "section", "@1", "@2", "@3", "@4", "@5", "@6", "@7",
  "@8", "@9", "@10", "@11", "inputAttrs", "@12", "inputAttr", "@13", "@14",
  "@15", "@16", "@17", "@18", "outputAttrs", "@19", "outputAttr", "@20",
  "@21", "@22", "@23", "inputRHS", "codeAttrs", "@24", "codeAttr", "@25",
  "@26", "@27", "@28", "@29", "@30", "varRef", "xformRef", "codeRef",
  "codePars", "@31", "paramAttrs", "@32", "paramAttr", "@33", "@34", "@35",
  "map", "@36", "@37", "mapEntries", "singletype", "typeSpec1", "@38",
  "typeSpec", "@39", "typeMulti", "@40", "@41", "@42", "@43", "@44", "@45",
  "@46", "typeList", "@47", "typeTuple", "@48", "@49", "patternSpec",
  "@50", "@51", "@52", "@53", "@54", "@55", "@56", "@57", "patternMulti",
  "@58", "@59", "patternList", "@60", "patternTuple", "@61", "@62",
  "constant", "parseType1", "@63", "@64", "@65", "@66", "@67", "@68",
  "parseType", "@69", "parseElem", "@70", "@71", "@72", "parseElemMulti",
  "@73", "parseElemTuple", "@74", "@75", "parseElemList", "@76",
  "constantOrVar", "codeRHS", "@77", "xformRHS", "@78", "xformAttrs",
  "@79", "xformAttr", "@80", "@81", "xformPars", "@82", "outputPars",
  "@83", "outputPar", "@84", "traceVars", "code", "@85", "codeIf", "@86",
  "@87", "@88", "codeIfHelp", "@89", "code1", "@90", "@91", "@92", "@93",
  "@94", "@95", "@96", "@97", "@98", "@99", "@100", "@101", "@102", "@103",
  "@104", "e_code4", "switchHelp", "@105", "@106", "@107", "@108", "@109",
  "code23", "code2", "@110", "@111", "code3", "@112", "@113", "@114",
  "code4", "@115", "@116", "@117", "@118", "@119", "@120", "@121", "@122",
  "@123", "@124", "code5", "@125", "@126", "@127", "@128", "@129", "@130",
  "@131", "@132", "@133", "@134", "code6", "@135", "@136", "@137", "@138",
  "@139", "@140", "@141", "@142", "code7", "@143", "code7Help", "@144",
  "code7Help2", "@145", "code8", "@146", "@147", "@148", "@149", "@150",
  "@151", "@152", "@153", "@154", "@155", "@156", "@157", "@158", "@159",
  "@160", "@161", "@162", "@163", "@164", "@165", "@166", "@167", "@168",
  "@169", "paramRHS", "@170", "eraseRHS", "@171", "debugConfig",
  "inputCodeList", "inputCodeList2", "inputCode", "@172", "@173",
  "inputBase", "lineAnnot", "@174", "@175", "typeMatch", "@176", "@177",
  "code9", "@178", "@179", "@180", "@181", "codeUnit", "@182", "varInvoke",
  "@183", "@184", "varInvoke1", "@185", "@186", "pond", "@187",
  "varInvokeType", "@188", "varInvokeType1", "@189", "@190",
  "varInvokePattern", "@191", "varInvokePattern1", "@192", "@193",
  "configOrSubRef", "selectRef", "@194", "configRef", "@195", "@196",
  "xformConfig", "@197", "@198", 0
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
     375,   376,   377,   378
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   124,   125,   126,   126,   128,   127,   129,   127,   130,
     127,   131,   127,   127,   132,   133,   127,   134,   135,   127,
     136,   137,   127,   138,   127,   127,   140,   139,   139,   142,
     141,   141,   141,   143,   141,   141,   144,   141,   145,   141,
     146,   141,   147,   141,   149,   148,   148,   151,   150,   152,
     150,   153,   150,   154,   150,   155,   155,   156,   157,   156,
     158,   159,   158,   158,   160,   158,   161,   158,   162,   158,
     158,   163,   158,   164,   158,   165,   165,   165,   165,   166,
     166,   167,   167,   168,   169,   168,   168,   170,   171,   170,
     173,   172,   174,   172,   175,   172,   172,   177,   178,   176,
     176,   176,   176,   176,   179,   179,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   181,   181,   181,   181,   182,
     181,   183,   184,   183,   186,   185,   185,   185,   185,   187,
     185,   188,   185,   189,   185,   190,   185,   191,   185,   192,
     185,   193,   194,   193,   196,   195,   197,   195,   198,   198,
     198,   198,   199,   198,   200,   198,   198,   201,   198,   202,
     198,   203,   198,   204,   198,   205,   198,   206,   198,   208,
     207,   207,   209,   207,   210,   211,   210,   213,   212,   214,
     212,   215,   215,   215,   217,   218,   216,   219,   220,   216,
     221,   222,   216,   223,   223,   224,   223,   223,   223,   225,
     225,   225,   225,   226,   225,   227,   225,   228,   225,   230,
     229,   229,   232,   231,   233,   231,   234,   235,   234,   236,
     236,   238,   237,   237,   240,   239,   239,   241,   242,   241,
     243,   244,   243,   245,   243,   246,   247,   246,   246,   249,
     248,   248,   250,   251,   250,   252,   252,   252,   254,   253,
     253,   253,   256,   257,   258,   255,   255,   259,   260,   259,
     262,   263,   264,   265,   261,   266,   267,   261,   268,   269,
     270,   271,   261,   272,   273,   274,   275,   261,   276,   261,
     261,   261,   277,   277,   279,   280,   278,   281,   282,   278,
     283,   278,   284,   284,   286,   285,   287,   285,   288,   288,
     289,   288,   290,   288,   291,   288,   288,   292,   293,   292,
     294,   292,   295,   292,   296,   292,   297,   292,   298,   292,
     299,   292,   300,   292,   301,   302,   292,   304,   303,   305,
     303,   306,   303,   307,   303,   308,   303,   309,   303,   310,
     303,   311,   303,   312,   303,   313,   303,   303,   314,   315,
     314,   316,   314,   317,   314,   318,   314,   319,   314,   320,
     314,   321,   314,   322,   314,   324,   323,   326,   325,   325,
     328,   327,   327,   330,   331,   332,   329,   333,   334,   329,
     335,   336,   329,   337,   338,   339,   329,   340,   329,   341,
     342,   329,   343,   329,   344,   345,   329,   346,   329,   347,
     329,   348,   329,   349,   329,   350,   329,   351,   352,   329,
     329,   353,   329,   329,   329,   354,   355,   354,   357,   356,
     356,   358,   358,   359,   360,   360,   362,   361,   363,   361,
     364,   364,   364,   364,   364,   366,   367,   365,   365,   368,
     369,   370,   368,   372,   371,   373,   371,   374,   371,   375,
     371,   371,   371,   377,   376,   376,   376,   376,   376,   379,
     378,   380,   378,   381,   382,   383,   381,   381,   385,   384,
     387,   386,   388,   389,   388,   390,   388,   392,   391,   393,
     394,   393,   395,   393,   396,   396,   396,   398,   397,   397,
     400,   399,   401,   399,   403,   402,   404,   402
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     0,     6,     0,     6,     0,
       5,     0,     5,     4,     0,     0,     7,     0,     0,     6,
       0,     0,     6,     0,     6,     3,     0,     3,     0,     0,
       4,     3,     3,     0,     4,     3,     0,     4,     0,     4,
       0,     4,     0,     4,     0,     3,     0,     0,     4,     0,
       4,     0,     4,     0,     4,     3,     1,     0,     0,     3,
       5,     0,     4,     3,     0,     4,     0,     4,     0,     4,
       3,     0,     4,     0,     4,     1,     3,     2,     2,     0,
       2,     0,     2,     1,     0,     4,     3,     0,     0,     3,
       0,     4,     0,     4,     0,     4,     3,     0,     0,     8,
       3,     3,     1,     4,     3,     5,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     2,     0,
       4,     1,     0,     4,     0,     3,     1,     2,     2,     0,
       4,     0,     4,     0,     4,     0,     4,     0,     4,     0,
       4,     0,     0,     3,     0,     4,     0,     4,     1,     2,
       2,     1,     0,     4,     0,     4,     2,     0,     4,     0,
       4,     0,     4,     0,     4,     0,     4,     0,     4,     0,
       3,     1,     0,     4,     0,     0,     3,     0,     4,     0,
       4,     1,     1,     1,     0,     0,     7,     0,     0,     8,
       0,     0,     8,     1,     1,     0,     4,     2,     2,     1,
       1,     2,     1,     0,     4,     0,     4,     0,     4,     0,
       3,     1,     0,     4,     0,     4,     0,     0,     3,     1,
       1,     0,     4,     1,     0,     4,     1,     0,     0,     3,
       5,     0,     6,     0,     4,     1,     0,     4,     3,     0,
       4,     1,     1,     0,     4,     1,     3,     3,     0,     3,
       1,     1,     0,     0,     0,     9,     1,     0,     0,     3,
       0,     0,     0,     0,    13,     0,     0,     9,     0,     0,
       0,     0,    13,     0,     0,     0,     0,    13,     0,     4,
       2,     2,     1,     0,     0,     0,     7,     0,     0,     6,
       0,     5,     1,     1,     0,     4,     0,     4,     1,     1,
       0,     3,     0,     3,     0,     3,     1,     1,     0,     4,
       0,     4,     0,     4,     0,     4,     0,     4,     0,     4,
       0,     4,     0,     4,     0,     0,     7,     0,     4,     0,
       4,     0,     3,     0,     4,     0,     4,     0,     4,     0,
       4,     0,     4,     0,     4,     0,     4,     1,     1,     0,
       4,     0,     4,     0,     4,     0,     4,     0,     4,     0,
       4,     0,     4,     0,     3,     0,     3,     0,     3,     0,
       0,     3,     0,     0,     0,     0,     9,     0,     0,     6,
       0,     0,     8,     0,     0,     0,    11,     0,     3,     0,
       0,     6,     0,     5,     0,     0,     8,     0,     6,     0,
       5,     0,     3,     0,     3,     0,     3,     0,     0,     8,
       2,     0,     4,     6,     1,     1,     0,     4,     0,     4,
       1,     0,     3,     2,     0,     2,     0,     4,     0,     5,
       0,     2,     2,     2,     2,     0,     0,     5,     1,     0,
       0,     0,     7,     0,     3,     0,     3,     0,     3,     0,
       3,     2,     1,     0,     4,     1,     1,     1,     1,     0,
       3,     0,     4,     0,     0,     0,     6,     1,     0,     3,
       0,     3,     0,     0,     4,     0,     3,     0,     3,     0,
       0,     4,     0,     3,     1,     1,     1,     0,     4,     3,
       0,     4,     0,     4,     0,     4,     0,     4
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       3,     0,   430,     1,   430,     0,   430,   430,   430,     4,
       0,   433,     0,     0,     9,    11,     0,    20,     0,    17,
       0,   432,   431,   434,     0,   430,     0,   424,     5,     7,
       0,     0,    23,    46,    14,    28,   245,     0,   428,   426,
      25,   423,   424,    87,     0,   302,   304,    79,    81,     0,
     182,   421,   300,   298,   299,     0,     0,     0,     0,     0,
     331,   363,     0,   447,     0,   387,     0,     0,   449,   102,
     401,     0,     0,     0,     0,   403,   405,     0,     0,     0,
     445,   443,   183,   456,   181,    75,   278,   453,   457,   458,
     455,     0,   248,   256,   251,   292,   293,   306,   307,   347,
     348,   365,   414,   459,   452,     0,   227,     0,     0,     0,
       0,    21,    44,    57,     0,     0,     0,     0,     0,     0,
       0,    18,    26,     0,     0,    13,     0,   439,   425,    88,
       0,     0,   111,   110,   109,   108,     0,     0,   113,   112,
     107,   119,   470,   114,   115,   194,   106,   193,     0,   117,
       0,     0,     0,    77,     0,    78,     0,     0,     0,     0,
     273,   268,   260,   265,   252,     0,     0,   394,     0,   407,
       0,   389,   377,     0,     0,    97,     0,   383,     0,   373,
     380,     0,     0,   451,   399,   392,     0,     0,   280,     0,
       0,    10,     0,   294,   281,   296,   329,   327,   343,   341,
     339,   337,   335,   333,   351,   349,   355,   359,   357,   353,
     361,   345,   320,   318,   316,   314,   312,   308,   324,   322,
     310,   369,   410,   411,   463,   461,    12,     0,     0,     0,
       0,   228,    53,    51,    47,    49,     0,    46,     0,     0,
       0,     0,     0,     0,     0,     0,    15,    58,    42,    40,
       0,    38,    33,    36,    29,     0,    28,   246,   247,     0,
      75,   207,   199,   202,     0,   200,     0,   435,   427,   438,
       6,     0,   116,   113,   121,   118,   190,   187,   184,     0,
     472,     8,   197,   198,   195,   303,   305,    80,    82,    76,
       0,   397,   301,     0,     0,   283,     0,     0,   332,   365,
     364,     0,   448,     0,   388,     0,     0,   450,   101,     0,
       0,   100,     0,   402,     0,     0,     0,     0,   404,   406,
       0,     0,   446,   444,     0,     0,   249,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   366,   367,     0,   468,   464,   460,
     467,     0,     0,     0,   233,   224,   226,    24,   227,     0,
       0,     0,     0,    22,    45,    64,     0,    73,    68,     0,
      71,    66,    61,     0,    57,     0,     0,    31,    32,     0,
       0,    35,     0,     0,     0,    56,    19,    27,   201,   203,
       0,   205,   429,   440,     0,     0,     0,     0,     0,    89,
     122,     0,     0,     0,   121,   124,     0,   126,   475,   473,
     471,     0,   422,     0,     0,     0,     0,   282,     0,     0,
       0,     0,   390,   378,   103,     0,     0,     0,     0,     0,
       0,     0,     0,   279,   454,   295,   297,   330,   328,   344,
     342,   340,   338,   336,   334,   352,   350,   356,   360,   358,
     354,   362,     0,     0,   152,   477,   148,   346,   151,   321,
     319,   317,   315,   313,   309,     0,   323,   311,   372,   412,
       0,     0,   459,   462,     0,   231,     0,     0,   229,    54,
      52,    48,    50,     0,     0,     0,     0,    70,     0,     0,
      63,     0,   221,   223,    16,    59,    43,    41,    39,    34,
      37,    30,     0,     0,   209,     0,   211,     0,     0,   436,
      90,     0,    94,    92,     0,     0,     0,   185,   129,   127,
     128,   144,   131,   137,   135,   133,   139,   141,   120,   146,
       0,     0,   196,     0,   274,   269,   261,     0,   253,   395,
     408,     0,     0,   104,    98,   384,     0,   374,   381,   400,
     393,   149,   150,     0,   479,   156,   154,   159,   165,   163,
     161,   167,   157,   325,   368,   370,   469,    75,   484,     0,
     486,   485,   235,     0,     0,   234,     0,    65,    83,     0,
      74,    69,    72,    67,    62,     0,    55,   204,   212,   216,
     208,   214,   206,     0,   439,     0,    96,     0,     0,   123,
     191,   188,   216,     0,     0,     0,     0,     0,     0,     0,
     142,   125,     0,   476,     0,     0,   398,     0,     0,   283,
     266,     0,     0,     0,   418,   420,   391,   416,   415,   379,
       0,     0,     0,   413,     0,     0,    75,   169,     0,   171,
     482,   480,   478,     0,     0,     0,     0,     0,     0,     0,
       0,   372,   490,     0,   465,   487,   492,   236,     0,   230,
     242,     0,   241,   225,    84,     0,    60,     0,     0,   217,
     210,     0,   441,   437,    91,    95,    93,     0,     0,     0,
     130,   145,   132,   138,   136,   134,   140,   141,   147,   474,
       0,     0,     0,     0,   254,     0,     0,     0,     0,     0,
     105,     0,     0,   375,     0,   172,   177,   174,   153,   179,
       0,     0,   155,   160,   166,   164,   162,   168,   158,   326,
     371,     0,   489,   463,     0,     0,     0,   238,   243,   232,
     239,     0,    86,   222,   213,   216,   215,   439,   220,   219,
       0,     0,   186,   143,   275,   270,   262,   287,     0,   257,
     396,   409,     0,     0,    99,   385,     0,   382,     0,     0,
     175,   170,     0,   483,     0,     0,   491,   466,     0,   488,
     493,   237,     0,     0,    85,   218,   442,   192,   189,     0,
       0,   283,     0,     0,   284,   267,   258,   255,   419,   417,
       0,   376,   173,   178,   174,   180,   494,   496,   481,   244,
     240,     0,     0,     0,     0,   290,     0,     0,     0,   176,
       0,     0,   276,   271,   263,   288,     0,     0,   259,   386,
     495,   497,     0,     0,     0,     0,   291,   285,   277,   272,
     264,   289,     0,   286
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     9,    43,    44,    30,    31,   113,   383,
      35,   255,    33,   236,   106,   121,   256,   122,   393,   390,
     392,   389,   386,   385,   111,   237,   112,   371,   372,   370,
     369,   396,   246,   384,   247,   501,   493,   499,   496,   498,
     495,    88,   153,   155,   589,   741,   129,   271,   409,   605,
     608,   607,    89,   312,   641,   309,   144,   274,   279,   415,
     524,   416,   537,   613,   615,   618,   617,   616,   619,   621,
     697,   417,   614,   622,   770,   563,   653,   659,   654,   657,
     656,   655,   658,   648,   717,   768,   771,   804,   649,   769,
     772,    90,   263,   413,   612,   412,   688,   411,   687,   148,
     421,   679,   513,   517,   400,   515,   599,   516,   678,   681,
     680,   745,   750,   504,   595,   367,   487,   230,   368,   231,
     584,   486,   583,   736,   671,   783,   672,   782,    37,    91,
     192,    92,   297,   631,   759,   797,   817,    93,   295,   629,
     791,   834,   296,   703,   294,   628,   790,   833,   293,   627,
     789,   832,   189,   426,   758,   816,   842,   792,   835,   826,
      94,    95,   327,   328,    96,   159,   150,   151,    97,   350,
     353,   349,   348,   347,   346,   345,   352,   351,   660,    98,
     330,   329,   165,   336,   335,   334,   333,   332,   331,   344,
      99,   338,   337,   342,   339,   341,   340,   343,   166,   100,
     221,   354,   478,   574,   661,   299,   316,   644,   766,   306,
     552,   317,   645,   314,   642,   800,   170,   305,   551,   321,
     301,   632,   423,   320,   176,   181,   182,   303,   633,   356,
     639,   709,   636,   708,   158,    26,    41,    27,   127,   126,
      10,   268,   404,   604,   269,   518,   747,   102,   187,   186,
     168,   173,   103,   190,   104,   224,   361,   359,   481,   733,
     360,   480,   149,   280,   420,   541,   540,   468,   564,   652,
     721,   720,   579,   580,   734,   581,   731,   735,   775,   820,
     821
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -630
static const yytype_int16 yypact[] =
{
    -630,    42,    12,  -630,     2,    96,     2,     2,     2,  -630,
      50,  -630,   -49,    -8,  -630,  -630,    -4,  -630,     7,  -630,
      20,  -630,  -630,  -630,   -39,     2,   120,    50,  -630,  -630,
    1437,  1437,  -630,   226,  -630,   271,   128,   150,  -630,  -630,
    -630,  -630,    50,  -630,   514,  -630,  -630,    78,    86,   119,
    -630,   126,  -630,  -630,  -630,   155,   158,   170,   173,   181,
    -630,  -630,   182,  -630,   183,  -630,   194,   195,  -630,   -89,
    -630,   206,   219,   223,   233,  -630,  -630,   125,   156,   188,
    -630,  -630,  -630,  -630,  -630,  -630,   232,  -630,  -630,  -630,
    -630,   332,    45,  -630,  -630,   297,    17,  -630,    75,   869,
    -630,   845,  -630,   -20,   301,   340,    -5,   308,   310,   312,
     313,  -630,  -630,    22,   314,   315,   318,   319,   322,   329,
     331,  -630,  -630,    85,    20,  -630,  1035,    30,  -630,   361,
     279,  1172,  -630,  -630,  -630,  -630,   276,   277,   278,  -630,
    -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,   186,  -630,
      85,    85,   280,  -630,   289,  -630,   290,   292,   287,    85,
    -630,  -630,  -630,  -630,  -630,  1823,  1823,  -630,    85,  -630,
      85,  -630,  -630,    85,   327,   288,    85,  -630,    85,  -630,
    -630,    85,    85,  -630,  -630,  -630,    85,    85,  -630,  1437,
    1437,  -630,  1437,  -630,  -630,  -630,  -630,  -630,  -630,  -630,
    -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,
    -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,
    -630,  1855,  -630,  -630,    76,  -630,  -630,   360,   363,   372,
     116,  -630,  -630,  -630,  -630,  -630,   408,   226,   375,   376,
     383,   384,   385,   386,   394,   397,  -630,  -630,  -630,  -630,
      -1,  -630,  -630,   440,  -630,   225,   271,  -630,  -630,  1172,
     403,  -630,  -630,  -630,   -15,  -630,   341,  -630,  -630,  -630,
    -630,   187,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  1172,
     193,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,
     345,  -630,  -630,  1823,  1823,  1296,  1437,  1296,   350,  -630,
     392,  1296,  -630,  1296,  -630,  1296,  1296,  -630,  -630,   353,
     407,  -630,  1172,  -630,  1296,   415,  1296,  1296,  -630,  -630,
    1437,  1437,  -630,  -630,   371,   379,  -630,  1647,  1647,  1296,
    1296,  1823,  1823,  1823,  1823,  1823,  1823,  1823,  1823,  1823,
    1823,  1823,  1823,  1823,  1690,  1296,  1296,  1296,  1296,  1296,
    1296,  1823,  1035,  1035,  -630,  -630,    85,  -630,  -630,  -630,
    -630,    85,   380,   382,  -630,  -630,  -630,  -630,    -5,    85,
      85,    85,    85,  -630,  -630,  -630,   388,  -630,  -630,   399,
    -630,  -630,   499,   229,    22,    85,    85,  -630,  -630,    85,
      85,  -630,   514,    85,    50,  -630,  -630,  -630,  -630,  -630,
    1035,  -630,  -630,  -630,  1035,   455,   457,   462,   466,  -630,
    -630,  1035,  1035,  1035,   402,   675,   404,   467,  -630,  -630,
    -630,   514,  -630,  1437,   410,   479,   488,  -630,   409,   412,
     149,   154,    75,    75,  -630,    85,   196,   221,    85,   238,
     263,   419,   422,  -630,  -630,  -630,  -630,   465,  -630,   350,
     350,   350,   350,   350,   350,   392,   392,   452,   452,   452,
     452,   452,   430,   431,  -630,  -630,  -630,   458,  -630,    75,
      75,    75,    75,    75,  -630,   567,   487,   487,  1855,  -630,
      85,  1735,  -630,  -630,   437,  -630,  1172,  1437,  -630,  -630,
    -630,  -630,  -630,    85,   446,    85,   514,  -630,  1172,    85,
    -630,    85,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,
     190,  -630,   523,  1035,   202,   441,   501,  1035,  1035,   487,
    -630,   530,  -630,  -630,  1172,   207,   212,   487,  -630,  -630,
    -630,  -630,  -630,  -630,  -630,  -630,  -630,  1172,  -630,  -630,
    1172,   460,  -630,   459,  -630,  -630,  -630,   456,  -630,  -630,
    -630,   -34,   -22,   519,  -630,  -630,   461,  -630,  -630,  -630,
    -630,  -630,  -630,  1943,   217,  -630,  -630,  -630,  -630,  -630,
    -630,  -630,  -630,  -630,  -630,  -630,  -630,    -2,    75,   464,
     524,   546,   481,   -18,   476,   536,   566,  -630,   504,     0,
    -630,   190,   536,  -630,  -630,  1437,  -630,   487,  -630,  1035,
    -630,  -630,  -630,     5,   605,  1172,  -630,   514,    85,  -630,
    -630,  -630,  1035,  1172,  1172,  1172,  1172,  1172,  1172,  1172,
     536,  -630,  1172,  -630,   556,   -19,  -630,  1690,  1690,  1296,
    -630,  1542,  1296,  1296,  -630,  -630,  -630,  -630,  -630,  -630,
      85,  1172,  1296,  -630,  1296,  1296,   559,   334,   497,   560,
    -630,  -630,  -630,  1690,  1690,  1690,  1690,  1690,  1690,  1690,
    1296,  1855,  -630,   506,  -630,  -630,  -630,  -630,   437,  -630,
     515,   500,   563,  -630,  -630,   446,  -630,   594,  1035,   487,
    -630,  1035,  -630,  -630,  -630,   190,  -630,    58,    58,   511,
    -630,   536,   536,   536,   536,   536,   536,  1172,   536,  -630,
      99,   260,   595,   584,  -630,   597,   -33,    10,  1296,  1296,
    -630,    13,   286,    75,    19,  -630,  -630,  1690,  -630,  -630,
    1690,   525,    71,    77,   -36,   -36,   -36,   -36,  -630,  -630,
    -630,  1296,  -630,    76,   535,   460,  1172,  -630,  -630,  -630,
    -630,   514,  -630,  -630,   487,  1035,   487,   605,  -630,  -630,
     516,   528,  -630,  -630,  -630,  -630,  -630,  -630,   -27,   599,
    -630,  -630,    23,    66,  -630,  -630,   -22,  -630,  1690,  1690,
     458,  -630,  1690,  -630,   596,   -13,    75,  -630,   538,  -630,
     546,   536,  1172,   476,   190,  -630,  -630,  -630,  -630,  1296,
    1296,  1296,  1690,   548,  -630,  -630,  -630,  -630,  -630,  -630,
    1296,  -630,   458,   458,  1690,   458,  -630,  -630,  -630,   536,
    -630,   540,   541,   542,   641,  -630,  1690,  1437,    79,  -630,
    1172,   525,  -630,  -630,  -630,  -630,  1437,   753,  -630,  -630,
     536,   618,  1542,  1542,  1542,  1437,  -630,  -630,  -630,  -630,
    -630,  -630,  1437,  -630
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,
    -630,  -630,  -630,  -630,  -630,   418,  -630,  -630,  -630,  -630,
    -630,  -630,  -630,  -630,   421,  -630,  -630,  -630,  -630,  -630,
    -630,  -630,   281,  -630,  -630,  -630,  -630,  -630,  -630,  -630,
    -630,    54,  -630,  -630,     3,  -630,  -630,  -630,  -630,  -630,
    -630,  -630,   694,  -630,  -630,    37,   570,   -37,  -630,  -122,
    -630,    74,  -630,  -630,  -630,  -630,  -630,  -630,  -630,   -17,
    -630,  -630,  -630,  -630,  -176,  -630,  -630,  -630,  -630,  -630,
    -630,  -630,  -630,  -630,  -630,  -630,  -121,  -630,  -630,  -630,
    -630,   373,   -41,  -630,  -630,  -630,  -630,  -630,  -630,  -388,
    -630,   -81,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,
    -601,  -630,     4,  -630,  -630,  -630,  -630,   316,  -630,  -630,
    -630,  -630,    18,  -630,   -96,  -630,  -630,  -630,   564,   -26,
    -630,  -127,  -630,  -630,  -630,  -630,  -630,  -629,  -630,  -630,
    -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,
    -630,  -630,  -630,  -621,  -630,  -630,  -630,  -630,  -630,  -630,
    -630,  -630,  -630,  -630,  -310,  -630,  -630,  -630,  -281,  -630,
    -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,   534,
    -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,
    -111,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,
    -630,  -630,  -630,    32,  -630,   -30,  -630,  -630,  -630,  -630,
    -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,
    -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,  -630,
     -75,  -630,  -630,  -630,  -630,   300,   655,   131,  -630,  -630,
     337,  -630,  -630,  -630,  -594,  -630,  -630,  -630,  -630,  -630,
    -630,  -630,  -110,  -630,   338,  -630,  -630,   -32,  -630,  -630,
    -630,  -630,   -97,  -630,  -630,  -630,  -630,  -630,  -630,  -630,
    -630,  -630,  -630,   -29,  -630,  -535,  -630,  -630,  -119,  -630,
    -630
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -251
static const yytype_int16 yytable[] =
{
     101,   101,   704,   147,   510,   105,   625,   145,   702,   275,
     683,   689,    -2,   257,   427,     4,   429,   445,   446,   387,
     666,   227,   793,   228,   794,     4,   807,   634,   222,   265,
     196,   197,   174,   542,   175,     5,     6,   238,   266,   637,
     285,   286,     3,   668,   571,   264,     6,   401,   239,   292,
     240,   241,   242,   243,   298,   300,   194,   662,   302,    24,
     304,   675,    25,   307,   310,    28,   313,   401,   315,   474,
    -250,   318,   319,   196,   197,   410,   322,   323,   195,   572,
    -250,  -250,   196,   197,    38,    47,   196,   197,   635,   760,
      48,    49,    50,   795,  -250,   223,  -250,   267,   142,   699,
     638,    12,    13,    14,   669,   808,    29,   402,   591,   229,
      32,    15,    47,   388,   663,     7,     8,    48,    49,    50,
     244,    34,   676,    16,    17,     7,     8,   682,    18,   196,
     197,    40,   761,    19,    36,   764,   245,   398,   196,   197,
     365,   767,   196,   197,   785,   798,   567,   565,   568,   569,
     570,   571,   366,   786,   568,   569,   570,   571,    42,   101,
     101,   566,   101,   324,   325,  -250,   326,  -250,   467,    82,
     813,    84,    85,    42,   567,   125,   568,   569,   570,   571,
     142,    69,   424,   425,    20,   142,   572,   123,   799,   124,
     436,   355,   572,   357,   152,   358,    82,    83,    84,    85,
     780,   829,   154,   838,   839,   840,   405,   754,    87,   406,
     549,   281,   196,   197,   572,   550,   407,   196,   197,   685,
     449,   450,   451,   452,   453,   454,   455,   456,   457,   458,
     459,   460,   461,   282,   283,   156,   408,   282,   283,   183,
     475,   107,   414,   108,   109,   157,   479,   110,   284,   394,
     395,   482,   284,   502,   503,   265,   265,   554,   410,   489,
     490,   491,   492,   598,   401,   101,   101,   101,   610,   401,
     428,   476,   477,   611,   401,   506,   507,   184,   160,   508,
     509,   161,   555,   511,   196,   197,   114,   115,   116,   117,
     101,   101,   118,   162,   441,   442,   163,   101,   101,   557,
     119,   196,   197,   265,   164,   167,   169,   265,   565,   185,
     418,   120,   419,   142,   265,   265,   265,   171,   172,   514,
     101,   705,   566,   519,   558,   553,   196,   197,   556,   177,
     525,   526,   527,   142,   650,   567,   651,   568,   569,   570,
     571,    11,   178,    21,    22,    23,   179,   765,   427,   196,
     197,   147,   188,   784,    47,   145,   180,   191,   193,    48,
      49,    50,    39,   225,   585,   226,   142,   232,   755,   233,
     576,   234,   235,   248,   249,   572,   592,   250,   251,   729,
     147,   252,   565,   587,   145,   590,   270,   647,   253,   593,
     254,   594,   272,   101,   287,   716,   566,   543,   465,   276,
     277,   278,   609,   288,   289,   290,   142,   142,   291,   567,
     311,   568,   569,   570,   571,   620,   265,   146,   623,   362,
     265,   265,   363,    69,   204,   205,   206,   207,   208,   209,
     210,   364,   597,   373,   375,   376,   602,   603,    82,    83,
      84,    85,   377,   378,   379,   380,   142,   308,   575,   572,
      87,   700,   701,   381,   142,   147,   382,   101,   142,   145,
     391,   586,   399,   422,   403,   142,   142,   142,   206,   207,
     208,   209,   210,   434,   435,   142,   438,   722,   723,   724,
     725,   726,   727,   728,   204,   205,   206,   207,   208,   209,
     210,   443,   691,   692,   693,   694,   695,   696,   686,   146,
     698,   444,   265,   484,   146,   485,   565,   705,   811,   812,
     427,   494,   497,   500,   520,   265,   521,   528,   544,   711,
     566,   522,   705,   705,   705,   523,   538,   546,   539,   197,
     310,   547,   210,   567,   548,   568,   569,   570,   571,   559,
     142,    47,   560,   561,   773,   562,    48,    49,    50,   401,
     142,   582,   142,   204,   205,   206,   207,   208,   209,   210,
     588,   596,   601,   600,   606,   101,   147,   142,   414,   677,
     145,   142,   142,   572,   624,   620,   690,   630,   142,   626,
     640,   265,   664,   643,   265,   666,   665,   545,   130,   667,
     670,   142,   802,   803,   142,   131,   805,   744,   410,   101,
     746,   101,   673,   132,   133,   134,   135,   136,   137,   138,
      69,   139,   674,   266,   781,   662,   814,   465,   715,   718,
     732,   719,   739,   738,   740,    82,   140,    84,    85,   743,
     101,   575,   146,   752,   756,   757,   194,   141,   787,   774,
     827,   204,   205,   206,   207,   208,   209,   210,   265,   778,
     788,   796,   146,   142,   663,   806,   815,   807,   374,   142,
     809,   142,   822,   823,   824,   505,   142,   142,   142,   142,
     142,   142,   142,   142,   397,   573,   142,   710,   742,   684,
     753,   465,   465,   819,   488,   146,   737,   810,   258,   565,
     828,   801,   751,   730,   512,   142,   262,   128,   830,   483,
     147,   777,   831,   566,   145,   779,     0,   465,   465,   465,
     465,   465,   465,   465,     0,     0,   567,   146,   568,   569,
     570,   571,   529,   530,     0,   146,   146,     0,     0,     0,
       0,     0,   142,     0,     0,   142,   531,   410,   143,     0,
       0,   748,   748,     0,     0,     0,     0,     0,     0,   825,
     532,   142,   533,   534,   535,   536,   572,     0,     0,   101,
     101,   101,     0,     0,     0,   146,     0,     0,     0,     0,
       0,   465,     0,   146,   465,     0,     0,   146,     0,     0,
       0,     0,     0,     0,   146,   146,   146,   101,     0,     0,
     142,     0,     0,     0,   146,   142,   101,     0,     0,   142,
     836,   565,   101,   101,   101,   101,     0,     0,     0,   841,
       0,     0,   101,     0,     0,   566,   843,     0,     0,     0,
     143,     0,   465,   465,     0,   143,   465,     0,   567,     0,
     568,   569,   570,   571,     0,   430,   142,   431,     0,   432,
     433,     0,     0,     0,     0,     0,   465,     0,   437,     0,
     439,   440,     0,     0,     0,     0,     0,     0,   465,   146,
       0,   837,     0,   447,   448,     0,     0,     0,   572,   146,
     465,   146,     0,     0,   142,     0,     0,     0,     0,   469,
     470,   471,   472,   473,     0,     0,   146,     0,     0,     0,
     146,   146,     0,     0,     0,     0,     0,   146,     0,   212,
     213,   214,   215,   216,   217,   218,     0,     0,     0,     0,
     146,   219,   220,   146,   466,     0,     0,     0,     0,     0,
       0,     0,   262,   262,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   146,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
       0,     0,     0,   143,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     262,     0,   146,   143,   262,     0,     0,   211,   146,     0,
     146,   262,   262,   262,     0,   146,   146,   146,   146,   146,
     146,   146,   146,     0,     0,   146,     0,     0,     0,     0,
     146,   146,     0,     0,     0,     0,   143,     0,     0,     0,
       0,     0,     0,     0,   146,   578,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   146,   146,   146,   146,
     146,   146,   146,     0,     0,     0,     0,     0,   143,     0,
       0,     0,     0,     0,     0,     0,   143,   143,     0,     0,
       0,   146,     0,     0,   146,     0,     0,     0,     0,     0,
     749,   749,    47,     0,     0,     0,     0,    48,    49,    50,
     146,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   262,     0,     0,   143,   262,   262,     0,
     146,     0,     0,   146,   143,     0,     0,     0,   143,     0,
       0,     0,     0,     0,     0,   143,   143,   143,     0,   146,
       0,     0,     0,     0,   146,   143,   259,     0,   146,     0,
       0,     0,     0,     0,   132,   133,   134,   135,   136,   137,
     138,    69,   139,   466,     0,     0,     0,     0,     0,     0,
       0,   146,   146,     0,     0,   146,    82,   140,    84,   260,
       0,     0,     0,     0,     0,   146,     0,     0,   261,     0,
       0,     0,     0,     0,     0,   146,   706,   707,     0,   262,
       0,     0,     0,     0,     0,     0,   712,   146,   713,   714,
     143,     0,   262,     0,     0,     0,     0,     0,     0,   146,
     143,     0,   143,   146,     0,     0,     0,   466,   466,    47,
       0,     0,     0,     0,    48,    49,    50,   143,     0,     0,
       0,   143,   143,     0,     0,     0,     0,     0,   143,     0,
       0,     0,     0,   466,   466,   466,   466,   466,   466,   466,
       0,   143,     0,     0,   143,     0,     0,     0,     0,     0,
       0,     0,   762,   763,     0,     0,   130,     0,   262,     0,
       0,   262,     0,   131,     0,     0,     0,   143,     0,     0,
       0,   132,   133,   134,   135,   776,     0,   273,    69,   139,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    82,   140,    84,    85,   466,     0,     0,
     466,     0,     0,   143,     0,   141,     0,     0,     0,   143,
       0,   143,     0,     0,     0,     0,   143,   143,   143,   143,
     143,   143,   143,   143,     0,   262,   143,     0,     0,     0,
       0,   143,   143,    47,     0,     0,     0,     0,    48,    49,
      50,     0,     0,     0,   818,   143,    51,     0,   466,   466,
       0,     0,   466,     0,     0,     0,     0,   143,   143,   143,
     143,   143,   143,   143,     0,     0,     0,     0,     0,     0,
       0,    60,   466,     0,     0,     0,     0,     0,     0,     0,
      61,     0,   143,     0,   466,   143,     0,     0,     0,    62,
      63,    64,    65,    66,    67,     0,   466,     0,    68,     0,
       0,   143,    69,     0,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,     0,    80,    81,    82,    83,    84,
      85,   143,     0,     0,   143,     0,     0,     0,     0,    87,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     143,     0,     0,     0,     0,   143,     0,     0,     0,   143,
      45,     0,     0,     0,     0,     0,     0,    46,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   143,   143,    47,     0,   143,     0,     0,    48,
      49,    50,     0,     0,     0,     0,   143,    51,    52,    53,
      54,    55,    56,    57,     0,     0,   143,    58,     0,     0,
      59,     0,     0,     0,     0,     0,     0,     0,   143,     0,
       0,     0,    60,     0,     0,     0,     0,     0,     0,     0,
     143,    61,     0,     0,   143,     0,     0,     0,     0,     0,
      62,    63,    64,    65,    66,    67,     0,     0,     0,    68,
       0,     0,     0,    69,     0,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    45,    80,    81,    82,    83,
      84,    85,    46,     0,     0,     0,     0,     0,    86,     0,
      87,     0,     0,     0,     0,     0,     0,     0,     0,    47,
       0,     0,     0,     0,    48,    49,    50,     0,     0,     0,
       0,     0,    51,    52,    53,    54,    55,    56,    57,     0,
       0,     0,    58,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    60,     0,     0,
       0,     0,     0,     0,     0,     0,    61,     0,     0,     0,
       0,     0,     0,     0,     0,    62,    63,    64,    65,    66,
      67,     0,     0,     0,    68,     0,     0,     0,    69,     0,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      45,    80,    81,    82,    83,    84,    85,    46,     0,     0,
       0,     0,     0,    86,     0,    87,     0,     0,     0,     0,
       0,     0,     0,     0,    47,     0,     0,     0,     0,    48,
      49,    50,     0,     0,     0,     0,     0,    51,    52,    53,
      54,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    60,     0,     0,     0,     0,    47,     0,     0,
       0,    61,    48,    49,    50,     0,     0,     0,     0,     0,
      62,    63,    64,    65,    66,    67,     0,     0,     0,    68,
       0,     0,     0,    69,     0,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,     0,    80,    81,    82,    83,
      84,    85,    47,     0,   462,     0,     0,    48,    49,    50,
      87,     0,     0,     0,     0,    51,     0,     0,     0,   132,
     133,   134,   135,     0,     0,   273,    69,   139,     0,     0,
       0,     0,     0,     0,     0,   463,     0,     0,     0,     0,
      60,    82,   140,    84,    85,     0,     0,     0,     0,    61,
       0,     0,     0,   464,     0,     0,     0,     0,    62,    63,
      64,    65,    66,    67,     0,     0,     0,    68,     0,     0,
       0,    69,     0,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,     0,    80,    81,    82,    83,    84,   577,
      47,     0,     0,     0,     0,    48,    49,    50,    87,     0,
       0,     0,     0,    51,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    47,     0,     0,     0,     0,    48,    49,    50,
       0,     0,     0,     0,     0,    51,     0,    61,     0,     0,
       0,     0,     0,     0,     0,     0,    62,    63,    64,    65,
      66,    67,     0,     0,     0,    68,     0,     0,     0,    69,
       0,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,     0,    80,    81,    82,    83,    84,    85,    62,    63,
      64,    65,    66,    67,     0,     0,    87,    68,     0,     0,
       0,    69,     0,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,     0,    80,    81,    82,    83,    84,    85,
      47,     0,     0,     0,     0,    48,    49,    50,    87,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   462,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   132,   133,   134,   135,     0,     0,   273,    69,
     139,     0,     0,     0,     0,     0,     0,     0,   463,     0,
       0,     0,     0,     0,    82,   140,    84,   646,     0,     0,
       0,     0,     0,     0,     0,     0,   464
};

static const yytype_int16 yycheck[] =
{
      30,    31,   631,    44,   392,    31,   541,    44,   629,   131,
     604,   612,     0,   123,   295,    13,   297,   327,   328,    20,
      39,    26,    49,    28,    51,    13,    39,    61,    48,   126,
      63,    64,   121,   421,   123,    23,    34,    15,     8,    61,
     150,   151,     0,    61,    80,   126,    34,    62,    26,   159,
      28,    29,    30,    31,   165,   166,    39,    59,   168,     9,
     170,    61,    12,   173,   174,   114,   176,    62,   178,   350,
      25,   181,   182,    63,    64,    62,   186,   187,    61,   115,
      35,    36,    63,    64,   123,    27,    63,    64,   122,   122,
      32,    33,    34,   120,    49,   115,    51,    67,    44,   118,
     122,     5,     6,     7,   122,   118,   114,   122,   496,   114,
     114,    15,    27,   114,   116,   113,   114,    32,    33,    34,
      98,   114,   122,    27,    28,   113,   114,   122,    32,    63,
      64,    11,   122,    37,   114,   122,   114,   259,    63,    64,
      24,   122,    63,    64,   745,   122,    75,    48,    77,    78,
      79,    80,    36,   747,    77,    78,    79,    80,    27,   189,
     190,    62,   192,   189,   190,   120,   192,   122,   344,   111,
     791,   113,   114,    42,    75,    25,    77,    78,    79,    80,
     126,    96,   293,   294,    88,   131,   115,    59,   122,    61,
     312,   221,   115,   117,   116,   119,   111,   112,   113,   114,
     735,   122,   116,   832,   833,   834,    19,   108,   123,    22,
      61,    25,    63,    64,   115,    61,    29,    63,    64,   607,
     331,   332,   333,   334,   335,   336,   337,   338,   339,   340,
     341,   342,   343,    47,    48,   116,    49,    47,    48,   114,
     351,    15,   279,    17,    18,   119,   356,    21,    62,    24,
      25,   361,    62,    24,    25,   352,   353,    61,    62,   369,
     370,   371,   372,    61,    62,   295,   296,   297,    61,    62,
     296,   352,   353,    61,    62,   385,   386,   121,   123,   389,
     390,   123,    61,   393,    63,    64,    15,    16,    17,    18,
     320,   321,    21,   123,   320,   321,   123,   327,   328,    61,
      29,    63,    64,   400,   123,   123,   123,   404,    48,   121,
     117,    40,   119,   259,   411,   412,   413,   123,   123,   400,
     350,   631,    62,   404,    61,   435,    63,    64,   438,   123,
     411,   412,   413,   279,   117,    75,   119,    77,    78,    79,
      80,     4,   123,     6,     7,     8,   123,    61,   629,    63,
      64,   392,   120,   741,    27,   392,   123,    25,    61,    32,
      33,    34,    25,    62,   486,    25,   312,    59,   108,    59,
     480,    59,    59,    59,    59,   115,   498,    59,    59,   660,
     421,    59,    48,   493,   421,   495,    25,   563,    59,   499,
      59,   501,   113,   423,   114,    61,    62,   423,   344,   123,
     123,   123,   524,   114,   114,   113,   352,   353,   121,    75,
     122,    77,    78,    79,    80,   537,   513,    44,   540,    59,
     517,   518,    59,    96,    74,    75,    76,    77,    78,    79,
      80,    59,   513,    25,    59,    59,   517,   518,   111,   112,
     113,   114,    59,    59,    59,    59,   392,   120,   478,   115,
     123,   627,   628,    59,   400,   496,    59,   487,   404,   496,
      20,   487,    59,   118,   123,   411,   412,   413,    76,    77,
      78,    79,    80,   120,    67,   421,    61,   653,   654,   655,
     656,   657,   658,   659,    74,    75,    76,    77,    78,    79,
      80,   120,   614,   615,   616,   617,   618,   619,   608,   126,
     622,   122,   599,   123,   131,   123,    48,   817,   789,   790,
     791,   123,   113,    14,    59,   612,    59,   115,   108,   641,
      62,    59,   832,   833,   834,    59,   122,    39,    61,    64,
     640,   122,    80,    75,   122,    77,    78,    79,    80,   120,
     486,    27,   120,   113,   720,   114,    32,    33,    34,    62,
     496,   114,   498,    74,    75,    76,    77,    78,    79,    80,
     114,    38,    61,   122,    34,   595,   607,   513,   605,   595,
     607,   517,   518,   115,   114,   697,   613,   121,   524,   120,
      61,   678,   118,   122,   681,    39,    62,   108,    74,   108,
     114,   537,   768,   769,   540,    81,   772,   678,    62,   629,
     681,   631,    36,    89,    90,    91,    92,    93,    94,    95,
      96,    97,   108,     8,   736,    59,   792,   563,    59,   122,
     114,    61,   122,   108,    61,   111,   112,   113,   114,    35,
     660,   661,   259,   122,    39,    51,    39,   123,   122,   114,
     816,    74,    75,    76,    77,    78,    79,    80,   745,   114,
     122,    52,   279,   599,   116,    59,   108,    39,   237,   605,
     782,   607,   122,   122,   122,   384,   612,   613,   614,   615,
     616,   617,   618,   619,   256,   108,   622,   640,   675,   605,
     697,   627,   628,   804,   368,   312,   668,   783,   124,    48,
     817,   766,   688,   661,   394,   641,   126,    42,   820,   361,
     741,   733,   821,    62,   741,   734,    -1,   653,   654,   655,
     656,   657,   658,   659,    -1,    -1,    75,   344,    77,    78,
      79,    80,    47,    48,    -1,   352,   353,    -1,    -1,    -1,
      -1,    -1,   678,    -1,    -1,   681,    61,    62,    44,    -1,
      -1,   687,   688,    -1,    -1,    -1,    -1,    -1,    -1,   108,
      75,   697,    77,    78,    79,    80,   115,    -1,    -1,   789,
     790,   791,    -1,    -1,    -1,   392,    -1,    -1,    -1,    -1,
      -1,   717,    -1,   400,   720,    -1,    -1,   404,    -1,    -1,
      -1,    -1,    -1,    -1,   411,   412,   413,   817,    -1,    -1,
     736,    -1,    -1,    -1,   421,   741,   826,    -1,    -1,   745,
     826,    48,   832,   833,   834,   835,    -1,    -1,    -1,   835,
      -1,    -1,   842,    -1,    -1,    62,   842,    -1,    -1,    -1,
     126,    -1,   768,   769,    -1,   131,   772,    -1,    75,    -1,
      77,    78,    79,    80,    -1,   301,   782,   303,    -1,   305,
     306,    -1,    -1,    -1,    -1,    -1,   792,    -1,   314,    -1,
     316,   317,    -1,    -1,    -1,    -1,    -1,    -1,   804,   486,
      -1,   108,    -1,   329,   330,    -1,    -1,    -1,   115,   496,
     816,   498,    -1,    -1,   820,    -1,    -1,    -1,    -1,   345,
     346,   347,   348,   349,    -1,    -1,   513,    -1,    -1,    -1,
     517,   518,    -1,    -1,    -1,    -1,    -1,   524,    -1,    54,
      55,    56,    57,    58,    59,    60,    -1,    -1,    -1,    -1,
     537,    66,    67,   540,   344,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   352,   353,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   563,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      -1,    -1,    -1,   259,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     400,    -1,   599,   279,   404,    -1,    -1,   108,   605,    -1,
     607,   411,   412,   413,    -1,   612,   613,   614,   615,   616,
     617,   618,   619,    -1,    -1,   622,    -1,    -1,    -1,    -1,
     627,   628,    -1,    -1,    -1,    -1,   312,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   641,   481,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   653,   654,   655,   656,
     657,   658,   659,    -1,    -1,    -1,    -1,    -1,   344,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   352,   353,    -1,    -1,
      -1,   678,    -1,    -1,   681,    -1,    -1,    -1,    -1,    -1,
     687,   688,    27,    -1,    -1,    -1,    -1,    32,    33,    34,
     697,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   513,    -1,    -1,   392,   517,   518,    -1,
     717,    -1,    -1,   720,   400,    -1,    -1,    -1,   404,    -1,
      -1,    -1,    -1,    -1,    -1,   411,   412,   413,    -1,   736,
      -1,    -1,    -1,    -1,   741,   421,    81,    -1,   745,    -1,
      -1,    -1,    -1,    -1,    89,    90,    91,    92,    93,    94,
      95,    96,    97,   563,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   768,   769,    -1,    -1,   772,   111,   112,   113,   114,
      -1,    -1,    -1,    -1,    -1,   782,    -1,    -1,   123,    -1,
      -1,    -1,    -1,    -1,    -1,   792,   632,   633,    -1,   599,
      -1,    -1,    -1,    -1,    -1,    -1,   642,   804,   644,   645,
     486,    -1,   612,    -1,    -1,    -1,    -1,    -1,    -1,   816,
     496,    -1,   498,   820,    -1,    -1,    -1,   627,   628,    27,
      -1,    -1,    -1,    -1,    32,    33,    34,   513,    -1,    -1,
      -1,   517,   518,    -1,    -1,    -1,    -1,    -1,   524,    -1,
      -1,    -1,    -1,   653,   654,   655,   656,   657,   658,   659,
      -1,   537,    -1,    -1,   540,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   708,   709,    -1,    -1,    74,    -1,   678,    -1,
      -1,   681,    -1,    81,    -1,    -1,    -1,   563,    -1,    -1,
      -1,    89,    90,    91,    92,   731,    -1,    95,    96,    97,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   111,   112,   113,   114,   717,    -1,    -1,
     720,    -1,    -1,   599,    -1,   123,    -1,    -1,    -1,   605,
      -1,   607,    -1,    -1,    -1,    -1,   612,   613,   614,   615,
     616,   617,   618,   619,    -1,   745,   622,    -1,    -1,    -1,
      -1,   627,   628,    27,    -1,    -1,    -1,    -1,    32,    33,
      34,    -1,    -1,    -1,   800,   641,    40,    -1,   768,   769,
      -1,    -1,   772,    -1,    -1,    -1,    -1,   653,   654,   655,
     656,   657,   658,   659,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    65,   792,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      74,    -1,   678,    -1,   804,   681,    -1,    -1,    -1,    83,
      84,    85,    86,    87,    88,    -1,   816,    -1,    92,    -1,
      -1,   697,    96,    -1,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,    -1,   109,   110,   111,   112,   113,
     114,   717,    -1,    -1,   720,    -1,    -1,    -1,    -1,   123,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     736,    -1,    -1,    -1,    -1,   741,    -1,    -1,    -1,   745,
       3,    -1,    -1,    -1,    -1,    -1,    -1,    10,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   768,   769,    27,    -1,   772,    -1,    -1,    32,
      33,    34,    -1,    -1,    -1,    -1,   782,    40,    41,    42,
      43,    44,    45,    46,    -1,    -1,   792,    50,    -1,    -1,
      53,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   804,    -1,
      -1,    -1,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     816,    74,    -1,    -1,   820,    -1,    -1,    -1,    -1,    -1,
      83,    84,    85,    86,    87,    88,    -1,    -1,    -1,    92,
      -1,    -1,    -1,    96,    -1,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,     3,   109,   110,   111,   112,
     113,   114,    10,    -1,    -1,    -1,    -1,    -1,   121,    -1,
     123,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,
      -1,    -1,    -1,    -1,    32,    33,    34,    -1,    -1,    -1,
      -1,    -1,    40,    41,    42,    43,    44,    45,    46,    -1,
      -1,    -1,    50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    65,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    74,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,
      88,    -1,    -1,    -1,    92,    -1,    -1,    -1,    96,    -1,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
       3,   109,   110,   111,   112,   113,   114,    10,    -1,    -1,
      -1,    -1,    -1,   121,    -1,   123,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,    32,
      33,    34,    -1,    -1,    -1,    -1,    -1,    40,    41,    42,
      43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    65,    -1,    -1,    -1,    -1,    27,    -1,    -1,
      -1,    74,    32,    33,    34,    -1,    -1,    -1,    -1,    -1,
      83,    84,    85,    86,    87,    88,    -1,    -1,    -1,    92,
      -1,    -1,    -1,    96,    -1,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,    -1,   109,   110,   111,   112,
     113,   114,    27,    -1,    74,    -1,    -1,    32,    33,    34,
     123,    -1,    -1,    -1,    -1,    40,    -1,    -1,    -1,    89,
      90,    91,    92,    -1,    -1,    95,    96,    97,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   105,    -1,    -1,    -1,    -1,
      65,   111,   112,   113,   114,    -1,    -1,    -1,    -1,    74,
      -1,    -1,    -1,   123,    -1,    -1,    -1,    -1,    83,    84,
      85,    86,    87,    88,    -1,    -1,    -1,    92,    -1,    -1,
      -1,    96,    -1,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,    -1,   109,   110,   111,   112,   113,   114,
      27,    -1,    -1,    -1,    -1,    32,    33,    34,   123,    -1,
      -1,    -1,    -1,    40,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    27,    -1,    -1,    -1,    -1,    32,    33,    34,
      -1,    -1,    -1,    -1,    -1,    40,    -1,    74,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    83,    84,    85,    86,
      87,    88,    -1,    -1,    -1,    92,    -1,    -1,    -1,    96,
      -1,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,    -1,   109,   110,   111,   112,   113,   114,    83,    84,
      85,    86,    87,    88,    -1,    -1,   123,    92,    -1,    -1,
      -1,    96,    -1,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,    -1,   109,   110,   111,   112,   113,   114,
      27,    -1,    -1,    -1,    -1,    32,    33,    34,   123,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    74,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    89,    90,    91,    92,    -1,    -1,    95,    96,
      97,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   105,    -1,
      -1,    -1,    -1,    -1,   111,   112,   113,   114,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   123
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   125,   126,     0,    13,    23,    34,   113,   114,   127,
     364,   364,     5,     6,     7,    15,    27,    28,    32,    37,
      88,   364,   364,   364,     9,    12,   359,   361,   114,   114,
     130,   131,   114,   136,   114,   134,   114,   252,   123,   364,
      11,   360,   361,   128,   129,     3,    10,    27,    32,    33,
      34,    40,    41,    42,    43,    44,    45,    46,    50,    53,
      65,    74,    83,    84,    85,    86,    87,    88,    92,    96,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     109,   110,   111,   112,   113,   114,   121,   123,   165,   176,
     215,   253,   255,   261,   284,   285,   288,   292,   303,   314,
     323,   329,   371,   376,   378,   253,   138,    15,    17,    18,
      21,   148,   150,   132,    15,    16,    17,    18,    21,    29,
      40,   139,   141,    59,    61,    25,   363,   362,   360,   170,
      74,    81,    89,    90,    91,    92,    93,    94,    95,    97,
     112,   123,   165,   176,   180,   181,   215,   216,   223,   386,
     290,   291,   116,   166,   116,   167,   116,   119,   358,   289,
     123,   123,   123,   123,   123,   306,   322,   123,   374,   123,
     340,   123,   123,   375,   121,   123,   348,   123,   123,   123,
     123,   349,   350,   114,   121,   121,   373,   372,   120,   276,
     377,    25,   254,    61,    39,    61,    63,    64,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,   108,    54,    55,    56,    57,    58,    59,    60,    66,
      67,   324,    48,   115,   379,    62,    25,    26,    28,   114,
     241,   243,    59,    59,    59,    59,   137,   149,    15,    26,
      28,    29,    30,    31,    98,   114,   156,   158,    59,    59,
      59,    59,    59,    59,    59,   135,   140,   376,   252,    81,
     114,   123,   180,   216,   225,   386,     8,    67,   365,   368,
      25,   171,   113,    95,   181,   183,   123,   123,   123,   182,
     387,    25,    47,    48,    62,   376,   376,   114,   114,   114,
     113,   121,   376,   272,   268,   262,   266,   256,   314,   329,
     314,   344,   376,   351,   376,   341,   333,   376,   120,   179,
     376,   122,   177,   376,   337,   376,   330,   335,   376,   376,
     347,   343,   376,   376,   253,   253,   253,   286,   287,   305,
     304,   312,   311,   310,   309,   308,   307,   316,   315,   318,
     320,   319,   317,   321,   313,   299,   298,   297,   296,   295,
     293,   301,   300,   294,   325,   329,   353,   117,   119,   381,
     384,   380,    59,    59,    59,    24,    36,   239,   242,   154,
     153,   151,   152,    25,   148,    59,    59,    59,    59,    59,
      59,    59,    59,   133,   157,   147,   146,    20,   114,   145,
     143,    20,   144,   142,    24,    25,   155,   139,   183,    59,
     228,    62,   122,   123,   366,    19,    22,    29,    49,   172,
      62,   221,   219,   217,   181,   183,   185,   195,   117,   119,
     388,   224,   118,   346,   314,   314,   277,   292,   253,   292,
     303,   303,   303,   303,   120,    67,   183,   303,    61,   303,
     303,   253,   253,   120,   122,   288,   288,   303,   303,   314,
     314,   314,   314,   314,   314,   314,   314,   314,   314,   314,
     314,   314,    74,   105,   123,   165,   180,   198,   391,   303,
     303,   303,   303,   303,   292,   314,   225,   225,   326,   376,
     385,   382,   376,   378,   123,   123,   245,   240,   241,   376,
     376,   376,   376,   160,   123,   164,   162,   113,   163,   161,
      14,   159,    24,    25,   237,   156,   376,   376,   376,   376,
     223,   376,   359,   226,   225,   229,   231,   227,   369,   225,
      59,    59,    59,    59,   184,   225,   225,   225,   115,    47,
      48,    61,    75,    77,    78,    79,    80,   186,   122,    61,
     390,   389,   223,   253,   108,   108,    39,   122,   122,    61,
      61,   342,   334,   376,    61,    61,   376,    61,    61,   120,
     120,   113,   114,   199,   392,    48,    62,    75,    77,    78,
      79,    80,   115,   108,   327,   329,   376,   114,   303,   396,
     397,   399,   114,   246,   244,   183,   253,   376,   114,   168,
     376,   223,   183,   376,   376,   238,    38,   225,    61,   230,
     122,    61,   225,   225,   367,   173,    34,   175,   174,   183,
      61,    61,   218,   187,   196,   188,   191,   190,   189,   192,
     183,   193,   197,   183,   114,   399,   120,   273,   269,   263,
     121,   257,   345,   352,    61,   122,   356,    61,   122,   354,
      61,   178,   338,   122,   331,   336,   114,   198,   207,   212,
     117,   119,   393,   200,   202,   205,   204,   203,   206,   201,
     302,   328,    59,   116,   118,    62,    39,   108,    61,   122,
     114,   248,   250,    36,   108,    61,   122,   253,   232,   225,
     234,   233,   122,   368,   185,   223,   376,   222,   220,   234,
     181,   183,   183,   183,   183,   183,   183,   194,   183,   118,
     198,   198,   277,   267,   261,   288,   303,   303,   357,   355,
     179,   183,   303,   303,   303,    59,    61,   208,   122,    61,
     395,   394,   198,   198,   198,   198,   198,   198,   198,   292,
     327,   400,   114,   383,   398,   401,   247,   246,   108,   122,
      61,   169,   168,    35,   225,   235,   225,   370,   165,   215,
     236,   236,   122,   193,   108,   108,    39,    51,   278,   258,
     122,   122,   303,   303,   122,    61,   332,   122,   209,   213,
     198,   210,   214,   198,   114,   402,   303,   381,   114,   397,
     399,   183,   251,   249,   223,   234,   368,   122,   122,   274,
     270,   264,   281,    49,    51,   120,    52,   259,   122,   122,
     339,   354,   198,   198,   211,   198,    59,    39,   118,   183,
     248,   292,   292,   277,   198,   108,   279,   260,   303,   210,
     403,   404,   122,   122,   122,   108,   283,   198,   255,   122,
     183,   402,   275,   271,   265,   282,   253,   108,   261,   261,
     261,   253,   280,   253
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
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

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

/* DQ (12/6/2016): Eliminate use of implicit function declaration, making warning into an error for warning: -Wdelete-non-virtual-dtor. */
int yylex();
#endif

/* DQ (12/6/2016): Eliminate use of implicit function declaration, making warning into an error for warning: -Wdelete-non-virtual-dtor. */
int yyerror (char* yymsg);

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
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
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
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
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


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

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
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

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
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
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

/* Line 1464 of yacc.c  */
#line 150 "poet_yacc.y"
    {(yyval).ptr=insert_parameter((yyvsp[(3) - (3)]).ptr);}
    break;

  case 6:

/* Line 1464 of yacc.c  */
#line 151 "poet_yacc.y"
    {finish_parameter((yyvsp[(4) - (6)])); }
    break;

  case 7:

/* Line 1464 of yacc.c  */
#line 152 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE; }
    break;

  case 8:

/* Line 1464 of yacc.c  */
#line 153 "poet_yacc.y"
    {eval_define(make_macroVar((yyvsp[(3) - (6)]).ptr),(yyvsp[(5) - (6)]).ptr); }
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 154 "poet_yacc.y"
    {(yyval).config=GLOBAL_VAR; }
    break;

  case 10:

/* Line 1464 of yacc.c  */
#line 154 "poet_yacc.y"
    { insert_eval((yyvsp[(4) - (5)]).ptr); }
    break;

  case 11:

/* Line 1464 of yacc.c  */
#line 155 "poet_yacc.y"
    {(yyval).config=GLOBAL_VAR; }
    break;

  case 12:

/* Line 1464 of yacc.c  */
#line 155 "poet_yacc.y"
    { insert_cond((yyvsp[(4) - (5)]).ptr); }
    break;

  case 13:

/* Line 1464 of yacc.c  */
#line 156 "poet_yacc.y"
    { insert_trace((yyvsp[(3) - (4)])); }
    break;

  case 14:

/* Line 1464 of yacc.c  */
#line 157 "poet_yacc.y"
    {(yyval).ptr=insert_code((yyvsp[(3) - (3)]).ptr);(yyval).config=0; }
    break;

  case 15:

/* Line 1464 of yacc.c  */
#line 157 "poet_yacc.y"
    {(yyval).ptr = (yyvsp[(4) - (5)]).ptr; }
    break;

  case 17:

/* Line 1464 of yacc.c  */
#line 158 "poet_yacc.y"
    {(yyval).ptr=insert_input(); }
    break;

  case 18:

/* Line 1464 of yacc.c  */
#line 158 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(3) - (4)]).ptr; }
    break;

  case 20:

/* Line 1464 of yacc.c  */
#line 159 "poet_yacc.y"
    {(yyval).ptr=insert_output(); }
    break;

  case 21:

/* Line 1464 of yacc.c  */
#line 159 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(3) - (4)]).ptr; }
    break;

  case 23:

/* Line 1464 of yacc.c  */
#line 160 "poet_yacc.y"
    { (yyval).ptr=insert_xform((yyvsp[(3) - (3)]).ptr); }
    break;

  case 24:

/* Line 1464 of yacc.c  */
#line 161 "poet_yacc.y"
    { set_xform_def((yyvsp[(4) - (6)]).ptr,(yyvsp[(6) - (6)]).ptr); }
    break;

  case 25:

/* Line 1464 of yacc.c  */
#line 163 "poet_yacc.y"
    { if ((yyvsp[(2) - (3)]).ptr==0) insert_source((yyvsp[(1) - (3)]).ptr); else insert_source(make_inputlist2((yyvsp[(1) - (3)]).ptr,(yyvsp[(2) - (3)]).ptr)); }
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 165 "poet_yacc.y"
    {(yyval).ptr = (yyvsp[(0) - (1)]).ptr; }
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 167 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 167 "poet_yacc.y"
    { set_input_debug((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 168 "poet_yacc.y"
    { set_input_target((yyvsp[(0) - (3)]).ptr, (yyvsp[(0) - (3)]).ptr); }
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 169 "poet_yacc.y"
    { set_input_target((yyvsp[(0) - (3)]).ptr,make_varRef((yyvsp[(3) - (3)]).ptr,GLOBAL_VAR)); }
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 170 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 170 "poet_yacc.y"
    {set_input_syntax((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr);}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 171 "poet_yacc.y"
    { set_input_codeType((yyvsp[(0) - (3)]).ptr, (yyvsp[(0) - (3)]).ptr); }
    break;

  case 36:

/* Line 1464 of yacc.c  */
#line 172 "poet_yacc.y"
    {(yyval).config=CODE_OR_XFORM_VAR; }
    break;

  case 37:

/* Line 1464 of yacc.c  */
#line 173 "poet_yacc.y"
    { set_input_codeType((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 38:

/* Line 1464 of yacc.c  */
#line 174 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 174 "poet_yacc.y"
    {set_input_files((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 175 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 175 "poet_yacc.y"
    { set_input_annot((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 42:

/* Line 1464 of yacc.c  */
#line 176 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 176 "poet_yacc.y"
    { set_input_cond((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 178 "poet_yacc.y"
    {(yyval).ptr = (yyvsp[(0) - (1)]).ptr; }
    break;

  case 47:

/* Line 1464 of yacc.c  */
#line 180 "poet_yacc.y"
    {(yyval).config=GLOBAL_VAR;}
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 181 "poet_yacc.y"
    { set_output_target((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 182 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 183 "poet_yacc.y"
    { set_output_syntax((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 184 "poet_yacc.y"
    {(yyval).config=GLOBAL_VAR;}
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 185 "poet_yacc.y"
    { set_output_file((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 186 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 187 "poet_yacc.y"
    { set_output_cond((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 188 "poet_yacc.y"
    { set_input_inline((yyvsp[(0) - (3)]).ptr,(yyvsp[(2) - (3)]).ptr);  }
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 191 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(0) - (1)]).ptr; (yyval).config=(yyvsp[(1) - (1)]).config;}
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 192 "poet_yacc.y"
    { set_code_params((yyvsp[(0) - (5)]).ptr,(yyvsp[(4) - (5)]).ptr); (yyval).config=(yyvsp[(0) - (5)]).config; }
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 193 "poet_yacc.y"
    {(yyval).config=ID_DEFAULT;}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 193 "poet_yacc.y"
    { set_code_attr((yyvsp[(0) - (4)]).ptr, (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 194 "poet_yacc.y"
    { set_code_inherit((yyvsp[(0) - (3)]).ptr, (yyvsp[(1) - (3)]).ptr); }
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 195 "poet_yacc.y"
    {(yyval).config=ID_DEFAULT;}
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 196 "poet_yacc.y"
    { set_local_static(make_sourceString("cond",4), (yyvsp[(4) - (4)]).ptr,LVAR_ATTR,(yyvsp[(4) - (4)]).ptr,1); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 197 "poet_yacc.y"
    {(yyval).config=ID_DEFAULT; }
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 198 "poet_yacc.y"
    { set_local_static(make_sourceString("rebuild",7), (yyvsp[(4) - (4)]).ptr,LVAR_ATTR,(yyvsp[(4) - (4)]).ptr,1); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 199 "poet_yacc.y"
    {(yyval).config=ID_DEFAULT; }
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 200 "poet_yacc.y"
    { set_code_parse((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 201 "poet_yacc.y"
    { set_code_lookahead((yyvsp[(0) - (3)]).ptr, (yyvsp[(3) - (3)]).ptr); (yyval).config=(yyvsp[(0) - (3)]).config; }
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 202 "poet_yacc.y"
    {(yyval).config=ID_DEFAULT; }
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 203 "poet_yacc.y"
    { set_local_static(make_sourceString("match",5), (yyvsp[(4) - (4)]).ptr,LVAR_ATTR,(yyvsp[(4) - (4)]).ptr,1); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 73:

/* Line 1464 of yacc.c  */
#line 204 "poet_yacc.y"
    {(yyval).config=ID_DEFAULT; }
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 205 "poet_yacc.y"
    { set_local_static(make_sourceString("output",6), (yyvsp[(4) - (4)]).ptr,LVAR_ATTR,(yyvsp[(4) - (4)]).ptr,1); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 75:

/* Line 1464 of yacc.c  */
#line 207 "poet_yacc.y"
    { (yyval).ptr= make_varRef((yyvsp[(1) - (1)]).ptr,(yyvsp[(0) - (1)]).config); (yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 208 "poet_yacc.y"
    { (yyval).ptr=make_varRef((yyvsp[(3) - (3)]).ptr,GLOBAL_SCOPE); (yyval).config=(yyvsp[(0) - (3)]).config; }
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 209 "poet_yacc.y"
    {(yyval)=(yyvsp[(2) - (2)]); }
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 210 "poet_yacc.y"
    {(yyval)=(yyvsp[(2) - (2)]); }
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 212 "poet_yacc.y"
    { (yyval).ptr = make_dummyOperator(POET_OP_XFORM); (yyval).config=(yyvsp[(0) - (0)]).config; }
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 213 "poet_yacc.y"
    { (yyval).ptr=make_varRef((yyvsp[(2) - (2)]).ptr,XFORM_VAR); (yyval).config=(yyvsp[(0) - (2)]).config; }
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 215 "poet_yacc.y"
    { (yyval).ptr = make_dummyOperator(POET_OP_CODE); }
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 216 "poet_yacc.y"
    { (yyval).ptr=make_varRef((yyvsp[(2) - (2)]).ptr,CODE_VAR); }
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 219 "poet_yacc.y"
    { (yyval).ptr=make_localPar((yyvsp[(1) - (1)]).ptr,0,LVAR_CODEPAR); }
    break;

  case 84:

/* Line 1464 of yacc.c  */
#line 220 "poet_yacc.y"
    {(yyval).config=CODE_OR_XFORM_VAR; }
    break;

  case 85:

/* Line 1464 of yacc.c  */
#line 220 "poet_yacc.y"
    { (yyval).ptr = make_localPar((yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr,LVAR_CODEPAR); }
    break;

  case 86:

/* Line 1464 of yacc.c  */
#line 221 "poet_yacc.y"
    {(yyval).ptr = make_sourceVector((yyvsp[(1) - (3)]).ptr, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 88:

/* Line 1464 of yacc.c  */
#line 223 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(0) - (1)]).ptr; }
    break;

  case 90:

/* Line 1464 of yacc.c  */
#line 224 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 91:

/* Line 1464 of yacc.c  */
#line 225 "poet_yacc.y"
    {set_param_type((yyvsp[(0) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 92:

/* Line 1464 of yacc.c  */
#line 226 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE;}
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 226 "poet_yacc.y"
    { set_param_default((yyvsp[(0) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 227 "poet_yacc.y"
    {(yyval).config=GLOBAL_SCOPE; }
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 227 "poet_yacc.y"
    { set_param_parse((yyvsp[(0) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 96:

/* Line 1464 of yacc.c  */
#line 228 "poet_yacc.y"
    { set_param_message((yyvsp[(0) - (3)]).ptr, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 231 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config; }
    break;

  case 98:

/* Line 1464 of yacc.c  */
#line 232 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config; }
    break;

  case 99:

/* Line 1464 of yacc.c  */
#line 233 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MAP, (yyvsp[(4) - (8)]).ptr, (yyvsp[(7) - (8)]).ptr); }
    break;

  case 100:

/* Line 1464 of yacc.c  */
#line 234 "poet_yacc.y"
    { (yyval).ptr=make_dummyOperator(POET_OP_MAP); }
    break;

  case 101:

/* Line 1464 of yacc.c  */
#line 235 "poet_yacc.y"
    { (yyval).ptr=make_dummyOperator(POET_OP_MAP); }
    break;

  case 102:

/* Line 1464 of yacc.c  */
#line 236 "poet_yacc.y"
    { (yyval).ptr=make_dummyOperator(POET_OP_MAP); }
    break;

  case 103:

/* Line 1464 of yacc.c  */
#line 238 "poet_yacc.y"
    {(yyval).ptr=make_sourceUop(POET_OP_MAP, (yyvsp[(3) - (4)]).ptr); }
    break;

  case 104:

/* Line 1464 of yacc.c  */
#line 241 "poet_yacc.y"
    { (yyval).ptr = make_inputlist(make_inputlist((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr),0); }
    break;

  case 105:

/* Line 1464 of yacc.c  */
#line 243 "poet_yacc.y"
    { (yyval).ptr = make_inputlist2(make_inputlist((yyvsp[(1) - (5)]).ptr,(yyvsp[(3) - (5)]).ptr),(yyvsp[(5) - (5)])); }
    break;

  case 106:

/* Line 1464 of yacc.c  */
#line 245 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 246 "poet_yacc.y"
    { (yyval).ptr = make_any(); }
    break;

  case 108:

/* Line 1464 of yacc.c  */
#line 247 "poet_yacc.y"
    { (yyval).ptr = make_type(TYPE_INT); }
    break;

  case 109:

/* Line 1464 of yacc.c  */
#line 248 "poet_yacc.y"
    { (yyval).ptr = make_type(TYPE_STRING); }
    break;

  case 110:

/* Line 1464 of yacc.c  */
#line 249 "poet_yacc.y"
    { (yyval).ptr = make_type(TYPE_ID); }
    break;

  case 111:

/* Line 1464 of yacc.c  */
#line 250 "poet_yacc.y"
    { (yyval).ptr = make_dummyOperator(POET_OP_EXP); }
    break;

  case 112:

/* Line 1464 of yacc.c  */
#line 251 "poet_yacc.y"
    {  (yyval).ptr = make_dummyOperator(POET_OP_VAR); }
    break;

  case 113:

/* Line 1464 of yacc.c  */
#line 252 "poet_yacc.y"
    { (yyval).ptr = make_dummyOperator(POET_OP_TUPLE); }
    break;

  case 114:

/* Line 1464 of yacc.c  */
#line 253 "poet_yacc.y"
    { (yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 115:

/* Line 1464 of yacc.c  */
#line 255 "poet_yacc.y"
    {(yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 116:

/* Line 1464 of yacc.c  */
#line 256 "poet_yacc.y"
    { (yyval).ptr = negate_Iconst((yyvsp[(2) - (2)]).ptr); }
    break;

  case 117:

/* Line 1464 of yacc.c  */
#line 257 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 118:

/* Line 1464 of yacc.c  */
#line 258 "poet_yacc.y"
    { (yyval).ptr = make_typeNot((yyvsp[(2) - (2)])); }
    break;

  case 119:

/* Line 1464 of yacc.c  */
#line 259 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 120:

/* Line 1464 of yacc.c  */
#line 259 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(3) - (4)]).ptr; }
    break;

  case 121:

/* Line 1464 of yacc.c  */
#line 261 "poet_yacc.y"
    {(yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 122:

/* Line 1464 of yacc.c  */
#line 262 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 123:

/* Line 1464 of yacc.c  */
#line 263 "poet_yacc.y"
    { (yyval).ptr = make_typeTor((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 265 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 125:

/* Line 1464 of yacc.c  */
#line 265 "poet_yacc.y"
    { (yyval).ptr = ((yyvsp[(3) - (3)]).ptr==0)? (yyvsp[(1) - (3)]).ptr : make_typelist2((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 126:

/* Line 1464 of yacc.c  */
#line 266 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 127:

/* Line 1464 of yacc.c  */
#line 267 "poet_yacc.y"
    {(yyval).ptr = make_typeUop(TYPE_LIST1, (yyvsp[(1) - (2)]).ptr); }
    break;

  case 128:

/* Line 1464 of yacc.c  */
#line 268 "poet_yacc.y"
    {(yyval).ptr = make_typeUop(TYPE_LIST, (yyvsp[(1) - (2)]).ptr); }
    break;

  case 129:

/* Line 1464 of yacc.c  */
#line 269 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 130:

/* Line 1464 of yacc.c  */
#line 270 "poet_yacc.y"
    {(yyval).ptr = make_sourceBop(POET_OP_RANGE, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr);}
    break;

  case 131:

/* Line 1464 of yacc.c  */
#line 271 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 132:

/* Line 1464 of yacc.c  */
#line 272 "poet_yacc.y"
    { (yyval).ptr=make_sourceBop(POET_OP_PLUS, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 133:

/* Line 1464 of yacc.c  */
#line 273 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 134:

/* Line 1464 of yacc.c  */
#line 274 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MULT,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 135:

/* Line 1464 of yacc.c  */
#line 275 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 136:

/* Line 1464 of yacc.c  */
#line 276 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_DIVIDE,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 137:

/* Line 1464 of yacc.c  */
#line 277 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 138:

/* Line 1464 of yacc.c  */
#line 277 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MOD,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 139:

/* Line 1464 of yacc.c  */
#line 278 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 140:

/* Line 1464 of yacc.c  */
#line 278 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_CONS,(yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 141:

/* Line 1464 of yacc.c  */
#line 280 "poet_yacc.y"
    {(yyval).ptr=0; }
    break;

  case 142:

/* Line 1464 of yacc.c  */
#line 281 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 143:

/* Line 1464 of yacc.c  */
#line 281 "poet_yacc.y"
    { (yyval).ptr = make_typelist2((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 144:

/* Line 1464 of yacc.c  */
#line 282 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 145:

/* Line 1464 of yacc.c  */
#line 283 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector2((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 146:

/* Line 1464 of yacc.c  */
#line 284 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 147:

/* Line 1464 of yacc.c  */
#line 284 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 148:

/* Line 1464 of yacc.c  */
#line 286 "poet_yacc.y"
    {(yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 149:

/* Line 1464 of yacc.c  */
#line 287 "poet_yacc.y"
    { (yyval).ptr = negate_Iconst((yyvsp[(2) - (2)]).ptr); }
    break;

  case 150:

/* Line 1464 of yacc.c  */
#line 288 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_CLEAR, make_varRef((yyvsp[(2) - (2)]).ptr,ASSIGN_VAR)); }
    break;

  case 151:

/* Line 1464 of yacc.c  */
#line 289 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 152:

/* Line 1464 of yacc.c  */
#line 290 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 153:

/* Line 1464 of yacc.c  */
#line 290 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(3) - (4)]).ptr; }
    break;

  case 154:

/* Line 1464 of yacc.c  */
#line 291 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 155:

/* Line 1464 of yacc.c  */
#line 292 "poet_yacc.y"
    { (yyval).ptr = make_typeTor((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 156:

/* Line 1464 of yacc.c  */
#line 293 "poet_yacc.y"
    {(yyval).ptr = make_typeUop(TYPE_LIST, (yyvsp[(1) - (2)]).ptr); }
    break;

  case 157:

/* Line 1464 of yacc.c  */
#line 294 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 158:

/* Line 1464 of yacc.c  */
#line 295 "poet_yacc.y"
    {(yyval).ptr = make_sourceBop(POET_OP_RANGE, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr);}
    break;

  case 159:

/* Line 1464 of yacc.c  */
#line 296 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 160:

/* Line 1464 of yacc.c  */
#line 297 "poet_yacc.y"
    { (yyval).ptr=make_sourceBop(POET_OP_PLUS, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 161:

/* Line 1464 of yacc.c  */
#line 298 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 162:

/* Line 1464 of yacc.c  */
#line 299 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MULT,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 163:

/* Line 1464 of yacc.c  */
#line 300 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 164:

/* Line 1464 of yacc.c  */
#line 301 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_DIVIDE,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 165:

/* Line 1464 of yacc.c  */
#line 302 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 166:

/* Line 1464 of yacc.c  */
#line 303 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MOD,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 167:

/* Line 1464 of yacc.c  */
#line 304 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 168:

/* Line 1464 of yacc.c  */
#line 304 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_CONS,(yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 169:

/* Line 1464 of yacc.c  */
#line 306 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 170:

/* Line 1464 of yacc.c  */
#line 306 "poet_yacc.y"
    { (yyval).ptr = ((yyvsp[(3) - (3)]).ptr==0)? (yyvsp[(1) - (3)]).ptr : make_typelist2((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 171:

/* Line 1464 of yacc.c  */
#line 307 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 172:

/* Line 1464 of yacc.c  */
#line 308 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 173:

/* Line 1464 of yacc.c  */
#line 308 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign(make_varRef((yyvsp[(1) - (4)]).ptr,ASSIGN_VAR), (yyvsp[(4) - (4)]).ptr); }
    break;

  case 174:

/* Line 1464 of yacc.c  */
#line 310 "poet_yacc.y"
    {(yyval).ptr=0; }
    break;

  case 175:

/* Line 1464 of yacc.c  */
#line 311 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 176:

/* Line 1464 of yacc.c  */
#line 311 "poet_yacc.y"
    { (yyval).ptr = make_typelist2((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 177:

/* Line 1464 of yacc.c  */
#line 312 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 178:

/* Line 1464 of yacc.c  */
#line 313 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector2((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 179:

/* Line 1464 of yacc.c  */
#line 314 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 180:

/* Line 1464 of yacc.c  */
#line 314 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 181:

/* Line 1464 of yacc.c  */
#line 316 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 182:

/* Line 1464 of yacc.c  */
#line 317 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 183:

/* Line 1464 of yacc.c  */
#line 318 "poet_yacc.y"
    { (yyval).ptr= make_empty(); }
    break;

  case 184:

/* Line 1464 of yacc.c  */
#line 320 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config; }
    break;

  case 185:

/* Line 1464 of yacc.c  */
#line 320 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (4)]).config;}
    break;

  case 186:

/* Line 1464 of yacc.c  */
#line 320 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_TUPLE, make_typelist2((yyvsp[(4) - (7)]).ptr,(yyvsp[(6) - (7)]).ptr)); }
    break;

  case 187:

/* Line 1464 of yacc.c  */
#line 321 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config; }
    break;

  case 188:

/* Line 1464 of yacc.c  */
#line 321 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 189:

/* Line 1464 of yacc.c  */
#line 321 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_LIST,(yyvsp[(4) - (8)]).ptr,(yyvsp[(7) - (8)]).ptr); }
    break;

  case 190:

/* Line 1464 of yacc.c  */
#line 322 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config; }
    break;

  case 191:

/* Line 1464 of yacc.c  */
#line 322 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 192:

/* Line 1464 of yacc.c  */
#line 322 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_LIST1,(yyvsp[(4) - (8)]).ptr,(yyvsp[(7) - (8)]).ptr); }
    break;

  case 193:

/* Line 1464 of yacc.c  */
#line 324 "poet_yacc.y"
    {(yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 194:

/* Line 1464 of yacc.c  */
#line 325 "poet_yacc.y"
    { (yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 195:

/* Line 1464 of yacc.c  */
#line 326 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 196:

/* Line 1464 of yacc.c  */
#line 327 "poet_yacc.y"
    { (yyval).ptr = make_typeTor((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 197:

/* Line 1464 of yacc.c  */
#line 328 "poet_yacc.y"
    {(yyval).ptr = make_typeUop(TYPE_LIST1, (yyvsp[(1) - (2)]).ptr); }
    break;

  case 198:

/* Line 1464 of yacc.c  */
#line 329 "poet_yacc.y"
    {(yyval).ptr = make_typeUop(TYPE_LIST, (yyvsp[(1) - (2)]).ptr); }
    break;

  case 199:

/* Line 1464 of yacc.c  */
#line 331 "poet_yacc.y"
    { (yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 200:

/* Line 1464 of yacc.c  */
#line 332 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 201:

/* Line 1464 of yacc.c  */
#line 333 "poet_yacc.y"
    { (yyval).ptr = make_typeNot((yyvsp[(2) - (2)])); }
    break;

  case 202:

/* Line 1464 of yacc.c  */
#line 334 "poet_yacc.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 203:

/* Line 1464 of yacc.c  */
#line 335 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 204:

/* Line 1464 of yacc.c  */
#line 335 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign(make_varRef((yyvsp[(1) - (4)]).ptr,ASSIGN_VAR), (yyvsp[(4) - (4)]).ptr); }
    break;

  case 205:

/* Line 1464 of yacc.c  */
#line 336 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 206:

/* Line 1464 of yacc.c  */
#line 337 "poet_yacc.y"
    { (yyval).ptr = make_typeTor((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 207:

/* Line 1464 of yacc.c  */
#line 338 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 208:

/* Line 1464 of yacc.c  */
#line 338 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(3) - (4)]).ptr; }
    break;

  case 209:

/* Line 1464 of yacc.c  */
#line 340 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 210:

/* Line 1464 of yacc.c  */
#line 340 "poet_yacc.y"
    { (yyval).ptr = ((yyvsp[(3) - (3)]).ptr==0)? (yyvsp[(1) - (3)]).ptr : make_typelist2((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 211:

/* Line 1464 of yacc.c  */
#line 341 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 212:

/* Line 1464 of yacc.c  */
#line 342 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 213:

/* Line 1464 of yacc.c  */
#line 343 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector2((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 214:

/* Line 1464 of yacc.c  */
#line 344 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 215:

/* Line 1464 of yacc.c  */
#line 344 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 216:

/* Line 1464 of yacc.c  */
#line 345 "poet_yacc.y"
    {(yyval).ptr=0; }
    break;

  case 217:

/* Line 1464 of yacc.c  */
#line 346 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 218:

/* Line 1464 of yacc.c  */
#line 346 "poet_yacc.y"
    { (yyval).ptr = make_typelist2((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 219:

/* Line 1464 of yacc.c  */
#line 348 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 220:

/* Line 1464 of yacc.c  */
#line 349 "poet_yacc.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 221:

/* Line 1464 of yacc.c  */
#line 351 "poet_yacc.y"
    {yaccState=YACC_CODE; (yyval).config=ID_DEFAULT;}
    break;

  case 222:

/* Line 1464 of yacc.c  */
#line 351 "poet_yacc.y"
    { yaccState=YACC_DEFAULT; set_code_def((yyvsp[(0) - (4)]).ptr,(yyvsp[(3) - (4)]).ptr); }
    break;

  case 223:

/* Line 1464 of yacc.c  */
#line 352 "poet_yacc.y"
    { set_code_def((yyvsp[(0) - (1)]).ptr, 0); }
    break;

  case 224:

/* Line 1464 of yacc.c  */
#line 353 "poet_yacc.y"
    {(yyval).config=ID_DEFAULT; }
    break;

  case 225:

/* Line 1464 of yacc.c  */
#line 353 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(3) - (4)]).ptr; }
    break;

  case 226:

/* Line 1464 of yacc.c  */
#line 354 "poet_yacc.y"
    {(yyval).ptr = 0; }
    break;

  case 228:

/* Line 1464 of yacc.c  */
#line 355 "poet_yacc.y"
    {(yyval)=(yyvsp[(0) - (1)]); }
    break;

  case 230:

/* Line 1464 of yacc.c  */
#line 356 "poet_yacc.y"
    {set_xform_params((yyvsp[(0) - (5)]).ptr,(yyvsp[(4) - (5)]).ptr); }
    break;

  case 231:

/* Line 1464 of yacc.c  */
#line 357 "poet_yacc.y"
    {(yyval).ptr=0;}
    break;

  case 233:

/* Line 1464 of yacc.c  */
#line 358 "poet_yacc.y"
    {(yyval).config=CODE_VAR;}
    break;

  case 234:

/* Line 1464 of yacc.c  */
#line 358 "poet_yacc.y"
    { (yyval).ptr = set_local_static((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr,LVAR_TUNE,0,1); }
    break;

  case 235:

/* Line 1464 of yacc.c  */
#line 359 "poet_yacc.y"
    { (yyval).ptr=make_localPar((yyvsp[(1) - (1)]).ptr,0,LVAR_XFORMPAR); }
    break;

  case 236:

/* Line 1464 of yacc.c  */
#line 360 "poet_yacc.y"
    {(yyval).config=CODE_VAR;}
    break;

  case 237:

/* Line 1464 of yacc.c  */
#line 360 "poet_yacc.y"
    { (yyval).ptr = make_localPar((yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr,LVAR_XFORMPAR); }
    break;

  case 238:

/* Line 1464 of yacc.c  */
#line 361 "poet_yacc.y"
    {(yyval).ptr = make_sourceVector((yyvsp[(1) - (3)]).ptr, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 239:

/* Line 1464 of yacc.c  */
#line 362 "poet_yacc.y"
    {(yyval).ptr = ((char*)(yyvsp[(0) - (2)]).ptr) + 1; }
    break;

  case 242:

/* Line 1464 of yacc.c  */
#line 363 "poet_yacc.y"
    { set_local_static((yyvsp[(1) - (1)]).ptr,make_Iconst1((yyvsp[(0) - (1)]).ptr), LVAR_OUTPUT,0,1); }
    break;

  case 243:

/* Line 1464 of yacc.c  */
#line 364 "poet_yacc.y"
    {(yyval).config=CODE_VAR;}
    break;

  case 244:

/* Line 1464 of yacc.c  */
#line 364 "poet_yacc.y"
    { set_local_static((yyvsp[(1) - (4)]).ptr,make_Iconst1((yyvsp[(0) - (4)]).ptr), LVAR_OUTPUT,(yyvsp[(4) - (4)]).ptr,1); }
    break;

  case 245:

/* Line 1464 of yacc.c  */
#line 365 "poet_yacc.y"
    { (yyval).ptr = make_traceVar((yyvsp[(1) - (1)]).ptr,0); }
    break;

  case 246:

/* Line 1464 of yacc.c  */
#line 366 "poet_yacc.y"
    { (yyval).ptr = make_traceVar((yyvsp[(1) - (3)]).ptr,0); eval_define((yyval).ptr,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 247:

/* Line 1464 of yacc.c  */
#line 368 "poet_yacc.y"
    { (yyval).ptr = make_traceVar((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr);  }
    break;

  case 248:

/* Line 1464 of yacc.c  */
#line 370 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 249:

/* Line 1464 of yacc.c  */
#line 370 "poet_yacc.y"
    { (yyval).ptr=make_seq((yyvsp[(1) - (3)]).ptr, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 250:

/* Line 1464 of yacc.c  */
#line 371 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 251:

/* Line 1464 of yacc.c  */
#line 372 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 252:

/* Line 1464 of yacc.c  */
#line 373 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 253:

/* Line 1464 of yacc.c  */
#line 373 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 254:

/* Line 1464 of yacc.c  */
#line 373 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (7)]).config;}
    break;

  case 255:

/* Line 1464 of yacc.c  */
#line 374 "poet_yacc.y"
    { (yyval).ptr = make_ifElse((yyvsp[(4) - (9)]).ptr, (yyvsp[(7) - (9)]).ptr, (yyvsp[(9) - (9)]).ptr); }
    break;

  case 256:

/* Line 1464 of yacc.c  */
#line 375 "poet_yacc.y"
    { (yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 257:

/* Line 1464 of yacc.c  */
#line 376 "poet_yacc.y"
    { (yyval).ptr=make_empty(); }
    break;

  case 258:

/* Line 1464 of yacc.c  */
#line 377 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 259:

/* Line 1464 of yacc.c  */
#line 377 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(3) - (3)]).ptr; }
    break;

  case 260:

/* Line 1464 of yacc.c  */
#line 378 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 261:

/* Line 1464 of yacc.c  */
#line 379 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 262:

/* Line 1464 of yacc.c  */
#line 380 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (8)]).config;}
    break;

  case 263:

/* Line 1464 of yacc.c  */
#line 380 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (11)]).config;}
    break;

  case 264:

/* Line 1464 of yacc.c  */
#line 381 "poet_yacc.y"
    { (yyval).ptr = make_sourceQop(POET_OP_FOR, (yyvsp[(4) - (13)]).ptr,(yyvsp[(7) - (13)]).ptr,(yyvsp[(10) - (13)]).ptr,(yyvsp[(13) - (13)]).ptr); }
    break;

  case 265:

/* Line 1464 of yacc.c  */
#line 382 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 266:

/* Line 1464 of yacc.c  */
#line 382 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (6)]).config;}
    break;

  case 267:

/* Line 1464 of yacc.c  */
#line 383 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_CASE, (yyvsp[(4) - (9)]).ptr,(yyvsp[(8) - (9)]).ptr); }
    break;

  case 268:

/* Line 1464 of yacc.c  */
#line 384 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 269:

/* Line 1464 of yacc.c  */
#line 385 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 270:

/* Line 1464 of yacc.c  */
#line 386 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (8)]).config;}
    break;

  case 271:

/* Line 1464 of yacc.c  */
#line 386 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (11)]).config;}
    break;

  case 272:

/* Line 1464 of yacc.c  */
#line 387 "poet_yacc.y"
    { (yyval).ptr = make_sourceQop(POET_OP_FOREACH, (yyvsp[(4) - (13)]).ptr,(yyvsp[(7) - (13)]).ptr,(yyvsp[(10) - (13)]).ptr,(yyvsp[(13) - (13)]).ptr); }
    break;

  case 273:

/* Line 1464 of yacc.c  */
#line 388 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 274:

/* Line 1464 of yacc.c  */
#line 389 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 275:

/* Line 1464 of yacc.c  */
#line 390 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (8)]).config;}
    break;

  case 276:

/* Line 1464 of yacc.c  */
#line 390 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (11)]).config;}
    break;

  case 277:

/* Line 1464 of yacc.c  */
#line 391 "poet_yacc.y"
    { (yyval).ptr = make_sourceQop(POET_OP_FOREACHR, (yyvsp[(4) - (13)]).ptr,(yyvsp[(7) - (13)]).ptr,(yyvsp[(10) - (13)]).ptr,(yyvsp[(13) - (13)]).ptr); }
    break;

  case 278:

/* Line 1464 of yacc.c  */
#line 392 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 279:

/* Line 1464 of yacc.c  */
#line 392 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(3) - (4)]).ptr; }
    break;

  case 280:

/* Line 1464 of yacc.c  */
#line 393 "poet_yacc.y"
    { (yyval).ptr=make_empty(); }
    break;

  case 281:

/* Line 1464 of yacc.c  */
#line 394 "poet_yacc.y"
    { (yyval).ptr = make_sourceStmt((yyvsp[(1) - (2)]).ptr); }
    break;

  case 282:

/* Line 1464 of yacc.c  */
#line 396 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 283:

/* Line 1464 of yacc.c  */
#line 397 "poet_yacc.y"
    { (yyval).ptr = make_empty(); }
    break;

  case 284:

/* Line 1464 of yacc.c  */
#line 399 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 285:

/* Line 1464 of yacc.c  */
#line 399 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 286:

/* Line 1464 of yacc.c  */
#line 400 "poet_yacc.y"
    {(yyval).ptr = make_sourceVector( (yyvsp[(1) - (7)]).ptr, make_inputlist2((yyvsp[(4) - (7)]).ptr,(yyvsp[(7) - (7)]).ptr)); }
    break;

  case 287:

/* Line 1464 of yacc.c  */
#line 401 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 288:

/* Line 1464 of yacc.c  */
#line 401 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (4)]).config;}
    break;

  case 289:

/* Line 1464 of yacc.c  */
#line 401 "poet_yacc.y"
    { (yyval).ptr = make_inputlist2((yyvsp[(3) - (6)]).ptr,(yyvsp[(6) - (6)]).ptr); }
    break;

  case 290:

/* Line 1464 of yacc.c  */
#line 402 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (3)]).config;}
    break;

  case 291:

/* Line 1464 of yacc.c  */
#line 403 "poet_yacc.y"
    {(yyval).ptr = make_sourceVector( (yyvsp[(1) - (5)]).ptr, make_inputlist2(make_any(),(yyvsp[(5) - (5)]).ptr)); }
    break;

  case 292:

/* Line 1464 of yacc.c  */
#line 405 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 293:

/* Line 1464 of yacc.c  */
#line 406 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 294:

/* Line 1464 of yacc.c  */
#line 408 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 295:

/* Line 1464 of yacc.c  */
#line 408 "poet_yacc.y"
    {(yyval).ptr = make_sourceVector((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 296:

/* Line 1464 of yacc.c  */
#line 409 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 297:

/* Line 1464 of yacc.c  */
#line 409 "poet_yacc.y"
    { (yyval).ptr = make_sourceVector2((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 298:

/* Line 1464 of yacc.c  */
#line 412 "poet_yacc.y"
    { (yyval).ptr=make_dummyOperator(POET_OP_CONTINUE); }
    break;

  case 299:

/* Line 1464 of yacc.c  */
#line 413 "poet_yacc.y"
    { (yyval).ptr=make_dummyOperator(POET_OP_BREAK); }
    break;

  case 300:

/* Line 1464 of yacc.c  */
#line 414 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 301:

/* Line 1464 of yacc.c  */
#line 414 "poet_yacc.y"
    { (yyval).ptr=make_sourceUop(POET_OP_RETURN, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 302:

/* Line 1464 of yacc.c  */
#line 415 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 303:

/* Line 1464 of yacc.c  */
#line 415 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_ERROR,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 304:

/* Line 1464 of yacc.c  */
#line 416 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 305:

/* Line 1464 of yacc.c  */
#line 416 "poet_yacc.y"
    { (yyval).ptr=make_sourceUop(POET_OP_PRINT,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 306:

/* Line 1464 of yacc.c  */
#line 417 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 307:

/* Line 1464 of yacc.c  */
#line 419 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 308:

/* Line 1464 of yacc.c  */
#line 420 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 309:

/* Line 1464 of yacc.c  */
#line 420 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign((yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 310:

/* Line 1464 of yacc.c  */
#line 421 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 311:

/* Line 1464 of yacc.c  */
#line 421 "poet_yacc.y"
    { (yyval).ptr = make_codeMatch((yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 312:

/* Line 1464 of yacc.c  */
#line 422 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 313:

/* Line 1464 of yacc.c  */
#line 423 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign((yyvsp[(1) - (4)]).ptr, make_sourceBop(POET_OP_PLUS, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr)); }
    break;

  case 314:

/* Line 1464 of yacc.c  */
#line 424 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 315:

/* Line 1464 of yacc.c  */
#line 425 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign((yyvsp[(1) - (4)]).ptr, make_sourceBop(POET_OP_MINUS, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr)); }
    break;

  case 316:

/* Line 1464 of yacc.c  */
#line 426 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 317:

/* Line 1464 of yacc.c  */
#line 427 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign((yyvsp[(1) - (4)]).ptr, make_sourceBop(POET_OP_MULT, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr)); }
    break;

  case 318:

/* Line 1464 of yacc.c  */
#line 428 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 319:

/* Line 1464 of yacc.c  */
#line 429 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign((yyvsp[(1) - (4)]).ptr, make_sourceBop(POET_OP_DIVIDE, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr)); }
    break;

  case 320:

/* Line 1464 of yacc.c  */
#line 430 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 321:

/* Line 1464 of yacc.c  */
#line 431 "poet_yacc.y"
    { (yyval).ptr=make_sourceAssign((yyvsp[(1) - (4)]).ptr, make_sourceBop(POET_OP_MOD, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr)); }
    break;

  case 322:

/* Line 1464 of yacc.c  */
#line 432 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 323:

/* Line 1464 of yacc.c  */
#line 432 "poet_yacc.y"
    { (yyval).ptr = make_codeMatchQ((yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 324:

/* Line 1464 of yacc.c  */
#line 433 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 325:

/* Line 1464 of yacc.c  */
#line 433 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 326:

/* Line 1464 of yacc.c  */
#line 434 "poet_yacc.y"
    { (yyval).ptr = make_ifElse( (yyvsp[(1) - (7)]).ptr, (yyvsp[(4) - (7)]).ptr, (yyvsp[(7) - (7)]).ptr); }
    break;

  case 327:

/* Line 1464 of yacc.c  */
#line 437 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 328:

/* Line 1464 of yacc.c  */
#line 437 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_AND,  (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 329:

/* Line 1464 of yacc.c  */
#line 438 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 330:

/* Line 1464 of yacc.c  */
#line 438 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_OR,   (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 331:

/* Line 1464 of yacc.c  */
#line 439 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 332:

/* Line 1464 of yacc.c  */
#line 439 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_NOT, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 333:

/* Line 1464 of yacc.c  */
#line 440 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 334:

/* Line 1464 of yacc.c  */
#line 440 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_LT, (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 335:

/* Line 1464 of yacc.c  */
#line 441 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 336:

/* Line 1464 of yacc.c  */
#line 441 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_LE, (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 337:

/* Line 1464 of yacc.c  */
#line 442 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 338:

/* Line 1464 of yacc.c  */
#line 442 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_EQ, (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 339:

/* Line 1464 of yacc.c  */
#line 443 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 340:

/* Line 1464 of yacc.c  */
#line 444 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_GT,   (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 341:

/* Line 1464 of yacc.c  */
#line 445 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 342:

/* Line 1464 of yacc.c  */
#line 446 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_GE,   (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 343:

/* Line 1464 of yacc.c  */
#line 447 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 344:

/* Line 1464 of yacc.c  */
#line 448 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_NE,   (yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr); }
    break;

  case 345:

/* Line 1464 of yacc.c  */
#line 449 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 346:

/* Line 1464 of yacc.c  */
#line 449 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_ASTMATCH, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 347:

/* Line 1464 of yacc.c  */
#line 450 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 348:

/* Line 1464 of yacc.c  */
#line 452 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 349:

/* Line 1464 of yacc.c  */
#line 453 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 350:

/* Line 1464 of yacc.c  */
#line 454 "poet_yacc.y"
    { (yyval).ptr=make_sourceBop(POET_OP_PLUS, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 351:

/* Line 1464 of yacc.c  */
#line 455 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 352:

/* Line 1464 of yacc.c  */
#line 456 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MINUS,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 353:

/* Line 1464 of yacc.c  */
#line 457 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 354:

/* Line 1464 of yacc.c  */
#line 458 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MULT,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 355:

/* Line 1464 of yacc.c  */
#line 459 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 356:

/* Line 1464 of yacc.c  */
#line 460 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_CONCAT, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 357:

/* Line 1464 of yacc.c  */
#line 461 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 358:

/* Line 1464 of yacc.c  */
#line 462 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_DIVIDE,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 359:

/* Line 1464 of yacc.c  */
#line 463 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 360:

/* Line 1464 of yacc.c  */
#line 464 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_MOD,  (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 361:

/* Line 1464 of yacc.c  */
#line 465 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 362:

/* Line 1464 of yacc.c  */
#line 465 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_CONS,(yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 363:

/* Line 1464 of yacc.c  */
#line 466 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 364:

/* Line 1464 of yacc.c  */
#line 466 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_UMINUS,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 365:

/* Line 1464 of yacc.c  */
#line 468 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 366:

/* Line 1464 of yacc.c  */
#line 469 "poet_yacc.y"
    { if ((yyvsp[(3) - (3)]).ptr==0) (yyval).ptr=(yyvsp[(1) - (3)]).ptr; else (yyval).ptr = make_xformList((yyvsp[(1) - (3)]).ptr, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 367:

/* Line 1464 of yacc.c  */
#line 471 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 368:

/* Line 1464 of yacc.c  */
#line 471 "poet_yacc.y"
    { (yyval).ptr =make_xformList((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr);}
    break;

  case 369:

/* Line 1464 of yacc.c  */
#line 472 "poet_yacc.y"
    { (yyval).ptr=0; }
    break;

  case 370:

/* Line 1464 of yacc.c  */
#line 473 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 371:

/* Line 1464 of yacc.c  */
#line 473 "poet_yacc.y"
    { (yyval).ptr =make_xformList((yyvsp[(1) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr);}
    break;

  case 372:

/* Line 1464 of yacc.c  */
#line 474 "poet_yacc.y"
    { (yyval).ptr=0; }
    break;

  case 373:

/* Line 1464 of yacc.c  */
#line 476 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 374:

/* Line 1464 of yacc.c  */
#line 476 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 375:

/* Line 1464 of yacc.c  */
#line 476 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (7)]).config;}
    break;

  case 376:

/* Line 1464 of yacc.c  */
#line 477 "poet_yacc.y"
    { (yyval).ptr=((yyvsp[(9) - (9)]).ptr==0)? make_sourceBop(POET_OP_REPLACE,(yyvsp[(4) - (9)]).ptr,(yyvsp[(7) - (9)]).ptr) 
                       : make_sourceTop(POET_OP_REPLACE,(yyvsp[(4) - (9)]).ptr,(yyvsp[(7) - (9)]).ptr,(yyvsp[(9) - (9)]).ptr); }
    break;

  case 377:

/* Line 1464 of yacc.c  */
#line 479 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 378:

/* Line 1464 of yacc.c  */
#line 479 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (4)]).config;}
    break;

  case 379:

/* Line 1464 of yacc.c  */
#line 480 "poet_yacc.y"
    { (yyval).ptr=((yyvsp[(6) - (6)]).ptr==0)? make_sourceUop(POET_OP_TRACE,(yyvsp[(4) - (6)]).ptr)
                      : make_sourceBop(POET_OP_TRACE,(yyvsp[(4) - (6)]).ptr,(yyvsp[(6) - (6)]).ptr); }
    break;

  case 380:

/* Line 1464 of yacc.c  */
#line 482 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 381:

/* Line 1464 of yacc.c  */
#line 482 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 382:

/* Line 1464 of yacc.c  */
#line 483 "poet_yacc.y"
    { (yyval).ptr=make_sourceBop(POET_OP_PERMUTE,(yyvsp[(4) - (8)]).ptr,(yyvsp[(7) - (8)]).ptr); }
    break;

  case 383:

/* Line 1464 of yacc.c  */
#line 484 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 384:

/* Line 1464 of yacc.c  */
#line 484 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 385:

/* Line 1464 of yacc.c  */
#line 484 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (8)]).config;}
    break;

  case 386:

/* Line 1464 of yacc.c  */
#line 485 "poet_yacc.y"
    { (yyval).ptr=make_sourceTop(POET_OP_DUPLICATE,(yyvsp[(4) - (11)]).ptr,(yyvsp[(7) - (11)]).ptr,(yyvsp[(10) - (11)]).ptr); }
    break;

  case 387:

/* Line 1464 of yacc.c  */
#line 486 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 388:

/* Line 1464 of yacc.c  */
#line 486 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_COPY, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 389:

/* Line 1464 of yacc.c  */
#line 487 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 390:

/* Line 1464 of yacc.c  */
#line 488 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (4)]).config;(yyval).ptr=(yyvsp[(4) - (4)]).ptr; }
    break;

  case 391:

/* Line 1464 of yacc.c  */
#line 489 "poet_yacc.y"
    {(yyval).ptr = make_sourceBop(POET_OP_ERASE,(yyvsp[(4) - (6)]).ptr,(yyvsp[(6) - (6)]).ptr); }
    break;

  case 392:

/* Line 1464 of yacc.c  */
#line 490 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 393:

/* Line 1464 of yacc.c  */
#line 490 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_DELAY, (yyvsp[(4) - (5)]).ptr); }
    break;

  case 394:

/* Line 1464 of yacc.c  */
#line 491 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 395:

/* Line 1464 of yacc.c  */
#line 491 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 396:

/* Line 1464 of yacc.c  */
#line 492 "poet_yacc.y"
    {  (yyval).ptr=make_sourceBop(POET_OP_INSERT, (yyvsp[(4) - (8)]).ptr,(yyvsp[(7) - (8)]).ptr); }
    break;

  case 397:

/* Line 1464 of yacc.c  */
#line 493 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (3)]).config;}
    break;

  case 398:

/* Line 1464 of yacc.c  */
#line 494 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_DEBUG,(yyvsp[(2) - (6)]).ptr,(yyvsp[(5) - (6)]).ptr); }
    break;

  case 399:

/* Line 1464 of yacc.c  */
#line 495 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 400:

/* Line 1464 of yacc.c  */
#line 495 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_APPLY, (yyvsp[(4) - (5)]).ptr); }
    break;

  case 401:

/* Line 1464 of yacc.c  */
#line 496 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 402:

/* Line 1464 of yacc.c  */
#line 496 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_REBUILD,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 403:

/* Line 1464 of yacc.c  */
#line 497 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 404:

/* Line 1464 of yacc.c  */
#line 497 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_RESTORE,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 405:

/* Line 1464 of yacc.c  */
#line 498 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 406:

/* Line 1464 of yacc.c  */
#line 498 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_SAVE,(yyvsp[(3) - (3)]).ptr); }
    break;

  case 407:

/* Line 1464 of yacc.c  */
#line 499 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 408:

/* Line 1464 of yacc.c  */
#line 499 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (5)]).config;}
    break;

  case 409:

/* Line 1464 of yacc.c  */
#line 500 "poet_yacc.y"
    { (yyval).ptr = make_sourceBop(POET_OP_SPLIT, (yyvsp[(4) - (8)]).ptr,(yyvsp[(7) - (8)]).ptr); }
    break;

  case 410:

/* Line 1464 of yacc.c  */
#line 501 "poet_yacc.y"
    {(yyval).ptr = make_typeUop(TYPE_LIST, (yyvsp[(1) - (2)]).ptr); }
    break;

  case 411:

/* Line 1464 of yacc.c  */
#line 502 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 412:

/* Line 1464 of yacc.c  */
#line 503 "poet_yacc.y"
    {(yyval).ptr = make_sourceBop(POET_OP_RANGE, (yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr);}
    break;

  case 413:

/* Line 1464 of yacc.c  */
#line 505 "poet_yacc.y"
    {(yyval).ptr = make_sourceBop(POET_OP_RANGE, (yyvsp[(3) - (6)]).ptr,(yyvsp[(6) - (6)]).ptr);}
    break;

  case 414:

/* Line 1464 of yacc.c  */
#line 506 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 415:

/* Line 1464 of yacc.c  */
#line 507 "poet_yacc.y"
    { (yyval).ptr = 0; }
    break;

  case 416:

/* Line 1464 of yacc.c  */
#line 507 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 417:

/* Line 1464 of yacc.c  */
#line 507 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(3) - (4)]).ptr; }
    break;

  case 418:

/* Line 1464 of yacc.c  */
#line 509 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 419:

/* Line 1464 of yacc.c  */
#line 509 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(3) - (4)]).ptr; }
    break;

  case 420:

/* Line 1464 of yacc.c  */
#line 510 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(0) - (1)]).ptr; }
    break;

  case 421:

/* Line 1464 of yacc.c  */
#line 512 "poet_yacc.y"
    {(yyval).ptr = make_Iconst1(1); }
    break;

  case 422:

/* Line 1464 of yacc.c  */
#line 513 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(2) - (3)]).ptr; }
    break;

  case 423:

/* Line 1464 of yacc.c  */
#line 515 "poet_yacc.y"
    { (yyval).ptr = make_inputlist2((yyvsp[(1) - (2)]).ptr,(yyvsp[(2) - (2)]).ptr); }
    break;

  case 424:

/* Line 1464 of yacc.c  */
#line 517 "poet_yacc.y"
    { (yyval).ptr = 0; }
    break;

  case 425:

/* Line 1464 of yacc.c  */
#line 518 "poet_yacc.y"
    { (yyval).ptr = make_inputlist2((yyvsp[(1) - (2)]).ptr,(yyvsp[(2) - (2)]).ptr); }
    break;

  case 426:

/* Line 1464 of yacc.c  */
#line 521 "poet_yacc.y"
    { (yyval).ptr = ((yyvsp[(2) - (2)]).ptr==0)? (yyvsp[(1) - (2)]).ptr : make_inputlist2((yyvsp[(1) - (2)]).ptr,(yyvsp[(2) - (2)]).ptr); }
    break;

  case 427:

/* Line 1464 of yacc.c  */
#line 521 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(4) - (4)]).ptr; }
    break;

  case 428:

/* Line 1464 of yacc.c  */
#line 522 "poet_yacc.y"
    {(yyval).config=CODE_OR_XFORM_VAR; }
    break;

  case 429:

/* Line 1464 of yacc.c  */
#line 523 "poet_yacc.y"
    { (yyval).ptr = make_annot((yyvsp[(4) - (5)]).ptr); }
    break;

  case 430:

/* Line 1464 of yacc.c  */
#line 525 "poet_yacc.y"
    { (yyval).ptr = 0; }
    break;

  case 431:

/* Line 1464 of yacc.c  */
#line 526 "poet_yacc.y"
    { (yyval).ptr = make_inputlist2((yyvsp[(1) - (2)]).ptr, (yyvsp[(2) - (2)]).ptr); }
    break;

  case 432:

/* Line 1464 of yacc.c  */
#line 527 "poet_yacc.y"
    { (yyval).ptr = make_inputlist2((yyvsp[(1) - (2)]).ptr, (yyvsp[(2) - (2)]).ptr); }
    break;

  case 433:

/* Line 1464 of yacc.c  */
#line 528 "poet_yacc.y"
    { (yyval).ptr = make_inputlist2((yyvsp[(1) - (2)]).ptr, (yyvsp[(2) - (2)]).ptr); }
    break;

  case 434:

/* Line 1464 of yacc.c  */
#line 529 "poet_yacc.y"
    { (yyval).ptr = make_inputlist2(make_varRef((yyvsp[(1) - (2)]).ptr,GLOBAL_VAR), (yyvsp[(2) - (2)]).ptr); }
    break;

  case 435:

/* Line 1464 of yacc.c  */
#line 532 "poet_yacc.y"
    {(yyval).config=CODE_OR_XFORM_VAR;}
    break;

  case 436:

/* Line 1464 of yacc.c  */
#line 532 "poet_yacc.y"
    { (yyval).ptr = make_codeMatch((yyvsp[(0) - (3)]).ptr, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 437:

/* Line 1464 of yacc.c  */
#line 532 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(5) - (5)]).ptr; }
    break;

  case 438:

/* Line 1464 of yacc.c  */
#line 533 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 439:

/* Line 1464 of yacc.c  */
#line 535 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(0) - (0)]).ptr; }
    break;

  case 440:

/* Line 1464 of yacc.c  */
#line 536 "poet_yacc.y"
    {(yyval).config=CODE_OR_XFORM_VAR; }
    break;

  case 441:

/* Line 1464 of yacc.c  */
#line 536 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(0) - (5)]).ptr; }
    break;

  case 442:

/* Line 1464 of yacc.c  */
#line 537 "poet_yacc.y"
    { (yyval).ptr = make_inputlist2(make_annot((yyvsp[(4) - (7)]).ptr), (yyvsp[(7) - (7)]).ptr); }
    break;

  case 443:

/* Line 1464 of yacc.c  */
#line 540 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 444:

/* Line 1464 of yacc.c  */
#line 540 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_CAR, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 445:

/* Line 1464 of yacc.c  */
#line 541 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 446:

/* Line 1464 of yacc.c  */
#line 541 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_CDR, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 447:

/* Line 1464 of yacc.c  */
#line 542 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 448:

/* Line 1464 of yacc.c  */
#line 542 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_LEN, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 449:

/* Line 1464 of yacc.c  */
#line 543 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 450:

/* Line 1464 of yacc.c  */
#line 543 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(TYPE_INT, (yyvsp[(3) - (3)]).ptr); }
    break;

  case 451:

/* Line 1464 of yacc.c  */
#line 544 "poet_yacc.y"
    { (yyval).ptr = make_sourceUop(POET_OP_CLEAR, make_varRef((yyvsp[(2) - (2)]).ptr,ASSIGN_VAR)); }
    break;

  case 452:

/* Line 1464 of yacc.c  */
#line 545 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 453:

/* Line 1464 of yacc.c  */
#line 547 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config;}
    break;

  case 454:

/* Line 1464 of yacc.c  */
#line 547 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(3) - (4)]).ptr; }
    break;

  case 455:

/* Line 1464 of yacc.c  */
#line 548 "poet_yacc.y"
    { (yyval).ptr=(yyvsp[(1) - (1)]).ptr; }
    break;

  case 456:

/* Line 1464 of yacc.c  */
#line 549 "poet_yacc.y"
    { (yyval).ptr = make_any(); }
    break;

  case 457:

/* Line 1464 of yacc.c  */
#line 550 "poet_yacc.y"
    { (yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 458:

/* Line 1464 of yacc.c  */
#line 551 "poet_yacc.y"
    {(yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 459:

/* Line 1464 of yacc.c  */
#line 553 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(1) - (1)]).ptr; (yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 460:

/* Line 1464 of yacc.c  */
#line 553 "poet_yacc.y"
    { (yyval) = (yyvsp[(3) - (3)]); }
    break;

  case 461:

/* Line 1464 of yacc.c  */
#line 554 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config; }
    break;

  case 462:

/* Line 1464 of yacc.c  */
#line 554 "poet_yacc.y"
    { (yyval).ptr = make_typeTor((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 463:

/* Line 1464 of yacc.c  */
#line 555 "poet_yacc.y"
    {(yyval)=(yyvsp[(0) - (0)]); }
    break;

  case 464:

/* Line 1464 of yacc.c  */
#line 556 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; (yyval).ptr=(yyvsp[(0) - (1)]).ptr; }
    break;

  case 465:

/* Line 1464 of yacc.c  */
#line 557 "poet_yacc.y"
    { (yyval).ptr = make_tupleAccess((yyvsp[(0) - (4)]).ptr, (yyvsp[(3) - (4)]).ptr); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 466:

/* Line 1464 of yacc.c  */
#line 558 "poet_yacc.y"
    { (yyval)=(yyvsp[(6) - (6)]); }
    break;

  case 467:

/* Line 1464 of yacc.c  */
#line 559 "poet_yacc.y"
    { (yyval)=(yyvsp[(1) - (1)]); }
    break;

  case 468:

/* Line 1464 of yacc.c  */
#line 561 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 469:

/* Line 1464 of yacc.c  */
#line 562 "poet_yacc.y"
    { (yyval).ptr = make_codeRef((yyvsp[(0) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr,(yyvsp[(0) - (3)]).config); (yyval).config=(yyvsp[(0) - (3)]).config; }
    break;

  case 470:

/* Line 1464 of yacc.c  */
#line 564 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(1) - (1)]).ptr; (yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 471:

/* Line 1464 of yacc.c  */
#line 564 "poet_yacc.y"
    { (yyval) = (yyvsp[(3) - (3)]); }
    break;

  case 472:

/* Line 1464 of yacc.c  */
#line 565 "poet_yacc.y"
    {(yyval)=(yyvsp[(0) - (0)]); }
    break;

  case 473:

/* Line 1464 of yacc.c  */
#line 566 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; (yyval).ptr=(yyvsp[(0) - (1)]).ptr; }
    break;

  case 474:

/* Line 1464 of yacc.c  */
#line 567 "poet_yacc.y"
    { (yyval).ptr = make_tupleAccess((yyvsp[(0) - (4)]).ptr, (yyvsp[(3) - (4)]).ptr); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 475:

/* Line 1464 of yacc.c  */
#line 568 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 476:

/* Line 1464 of yacc.c  */
#line 569 "poet_yacc.y"
    { (yyval).ptr = make_codeRef((yyvsp[(0) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr,(yyvsp[(0) - (3)]).config);(yyval).config=(yyvsp[(0) - (3)]).config; }
    break;

  case 477:

/* Line 1464 of yacc.c  */
#line 571 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(1) - (1)]).ptr; (yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 478:

/* Line 1464 of yacc.c  */
#line 571 "poet_yacc.y"
    { (yyval) = (yyvsp[(3) - (3)]); }
    break;

  case 479:

/* Line 1464 of yacc.c  */
#line 572 "poet_yacc.y"
    {(yyval)=(yyvsp[(0) - (0)]); }
    break;

  case 480:

/* Line 1464 of yacc.c  */
#line 573 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; (yyval).ptr=(yyvsp[(0) - (1)]).ptr; }
    break;

  case 481:

/* Line 1464 of yacc.c  */
#line 574 "poet_yacc.y"
    { (yyval).ptr = make_tupleAccess((yyvsp[(0) - (4)]).ptr, (yyvsp[(3) - (4)]).ptr); (yyval).config=(yyvsp[(0) - (4)]).config; }
    break;

  case 482:

/* Line 1464 of yacc.c  */
#line 575 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (1)]).config; }
    break;

  case 483:

/* Line 1464 of yacc.c  */
#line 576 "poet_yacc.y"
    { (yyval).ptr = make_codeRef((yyvsp[(0) - (3)]).ptr,(yyvsp[(3) - (3)]).ptr,(yyvsp[(0) - (3)]).config);(yyval).config=(yyvsp[(0) - (3)]).config; }
    break;

  case 484:

/* Line 1464 of yacc.c  */
#line 578 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(1) - (1)]).ptr;}
    break;

  case 485:

/* Line 1464 of yacc.c  */
#line 579 "poet_yacc.y"
    { (yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 486:

/* Line 1464 of yacc.c  */
#line 580 "poet_yacc.y"
    {(yyval).ptr = (yyvsp[(1) - (1)]).ptr; }
    break;

  case 487:

/* Line 1464 of yacc.c  */
#line 582 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 488:

/* Line 1464 of yacc.c  */
#line 583 "poet_yacc.y"
    { (yyval).ptr = make_typeTor((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 489:

/* Line 1464 of yacc.c  */
#line 584 "poet_yacc.y"
    { (yyval).ptr = make_attrAccess(make_varRef((yyvsp[(1) - (3)]).ptr,CODE_OR_XFORM_VAR), (yyvsp[(3) - (3)]).ptr); }
    break;

  case 490:

/* Line 1464 of yacc.c  */
#line 587 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 491:

/* Line 1464 of yacc.c  */
#line 588 "poet_yacc.y"
    { (yyval).ptr = make_sourceAssign( make_attrAccess((yyvsp[(0) - (4)]).ptr,(yyvsp[(1) - (4)]).ptr),(yyvsp[(4) - (4)]).ptr); }
    break;

  case 492:

/* Line 1464 of yacc.c  */
#line 589 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(0) - (2)]).ptr; (yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 493:

/* Line 1464 of yacc.c  */
#line 590 "poet_yacc.y"
    { (yyval).ptr = make_seq((yyvsp[(1) - (4)]).ptr,(yyvsp[(4) - (4)]).ptr); }
    break;

  case 494:

/* Line 1464 of yacc.c  */
#line 593 "poet_yacc.y"
    {(yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 495:

/* Line 1464 of yacc.c  */
#line 594 "poet_yacc.y"
    { (yyval).ptr = make_sourceAssign( make_attrAccess((yyvsp[(0) - (4)]).ptr,(yyvsp[(1) - (4)]).ptr),(yyvsp[(4) - (4)]).ptr); }
    break;

  case 496:

/* Line 1464 of yacc.c  */
#line 595 "poet_yacc.y"
    {(yyval).ptr=(yyvsp[(0) - (2)]).ptr; (yyval).config=(yyvsp[(0) - (2)]).config;}
    break;

  case 497:

/* Line 1464 of yacc.c  */
#line 596 "poet_yacc.y"
    { (yyval).ptr = make_seq((yyvsp[(1) - (4)]).ptr, (yyvsp[(4) - (4)]).ptr);}
    break;



/* Line 1464 of yacc.c  */
#line 6133 "poet_yacc.c"
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
      /* If just tried and failed to reuse lookahead token after an
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

  /* Else will try to reuse lookahead token after shifting the error
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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
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



/* Line 1684 of yacc.c  */
#line 597 "poet_yacc.y"


