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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* If NAME_PREFIX is specified substitute the variables and functions
   names.  */
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




/* Copy the first part of user declarations.  */
#line 8 "ompparser.yy"

#include <stdio.h>
#include <assert.h>

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

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 239 "ompparser.cc"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
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
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  33
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   381

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  80
/* YYNRULES -- Number of rules. */
#define YYNRULES  170
/* YYNRULES -- Number of states. */
#define YYNSTATES  248

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   299

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    45,     2,
      37,    39,    43,    42,    40,    44,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    41,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      35,    36,    38,    48,    49,    50,    51,    52,    53,    54
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    11,    13,    15,    17,
      19,    21,    23,    25,    27,    29,    31,    33,    35,    37,
      39,    40,    46,    47,    49,    51,    54,    58,    60,    62,
      64,    66,    68,    70,    71,    77,    78,    84,    85,    91,
      92,    98,    99,   101,   103,   106,   110,   112,   114,   116,
     118,   120,   122,   124,   129,   130,   138,   139,   145,   147,
     149,   151,   153,   155,   156,   162,   163,   165,   167,   170,
     174,   176,   178,   180,   182,   184,   188,   189,   195,   196,
     198,   200,   203,   207,   209,   211,   213,   215,   216,   222,
     223,   229,   231,   234,   238,   240,   242,   244,   246,   248,
     249,   255,   257,   258,   265,   266,   268,   270,   273,   277,
     279,   281,   283,   285,   287,   289,   291,   293,   294,   301,
     302,   304,   306,   309,   313,   315,   317,   319,   321,   323,
     325,   327,   331,   332,   338,   339,   341,   345,   349,   353,
     357,   358,   364,   365,   367,   371,   375,   376,   384,   389,
     394,   395,   401,   402,   408,   409,   415,   416,   422,   423,
     431,   433,   435,   437,   439,   441,   443,   445,   447,   449,
     453
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
      58,     0,    -1,    -1,    57,    50,    -1,    59,    -1,    68,
      -1,    77,    -1,    83,    -1,    96,    -1,   101,    -1,    90,
      -1,   106,    -1,   107,    -1,   113,    -1,   118,    -1,   111,
      -1,   112,    -1,   114,    -1,   119,    -1,    82,    -1,    -1,
       3,     4,    60,    61,   134,    -1,    -1,    62,    -1,    63,
      -1,    62,    63,    -1,    62,    40,    63,    -1,    64,    -1,
     121,    -1,   122,    -1,   124,    -1,   128,    -1,   130,    -1,
      -1,     5,    65,    37,    56,    39,    -1,    -1,     6,    66,
      37,    56,    39,    -1,    -1,    32,    67,    37,   133,    39,
      -1,    -1,     3,    17,    69,    70,   134,    -1,    -1,    71,
      -1,    72,    -1,    71,    72,    -1,    71,    40,    72,    -1,
      73,    -1,   122,    -1,   124,    -1,   126,    -1,   130,    -1,
      15,    -1,     7,    -1,     8,    37,    76,    39,    -1,    -1,
       8,    37,    76,    40,    74,    56,    39,    -1,    -1,    36,
      75,    37,    56,    39,    -1,     9,    -1,    10,    -1,    11,
      -1,    38,    -1,    12,    -1,    -1,     3,    13,    78,    79,
     134,    -1,    -1,    80,    -1,    81,    -1,    80,    81,    -1,
      80,    40,    81,    -1,   122,    -1,   124,    -1,   126,    -1,
     130,    -1,    15,    -1,     3,    16,   134,    -1,    -1,     3,
      14,    84,    85,   134,    -1,    -1,    86,    -1,    87,    -1,
      86,    87,    -1,    86,    40,    87,    -1,    88,    -1,   122,
      -1,   124,    -1,    15,    -1,    -1,    25,    89,    37,   133,
      39,    -1,    -1,     3,    33,    91,    92,   134,    -1,    93,
      -1,    92,    93,    -1,    92,    40,    93,    -1,    94,    -1,
     121,    -1,   122,    -1,   124,    -1,   128,    -1,    -1,     5,
      95,    37,    56,    39,    -1,    35,    -1,    -1,     3,     4,
      17,    97,    98,   134,    -1,    -1,    99,    -1,   100,    -1,
      99,   100,    -1,    99,    40,   100,    -1,    64,    -1,    73,
      -1,   121,    -1,   122,    -1,   124,    -1,   126,    -1,   128,
      -1,   130,    -1,    -1,     3,     4,    13,   102,   103,   134,
      -1,    -1,   104,    -1,   105,    -1,   104,   105,    -1,   104,
      40,   105,    -1,    64,    -1,   121,    -1,   122,    -1,   124,
      -1,   126,    -1,   128,    -1,   130,    -1,     3,    18,   134,
      -1,    -1,     3,    19,   108,   109,   134,    -1,    -1,   110,
      -1,    37,    51,    39,    -1,     3,    20,   134,    -1,     3,
      34,   134,    -1,     3,    21,   134,    -1,    -1,     3,    22,
     115,   116,   134,    -1,    -1,   117,    -1,    37,   133,    39,
      -1,     3,     7,   134,    -1,    -1,     3,    23,   120,    37,
     133,    39,   134,    -1,    29,    37,    28,    39,    -1,    29,
      37,    30,    39,    -1,    -1,    24,   123,    37,   133,    39,
      -1,    -1,    26,   125,    37,   133,    39,    -1,    -1,    27,
     127,    37,   133,    39,    -1,    -1,    28,   129,    37,   133,
      39,    -1,    -1,    31,   131,    37,   132,    41,   133,    39,
      -1,    42,    -1,    43,    -1,    44,    -1,    45,    -1,    46,
      -1,    47,    -1,    48,    -1,    49,    -1,    51,    -1,   133,
      40,    51,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,    79,    79,    79,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
     102,   101,   108,   110,   114,   115,   116,   119,   120,   121,
     122,   123,   124,   128,   128,   133,   132,   138,   137,   144,
     143,   150,   151,   154,   155,   156,   159,   160,   161,   162,
     163,   164,   172,   175,   182,   181,   188,   187,   195,   196,
     197,   198,   199,   204,   203,   208,   210,   214,   215,   216,
     219,   220,   221,   222,   223,   229,   234,   233,   239,   241,
     245,   246,   247,   250,   251,   252,   253,   258,   257,   263,
     262,   268,   269,   270,   273,   274,   275,   276,   277,   281,
     280,   284,   291,   290,   295,   297,   301,   302,   303,   307,
     308,   309,   310,   311,   312,   313,   314,   319,   318,   324,
     326,   330,   331,   332,   336,   337,   338,   339,   340,   341,
     342,   345,   351,   350,   357,   358,   365,   372,   376,   380,
     385,   384,   390,   391,   394,   398,   404,   403,   410,   415,
     422,   421,   427,   426,   433,   432,   439,   438,   444,   443,
     449,   450,   451,   452,   453,   454,   455,   456,   466,   467,
     470
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "OMP", "PARALLEL", "IF", "NUM_THREADS",
  "ORDERED", "SCHEDULE", "STATIC", "DYNAMIC", "GUIDED", "RUNTIME",
  "SECTIONS", "SINGLE", "NOWAIT", "SECTION", "FOR", "MASTER", "CRITICAL",
  "BARRIER", "ATOMIC", "FLUSH", "THREADPRIVATE", "PRIVATE", "COPYPRIVATE",
  "FIRSTPRIVATE", "LASTPRIVATE", "SHARED", "DEFAULT", "NONE", "REDUCTION",
  "COPYIN", "TASK", "TASKWAIT", "UNTIED", "COLLAPSE", "'('", "AUTO", "')'",
  "','", "':'", "'+'", "'*'", "'-'", "'&'", "'^'", "'|'", "LOGAND",
  "LOGOR", "EXPRESSION", "ID_EXPRESSION", "IDENTIFIER", "NEWLINE",
  "LEXICALERROR", "$accept", "expression", "@1", "openmp_directive",
  "parallel_directive", "@2", "parallel_clause_optseq",
  "parallel_clause_seq", "parallel_clause", "unique_parallel_clause", "@3",
  "@4", "@5", "for_directive", "@6", "for_clause_optseq", "for_clause_seq",
  "for_clause", "unique_for_clause", "@7", "@8", "schedule_kind",
  "sections_directive", "@9", "sections_clause_optseq",
  "sections_clause_seq", "sections_clause", "section_directive",
  "single_directive", "@10", "single_clause_optseq", "single_clause_seq",
  "single_clause", "unique_single_clause", "@11", "task_directive", "@12",
  "task_clause_optseq", "task_clause", "unique_task_clause", "@13",
  "parallel_for_directive", "@14", "parallel_for_clauseoptseq",
  "parallel_for_clause_seq", "parallel_for_clause",
  "parallel_sections_directive", "@15", "parallel_sections_clause_optseq",
  "parallel_sections_clause_seq", "parallel_sections_clause",
  "master_directive", "critical_directive", "@16", "region_phraseopt",
  "region_phrase", "barrier_directive", "taskwait_directive",
  "atomic_directive", "flush_directive", "@17", "flush_varsopt",
  "flush_vars", "ordered_directive", "threadprivate_directive", "@18",
  "data_default_clause", "data_privatization_clause", "@19",
  "data_privatization_in_clause", "@20", "data_privatization_out_clause",
  "@21", "data_sharing_clause", "@22", "data_reduction_clause", "@23",
  "reduction_operator", "variable_list", "new_line", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,    40,   292,    41,
      44,    58,    43,    42,    45,    38,    94,   124,   293,   294,
     295,   296,   297,   298,   299
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    55,    57,    56,    58,    58,    58,    58,    58,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      60,    59,    61,    61,    62,    62,    62,    63,    63,    63,
      63,    63,    63,    65,    64,    66,    64,    67,    64,    69,
      68,    70,    70,    71,    71,    71,    72,    72,    72,    72,
      72,    72,    73,    73,    74,    73,    75,    73,    76,    76,
      76,    76,    76,    78,    77,    79,    79,    80,    80,    80,
      81,    81,    81,    81,    81,    82,    84,    83,    85,    85,
      86,    86,    86,    87,    87,    87,    87,    89,    88,    91,
      90,    92,    92,    92,    93,    93,    93,    93,    93,    95,
      94,    94,    97,    96,    98,    98,    99,    99,    99,   100,
     100,   100,   100,   100,   100,   100,   100,   102,   101,   103,
     103,   104,   104,   104,   105,   105,   105,   105,   105,   105,
     105,   106,   108,   107,   109,   109,   110,   111,   112,   113,
     115,   114,   116,   116,   117,   118,   120,   119,   121,   121,
     123,   122,   125,   124,   127,   126,   129,   128,   131,   130,
     132,   132,   132,   132,   132,   132,   132,   132,   133,   133,
     134
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     5,     0,     1,     1,     2,     3,     1,     1,     1,
       1,     1,     1,     0,     5,     0,     5,     0,     5,     0,
       5,     0,     1,     1,     2,     3,     1,     1,     1,     1,
       1,     1,     1,     4,     0,     7,     0,     5,     1,     1,
       1,     1,     1,     0,     5,     0,     1,     1,     2,     3,
       1,     1,     1,     1,     1,     3,     0,     5,     0,     1,
       1,     2,     3,     1,     1,     1,     1,     0,     5,     0,
       5,     1,     2,     3,     1,     1,     1,     1,     1,     0,
       5,     1,     0,     6,     0,     1,     1,     2,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     6,     0,
       1,     1,     2,     3,     1,     1,     1,     1,     1,     1,
       1,     3,     0,     5,     0,     1,     3,     3,     3,     3,
       0,     5,     0,     1,     3,     3,     0,     7,     4,     4,
       0,     5,     0,     5,     0,     5,     0,     5,     0,     7,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     0,     0,     4,     5,     6,    19,     7,    10,     8,
       9,    11,    12,    15,    16,    13,    17,    14,    18,    20,
     170,    63,    76,   170,    39,   170,   132,   170,   170,   140,
     146,    89,   170,     1,   117,   102,    22,   145,    65,    78,
      75,    41,   131,   134,   137,   139,   142,     0,     0,   138,
     119,   104,    33,    35,   150,   152,   156,     0,   158,    37,
     170,    23,    24,    27,    28,    29,    30,    31,    32,    74,
     154,   170,    66,    67,    70,    71,    72,    73,    86,    87,
     170,    79,    80,    83,    84,    85,    52,     0,    51,    56,
     170,    42,    43,    46,    47,    48,    49,    50,     0,   170,
     135,     0,   170,   143,     0,    99,   101,   170,    91,    94,
      95,    96,    97,    98,   124,   170,   120,   121,   125,   126,
     127,   128,   129,   130,   109,   110,   170,   105,   106,   111,
     112,   113,   114,   115,   116,     0,     0,     0,     0,     0,
       0,     0,     0,    21,     0,    25,     0,    64,     0,    68,
       0,    77,     0,    81,     0,     0,    40,     0,    44,     0,
     133,   168,     0,   141,     0,     0,     0,    92,    90,   118,
       0,   122,   103,     0,   107,     2,     2,     0,     0,     0,
       0,     0,     0,     0,    26,     0,    69,     0,    82,    58,
      59,    60,    62,    61,     0,     2,    45,   136,   144,     0,
     170,     2,    93,   123,   108,     0,     0,     0,     0,     0,
       0,   148,   149,   160,   161,   162,   163,   164,   165,   166,
     167,     0,     0,     0,     0,    53,    54,     0,   169,   147,
       0,    34,     3,    36,   151,   153,   157,     0,    38,   155,
      88,     2,    57,   100,     0,     0,   159,    55
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,   205,   206,     2,     3,    36,    60,    61,    62,    63,
     135,   136,   142,     4,    41,    90,    91,    92,    93,   241,
     155,   194,     5,    38,    71,    72,    73,     6,     7,    39,
      80,    81,    82,    83,   150,     8,    48,   107,   108,   109,
     165,     9,    51,   126,   127,   128,    10,    50,   115,   116,
     117,    11,    12,    43,    99,   100,    13,    14,    15,    16,
      46,   102,   103,    17,    18,    47,    64,    65,   137,    66,
     138,    76,   146,    67,   139,    68,   141,   221,   162,    37
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -113
static const short yypact[] =
{
      10,   291,    33,  -113,  -113,  -113,  -113,  -113,  -113,  -113,
    -113,  -113,  -113,  -113,  -113,  -113,  -113,  -113,  -113,    21,
    -113,  -113,  -113,  -113,  -113,  -113,  -113,  -113,  -113,  -113,
    -113,  -113,  -113,  -113,  -113,  -113,   323,  -113,   156,   341,
    -113,   308,  -113,     3,  -113,  -113,    13,    15,   251,  -113,
     314,   265,  -113,  -113,  -113,  -113,  -113,    26,  -113,  -113,
    -113,   210,  -113,  -113,  -113,  -113,  -113,  -113,  -113,  -113,
    -113,  -113,   333,  -113,  -113,  -113,  -113,  -113,  -113,  -113,
    -113,   242,  -113,  -113,  -113,  -113,  -113,    34,  -113,  -113,
    -113,   238,  -113,  -113,  -113,  -113,  -113,  -113,   -35,  -113,
    -113,    24,  -113,  -113,    24,  -113,  -113,   223,  -113,  -113,
    -113,  -113,  -113,  -113,  -113,  -113,   195,  -113,  -113,  -113,
    -113,  -113,  -113,  -113,  -113,  -113,  -113,   167,  -113,  -113,
    -113,  -113,  -113,  -113,  -113,    41,    44,    48,    50,    57,
      -2,    59,    77,  -113,   323,  -113,    79,  -113,   156,  -113,
      83,  -113,   341,  -113,   221,    85,  -113,   308,  -113,    -4,
    -113,  -113,     4,  -113,     7,    89,   251,  -113,  -113,  -113,
     314,  -113,  -113,   265,  -113,  -113,  -113,    24,    24,    24,
      27,    63,   332,    24,  -113,    24,  -113,    24,  -113,  -113,
    -113,  -113,  -113,  -113,    16,  -113,  -113,  -113,  -113,    86,
    -113,  -113,  -113,  -113,  -113,    95,    78,   103,    29,    65,
      73,  -113,  -113,  -113,  -113,  -113,  -113,  -113,  -113,  -113,
    -113,   109,    93,   101,   123,  -113,  -113,   105,  -113,  -113,
     114,  -113,  -113,  -113,  -113,  -113,  -113,    24,  -113,  -113,
    -113,  -113,  -113,  -113,   149,   116,  -113,  -113
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
    -113,   -16,  -113,  -113,  -113,  -113,  -113,  -113,   -51,    32,
    -113,  -113,  -113,  -113,  -113,  -113,  -113,   -84,   -50,  -113,
    -113,  -113,  -113,  -113,  -113,  -113,   -47,  -113,  -113,  -113,
    -113,  -113,   -78,  -113,  -113,  -113,  -113,  -113,  -101,  -113,
    -113,  -113,  -113,  -113,  -113,  -112,  -113,  -113,  -113,  -113,
    -108,  -113,  -113,  -113,  -113,  -113,  -113,  -113,  -113,  -113,
    -113,  -113,  -113,  -113,  -113,  -113,    -9,   -27,  -113,   -21,
    -113,     8,  -113,    40,  -113,   -19,  -113,  -113,   -68,   -23
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      40,   125,    42,   153,    44,    45,   167,   158,   171,    49,
     145,    74,    84,     1,    94,   174,   159,    75,    85,    77,
      95,   111,    97,   119,   130,   149,   180,   112,   181,   120,
     131,   123,   134,    33,    34,   197,   164,   143,    35,   110,
      98,   118,   129,   198,   199,    74,   200,   199,   147,    96,
     101,    75,   104,    77,    84,   225,   226,   151,   121,   132,
      85,   204,   203,   140,    94,   202,   211,   156,   234,   199,
      95,   154,    97,   196,   188,   161,   160,   125,   175,   163,
     111,   176,   114,   124,   168,   177,   112,   178,   113,   119,
     122,   133,   169,   184,   179,   120,   182,   123,   110,    96,
     130,   186,   212,   172,   235,   199,   131,   118,   134,   208,
     209,   210,   236,   199,   183,   222,   185,   223,   129,   224,
     187,    74,   195,   125,   121,    84,   201,    75,   232,    77,
      94,    85,   238,   199,   231,   132,    95,   228,    97,   111,
     239,   199,   233,   119,   242,   112,   130,   113,   114,   120,
     237,   123,   131,   243,   134,   247,   122,   110,     0,   124,
     207,   118,   240,   199,   129,    96,     0,   133,     0,   244,
       0,    69,    52,    53,    86,    87,     0,   229,   121,   227,
      54,   132,    55,    70,     0,   230,     0,    58,   246,   199,
       0,    54,     0,    55,    70,    56,    57,     0,    58,    59,
      52,    53,   114,    89,     0,   124,   113,   173,     0,     0,
     122,     0,     0,   133,     0,    52,    53,     0,     0,    54,
       0,    55,    70,    56,    57,   245,    58,    59,   105,     0,
     189,   190,   191,   192,    54,   170,    55,     0,    56,    57,
       0,    58,    59,     0,     0,    86,    87,    54,     0,    55,
     144,    56,    57,    88,     0,     0,   105,    78,   106,   193,
       0,     0,    54,   166,    55,    70,    54,    79,    55,    58,
      52,    53,    86,    87,    89,    54,     0,    55,   157,    56,
      57,     0,   152,     0,     0,     0,   106,     0,     0,    54,
       0,    55,    70,    56,    57,    19,    58,    59,    20,     0,
       0,    89,     0,     0,    21,    22,     0,    23,    24,    25,
      26,    27,    28,    29,    30,    86,    87,     0,     0,    52,
      53,     0,     0,    88,    31,    32,     0,     0,    52,    53,
       0,     0,    54,     0,    55,    70,     0,     0,    54,    58,
      55,    70,    56,    57,    89,    58,    59,    54,    69,    55,
       0,    56,    57,     0,    58,    59,    78,    54,     0,    55,
      70,     0,     0,     0,    58,    54,    79,    55,     0,     0,
       0,     0,     0,   148,   213,   214,   215,   216,   217,   218,
     219,   220
};

static const short yycheck[] =
{
      23,    51,    25,    81,    27,    28,   107,    91,   116,    32,
      61,    38,    39,     3,    41,   127,    51,    38,    39,    38,
      41,    48,    41,    50,    51,    72,    28,    48,    30,    50,
      51,    50,    51,     0,    13,    39,   104,    60,    17,    48,
      37,    50,    51,    39,    40,    72,    39,    40,    71,    41,
      37,    72,    37,    72,    81,    39,    40,    80,    50,    51,
      81,   173,   170,    37,    91,   166,    39,    90,    39,    40,
      91,    37,    91,   157,   152,    51,    99,   127,    37,   102,
     107,    37,    50,    51,   107,    37,   107,    37,    48,   116,
      50,    51,   115,   144,    37,   116,    37,   116,   107,    91,
     127,   148,    39,   126,    39,    40,   127,   116,   127,   177,
     178,   179,    39,    40,    37,   183,    37,   185,   127,   187,
      37,   148,    37,   173,   116,   152,    37,   148,    50,   148,
     157,   152,    39,    40,    39,   127,   157,    51,   157,   166,
      39,    40,    39,   170,    39,   166,   173,   107,   116,   170,
      41,   170,   173,    39,   173,    39,   116,   166,    -1,   127,
     176,   170,    39,    40,   173,   157,    -1,   127,    -1,   237,
      -1,    15,     5,     6,     7,     8,    -1,   200,   170,   195,
      24,   173,    26,    27,    -1,   201,    -1,    31,    39,    40,
      -1,    24,    -1,    26,    27,    28,    29,    -1,    31,    32,
       5,     6,   170,    36,    -1,   173,   166,    40,    -1,    -1,
     170,    -1,    -1,   173,    -1,     5,     6,    -1,    -1,    24,
      -1,    26,    27,    28,    29,   241,    31,    32,     5,    -1,
       9,    10,    11,    12,    24,    40,    26,    -1,    28,    29,
      -1,    31,    32,    -1,    -1,     7,     8,    24,    -1,    26,
      40,    28,    29,    15,    -1,    -1,     5,    15,    35,    38,
      -1,    -1,    24,    40,    26,    27,    24,    25,    26,    31,
       5,     6,     7,     8,    36,    24,    -1,    26,    40,    28,
      29,    -1,    40,    -1,    -1,    -1,    35,    -1,    -1,    24,
      -1,    26,    27,    28,    29,     4,    31,    32,     7,    -1,
      -1,    36,    -1,    -1,    13,    14,    -1,    16,    17,    18,
      19,    20,    21,    22,    23,     7,     8,    -1,    -1,     5,
       6,    -1,    -1,    15,    33,    34,    -1,    -1,     5,     6,
      -1,    -1,    24,    -1,    26,    27,    -1,    -1,    24,    31,
      26,    27,    28,    29,    36,    31,    32,    24,    15,    26,
      -1,    28,    29,    -1,    31,    32,    15,    24,    -1,    26,
      27,    -1,    -1,    -1,    31,    24,    25,    26,    -1,    -1,
      -1,    -1,    -1,    40,    42,    43,    44,    45,    46,    47,
      48,    49
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     3,    58,    59,    68,    77,    82,    83,    90,    96,
     101,   106,   107,   111,   112,   113,   114,   118,   119,     4,
       7,    13,    14,    16,    17,    18,    19,    20,    21,    22,
      23,    33,    34,     0,    13,    17,    60,   134,    78,    84,
     134,    69,   134,   108,   134,   134,   115,   120,    91,   134,
     102,    97,     5,     6,    24,    26,    28,    29,    31,    32,
      61,    62,    63,    64,   121,   122,   124,   128,   130,    15,
      27,    79,    80,    81,   122,   124,   126,   130,    15,    25,
      85,    86,    87,    88,   122,   124,     7,     8,    15,    36,
      70,    71,    72,    73,   122,   124,   126,   130,    37,   109,
     110,    37,   116,   117,    37,     5,    35,    92,    93,    94,
     121,   122,   124,   128,    64,   103,   104,   105,   121,   122,
     124,   126,   128,   130,    64,    73,    98,    99,   100,   121,
     122,   124,   126,   128,   130,    65,    66,   123,   125,   129,
      37,   131,    67,   134,    40,    63,   127,   134,    40,    81,
      89,   134,    40,    87,    37,    75,   134,    40,    72,    51,
     134,    51,   133,   134,   133,    95,    40,    93,   134,   134,
      40,   105,   134,    40,   100,    37,    37,    37,    37,    37,
      28,    30,    37,    37,    63,    37,    81,    37,    87,     9,
      10,    11,    12,    38,    76,    37,    72,    39,    39,    40,
      39,    37,    93,   105,   100,    56,    57,    56,   133,   133,
     133,    39,    39,    42,    43,    44,    45,    46,    47,    48,
      49,   132,   133,   133,   133,    39,    40,    56,    51,   134,
      56,    39,    50,    39,    39,    39,    39,    41,    39,    39,
      39,    74,    39,    39,   133,    56,    39,    39
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

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
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
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
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
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
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
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



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

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
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
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

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

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
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
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

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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
        case 2:
#line 79 "ompparser.yy"
    { omp_parse_expr(); ;}
    break;

  case 3:
#line 79 "ompparser.yy"
    { if (!addExpression((const char*)yyvsp[0])) YYABORT; ;}
    break;

  case 20:
#line 102 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_parallel,gNode);
		    omptype = e_parallel; 
		  ;}
    break;

  case 33:
#line 128 "ompparser.yy"
    { 
                        ompattribute->addClause(e_if);
                        omptype = e_if;
		     ;}
    break;

  case 35:
#line 133 "ompparser.yy"
    { 
                    ompattribute->addClause(e_num_threads);       
		    omptype = e_num_threads;
		   ;}
    break;

  case 37:
#line 138 "ompparser.yy"
    { ompattribute->addClause(e_copyin);
		    omptype = e_copyin;
		  ;}
    break;

  case 39:
#line 144 "ompparser.yy"
    { 
                   ompattribute = buildOmpAttribute(e_for,gNode); 
                   ;}
    break;

  case 51:
#line 164 "ompparser.yy"
    { 
                           ompattribute->addClause(e_nowait);
                           //Not correct since nowait cannot have expression or var_list
                           //omptype =e_nowait;
			 ;}
    break;

  case 52:
#line 172 "ompparser.yy"
    { 
			    ompattribute->addClause(e_ordered_clause);
			  ;}
    break;

  case 53:
#line 176 "ompparser.yy"
    { 
                    ompattribute->addClause(e_schedule);
                    ompattribute->setScheduleKind(static_cast<omp_construct_enum>(yyvsp[-1]));
		    omptype = e_schedule;
		  ;}
    break;

  case 54:
#line 182 "ompparser.yy"
    { 
                    ompattribute->addClause(e_schedule);
		    ompattribute->setScheduleKind(static_cast<omp_construct_enum>(yyvsp[-1]));
		    omptype = e_schedule;
		  ;}
    break;

  case 56:
#line 188 "ompparser.yy"
    {
		    ompattribute->addClause(e_collapse);
		    omptype = e_collapse;
		  ;}
    break;

  case 58:
#line 195 "ompparser.yy"
    { yyval = e_schedule_static; ;}
    break;

  case 59:
#line 196 "ompparser.yy"
    { yyval = e_schedule_dynamic; ;}
    break;

  case 60:
#line 197 "ompparser.yy"
    { yyval = e_schedule_guided; ;}
    break;

  case 61:
#line 198 "ompparser.yy"
    { yyval = e_schedule_auto; ;}
    break;

  case 62:
#line 199 "ompparser.yy"
    { yyval = e_schedule_runtime; ;}
    break;

  case 63:
#line 204 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_sections,gNode); ;}
    break;

  case 74:
#line 223 "ompparser.yy"
    { 
			   ompattribute->addClause(e_nowait);
			 ;}
    break;

  case 75:
#line 230 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_section,gNode); ;}
    break;

  case 76:
#line 234 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_single,gNode); 
		    omptype = e_single; ;}
    break;

  case 86:
#line 253 "ompparser.yy"
    { 
                            ompattribute->addClause(e_nowait);
			 ;}
    break;

  case 87:
#line 258 "ompparser.yy"
    { ompattribute->addClause(e_copyprivate);
			  omptype = e_copyprivate; ;}
    break;

  case 89:
#line 263 "ompparser.yy"
    {ompattribute = buildOmpAttribute(e_task,gNode);
		  omptype = e_task; ;}
    break;

  case 99:
#line 281 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_if, gNode);
		    omptype = e_if; ;}
    break;

  case 101:
#line 285 "ompparser.yy"
    {
		   ompattribute->addClause(e_untied);
		  ;}
    break;

  case 102:
#line 291 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_parallel_for,gNode); ;}
    break;

  case 117:
#line 319 "ompparser.yy"
    { ompattribute =buildOmpAttribute(e_parallel_sections,gNode); 
		    omptype = e_parallel_sections; ;}
    break;

  case 131:
#line 346 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_master, gNode);;}
    break;

  case 132:
#line 351 "ompparser.yy"
    {
                  ompattribute = buildOmpAttribute(e_critical, gNode); 
                  ;}
    break;

  case 136:
#line 366 "ompparser.yy"
    { 
                      ompattribute->setCriticalName((const char*)yyvsp[-1]);
                    ;}
    break;

  case 137:
#line 373 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_barrier,gNode); ;}
    break;

  case 138:
#line 377 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_taskwait, gNode); ;}
    break;

  case 139:
#line 381 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_atomic,gNode); ;}
    break;

  case 140:
#line 385 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_flush,gNode);
	           omptype = e_flush; ;}
    break;

  case 145:
#line 399 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_ordered_directive,gNode); ;}
    break;

  case 146:
#line 404 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_threadprivate,gNode); 
                    omptype = e_threadprivate; ;}
    break;

  case 148:
#line 411 "ompparser.yy"
    { 
		    ompattribute->addClause(e_default);
		    ompattribute->setDefaultValue(e_default_shared); 
		  ;}
    break;

  case 149:
#line 416 "ompparser.yy"
    {
		    ompattribute->addClause(e_default);
		    ompattribute->setDefaultValue(e_default_none);
		  ;}
    break;

  case 150:
#line 422 "ompparser.yy"
    { ompattribute->addClause(e_private); omptype = e_private;;}
    break;

  case 152:
#line 427 "ompparser.yy"
    { ompattribute->addClause(e_firstprivate); 
		    omptype = e_firstprivate;;}
    break;

  case 154:
#line 433 "ompparser.yy"
    { ompattribute->addClause(e_lastprivate); 
		    omptype = e_lastprivate;;}
    break;

  case 156:
#line 439 "ompparser.yy"
    { ompattribute->addClause(e_shared); omptype = e_shared; ;}
    break;

  case 158:
#line 444 "ompparser.yy"
    { ompattribute->addClause(e_reduction); omptype = e_reduction;;}
    break;

  case 160:
#line 449 "ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_plus); ;}
    break;

  case 161:
#line 450 "ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_mul);  ;}
    break;

  case 162:
#line 451 "ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_minus); ;}
    break;

  case 163:
#line 452 "ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_bitand);  ;}
    break;

  case 164:
#line 453 "ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_bitxor);  ;}
    break;

  case 165:
#line 454 "ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_bitor);  ;}
    break;

  case 166:
#line 455 "ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_logand);  ;}
    break;

  case 167:
#line 456 "ompparser.yy"
    { ompattribute->setReductionOperator(e_reduction_logor); ;}
    break;

  case 168:
#line 466 "ompparser.yy"
    { if (!addVar((const char*)yyvsp[0])) YYABORT; ;}
    break;

  case 169:
#line 467 "ompparser.yy"
    { if (!addVar((const char*)yyvsp[0])) YYABORT; ;}
    break;


    }

/* Line 1000 of yacc.c.  */
#line 1743 "ompparser.cc"

  yyvsp -= yylen;
  yyssp -= yylen;


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
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval);
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

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

  yyvsp -= yylen;
  yyssp -= yylen;
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

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


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
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 473 "ompparser.yy"


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

/* // do it in OmpAttribute::addVariable() instead
static std::auto_ptr<SgNode> resolveVar(const char* var) {
       
	std::auto_ptr<SgNode> node(gNode->resolveVariable(var));

	if (!node.get())
		printf("No such var <%s>!\n", var);
	return node;
}
*/

static bool addVar(const char* var) {
	ompattribute->addVariable(omptype,var);
	return true;
}

// The ROSE's string-based AST construction is not stable,
// pass expression as string for now, Liao
static bool addExpression(const char* expr) {
  //TODO parse the expression into Sage AST subtree
        ompattribute->addExpression(omptype,std::string(expr),NULL);
	return true;
}


/**
 * @file
 * Parser for OpenMP-pragmas.
 */

#if 0
int main()
{
        yyparse();
        printf("you win!\n");
        return (0);
}
#endif

/* The following rules have been removed from the standard-grammar because
   they specify how to extend the C++ grammar with OpenMP pragmas in some
   allowed contexts only. In ROSE we cannot change the C++ grammar (EDG front-end).
   Therefore, the context-tests, which are specified by the following rules have to
   be performed on the ROSE AST.

// extended in above grammar with all openmp-directives
openmp-directive: barrier-directive
		| flush-directive
		;

statement: .. standard statements ..
	openmp-construct
	;

openmp-construct: parallel-construct 
		| for-construct 
		| sections-construct 
		| single-construct 
		| parallel-for-construct 
		| parallel-sections-construct 
		| master-construct 
		| critical-construct 
		| atomic-construct 
		| ordered-construct
		;

structured-block: statement 
		; 
parallel-construct
		: parallel-directive structured-block 
		;

for-construct	: for-directive iteration-statement
		;

sections-construct
		: sections-directive section-scope
		;

section-scope	: '{' section-sequence '}' 
		;

section-sequence: section-directiveopt structured-block 
		| section-sequence section-directive structured-block 
		;

section-directiveopt
		: // empty 
		| section-directive
		;

single-construct: single-directive structured-block
		;


parallel-for-construct
		: parallel-for-directive iteration-statement
		;

parallel-sections-construct
		: parallel-sections-directive 
		| section-scope
		;

master-construct: master-directive structured-block
		;

critical-construct
		: critical-directive structured-block
		;

atomic-construct: atomic-directive expression-statement
		;

ordered-construct
		: ordered-directive structured-block
		;

declaration	: -- standard declarations --
		| threadprivate-directive
		;

*/


