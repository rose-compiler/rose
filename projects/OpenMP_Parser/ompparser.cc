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
     AUTO = 268,
     SECTIONS = 269,
     SINGLE = 270,
     NOWAIT = 271,
     SECTION = 272,
     FOR = 273,
     MASTER = 274,
     CRITICAL = 275,
     BARRIER = 276,
     ATOMIC = 277,
     FLUSH = 278,
     THREADPRIVATE = 279,
     PRIVATE = 280,
     COPYPRIVATE = 281,
     FIRSTPRIVATE = 282,
     LASTPRIVATE = 283,
     SHARED = 284,
     DEFAULT = 285,
     NONE = 286,
     REDUCTION = 287,
     COPYIN = 288,
     LOGAND = 289,
     LOGOR = 290,
     EXPRESSION = 291,
     ID_EXPRESSION = 292,
     IDENTIFIER = 293,
     NEWLINE = 294,
     LEXICALERROR = 295
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
#define AUTO 268
#define SECTIONS 269
#define SINGLE 270
#define NOWAIT 271
#define SECTION 272
#define FOR 273
#define MASTER 274
#define CRITICAL 275
#define BARRIER 276
#define ATOMIC 277
#define FLUSH 278
#define THREADPRIVATE 279
#define PRIVATE 280
#define COPYPRIVATE 281
#define FIRSTPRIVATE 282
#define LASTPRIVATE 283
#define SHARED 284
#define DEFAULT 285
#define NONE 286
#define REDUCTION 287
#define COPYIN 288
#define LOGAND 289
#define LOGOR 290
#define EXPRESSION 291
#define ID_EXPRESSION 292
#define IDENTIFIER 293
#define NEWLINE 294
#define LEXICALERROR 295




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
#line 231 "ompparser.cc"

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
#define YYFINAL  29
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   341

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  51
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  66
/* YYNRULES -- Number of rules. */
#define YYNRULES  130
/* YYNRULES -- Number of states. */
#define YYNSTATES  198

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   295

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    41,     2,
      33,    35,    39,    38,    36,    40,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    37,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    42,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    43,     2,     2,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    32,    34,    44,
      45,    46,    47,    48,    49,    50
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    11,    13,    15,    17,
      19,    21,    23,    25,    27,    29,    31,    33,    35,    36,
      42,    43,    45,    47,    50,    54,    56,    58,    59,    65,
      66,    72,    73,    79,    80,    82,    84,    87,    91,    93,
      95,    97,    99,   104,   105,   113,   115,   117,   119,   121,
     123,   124,   130,   131,   133,   135,   138,   142,   144,   146,
     150,   151,   157,   158,   160,   162,   165,   169,   171,   173,
     174,   181,   182,   184,   186,   189,   193,   195,   197,   199,
     200,   207,   208,   210,   212,   215,   219,   221,   223,   227,
     228,   234,   235,   237,   241,   245,   249,   250,   256,   257,
     259,   263,   267,   268,   276,   277,   283,   284,   290,   291,
     297,   298,   304,   305,   311,   316,   321,   322,   330,   331,
     337,   339,   341,   343,   345,   347,   349,   351,   353,   355,
     359
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      54,     0,    -1,    -1,    53,    46,    -1,    97,    -1,    99,
      -1,    55,    -1,    63,    -1,    71,    -1,    77,    -1,    82,
      -1,    87,    -1,    92,    -1,    93,    -1,    98,    -1,   103,
      -1,   104,    -1,    76,    -1,    -1,     3,     4,    56,    57,
     116,    -1,    -1,    58,    -1,    59,    -1,    58,    59,    -1,
      58,    36,    59,    -1,    60,    -1,   106,    -1,    -1,     5,
      61,    33,    52,    35,    -1,    -1,     6,    62,    33,    52,
      35,    -1,    -1,     3,    18,    64,    65,   116,    -1,    -1,
      66,    -1,    67,    -1,    66,    67,    -1,    66,    36,    67,
      -1,    68,    -1,   106,    -1,    16,    -1,     7,    -1,     8,
      33,    70,    35,    -1,    -1,     8,    33,    70,    36,    69,
      52,    35,    -1,     9,    -1,    10,    -1,    11,    -1,    12,
      -1,    13,    -1,    -1,     3,    14,    72,    73,   116,    -1,
      -1,    74,    -1,    75,    -1,    74,    75,    -1,    74,    36,
      75,    -1,   106,    -1,    16,    -1,     3,    17,   116,    -1,
      -1,     3,    15,    78,    79,   116,    -1,    -1,    80,    -1,
      81,    -1,    80,    81,    -1,    80,    36,    81,    -1,   106,
      -1,    16,    -1,    -1,     3,     4,    18,    83,    84,   116,
      -1,    -1,    85,    -1,    86,    -1,    85,    86,    -1,    85,
      36,    86,    -1,    60,    -1,    68,    -1,   106,    -1,    -1,
       3,     4,    14,    88,    89,   116,    -1,    -1,    90,    -1,
      91,    -1,    90,    91,    -1,    90,    36,    91,    -1,    60,
      -1,   106,    -1,     3,    19,   116,    -1,    -1,     3,    20,
      94,    95,   116,    -1,    -1,    96,    -1,    33,    47,    35,
      -1,     3,    21,   116,    -1,     3,    22,   116,    -1,    -1,
       3,    23,   100,   101,   116,    -1,    -1,   102,    -1,    33,
     115,    35,    -1,     3,     7,   116,    -1,    -1,     3,    24,
     105,    33,   115,    35,   116,    -1,    -1,    25,   107,    33,
     115,    35,    -1,    -1,    26,   108,    33,   115,    35,    -1,
      -1,    27,   109,    33,   115,    35,    -1,    -1,    28,   110,
      33,   115,    35,    -1,    -1,    29,   111,    33,   115,    35,
      -1,    30,    33,    29,    35,    -1,    30,    33,    31,    35,
      -1,    -1,    32,   112,    33,   114,    37,   115,    35,    -1,
      -1,    34,   113,    33,   115,    35,    -1,    38,    -1,    39,
      -1,    40,    -1,    41,    -1,    42,    -1,    43,    -1,    44,
      -1,    45,    -1,    47,    -1,   115,    36,    47,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,    78,    78,    78,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    98,    97,
     103,   105,   109,   110,   111,   114,   115,   119,   119,   124,
     123,   132,   131,   138,   139,   142,   143,   144,   147,   148,
     149,   157,   160,   167,   166,   174,   175,   176,   177,   178,
     183,   182,   187,   189,   193,   194,   195,   198,   199,   205,
     210,   209,   214,   216,   220,   221,   222,   225,   226,   233,
     232,   237,   239,   243,   244,   245,   249,   250,   251,   256,
     255,   262,   264,   268,   269,   270,   274,   275,   278,   284,
     283,   290,   291,   298,   305,   309,   314,   313,   319,   320,
     323,   327,   333,   332,   339,   338,   342,   341,   345,   344,
     348,   347,   351,   350,   353,   356,   360,   359,   363,   362,
     368,   369,   370,   371,   372,   373,   374,   375,   385,   386,
     389
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "OMP", "PARALLEL", "IF", "NUM_THREADS",
  "ORDERED", "SCHEDULE", "STATIC", "DYNAMIC", "GUIDED", "RUNTIME", "AUTO",
  "SECTIONS", "SINGLE", "NOWAIT", "SECTION", "FOR", "MASTER", "CRITICAL",
  "BARRIER", "ATOMIC", "FLUSH", "THREADPRIVATE", "PRIVATE", "COPYPRIVATE",
  "FIRSTPRIVATE", "LASTPRIVATE", "SHARED", "DEFAULT", "NONE", "REDUCTION",
  "'('", "COPYIN", "')'", "','", "':'", "'+'", "'*'", "'-'", "'&'", "'^'",
  "'|'", "LOGAND", "LOGOR", "EXPRESSION", "ID_EXPRESSION", "IDENTIFIER",
  "NEWLINE", "LEXICALERROR", "$accept", "expression", "@1",
  "openmp_directive", "parallel_directive", "@2", "parallel_clauseoptseq",
  "parallel_clause_seq", "parallel_clause", "unique_parallel_clause", "@3",
  "@4", "for_directive", "@5", "for_clauseoptseq", "for_clause_seq",
  "for_clause", "unique_for_clause", "@6", "schedule_kind",
  "sections_directive", "@7", "sections_clauseoptseq",
  "sections_clause_seq", "sections_clause", "section_directive",
  "single_directive", "@8", "single_clauseoptseq", "single_clause_seq",
  "single_clause", "parallel_for_directive", "@9",
  "parallel_for_clauseoptseq", "parallel_for_clause_seq",
  "parallel_for_clause", "parallel_sections_directive", "@10",
  "parallel_sections_clauseoptseq", "parallel_sections_clause_seq",
  "parallel_sections_clause", "master_directive", "critical_directive",
  "@11", "region_phraseopt", "region_phrase", "barrier_directive",
  "atomic_directive", "flush_directive", "@12", "flush_varsopt",
  "flush_vars", "ordered_directive", "threadprivate_directive", "@13",
  "data_clause", "@14", "@15", "@16", "@17", "@18", "@19", "@20",
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
     285,   286,   287,    40,   288,    41,    44,    58,    43,    42,
      45,    38,    94,   124,   289,   290,   291,   292,   293,   294,
     295
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    51,    53,    52,    54,    54,    54,    54,    54,    54,
      54,    54,    54,    54,    54,    54,    54,    54,    56,    55,
      57,    57,    58,    58,    58,    59,    59,    61,    60,    62,
      60,    64,    63,    65,    65,    66,    66,    66,    67,    67,
      67,    68,    68,    69,    68,    70,    70,    70,    70,    70,
      72,    71,    73,    73,    74,    74,    74,    75,    75,    76,
      78,    77,    79,    79,    80,    80,    80,    81,    81,    83,
      82,    84,    84,    85,    85,    85,    86,    86,    86,    88,
      87,    89,    89,    90,    90,    90,    91,    91,    92,    94,
      93,    95,    95,    96,    97,    98,   100,    99,   101,   101,
     102,   103,   105,   104,   107,   106,   108,   106,   109,   106,
     110,   106,   111,   106,   106,   106,   112,   106,   113,   106,
     114,   114,   114,   114,   114,   114,   114,   114,   115,   115,
     116
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     5,
       0,     1,     1,     2,     3,     1,     1,     0,     5,     0,
       5,     0,     5,     0,     1,     1,     2,     3,     1,     1,
       1,     1,     4,     0,     7,     1,     1,     1,     1,     1,
       0,     5,     0,     1,     1,     2,     3,     1,     1,     3,
       0,     5,     0,     1,     1,     2,     3,     1,     1,     0,
       6,     0,     1,     1,     2,     3,     1,     1,     1,     0,
       6,     0,     1,     1,     2,     3,     1,     1,     3,     0,
       5,     0,     1,     3,     3,     3,     0,     5,     0,     1,
       3,     3,     0,     7,     0,     5,     0,     5,     0,     5,
       0,     5,     0,     5,     4,     4,     0,     7,     0,     5,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     0,     0,     6,     7,     8,    17,     9,    10,    11,
      12,    13,     4,    14,     5,    15,    16,    18,   130,    50,
      60,   130,    31,   130,    89,   130,   130,    96,   102,     1,
      79,    69,    20,   101,    52,    62,    59,    33,    88,    91,
      94,    95,    98,     0,    81,    71,    27,    29,   104,   106,
     108,   110,   112,     0,   116,   118,   130,    21,    22,    25,
      26,    58,   130,    53,    54,    57,    68,   130,    63,    64,
      67,    41,     0,    40,   130,    34,    35,    38,    39,     0,
     130,    92,     0,   130,    99,     0,    86,   130,    82,    83,
      87,    76,    77,   130,    72,    73,    78,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    19,     0,    23,
      51,     0,    55,    61,     0,    65,     0,    32,     0,    36,
       0,    90,   128,     0,    97,     0,    80,     0,    84,    70,
       0,    74,     2,     2,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    24,    56,    66,    45,    46,    47,    48,
      49,     0,    37,    93,   100,     0,   130,    85,    75,     0,
       0,     0,     0,     0,     0,     0,     0,   114,   115,   120,
     121,   122,   123,   124,   125,   126,   127,     0,     0,    42,
      43,   129,   103,    28,     3,    30,   105,   107,   109,   111,
     113,     0,   119,     2,     0,     0,   117,    44
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,   159,   160,     2,     3,    32,    56,    57,    58,    59,
      97,    98,     4,    37,    74,    75,    76,    77,   193,   151,
       5,    34,    62,    63,    64,     6,     7,    35,    67,    68,
      69,     8,    45,    93,    94,    95,     9,    44,    87,    88,
      89,    10,    11,    39,    80,    81,    12,    13,    14,    42,
      83,    84,    15,    16,    43,    60,    99,   100,   101,   102,
     103,   105,   106,   177,   123,    33
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -121
static const short yypact[] =
{
      14,   253,    22,  -121,  -121,  -121,  -121,  -121,  -121,  -121,
    -121,  -121,  -121,  -121,  -121,  -121,  -121,     1,  -121,  -121,
    -121,  -121,  -121,  -121,  -121,  -121,  -121,  -121,  -121,  -121,
    -121,  -121,   196,  -121,   292,   307,  -121,   224,  -121,    -4,
    -121,  -121,     3,    12,   196,   156,  -121,  -121,  -121,  -121,
    -121,  -121,  -121,    18,  -121,  -121,  -121,   112,  -121,  -121,
    -121,  -121,  -121,   262,  -121,  -121,  -121,  -121,   277,  -121,
    -121,  -121,    32,  -121,  -121,   184,  -121,  -121,  -121,   -13,
    -121,  -121,     8,  -121,  -121,     8,  -121,  -121,   144,  -121,
    -121,  -121,  -121,  -121,   100,  -121,  -121,    54,    57,    58,
      60,    61,    62,    63,   -15,    65,    66,  -121,   196,  -121,
    -121,   292,  -121,  -121,   307,  -121,    15,  -121,   224,  -121,
      67,  -121,  -121,    -3,  -121,     2,  -121,   196,  -121,  -121,
     156,  -121,  -121,  -121,     8,     8,     8,     8,     8,    68,
      74,   113,     8,  -121,  -121,  -121,  -121,  -121,  -121,  -121,
    -121,     4,  -121,  -121,  -121,    64,  -121,  -121,  -121,    75,
      55,    77,     7,    28,    41,    43,    46,  -121,  -121,  -121,
    -121,  -121,  -121,  -121,  -121,  -121,  -121,    78,    48,  -121,
    -121,  -121,  -121,  -121,  -121,  -121,  -121,  -121,  -121,  -121,
    -121,     8,  -121,  -121,    50,    79,  -121,  -121
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
    -121,  -120,  -121,  -121,  -121,  -121,  -121,  -121,   -48,   -38,
    -121,  -121,  -121,  -121,  -121,  -121,   -74,   -42,  -121,  -121,
    -121,  -121,  -121,  -121,   -53,  -121,  -121,  -121,  -121,  -121,
     -57,  -121,  -121,  -121,  -121,   -82,  -121,  -121,  -121,  -121,
     -80,  -121,  -121,  -121,  -121,  -121,  -121,  -121,  -121,  -121,
    -121,  -121,  -121,  -121,  -121,   -14,  -121,  -121,  -121,  -121,
    -121,  -121,  -121,  -121,   -67,   -21
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      36,   119,    38,    92,    40,    41,    86,    91,   128,   109,
     112,   115,   131,   161,   139,    30,   140,     1,   125,    31,
      65,    70,    29,    78,   146,   147,   148,   149,   150,    79,
      90,    96,   154,   155,   120,   107,    82,   156,   155,   179,
     180,   110,   186,   155,   152,    85,   113,   157,   158,    65,
      86,   104,    92,   117,    70,   122,    91,   145,   144,   121,
     143,    78,   124,   187,   155,   116,   126,   162,   163,   164,
     165,   166,   129,   195,    90,   178,   188,   155,   189,   155,
      96,   190,   155,   192,   155,   196,   155,   132,    92,    86,
     133,   134,    91,   135,   136,   137,   138,    65,   141,   142,
      70,   184,   153,   167,    78,    46,    47,    71,    72,   168,
     183,   181,   185,    90,   197,   191,    96,    46,    47,     0,
       0,     0,     0,     0,   194,    48,    49,    50,    51,    52,
      53,     0,    54,     0,    55,   182,   130,    48,    49,    50,
      51,    52,    53,     0,    54,     0,    55,     0,   108,    46,
      47,   169,   170,   171,   172,   173,   174,   175,   176,     0,
       0,    46,    47,    71,    72,     0,     0,     0,     0,    48,
      49,    50,    51,    52,    53,     0,    54,     0,    55,     0,
     127,    48,    49,    50,    51,    52,    53,     0,    54,     0,
      55,    71,    72,     0,     0,     0,     0,     0,     0,     0,
      73,    46,    47,     0,     0,     0,     0,     0,     0,    48,
      49,    50,    51,    52,    53,     0,    54,     0,    55,     0,
     118,    48,    49,    50,    51,    52,    53,     0,    54,     0,
      55,    71,    72,     0,     0,     0,     0,     0,     0,     0,
      73,     0,     0,     0,     0,     0,     0,     0,     0,    48,
      49,    50,    51,    52,    53,     0,    54,    17,    55,     0,
      18,     0,     0,     0,     0,     0,     0,    19,    20,     0,
      21,    22,    23,    24,    25,    26,    27,    28,    61,     0,
       0,     0,     0,     0,     0,     0,     0,    48,    49,    50,
      51,    52,    53,    66,    54,     0,    55,     0,   111,     0,
       0,     0,    48,    49,    50,    51,    52,    53,    61,    54,
       0,    55,     0,   114,     0,     0,     0,    48,    49,    50,
      51,    52,    53,    66,    54,     0,    55,     0,     0,     0,
       0,     0,    48,    49,    50,    51,    52,    53,     0,    54,
       0,    55
};

static const short yycheck[] =
{
      21,    75,    23,    45,    25,    26,    44,    45,    88,    57,
      63,    68,    94,   133,    29,    14,    31,     3,    85,    18,
      34,    35,     0,    37,     9,    10,    11,    12,    13,    33,
      44,    45,    35,    36,    47,    56,    33,    35,    36,    35,
      36,    62,    35,    36,   118,    33,    67,   127,   130,    63,
      88,    33,    94,    74,    68,    47,    94,   114,   111,    80,
     108,    75,    83,    35,    36,    33,    87,   134,   135,   136,
     137,   138,    93,   193,    88,   142,    35,    36,    35,    36,
      94,    35,    36,    35,    36,    35,    36,    33,   130,   127,
      33,    33,   130,    33,    33,    33,    33,   111,    33,    33,
     114,    46,    35,    35,   118,     5,     6,     7,     8,    35,
      35,    47,    35,   127,    35,    37,   130,     5,     6,    -1,
      -1,    -1,    -1,    -1,   191,    25,    26,    27,    28,    29,
      30,    -1,    32,    -1,    34,   156,    36,    25,    26,    27,
      28,    29,    30,    -1,    32,    -1,    34,    -1,    36,     5,
       6,    38,    39,    40,    41,    42,    43,    44,    45,    -1,
      -1,     5,     6,     7,     8,    -1,    -1,    -1,    -1,    25,
      26,    27,    28,    29,    30,    -1,    32,    -1,    34,    -1,
      36,    25,    26,    27,    28,    29,    30,    -1,    32,    -1,
      34,     7,     8,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      16,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    25,
      26,    27,    28,    29,    30,    -1,    32,    -1,    34,    -1,
      36,    25,    26,    27,    28,    29,    30,    -1,    32,    -1,
      34,     7,     8,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      16,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    25,
      26,    27,    28,    29,    30,    -1,    32,     4,    34,    -1,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    14,    15,    -1,
      17,    18,    19,    20,    21,    22,    23,    24,    16,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    25,    26,    27,
      28,    29,    30,    16,    32,    -1,    34,    -1,    36,    -1,
      -1,    -1,    25,    26,    27,    28,    29,    30,    16,    32,
      -1,    34,    -1,    36,    -1,    -1,    -1,    25,    26,    27,
      28,    29,    30,    16,    32,    -1,    34,    -1,    -1,    -1,
      -1,    -1,    25,    26,    27,    28,    29,    30,    -1,    32,
      -1,    34
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     3,    54,    55,    63,    71,    76,    77,    82,    87,
      92,    93,    97,    98,    99,   103,   104,     4,     7,    14,
      15,    17,    18,    19,    20,    21,    22,    23,    24,     0,
      14,    18,    56,   116,    72,    78,   116,    64,   116,    94,
     116,   116,   100,   105,    88,    83,     5,     6,    25,    26,
      27,    28,    29,    30,    32,    34,    57,    58,    59,    60,
     106,    16,    73,    74,    75,   106,    16,    79,    80,    81,
     106,     7,     8,    16,    65,    66,    67,    68,   106,    33,
      95,    96,    33,   101,   102,    33,    60,    89,    90,    91,
     106,    60,    68,    84,    85,    86,   106,    61,    62,   107,
     108,   109,   110,   111,    33,   112,   113,   116,    36,    59,
     116,    36,    75,   116,    36,    81,    33,   116,    36,    67,
      47,   116,    47,   115,   116,   115,   116,    36,    91,   116,
      36,    86,    33,    33,    33,    33,    33,    33,    33,    29,
      31,    33,    33,    59,    75,    81,     9,    10,    11,    12,
      13,    70,    67,    35,    35,    36,    35,    91,    86,    52,
      53,    52,   115,   115,   115,   115,   115,    35,    35,    38,
      39,    40,    41,    42,    43,    44,    45,   114,   115,    35,
      36,    47,   116,    35,    46,    35,    35,    35,    35,    35,
      35,    37,    35,    69,   115,    52,    35,    35
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
#line 78 "ompparser.yy"
    { omp_parse_expr(); ;}
    break;

  case 3:
#line 78 "ompparser.yy"
    { if (!addExpression((const char*)yyvsp[0])) YYABORT; ;}
    break;

  case 18:
#line 98 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_parallel,gNode);
				  omptype = e_parallel; ;}
    break;

  case 27:
#line 119 "ompparser.yy"
    { 
                        ompattribute->addClause(e_if);
                        omptype = e_if;
		     ;}
    break;

  case 29:
#line 124 "ompparser.yy"
    { 
                    ompattribute->addClause(e_num_threads);       
		    omptype = e_num_threads;
		   ;}
    break;

  case 31:
#line 132 "ompparser.yy"
    { 
                                  ompattribute = buildOmpAttribute(e_for,gNode); 
                                ;}
    break;

  case 40:
#line 149 "ompparser.yy"
    { 
                           ompattribute->addClause(e_nowait);
                           //Not correct since nowait cannot have expression or var_list
                           //omptype =e_nowait;
			 ;}
    break;

  case 41:
#line 157 "ompparser.yy"
    { 
			    ompattribute->addClause(e_ordered_clause);
			  ;}
    break;

  case 42:
#line 161 "ompparser.yy"
    { 
                    ompattribute->addClause(e_schedule);
                    ompattribute->setScheduleKind(static_cast<omp_construct_enum>(yyvsp[-1]));
		    omptype = e_schedule;
		  ;}
    break;

  case 43:
#line 167 "ompparser.yy"
    { 
                    ompattribute->addClause(e_schedule);
		    ompattribute->setScheduleKind(static_cast<omp_construct_enum>(yyvsp[-1]));
		    omptype = e_schedule;
		  ;}
    break;

  case 45:
#line 174 "ompparser.yy"
    { yyval = e_schedule_static; ;}
    break;

  case 46:
#line 175 "ompparser.yy"
    { yyval = e_schedule_dynamic; ;}
    break;

  case 47:
#line 176 "ompparser.yy"
    { yyval = e_schedule_guided; ;}
    break;

  case 48:
#line 177 "ompparser.yy"
    { yyval = e_schedule_runtime; ;}
    break;

  case 49:
#line 178 "ompparser.yy"
    { yyval = e_schedule_auto; ;}
    break;

  case 50:
#line 183 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_sections,gNode); ;}
    break;

  case 58:
#line 199 "ompparser.yy"
    { 
			   ompattribute->addClause(e_nowait);
			 ;}
    break;

  case 59:
#line 206 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_section,gNode); ;}
    break;

  case 60:
#line 210 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_single,gNode); ;}
    break;

  case 68:
#line 226 "ompparser.yy"
    { 
                            ompattribute->addClause(e_nowait);
			 ;}
    break;

  case 69:
#line 233 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_parallel_for,gNode); ;}
    break;

  case 79:
#line 256 "ompparser.yy"
    { ompattribute =buildOmpAttribute(e_parallel_sections,gNode); 
				  omptype = e_parallel_sections; 
                                 ;}
    break;

  case 88:
#line 279 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_master, gNode);;}
    break;

  case 89:
#line 284 "ompparser.yy"
    {
                                  ompattribute = buildOmpAttribute(e_critical, gNode); 
                                ;}
    break;

  case 93:
#line 299 "ompparser.yy"
    { 
                      ompattribute->setCriticalName((const char*)yyvsp[-1]);
                    ;}
    break;

  case 94:
#line 306 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_barrier,gNode); ;}
    break;

  case 95:
#line 310 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_atomic,gNode); ;}
    break;

  case 96:
#line 314 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_flush,gNode);
				omptype = e_flush; ;}
    break;

  case 101:
#line 328 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_ordered_directive,gNode); ;}
    break;

  case 102:
#line 333 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_threadprivate,gNode); 
                                  omptype = e_threadprivate; ;}
    break;

  case 104:
#line 339 "ompparser.yy"
    { ompattribute->addClause(e_private); omptype = e_private;;}
    break;

  case 106:
#line 342 "ompparser.yy"
    { ompattribute->addClause(e_copyprivate); omptype = e_copyprivate;;}
    break;

  case 108:
#line 345 "ompparser.yy"
    { ompattribute->addClause(e_firstprivate); omptype = e_firstprivate;;}
    break;

  case 110:
#line 348 "ompparser.yy"
    { ompattribute->addClause(e_lastprivate), omptype = e_lastprivate;;}
    break;

  case 112:
#line 351 "ompparser.yy"
    { ompattribute->addClause(e_shared); omptype = e_shared; ;}
    break;

  case 114:
#line 354 "ompparser.yy"
    { ompattribute->addClause(e_default); 
                    ompattribute->setDefaultValue(e_default_shared); ;}
    break;

  case 115:
#line 357 "ompparser.yy"
    { ompattribute->addClause(e_default);
                    ompattribute->setDefaultValue(e_default_none);;}
    break;

  case 116:
#line 360 "ompparser.yy"
    { ompattribute->addClause(e_reduction); omptype = e_reduction;;}
    break;

  case 118:
#line 363 "ompparser.yy"
    { ompattribute->addClause(e_copyin); omptype = e_copyin;;}
    break;

  case 120:
#line 368 "ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_plus); ;}
    break;

  case 121:
#line 369 "ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_mul);  ;}
    break;

  case 122:
#line 370 "ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_minus); ;}
    break;

  case 123:
#line 371 "ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_bitand);  ;}
    break;

  case 124:
#line 372 "ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_bitxor);  ;}
    break;

  case 125:
#line 373 "ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_bitor);  ;}
    break;

  case 126:
#line 374 "ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_logand);  ;}
    break;

  case 127:
#line 375 "ompparser.yy"
    { ompattribute->setReductionOperator(e_reduction_logor); ;}
    break;

  case 128:
#line 385 "ompparser.yy"
    { if (!addVar((const char*)yyvsp[0])) YYABORT; ;}
    break;

  case 129:
#line 386 "ompparser.yy"
    { if (!addVar((const char*)yyvsp[0])) YYABORT; ;}
    break;


    }

/* Line 1000 of yacc.c.  */
#line 1637 "ompparser.cc"

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


#line 392 "ompparser.yy"


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


