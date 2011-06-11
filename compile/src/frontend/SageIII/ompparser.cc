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

/* Substitute the variable and function names.  */
#define yyparse omp_parse
#define yylex   omp_lex
#define yyerror omp_error
#define yylval  omp_lval
#define yychar  omp_char
#define yydebug omp_debug
#define yynerrs omp_nerrs


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




/* Copy the first part of user declarations.  */
#line 10 "../../../../src/frontend/SageIII/ompparser.yy"

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include "sage3basic.h" // Sage Interface and Builders
#include "sageBuilder.h"
#include "OmpAttribute.h"
using namespace OmpSupport;

/* Parser - BISON */

/*the scanner function*/
extern int omp_lex(); 

/*A customized initialization function for the scanner, str is the string to be scanned.*/
extern void omp_lexer_init(const char* str);

//! Initialize the parser with the originating SgPragmaDeclaration and its pragma text
extern void omp_parser_init(SgNode* aNode, const char* str);

/*Treat the entire expression as a string for now
  Implemented in the scanner*/
extern void omp_parse_expr();

//The result AST representing the annotation
extern OmpAttribute* getParsedDirective();

static int omp_error(const char*);

//Insert variable into var_list of some clause
static bool addVar(const char* var);

//Insert expression into some clause
static bool addExpression(const char* expr);

// The current AST annotation being built
static OmpAttribute* ompattribute = NULL;

// The current OpenMP construct or clause type which is being parsed
// It is automatically associated with the current ompattribute
// Used to indicate the OpenMP directive or clause to which a variable list or an expression should get added for the current OpenMP pragma being parsed.
static omp_construct_enum omptype = e_unknown;

// The node to which vars/expressions should get added
//static OmpAttribute* omptype = 0;

// The context node with the pragma annotation being parsed
//
// We attach the attribute to the pragma declaration directly for now, 
// A few OpenMP directive does not affect the next structure block
// This variable is set by the prefix_parser_init() before prefix_parse() is called.
//Liao
static SgNode* gNode;

// The current expression node being generated 
static SgExpression* current_exp = NULL;
bool b_within_variable_list  = false; 


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 73 "../../../../src/frontend/SageIII/ompparser.yy"
{  int itype;
          double ftype;
          const char* stype;
          void* ptype; /* For expressions */
        }
/* Line 193 of yacc.c.  */
#line 285 "ompparser.cc"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 298 "ompparser.cc"

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
#define YYFINAL  33
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   396

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  72
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  91
/* YYNRULES -- Number of rules.  */
#define YYNRULES  201
/* YYNRULES -- Number of states.  */
#define YYNSTATES  280

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   313

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    45,     2,
      37,    39,    43,    42,    40,    44,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    41,     2,
      70,    69,    71,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    46,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    47,     2,     2,     2,     2,     2,
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
      35,    36,    38,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    13,    15,    17,
      19,    21,    23,    25,    27,    29,    31,    33,    35,    36,
      41,    42,    44,    46,    49,    53,    55,    57,    59,    61,
      63,    65,    66,    72,    73,    79,    80,    81,    88,    89,
      94,    95,    97,    99,   102,   106,   108,   110,   112,   114,
     116,   118,   120,   125,   126,   134,   135,   141,   143,   145,
     147,   149,   151,   152,   157,   158,   160,   162,   165,   169,
     171,   173,   175,   177,   179,   182,   183,   188,   189,   191,
     193,   196,   200,   202,   204,   206,   208,   209,   210,   217,
     218,   223,   224,   226,   229,   233,   235,   237,   239,   241,
     243,   244,   250,   252,   253,   259,   260,   262,   264,   267,
     271,   273,   275,   277,   279,   281,   283,   285,   287,   288,
     294,   295,   297,   299,   302,   306,   308,   310,   312,   314,
     316,   318,   320,   323,   324,   329,   330,   332,   336,   339,
     342,   345,   346,   351,   352,   354,   355,   360,   363,   364,
     365,   373,   378,   383,   384,   385,   392,   393,   394,   401,
     402,   403,   410,   411,   412,   419,   420,   421,   430,   432,
     434,   436,   438,   440,   442,   444,   446,   448,   450,   454,
     458,   462,   466,   470,   474,   478,   482,   486,   490,   494,
     496,   500,   504,   506,   510,   514,   518,   522,   524,   526,
     528,   529
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      73,     0,    -1,    74,    -1,    84,    -1,    93,    -1,    99,
      -1,   113,    -1,   118,    -1,   107,    -1,   123,    -1,   124,
      -1,   130,    -1,   136,    -1,   128,    -1,   129,    -1,   131,
      -1,   137,    -1,    98,    -1,    -1,     3,     4,    75,    76,
      -1,    -1,    77,    -1,    78,    -1,    77,    78,    -1,    77,
      40,    78,    -1,    79,    -1,   140,    -1,   141,    -1,   144,
      -1,   150,    -1,   153,    -1,    -1,     5,    80,    37,   157,
      39,    -1,    -1,     6,    81,    37,   157,    39,    -1,    -1,
      -1,    32,    82,    37,    83,   162,    39,    -1,    -1,     3,
      17,    85,    86,    -1,    -1,    87,    -1,    88,    -1,    87,
      88,    -1,    87,    40,    88,    -1,    89,    -1,   141,    -1,
     144,    -1,   147,    -1,   153,    -1,    15,    -1,     7,    -1,
       8,    37,    92,    39,    -1,    -1,     8,    37,    92,    40,
      90,   157,    39,    -1,    -1,    36,    91,    37,   157,    39,
      -1,     9,    -1,    10,    -1,    11,    -1,    38,    -1,    12,
      -1,    -1,     3,    13,    94,    95,    -1,    -1,    96,    -1,
      97,    -1,    96,    97,    -1,    96,    40,    97,    -1,   141,
      -1,   144,    -1,   147,    -1,   153,    -1,    15,    -1,     3,
      16,    -1,    -1,     3,    14,   100,   101,    -1,    -1,   102,
      -1,   103,    -1,   102,   103,    -1,   102,    40,   103,    -1,
     104,    -1,   141,    -1,   144,    -1,    15,    -1,    -1,    -1,
      25,   105,    37,   106,   162,    39,    -1,    -1,     3,    33,
     108,   109,    -1,    -1,   110,    -1,   109,   110,    -1,   109,
      40,   110,    -1,   111,    -1,   140,    -1,   141,    -1,   144,
      -1,   150,    -1,    -1,     5,   112,    37,   157,    39,    -1,
      35,    -1,    -1,     3,     4,    17,   114,   115,    -1,    -1,
     116,    -1,   117,    -1,   116,   117,    -1,   116,    40,   117,
      -1,    79,    -1,    89,    -1,   140,    -1,   141,    -1,   144,
      -1,   147,    -1,   150,    -1,   153,    -1,    -1,     3,     4,
      13,   119,   120,    -1,    -1,   121,    -1,   122,    -1,   121,
     122,    -1,   121,    40,   122,    -1,    79,    -1,   140,    -1,
     141,    -1,   144,    -1,   147,    -1,   150,    -1,   153,    -1,
       3,    18,    -1,    -1,     3,    19,   125,   126,    -1,    -1,
     127,    -1,    37,    68,    39,    -1,     3,    20,    -1,     3,
      34,    -1,     3,    21,    -1,    -1,     3,    22,   132,   133,
      -1,    -1,   134,    -1,    -1,    37,   135,   162,    39,    -1,
       3,     7,    -1,    -1,    -1,     3,    23,   138,    37,   139,
     162,    39,    -1,    29,    37,    28,    39,    -1,    29,    37,
      30,    39,    -1,    -1,    -1,    24,   142,    37,   143,   162,
      39,    -1,    -1,    -1,    26,   145,    37,   146,   162,    39,
      -1,    -1,    -1,    27,   148,    37,   149,   162,    39,    -1,
      -1,    -1,    28,   151,    37,   152,   162,    39,    -1,    -1,
      -1,    31,   154,    37,   156,    41,   155,   162,    39,    -1,
      42,    -1,    43,    -1,    44,    -1,    45,    -1,    46,    -1,
      47,    -1,    48,    -1,    49,    -1,   158,    -1,   159,    -1,
     161,    69,   158,    -1,   161,    54,   158,    -1,   161,    55,
     158,    -1,   161,    56,   158,    -1,   161,    57,   158,    -1,
     161,    58,   158,    -1,   161,    59,   158,    -1,   161,    60,
     158,    -1,   161,    61,   158,    -1,   161,    62,   158,    -1,
     161,    63,   158,    -1,   160,    -1,   159,    52,   160,    -1,
     159,    53,   160,    -1,   161,    -1,   160,    70,   161,    -1,
     160,    71,   161,    -1,   160,    50,   161,    -1,   160,    51,
     161,    -1,    66,    -1,    68,    -1,    68,    -1,    -1,   162,
      40,    68,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   110,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   130,   129,
     136,   138,   142,   143,   144,   147,   148,   149,   150,   151,
     152,   156,   156,   162,   161,   168,   170,   167,   174,   173,
     180,   181,   184,   185,   186,   189,   190,   191,   192,   193,
     194,   202,   205,   212,   211,   219,   218,   227,   228,   229,
     230,   231,   236,   235,   240,   242,   246,   247,   248,   251,
     252,   253,   254,   255,   261,   266,   265,   271,   273,   277,
     278,   279,   282,   283,   284,   285,   290,   292,   289,   295,
     294,   300,   301,   302,   303,   306,   307,   308,   309,   310,
     314,   313,   318,   325,   324,   329,   331,   335,   336,   337,
     341,   342,   343,   344,   345,   346,   347,   348,   353,   352,
     358,   360,   364,   365,   366,   370,   371,   372,   373,   374,
     375,   376,   379,   385,   384,   391,   392,   399,   406,   410,
     414,   419,   418,   424,   425,   428,   428,   432,   438,   440,
     437,   444,   449,   456,   457,   455,   461,   463,   460,   467,
     469,   466,   473,   474,   472,   478,   479,   477,   483,   487,
     490,   493,   496,   499,   502,   505,   514,   518,   519,   526,
     533,   540,   547,   554,   562,   569,   576,   584,   591,   601,
     602,   609,   619,   620,   628,   635,   642,   653,   656,   669,
     670,   671
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "OMP", "PARALLEL", "IF", "NUM_THREADS",
  "ORDERED", "SCHEDULE", "STATIC", "DYNAMIC", "GUIDED", "RUNTIME",
  "SECTIONS", "SINGLE", "NOWAIT", "SECTION", "FOR", "MASTER", "CRITICAL",
  "BARRIER", "ATOMIC", "FLUSH", "THREADPRIVATE", "PRIVATE", "COPYPRIVATE",
  "FIRSTPRIVATE", "LASTPRIVATE", "SHARED", "DEFAULT", "NONE", "REDUCTION",
  "COPYIN", "TASK", "TASKWAIT", "UNTIED", "COLLAPSE", "'('", "AUTO", "')'",
  "','", "':'", "'+'", "'*'", "'-'", "'&'", "'^'", "'|'", "LOGAND",
  "LOGOR", "LE_OP2", "GE_OP2", "EQ_OP2", "NE_OP2", "RIGHT_ASSIGN2",
  "LEFT_ASSIGN2", "ADD_ASSIGN2", "SUB_ASSIGN2", "MUL_ASSIGN2",
  "DIV_ASSIGN2", "MOD_ASSIGN2", "AND_ASSIGN2", "XOR_ASSIGN2", "OR_ASSIGN2",
  "LEXICALERROR", "IDENTIFIER", "ICONSTANT", "EXPRESSION", "ID_EXPRESSION",
  "'='", "'<'", "'>'", "$accept", "openmp_directive", "parallel_directive",
  "@1", "parallel_clause_optseq", "parallel_clause_seq", "parallel_clause",
  "unique_parallel_clause", "@2", "@3", "@4", "@5", "for_directive", "@6",
  "for_clause_optseq", "for_clause_seq", "for_clause", "unique_for_clause",
  "@7", "@8", "schedule_kind", "sections_directive", "@9",
  "sections_clause_optseq", "sections_clause_seq", "sections_clause",
  "section_directive", "single_directive", "@10", "single_clause_optseq",
  "single_clause_seq", "single_clause", "unique_single_clause", "@11",
  "@12", "task_directive", "@13", "task_clause_optseq", "task_clause",
  "unique_task_clause", "@14", "parallel_for_directive", "@15",
  "parallel_for_clauseoptseq", "parallel_for_clause_seq",
  "parallel_for_clause", "parallel_sections_directive", "@16",
  "parallel_sections_clause_optseq", "parallel_sections_clause_seq",
  "parallel_sections_clause", "master_directive", "critical_directive",
  "@17", "region_phraseopt", "region_phrase", "barrier_directive",
  "taskwait_directive", "atomic_directive", "flush_directive", "@18",
  "flush_varsopt", "flush_vars", "@19", "ordered_directive",
  "threadprivate_directive", "@20", "@21", "data_default_clause",
  "data_privatization_clause", "@22", "@23",
  "data_privatization_in_clause", "@24", "@25",
  "data_privatization_out_clause", "@26", "@27", "data_sharing_clause",
  "@28", "@29", "data_reduction_clause", "@30", "@31",
  "reduction_operator", "expression", "assignment_expr", "equality_expr",
  "relational_expr", "unary_expr", "variable_list", 0
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
     285,   286,   287,   288,   289,   290,   291,    40,   292,    41,
      44,    58,    43,    42,    45,    38,    94,   124,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,    61,
      60,    62
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    72,    73,    73,    73,    73,    73,    73,    73,    73,
      73,    73,    73,    73,    73,    73,    73,    73,    75,    74,
      76,    76,    77,    77,    77,    78,    78,    78,    78,    78,
      78,    80,    79,    81,    79,    82,    83,    79,    85,    84,
      86,    86,    87,    87,    87,    88,    88,    88,    88,    88,
      88,    89,    89,    90,    89,    91,    89,    92,    92,    92,
      92,    92,    94,    93,    95,    95,    96,    96,    96,    97,
      97,    97,    97,    97,    98,   100,    99,   101,   101,   102,
     102,   102,   103,   103,   103,   103,   105,   106,   104,   108,
     107,   109,   109,   109,   109,   110,   110,   110,   110,   110,
     112,   111,   111,   114,   113,   115,   115,   116,   116,   116,
     117,   117,   117,   117,   117,   117,   117,   117,   119,   118,
     120,   120,   121,   121,   121,   122,   122,   122,   122,   122,
     122,   122,   123,   125,   124,   126,   126,   127,   128,   129,
     130,   132,   131,   133,   133,   135,   134,   136,   138,   139,
     137,   140,   140,   142,   143,   141,   145,   146,   144,   148,
     149,   147,   151,   152,   150,   154,   155,   153,   156,   156,
     156,   156,   156,   156,   156,   156,   157,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   159,
     159,   159,   160,   160,   160,   160,   160,   161,   161,   162,
     162,   162
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     4,
       0,     1,     1,     2,     3,     1,     1,     1,     1,     1,
       1,     0,     5,     0,     5,     0,     0,     6,     0,     4,
       0,     1,     1,     2,     3,     1,     1,     1,     1,     1,
       1,     1,     4,     0,     7,     0,     5,     1,     1,     1,
       1,     1,     0,     4,     0,     1,     1,     2,     3,     1,
       1,     1,     1,     1,     2,     0,     4,     0,     1,     1,
       2,     3,     1,     1,     1,     1,     0,     0,     6,     0,
       4,     0,     1,     2,     3,     1,     1,     1,     1,     1,
       0,     5,     1,     0,     5,     0,     1,     1,     2,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     5,
       0,     1,     1,     2,     3,     1,     1,     1,     1,     1,
       1,     1,     2,     0,     4,     0,     1,     3,     2,     2,
       2,     0,     4,     0,     1,     0,     4,     2,     0,     0,
       7,     4,     4,     0,     0,     6,     0,     0,     6,     0,
       0,     6,     0,     0,     6,     0,     0,     8,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     1,
       3,     3,     1,     3,     3,     3,     3,     1,     1,     1,
       0,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     2,     3,     4,    17,     5,     8,     6,
       7,     9,    10,    13,    14,    11,    15,    12,    16,    18,
     147,    62,    75,    74,    38,   132,   133,   138,   140,   141,
     148,    89,   139,     1,   118,   103,    20,    64,    77,    40,
     135,   143,     0,    91,   120,   105,    31,    33,   153,   156,
     162,     0,   165,    35,    19,    21,    22,    25,    26,    27,
      28,    29,    30,    73,   159,    63,    65,    66,    69,    70,
      71,    72,    85,    86,    76,    78,    79,    82,    83,    84,
      51,     0,    50,    55,    39,    41,    42,    45,    46,    47,
      48,    49,     0,   134,   136,   145,   142,   144,   149,   100,
     102,    90,    92,    95,    96,    97,    98,    99,   125,   119,
     121,   122,   126,   127,   128,   129,   130,   131,   110,   111,
     104,   106,   107,   112,   113,   114,   115,   116,   117,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    23,     0,
       0,    67,     0,     0,    80,     0,     0,     0,    43,     0,
     200,   200,     0,     0,    93,     0,   123,     0,   108,     0,
       0,   154,   157,   163,     0,     0,     0,    36,    24,   160,
      68,    87,    81,    57,    58,    59,    61,    60,     0,     0,
      44,   137,   199,     0,     0,     0,    94,   124,   109,   197,
     198,     0,   176,   177,   189,   192,     0,   200,   200,   200,
     151,   152,   168,   169,   170,   171,   172,   173,   174,   175,
       0,   200,   200,   200,    52,    53,     0,   146,     0,   150,
       0,    32,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    34,
       0,     0,     0,   166,     0,     0,     0,     0,    56,   201,
     101,   190,   192,   191,   195,   196,   193,   194,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   178,   155,
     158,   164,   200,    37,   161,    88,     0,     0,    54,   167
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,    36,    54,    55,    56,    57,   129,   130,
     136,   211,     4,    39,    84,    85,    86,    87,   247,   146,
     178,     5,    37,    65,    66,    67,     6,     7,    38,    74,
      75,    76,    77,   142,   213,     8,    43,   101,   102,   103,
     152,     9,    45,   120,   121,   122,    10,    44,   109,   110,
     111,    11,    12,    40,    93,    94,    13,    14,    15,    16,
      41,    96,    97,   150,    17,    18,    42,   151,    58,    59,
     131,   197,    60,   132,   198,    70,   139,   212,    61,   133,
     199,    62,   135,   272,   210,   191,   192,   193,   194,   195,
     183
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -152
static const yytype_int16 yypact[] =
{
      11,   279,    18,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,     0,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,   311,   329,    17,   296,
     -14,     2,     7,   239,   302,   253,  -152,  -152,  -152,  -152,
    -152,    14,  -152,  -152,  -152,   199,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,   321,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,   182,  -152,  -152,  -152,  -152,
    -152,    22,  -152,  -152,  -152,   226,  -152,  -152,  -152,  -152,
    -152,  -152,     4,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,   214,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
     186,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,   154,  -152,  -152,  -152,  -152,  -152,  -152,  -152,    48,
      55,    59,    62,    65,    67,    69,    73,   311,  -152,   102,
     329,  -152,   104,    17,  -152,   146,   107,   296,  -152,    39,
      45,    45,   140,   239,  -152,   302,  -152,   253,  -152,    41,
      41,  -152,  -152,  -152,    92,   110,   347,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,    16,    41,
    -152,  -152,  -152,    42,    76,    41,  -152,  -152,  -152,  -152,
    -152,   149,  -152,   -23,     3,   308,   159,    45,    45,    45,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
     152,    45,    45,    45,  -152,  -152,   161,  -152,   135,  -152,
     163,  -152,    41,    41,    41,    41,    41,    41,    41,    41,
      41,    41,    41,    41,    41,    41,    41,    41,    41,  -152,
      85,    89,    94,  -152,   112,   124,   134,    41,  -152,  -152,
    -152,     3,  -152,     3,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,    45,  -152,  -152,  -152,   170,   136,  -152,  -152
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -152,  -152,  -152,  -152,  -152,  -152,   -50,    44,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,   -84,   -38,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,   -60,  -152,  -152,  -152,  -152,
    -152,   -64,  -152,  -152,  -152,  -152,  -152,  -152,   -86,  -152,
    -152,  -152,  -152,  -152,  -152,  -105,  -152,  -152,  -152,  -152,
     -98,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,   -10,   -35,
    -152,  -152,   -17,  -152,  -152,   -20,  -152,  -152,    -7,  -152,
    -152,    32,  -152,  -152,  -152,  -115,   150,  -152,   -27,   -56,
    -151
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
     184,   148,    68,    78,    88,   138,   141,   119,   105,   113,
     124,   144,   156,    34,     1,   154,   158,    35,    33,    90,
      69,    79,    89,    92,   115,   126,   106,   114,   125,   222,
     223,    68,    72,   104,   112,   123,   107,   116,   127,    95,
      78,    48,    73,    49,    98,   196,   240,   241,   242,    69,
      88,   134,   188,   224,   225,   214,   215,   187,    79,   145,
     244,   245,   246,   180,   216,    90,   105,   186,    89,    71,
     220,    91,   149,   226,   227,   113,   117,   128,   181,   172,
     170,   217,   218,   119,   106,   159,   124,   168,   108,   118,
     115,   104,   160,   114,   107,   164,   161,   165,    71,   162,
     112,   126,   163,   116,   125,    68,   166,   189,    78,   190,
     167,   123,    88,   182,   127,   219,   218,    91,   105,   119,
     113,   277,   124,    69,   269,   218,    79,    90,   270,   218,
      89,   200,   276,   271,   218,   115,   106,   126,   114,   169,
     125,   171,   117,   104,   179,   112,   107,   123,   116,   201,
     127,   273,   218,   128,   108,   173,   174,   175,   176,    46,
      47,    80,    81,   274,   218,   118,   252,   252,   254,   255,
     256,   257,    71,   275,   218,   279,   218,   185,    48,    91,
      49,    64,    50,    51,   177,    52,    53,   117,   221,   128,
      83,    46,    47,   243,   157,   251,   253,    72,   239,   108,
     248,   118,   250,   249,    46,    47,    48,    73,    49,   278,
      48,     0,    49,    64,    50,    51,     0,    52,    53,    99,
       0,     0,   143,    48,     0,    49,   155,    50,    51,     0,
      52,    53,     0,    80,    81,     0,     0,     0,    48,   137,
      49,    82,    50,    51,    99,     0,     0,     0,     0,   100,
      48,     0,    49,    64,   153,     0,     0,    52,    46,    47,
      80,    81,    83,    48,     0,    49,   147,    50,    51,     0,
       0,     0,     0,     0,   100,     0,     0,    48,     0,    49,
      64,    50,    51,    19,    52,    53,    20,     0,     0,    83,
       0,     0,    21,    22,     0,    23,    24,    25,    26,    27,
      28,    29,    30,    80,    81,     0,     0,    46,    47,     0,
       0,    82,    31,    32,     0,     0,    46,    47,     0,     0,
      48,     0,    49,    64,     0,     0,    48,    52,    49,    64,
      50,    51,    83,    52,    53,    48,    63,    49,     0,    50,
      51,     0,    52,    53,    63,    48,     0,    49,    64,     0,
       0,     0,    52,    48,     0,    49,    64,     0,     0,     0,
      52,   140,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,     0,     0,     0,     0,     0,   238,   258,   259,
     260,   261,   262,   263,   264,   265,   266,   267,   268,   202,
     203,   204,   205,   206,   207,   208,   209
};

static const yytype_int16 yycheck[] =
{
     151,    85,    37,    38,    39,    55,    66,    45,    43,    44,
      45,    75,   110,    13,     3,   101,   121,    17,     0,    39,
      37,    38,    39,    37,    44,    45,    43,    44,    45,    52,
      53,    66,    15,    43,    44,    45,    43,    44,    45,    37,
      75,    24,    25,    26,    37,   160,   197,   198,   199,    66,
      85,    37,   157,    50,    51,    39,    40,   155,    75,    37,
     211,   212,   213,   147,   179,    85,   101,   153,    85,    37,
     185,    39,    68,    70,    71,   110,    44,    45,    39,   143,
     140,    39,    40,   121,   101,    37,   121,   137,    44,    45,
     110,   101,    37,   110,   101,    28,    37,    30,    66,    37,
     110,   121,    37,   110,   121,   140,    37,    66,   143,    68,
      37,   121,   147,    68,   121,    39,    40,    85,   153,   157,
     155,   272,   157,   140,    39,    40,   143,   147,    39,    40,
     147,    39,   247,    39,    40,   155,   153,   157,   155,    37,
     157,    37,   110,   153,    37,   155,   153,   157,   155,    39,
     157,    39,    40,   121,   110,     9,    10,    11,    12,     5,
       6,     7,     8,    39,    40,   121,   222,   223,   224,   225,
     226,   227,   140,    39,    40,    39,    40,    37,    24,   147,
      26,    27,    28,    29,    38,    31,    32,   155,    39,   157,
      36,     5,     6,    41,    40,   222,   223,    15,    39,   155,
      39,   157,    39,    68,     5,     6,    24,    25,    26,    39,
      24,    -1,    26,    27,    28,    29,    -1,    31,    32,     5,
      -1,    -1,    40,    24,    -1,    26,    40,    28,    29,    -1,
      31,    32,    -1,     7,     8,    -1,    -1,    -1,    24,    40,
      26,    15,    28,    29,     5,    -1,    -1,    -1,    -1,    35,
      24,    -1,    26,    27,    40,    -1,    -1,    31,     5,     6,
       7,     8,    36,    24,    -1,    26,    40,    28,    29,    -1,
      -1,    -1,    -1,    -1,    35,    -1,    -1,    24,    -1,    26,
      27,    28,    29,     4,    31,    32,     7,    -1,    -1,    36,
      -1,    -1,    13,    14,    -1,    16,    17,    18,    19,    20,
      21,    22,    23,     7,     8,    -1,    -1,     5,     6,    -1,
      -1,    15,    33,    34,    -1,    -1,     5,     6,    -1,    -1,
      24,    -1,    26,    27,    -1,    -1,    24,    31,    26,    27,
      28,    29,    36,    31,    32,    24,    15,    26,    -1,    28,
      29,    -1,    31,    32,    15,    24,    -1,    26,    27,    -1,
      -1,    -1,    31,    24,    -1,    26,    27,    -1,    -1,    -1,
      31,    40,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    -1,    -1,    -1,    -1,    -1,    69,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,    42,
      43,    44,    45,    46,    47,    48,    49
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,    73,    74,    84,    93,    98,    99,   107,   113,
     118,   123,   124,   128,   129,   130,   131,   136,   137,     4,
       7,    13,    14,    16,    17,    18,    19,    20,    21,    22,
      23,    33,    34,     0,    13,    17,    75,    94,   100,    85,
     125,   132,   138,   108,   119,   114,     5,     6,    24,    26,
      28,    29,    31,    32,    76,    77,    78,    79,   140,   141,
     144,   150,   153,    15,    27,    95,    96,    97,   141,   144,
     147,   153,    15,    25,   101,   102,   103,   104,   141,   144,
       7,     8,    15,    36,    86,    87,    88,    89,   141,   144,
     147,   153,    37,   126,   127,    37,   133,   134,    37,     5,
      35,   109,   110,   111,   140,   141,   144,   150,    79,   120,
     121,   122,   140,   141,   144,   147,   150,   153,    79,    89,
     115,   116,   117,   140,   141,   144,   147,   150,   153,    80,
      81,   142,   145,   151,    37,   154,    82,    40,    78,   148,
      40,    97,   105,    40,   103,    37,    91,    40,    88,    68,
     135,   139,   112,    40,   110,    40,   122,    40,   117,    37,
      37,    37,    37,    37,    28,    30,    37,    37,    78,    37,
      97,    37,   103,     9,    10,    11,    12,    38,    92,    37,
      88,    39,    68,   162,   162,    37,   110,   122,   117,    66,
      68,   157,   158,   159,   160,   161,   157,   143,   146,   152,
      39,    39,    42,    43,    44,    45,    46,    47,    48,    49,
     156,    83,   149,   106,    39,    40,   157,    39,    40,    39,
     157,    39,    52,    53,    50,    51,    70,    71,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    69,    39,
     162,   162,   162,    41,   162,   162,   162,    90,    39,    68,
      39,   160,   161,   160,   161,   161,   161,   161,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,    39,
      39,    39,   155,    39,    39,    39,   157,   162,    39,    39
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
        case 18:
#line 130 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute = buildOmpAttribute(e_parallel,gNode,true);
                    omptype = e_parallel; 
                  }
    break;

  case 31:
#line 156 "../../../../src/frontend/SageIII/ompparser.yy"
    { 
                        ompattribute->addClause(e_if);
                        omptype = e_if;
                     }
    break;

  case 32:
#line 160 "../../../../src/frontend/SageIII/ompparser.yy"
    { addExpression("");}
    break;

  case 33:
#line 162 "../../../../src/frontend/SageIII/ompparser.yy"
    { 
                    ompattribute->addClause(e_num_threads);       
                    omptype = e_num_threads;
                   }
    break;

  case 34:
#line 166 "../../../../src/frontend/SageIII/ompparser.yy"
    { addExpression("");}
    break;

  case 35:
#line 168 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute->addClause(e_copyin);
                    omptype = e_copyin;
                  }
    break;

  case 36:
#line 170 "../../../../src/frontend/SageIII/ompparser.yy"
    {b_within_variable_list = true;}
    break;

  case 37:
#line 170 "../../../../src/frontend/SageIII/ompparser.yy"
    {b_within_variable_list =false;}
    break;

  case 38:
#line 174 "../../../../src/frontend/SageIII/ompparser.yy"
    { 
                   ompattribute = buildOmpAttribute(e_for,gNode,true); 
                   }
    break;

  case 50:
#line 194 "../../../../src/frontend/SageIII/ompparser.yy"
    { 
                           ompattribute->addClause(e_nowait);
                           //Not correct since nowait cannot have expression or var_list
                           //omptype =e_nowait;
                         }
    break;

  case 51:
#line 202 "../../../../src/frontend/SageIII/ompparser.yy"
    { 
                            ompattribute->addClause(e_ordered_clause);
                          }
    break;

  case 52:
#line 206 "../../../../src/frontend/SageIII/ompparser.yy"
    { 
                    ompattribute->addClause(e_schedule);
                    ompattribute->setScheduleKind(static_cast<omp_construct_enum>((yyvsp[(3) - (4)].itype)));
                    omptype = e_schedule;
                  }
    break;

  case 53:
#line 212 "../../../../src/frontend/SageIII/ompparser.yy"
    { 
                    ompattribute->addClause(e_schedule);
                    ompattribute->setScheduleKind(static_cast<omp_construct_enum>((yyvsp[(3) - (4)].itype)));
                    omptype = e_schedule;
                  }
    break;

  case 54:
#line 217 "../../../../src/frontend/SageIII/ompparser.yy"
    { addExpression("");}
    break;

  case 55:
#line 219 "../../../../src/frontend/SageIII/ompparser.yy"
    {
                    ompattribute->addClause(e_collapse);
                    omptype = e_collapse;
                  }
    break;

  case 56:
#line 224 "../../../../src/frontend/SageIII/ompparser.yy"
    { addExpression("");}
    break;

  case 57:
#line 227 "../../../../src/frontend/SageIII/ompparser.yy"
    { (yyval.itype) = e_schedule_static; }
    break;

  case 58:
#line 228 "../../../../src/frontend/SageIII/ompparser.yy"
    { (yyval.itype) = e_schedule_dynamic; }
    break;

  case 59:
#line 229 "../../../../src/frontend/SageIII/ompparser.yy"
    { (yyval.itype) = e_schedule_guided; }
    break;

  case 60:
#line 230 "../../../../src/frontend/SageIII/ompparser.yy"
    { (yyval.itype) = e_schedule_auto; }
    break;

  case 61:
#line 231 "../../../../src/frontend/SageIII/ompparser.yy"
    { (yyval.itype) = e_schedule_runtime; }
    break;

  case 62:
#line 236 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute = buildOmpAttribute(e_sections,gNode, true); }
    break;

  case 73:
#line 255 "../../../../src/frontend/SageIII/ompparser.yy"
    { 
                           ompattribute->addClause(e_nowait);
                         }
    break;

  case 74:
#line 262 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute = buildOmpAttribute(e_section,gNode,true); }
    break;

  case 75:
#line 266 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute = buildOmpAttribute(e_single,gNode,true); 
                    omptype = e_single; }
    break;

  case 85:
#line 285 "../../../../src/frontend/SageIII/ompparser.yy"
    { 
                            ompattribute->addClause(e_nowait);
                         }
    break;

  case 86:
#line 290 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute->addClause(e_copyprivate);
                          omptype = e_copyprivate; }
    break;

  case 87:
#line 292 "../../../../src/frontend/SageIII/ompparser.yy"
    {b_within_variable_list = true;}
    break;

  case 88:
#line 292 "../../../../src/frontend/SageIII/ompparser.yy"
    {b_within_variable_list =false;}
    break;

  case 89:
#line 295 "../../../../src/frontend/SageIII/ompparser.yy"
    {ompattribute = buildOmpAttribute(e_task,gNode,true);
                  omptype = e_task; }
    break;

  case 100:
#line 314 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute->addClause(e_if);
                    omptype = e_if; }
    break;

  case 101:
#line 317 "../../../../src/frontend/SageIII/ompparser.yy"
    { addExpression("");}
    break;

  case 102:
#line 319 "../../../../src/frontend/SageIII/ompparser.yy"
    {
                   ompattribute->addClause(e_untied);
                  }
    break;

  case 103:
#line 325 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute = buildOmpAttribute(e_parallel_for,gNode, true); }
    break;

  case 118:
#line 353 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute =buildOmpAttribute(e_parallel_sections,gNode, true); 
                    omptype = e_parallel_sections; }
    break;

  case 132:
#line 380 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute = buildOmpAttribute(e_master, gNode, true);}
    break;

  case 133:
#line 385 "../../../../src/frontend/SageIII/ompparser.yy"
    {
                  ompattribute = buildOmpAttribute(e_critical, gNode, true); 
                  }
    break;

  case 137:
#line 400 "../../../../src/frontend/SageIII/ompparser.yy"
    { 
                      ompattribute->setCriticalName((const char*)(yyvsp[(2) - (3)].stype));
                    }
    break;

  case 138:
#line 407 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute = buildOmpAttribute(e_barrier,gNode, true); }
    break;

  case 139:
#line 411 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute = buildOmpAttribute(e_taskwait, gNode, true); }
    break;

  case 140:
#line 415 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute = buildOmpAttribute(e_atomic,gNode, true); }
    break;

  case 141:
#line 419 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute = buildOmpAttribute(e_flush,gNode, true);
                   omptype = e_flush; }
    break;

  case 145:
#line 428 "../../../../src/frontend/SageIII/ompparser.yy"
    {b_within_variable_list = true;}
    break;

  case 146:
#line 428 "../../../../src/frontend/SageIII/ompparser.yy"
    {b_within_variable_list =false;}
    break;

  case 147:
#line 433 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute = buildOmpAttribute(e_ordered_directive,gNode, true); }
    break;

  case 148:
#line 438 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute = buildOmpAttribute(e_threadprivate,gNode, true); 
                    omptype = e_threadprivate; }
    break;

  case 149:
#line 440 "../../../../src/frontend/SageIII/ompparser.yy"
    {b_within_variable_list = true;}
    break;

  case 150:
#line 440 "../../../../src/frontend/SageIII/ompparser.yy"
    {b_within_variable_list =false;}
    break;

  case 151:
#line 445 "../../../../src/frontend/SageIII/ompparser.yy"
    { 
                    ompattribute->addClause(e_default);
                    ompattribute->setDefaultValue(e_default_shared); 
                  }
    break;

  case 152:
#line 450 "../../../../src/frontend/SageIII/ompparser.yy"
    {
                    ompattribute->addClause(e_default);
                    ompattribute->setDefaultValue(e_default_none);
                  }
    break;

  case 153:
#line 456 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute->addClause(e_private); omptype = e_private;}
    break;

  case 154:
#line 457 "../../../../src/frontend/SageIII/ompparser.yy"
    {b_within_variable_list = true;}
    break;

  case 155:
#line 457 "../../../../src/frontend/SageIII/ompparser.yy"
    {b_within_variable_list =false;}
    break;

  case 156:
#line 461 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute->addClause(e_firstprivate); 
                    omptype = e_firstprivate;}
    break;

  case 157:
#line 463 "../../../../src/frontend/SageIII/ompparser.yy"
    {b_within_variable_list = true;}
    break;

  case 158:
#line 463 "../../../../src/frontend/SageIII/ompparser.yy"
    {b_within_variable_list =false;}
    break;

  case 159:
#line 467 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute->addClause(e_lastprivate); 
                    omptype = e_lastprivate;}
    break;

  case 160:
#line 469 "../../../../src/frontend/SageIII/ompparser.yy"
    {b_within_variable_list = true;}
    break;

  case 161:
#line 469 "../../../../src/frontend/SageIII/ompparser.yy"
    {b_within_variable_list =false;}
    break;

  case 162:
#line 473 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute->addClause(e_shared); omptype = e_shared; }
    break;

  case 163:
#line 474 "../../../../src/frontend/SageIII/ompparser.yy"
    {b_within_variable_list = true;}
    break;

  case 164:
#line 474 "../../../../src/frontend/SageIII/ompparser.yy"
    {b_within_variable_list =false;}
    break;

  case 165:
#line 478 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute->addClause(e_reduction);}
    break;

  case 166:
#line 479 "../../../../src/frontend/SageIII/ompparser.yy"
    {b_within_variable_list = true;}
    break;

  case 167:
#line 479 "../../../../src/frontend/SageIII/ompparser.yy"
    {b_within_variable_list =false;}
    break;

  case 168:
#line 483 "../../../../src/frontend/SageIII/ompparser.yy"
    {
                       ompattribute->setReductionOperator(e_reduction_plus); 
                       omptype = e_reduction_plus; /*variables are stored for each operator*/
                       }
    break;

  case 169:
#line 487 "../../../../src/frontend/SageIII/ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_mul);  
                       omptype = e_reduction_mul;
                      }
    break;

  case 170:
#line 490 "../../../../src/frontend/SageIII/ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_minus); 
                       omptype = e_reduction_minus;
                      }
    break;

  case 171:
#line 493 "../../../../src/frontend/SageIII/ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_bitand);  
                       omptype = e_reduction_bitand;
                      }
    break;

  case 172:
#line 496 "../../../../src/frontend/SageIII/ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_bitxor);  
                       omptype = e_reduction_bitxor;
                      }
    break;

  case 173:
#line 499 "../../../../src/frontend/SageIII/ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_bitor);  
                       omptype = e_reduction_bitor;
                      }
    break;

  case 174:
#line 502 "../../../../src/frontend/SageIII/ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_logand);  
                                   omptype = e_reduction_logand;
                                  }
    break;

  case 175:
#line 505 "../../../../src/frontend/SageIII/ompparser.yy"
    { ompattribute->setReductionOperator(e_reduction_logor); 
                                   omptype = e_reduction_logor;
                                 }
    break;

  case 178:
#line 520 "../../../../src/frontend/SageIII/ompparser.yy"
    {
          current_exp = SageBuilder::buildAssignOp(
                      (SgExpression*)((yyvsp[(1) - (3)].ptype)),
                      (SgExpression*)((yyvsp[(3) - (3)].ptype))); 
          (yyval.ptype) = current_exp;
        }
    break;

  case 179:
#line 527 "../../../../src/frontend/SageIII/ompparser.yy"
    {
          current_exp = SageBuilder::buildRshiftAssignOp(
                      (SgExpression*)((yyvsp[(1) - (3)].ptype)),
                      (SgExpression*)((yyvsp[(3) - (3)].ptype))); 
          (yyval.ptype) = current_exp;
        }
    break;

  case 180:
#line 534 "../../../../src/frontend/SageIII/ompparser.yy"
    {
          current_exp = SageBuilder::buildLshiftAssignOp(
                      (SgExpression*)((yyvsp[(1) - (3)].ptype)),
                      (SgExpression*)((yyvsp[(3) - (3)].ptype))); 
          (yyval.ptype) = current_exp;
        }
    break;

  case 181:
#line 541 "../../../../src/frontend/SageIII/ompparser.yy"
    {
          current_exp = SageBuilder::buildPlusAssignOp(
                      (SgExpression*)((yyvsp[(1) - (3)].ptype)),
                      (SgExpression*)((yyvsp[(3) - (3)].ptype))); 
          (yyval.ptype) = current_exp;
        }
    break;

  case 182:
#line 548 "../../../../src/frontend/SageIII/ompparser.yy"
    {
          current_exp = SageBuilder::buildMinusAssignOp(
                      (SgExpression*)((yyvsp[(1) - (3)].ptype)),
                      (SgExpression*)((yyvsp[(3) - (3)].ptype))); 
          (yyval.ptype) = current_exp;
        }
    break;

  case 183:
#line 555 "../../../../src/frontend/SageIII/ompparser.yy"
    {
          current_exp = SageBuilder::buildMultAssignOp(
                      (SgExpression*)((yyvsp[(1) - (3)].ptype)),
                      (SgExpression*)((yyvsp[(3) - (3)].ptype))); 
          (yyval.ptype) = current_exp;
        }
    break;

  case 184:
#line 563 "../../../../src/frontend/SageIII/ompparser.yy"
    {
          current_exp = SageBuilder::buildDivAssignOp(
                      (SgExpression*)((yyvsp[(1) - (3)].ptype)),
                      (SgExpression*)((yyvsp[(3) - (3)].ptype))); 
          (yyval.ptype) = current_exp;
        }
    break;

  case 185:
#line 570 "../../../../src/frontend/SageIII/ompparser.yy"
    {
          current_exp = SageBuilder::buildModAssignOp(
                      (SgExpression*)((yyvsp[(1) - (3)].ptype)),
                      (SgExpression*)((yyvsp[(3) - (3)].ptype))); 
          (yyval.ptype) = current_exp;
        }
    break;

  case 186:
#line 577 "../../../../src/frontend/SageIII/ompparser.yy"
    {
          current_exp = SageBuilder::buildAndAssignOp(
                      (SgExpression*)((yyvsp[(1) - (3)].ptype)),
                      (SgExpression*)((yyvsp[(3) - (3)].ptype))); 
          (yyval.ptype) = current_exp;
        }
    break;

  case 187:
#line 585 "../../../../src/frontend/SageIII/ompparser.yy"
    {
          current_exp = SageBuilder::buildXorAssignOp(
                      (SgExpression*)((yyvsp[(1) - (3)].ptype)),
                      (SgExpression*)((yyvsp[(3) - (3)].ptype))); 
          (yyval.ptype) = current_exp;
        }
    break;

  case 188:
#line 592 "../../../../src/frontend/SageIII/ompparser.yy"
    {
          current_exp = SageBuilder::buildIorAssignOp(
                      (SgExpression*)((yyvsp[(1) - (3)].ptype)),
                      (SgExpression*)((yyvsp[(3) - (3)].ptype))); 
          (yyval.ptype) = current_exp;
        }
    break;

  case 190:
#line 603 "../../../../src/frontend/SageIII/ompparser.yy"
    {
          current_exp = SageBuilder::buildEqualityOp(
                      (SgExpression*)((yyvsp[(1) - (3)].ptype)),
                      (SgExpression*)((yyvsp[(3) - (3)].ptype))); 
          (yyval.ptype) = current_exp;
        }
    break;

  case 191:
#line 610 "../../../../src/frontend/SageIII/ompparser.yy"
    {
          current_exp = SageBuilder::buildNotEqualOp(
                      (SgExpression*)((yyvsp[(1) - (3)].ptype)),
                      (SgExpression*)((yyvsp[(3) - (3)].ptype))); 
          (yyval.ptype) = current_exp;
        }
    break;

  case 193:
#line 621 "../../../../src/frontend/SageIII/ompparser.yy"
    { 
                    current_exp = SageBuilder::buildLessThanOp(
                      (SgExpression*)((yyvsp[(1) - (3)].ptype)),
                      (SgExpression*)((yyvsp[(3) - (3)].ptype))); 
                    (yyval.ptype) = current_exp; 
                  //  std::cout<<"debug: buildLessThanOp():\n"<<current_exp->unparseToString()<<std::endl;
                  }
    break;

  case 194:
#line 629 "../../../../src/frontend/SageIII/ompparser.yy"
    {
                    current_exp = SageBuilder::buildGreaterThanOp(
                      (SgExpression*)((yyvsp[(1) - (3)].ptype)),
                      (SgExpression*)((yyvsp[(3) - (3)].ptype))); 
                    (yyval.ptype) = current_exp; 
                }
    break;

  case 195:
#line 636 "../../../../src/frontend/SageIII/ompparser.yy"
    {
                    current_exp = SageBuilder::buildLessOrEqualOp(
                      (SgExpression*)((yyvsp[(1) - (3)].ptype)),
                      (SgExpression*)((yyvsp[(3) - (3)].ptype))); 
                    (yyval.ptype) = current_exp; 
                }
    break;

  case 196:
#line 643 "../../../../src/frontend/SageIII/ompparser.yy"
    {
                    current_exp = SageBuilder::buildGreaterOrEqualOp(
                      (SgExpression*)((yyvsp[(1) - (3)].ptype)),
                      (SgExpression*)((yyvsp[(3) - (3)].ptype))); 
                    (yyval.ptype) = current_exp; 
                }
    break;

  case 197:
#line 654 "../../../../src/frontend/SageIII/ompparser.yy"
    {current_exp = SageBuilder::buildIntVal((yyvsp[(1) - (1)].itype)); 
                    (yyval.ptype) = current_exp; }
    break;

  case 198:
#line 657 "../../../../src/frontend/SageIII/ompparser.yy"
    { current_exp = SageBuilder::buildVarRefExp(
                      (const char*)((yyvsp[(1) - (1)].stype)),SageInterface::getScope(gNode)); 
                    (yyval.ptype) = current_exp; }
    break;

  case 199:
#line 669 "../../../../src/frontend/SageIII/ompparser.yy"
    { if (!addVar((const char*)(yyvsp[(1) - (1)].stype))) YYABORT; }
    break;

  case 201:
#line 671 "../../../../src/frontend/SageIII/ompparser.yy"
    { if (!addVar((const char*)(yyvsp[(3) - (3)].stype))) YYABORT; }
    break;


/* Line 1267 of yacc.c.  */
#line 2463 "ompparser.cc"
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


#line 674 "../../../../src/frontend/SageIII/ompparser.yy"


int yyerror(const char *s) {
        printf("%s!\n", s);
        return 0;
}


OmpAttribute* getParsedDirective() {
        return ompattribute;
}

void omp_parser_init(SgNode* aNode, const char* str) {
        omp_lexer_init(str);
        gNode = aNode;
}

static bool addVar(const char* var) 
{
  ompattribute->addVariable(omptype,var);
  return true;
}

// The ROSE's string-based AST construction is not stable,
// pass real expressions as SgExpression, Liao
static bool addExpression(const char* expr) {
        //ompattribute->addExpression(omptype,std::string(expr),NULL);
//  std::cout<<"debug: current expression is:"<<current_exp->unparseToString()<<std::endl;
      ompattribute->addExpression(omptype,std::string(expr),current_exp);
        return true;
}


