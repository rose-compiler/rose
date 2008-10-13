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
     NEWLINE = 309,
     LEXICALERROR = 310,
     IDENTIFIER = 311,
     EXPRESSION = 312,
     ID_EXPRESSION = 313,
     ICONSTANT = 314
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
#define NEWLINE 309
#define LEXICALERROR 310
#define IDENTIFIER 311
#define EXPRESSION 312
#define ID_EXPRESSION 313
#define ICONSTANT 314




/* Copy the first part of user declarations.  */
#line 8 "ompparser.yy"

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include "rose.h" // Sage Interface and Builders
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
#line 65 "ompparser.yy"
typedef union YYSTYPE {  int itype;
          double ftype;
          const char* stype;
          void* ptype; /* For expressions */
        } YYSTYPE;
/* Line 191 of yacc.c.  */
#line 266 "ompparser.cc"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 278 "ompparser.cc"

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
#define YYLAST   384

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  73
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  83
/* YYNRULES -- Number of rules. */
#define YYNRULES  191
/* YYNRULES -- Number of states. */
#define YYNSTATES  287

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   314

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
      71,    70,    72,     2,     2,     2,     2,     2,     2,     2,
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
      65,    66,    67,    68,    69
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    13,    15,    17,
      19,    21,    23,    25,    27,    29,    31,    33,    35,    36,
      42,    43,    45,    47,    50,    54,    56,    58,    60,    62,
      64,    66,    67,    73,    74,    80,    81,    87,    88,    94,
      95,    97,    99,   102,   106,   108,   110,   112,   114,   116,
     118,   120,   125,   126,   134,   135,   141,   143,   145,   147,
     149,   151,   152,   158,   159,   161,   163,   166,   170,   172,
     174,   176,   178,   180,   184,   185,   191,   192,   194,   196,
     199,   203,   205,   207,   209,   211,   212,   218,   219,   225,
     227,   230,   234,   236,   238,   240,   242,   244,   245,   251,
     253,   254,   261,   262,   264,   266,   269,   273,   275,   277,
     279,   281,   283,   285,   287,   289,   290,   297,   298,   300,
     302,   305,   309,   311,   313,   315,   317,   319,   321,   323,
     327,   328,   334,   335,   337,   341,   345,   349,   353,   354,
     360,   361,   363,   367,   371,   372,   380,   385,   390,   391,
     397,   398,   404,   405,   411,   412,   418,   419,   427,   429,
     431,   433,   435,   437,   439,   441,   443,   445,   447,   451,
     455,   459,   463,   467,   471,   475,   479,   483,   487,   491,
     493,   497,   501,   503,   507,   511,   515,   519,   521,   523,
     525,   529
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
      74,     0,    -1,    75,    -1,    84,    -1,    93,    -1,    99,
      -1,   112,    -1,   117,    -1,   106,    -1,   122,    -1,   123,
      -1,   129,    -1,   134,    -1,   127,    -1,   128,    -1,   130,
      -1,   135,    -1,    98,    -1,    -1,     3,     4,    76,    77,
     155,    -1,    -1,    78,    -1,    79,    -1,    78,    79,    -1,
      78,    40,    79,    -1,    80,    -1,   137,    -1,   138,    -1,
     140,    -1,   144,    -1,   146,    -1,    -1,     5,    81,    37,
     149,    39,    -1,    -1,     6,    82,    37,   149,    39,    -1,
      -1,    32,    83,    37,   154,    39,    -1,    -1,     3,    17,
      85,    86,   155,    -1,    -1,    87,    -1,    88,    -1,    87,
      88,    -1,    87,    40,    88,    -1,    89,    -1,   138,    -1,
     140,    -1,   142,    -1,   146,    -1,    15,    -1,     7,    -1,
       8,    37,    92,    39,    -1,    -1,     8,    37,    92,    40,
      90,   149,    39,    -1,    -1,    36,    91,    37,   149,    39,
      -1,     9,    -1,    10,    -1,    11,    -1,    38,    -1,    12,
      -1,    -1,     3,    13,    94,    95,   155,    -1,    -1,    96,
      -1,    97,    -1,    96,    97,    -1,    96,    40,    97,    -1,
     138,    -1,   140,    -1,   142,    -1,   146,    -1,    15,    -1,
       3,    16,   155,    -1,    -1,     3,    14,   100,   101,   155,
      -1,    -1,   102,    -1,   103,    -1,   102,   103,    -1,   102,
      40,   103,    -1,   104,    -1,   138,    -1,   140,    -1,    15,
      -1,    -1,    25,   105,    37,   154,    39,    -1,    -1,     3,
      33,   107,   108,   155,    -1,   109,    -1,   108,   109,    -1,
     108,    40,   109,    -1,   110,    -1,   137,    -1,   138,    -1,
     140,    -1,   144,    -1,    -1,     5,   111,    37,   149,    39,
      -1,    35,    -1,    -1,     3,     4,    17,   113,   114,   155,
      -1,    -1,   115,    -1,   116,    -1,   115,   116,    -1,   115,
      40,   116,    -1,    80,    -1,    89,    -1,   137,    -1,   138,
      -1,   140,    -1,   142,    -1,   144,    -1,   146,    -1,    -1,
       3,     4,    13,   118,   119,   155,    -1,    -1,   120,    -1,
     121,    -1,   120,   121,    -1,   120,    40,   121,    -1,    80,
      -1,   137,    -1,   138,    -1,   140,    -1,   142,    -1,   144,
      -1,   146,    -1,     3,    18,   155,    -1,    -1,     3,    19,
     124,   125,   155,    -1,    -1,   126,    -1,    37,    68,    39,
      -1,     3,    20,   155,    -1,     3,    34,   155,    -1,     3,
      21,   155,    -1,    -1,     3,    22,   131,   132,   155,    -1,
      -1,   133,    -1,    37,   154,    39,    -1,     3,     7,   155,
      -1,    -1,     3,    23,   136,    37,   154,    39,   155,    -1,
      29,    37,    28,    39,    -1,    29,    37,    30,    39,    -1,
      -1,    24,   139,    37,   154,    39,    -1,    -1,    26,   141,
      37,   154,    39,    -1,    -1,    27,   143,    37,   154,    39,
      -1,    -1,    28,   145,    37,   154,    39,    -1,    -1,    31,
     147,    37,   148,    41,   154,    39,    -1,    42,    -1,    43,
      -1,    44,    -1,    45,    -1,    46,    -1,    47,    -1,    48,
      -1,    49,    -1,   150,    -1,   151,    -1,   153,    70,   150,
      -1,   153,    54,   150,    -1,   153,    55,   150,    -1,   153,
      56,   150,    -1,   153,    57,   150,    -1,   153,    58,   150,
      -1,   153,    59,   150,    -1,   153,    60,   150,    -1,   153,
      61,   150,    -1,   153,    62,   150,    -1,   153,    63,   150,
      -1,   152,    -1,   151,    52,   152,    -1,   151,    53,   152,
      -1,   153,    -1,   152,    71,   153,    -1,   152,    72,   153,
      -1,   152,    50,   153,    -1,   152,    51,   153,    -1,    69,
      -1,    68,    -1,    68,    -1,   154,    40,    68,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   101,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   121,   120,
     127,   129,   133,   134,   135,   138,   139,   140,   141,   142,
     143,   147,   147,   153,   152,   159,   158,   165,   164,   171,
     172,   175,   176,   177,   180,   181,   182,   183,   184,   185,
     193,   196,   203,   202,   210,   209,   218,   219,   220,   221,
     222,   227,   226,   231,   233,   237,   238,   239,   242,   243,
     244,   245,   246,   252,   257,   256,   262,   264,   268,   269,
     270,   273,   274,   275,   276,   281,   280,   286,   285,   291,
     292,   293,   296,   297,   298,   299,   300,   304,   303,   308,
     315,   314,   319,   321,   325,   326,   327,   331,   332,   333,
     334,   335,   336,   337,   338,   343,   342,   348,   350,   354,
     355,   356,   360,   361,   362,   363,   364,   365,   366,   369,
     375,   374,   381,   382,   389,   396,   400,   404,   409,   408,
     414,   415,   418,   422,   428,   427,   434,   439,   446,   445,
     451,   450,   457,   456,   463,   462,   468,   467,   473,   474,
     475,   476,   477,   478,   479,   480,   487,   491,   492,   499,
     506,   513,   520,   527,   535,   542,   549,   557,   564,   574,
     575,   582,   592,   593,   601,   608,   615,   626,   629,   642,
     643,   646
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
  "LOGOR", "LE_OP2", "GE_OP2", "EQ_OP2", "NE_OP2", "RIGHT_ASSIGN2",
  "LEFT_ASSIGN2", "ADD_ASSIGN2", "SUB_ASSIGN2", "MUL_ASSIGN2",
  "DIV_ASSIGN2", "MOD_ASSIGN2", "AND_ASSIGN2", "XOR_ASSIGN2", "OR_ASSIGN2",
  "NEWLINE", "LEXICALERROR", "IDENTIFIER", "EXPRESSION", "ID_EXPRESSION",
  "ICONSTANT", "'='", "'<'", "'>'", "$accept", "openmp_directive",
  "parallel_directive", "@1", "parallel_clause_optseq",
  "parallel_clause_seq", "parallel_clause", "unique_parallel_clause", "@2",
  "@3", "@4", "for_directive", "@5", "for_clause_optseq", "for_clause_seq",
  "for_clause", "unique_for_clause", "@6", "@7", "schedule_kind",
  "sections_directive", "@8", "sections_clause_optseq",
  "sections_clause_seq", "sections_clause", "section_directive",
  "single_directive", "@9", "single_clause_optseq", "single_clause_seq",
  "single_clause", "unique_single_clause", "@10", "task_directive", "@11",
  "task_clause_optseq", "task_clause", "unique_task_clause", "@12",
  "parallel_for_directive", "@13", "parallel_for_clauseoptseq",
  "parallel_for_clause_seq", "parallel_for_clause",
  "parallel_sections_directive", "@14", "parallel_sections_clause_optseq",
  "parallel_sections_clause_seq", "parallel_sections_clause",
  "master_directive", "critical_directive", "@15", "region_phraseopt",
  "region_phrase", "barrier_directive", "taskwait_directive",
  "atomic_directive", "flush_directive", "@16", "flush_varsopt",
  "flush_vars", "ordered_directive", "threadprivate_directive", "@17",
  "data_default_clause", "data_privatization_clause", "@18",
  "data_privatization_in_clause", "@19", "data_privatization_out_clause",
  "@20", "data_sharing_clause", "@21", "data_reduction_clause", "@22",
  "reduction_operator", "expression", "assignment_expr", "equality_expr",
  "relational_expr", "unary_expr", "variable_list", "new_line", 0
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
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
      61,    60,    62
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    73,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    76,    75,
      77,    77,    78,    78,    78,    79,    79,    79,    79,    79,
      79,    81,    80,    82,    80,    83,    80,    85,    84,    86,
      86,    87,    87,    87,    88,    88,    88,    88,    88,    88,
      89,    89,    90,    89,    91,    89,    92,    92,    92,    92,
      92,    94,    93,    95,    95,    96,    96,    96,    97,    97,
      97,    97,    97,    98,   100,    99,   101,   101,   102,   102,
     102,   103,   103,   103,   103,   105,   104,   107,   106,   108,
     108,   108,   109,   109,   109,   109,   109,   111,   110,   110,
     113,   112,   114,   114,   115,   115,   115,   116,   116,   116,
     116,   116,   116,   116,   116,   118,   117,   119,   119,   120,
     120,   120,   121,   121,   121,   121,   121,   121,   121,   122,
     124,   123,   125,   125,   126,   127,   128,   129,   131,   130,
     132,   132,   133,   134,   136,   135,   137,   137,   139,   138,
     141,   140,   143,   142,   145,   144,   147,   146,   148,   148,
     148,   148,   148,   148,   148,   148,   149,   150,   150,   150,
     150,   150,   150,   150,   150,   150,   150,   150,   150,   151,
     151,   151,   152,   152,   152,   152,   152,   153,   153,   154,
     154,   155
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     5,
       0,     1,     1,     2,     3,     1,     1,     1,     1,     1,
       1,     0,     5,     0,     5,     0,     5,     0,     5,     0,
       1,     1,     2,     3,     1,     1,     1,     1,     1,     1,
       1,     4,     0,     7,     0,     5,     1,     1,     1,     1,
       1,     0,     5,     0,     1,     1,     2,     3,     1,     1,
       1,     1,     1,     3,     0,     5,     0,     1,     1,     2,
       3,     1,     1,     1,     1,     0,     5,     0,     5,     1,
       2,     3,     1,     1,     1,     1,     1,     0,     5,     1,
       0,     6,     0,     1,     1,     2,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     6,     0,     1,     1,
       2,     3,     1,     1,     1,     1,     1,     1,     1,     3,
       0,     5,     0,     1,     3,     3,     3,     3,     0,     5,
       0,     1,     3,     3,     0,     7,     4,     4,     0,     5,
       0,     5,     0,     5,     0,     5,     0,     7,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     1,
       3,     3,     1,     3,     3,     3,     3,     1,     1,     1,
       3,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     0,     0,     2,     3,     4,    17,     5,     8,     6,
       7,     9,    10,    13,    14,    11,    15,    12,    16,    18,
     191,    61,    74,   191,    37,   191,   130,   191,   191,   138,
     144,    87,   191,     1,   115,   100,    20,   143,    63,    76,
      73,    39,   129,   132,   135,   137,   140,     0,     0,   136,
     117,   102,    31,    33,   148,   150,   154,     0,   156,    35,
     191,    21,    22,    25,    26,    27,    28,    29,    30,    72,
     152,   191,    64,    65,    68,    69,    70,    71,    84,    85,
     191,    77,    78,    81,    82,    83,    50,     0,    49,    54,
     191,    40,    41,    44,    45,    46,    47,    48,     0,   191,
     133,     0,   191,   141,     0,    97,    99,   191,    89,    92,
      93,    94,    95,    96,   122,   191,   118,   119,   123,   124,
     125,   126,   127,   128,   107,   108,   191,   103,   104,   109,
     110,   111,   112,   113,   114,     0,     0,     0,     0,     0,
       0,     0,     0,    19,     0,    23,     0,    62,     0,    66,
       0,    75,     0,    79,     0,     0,    38,     0,    42,     0,
     131,   189,     0,   139,     0,     0,     0,    90,    88,   116,
       0,   120,   101,     0,   105,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    24,     0,    67,     0,    80,    56,
      57,    58,    60,    59,     0,     0,    43,   134,   142,     0,
     191,     0,    91,   121,   106,   188,   187,     0,   166,   167,
     179,   182,     0,     0,     0,     0,   146,   147,   158,   159,
     160,   161,   162,   163,   164,   165,     0,     0,     0,     0,
      51,    52,     0,   190,   145,     0,    32,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    34,   149,   151,   155,     0,    36,
     153,    86,     0,    55,    98,   180,   182,   181,   185,   186,
     183,   184,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   178,   168,     0,     0,   157,    53
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     2,     3,    36,    60,    61,    62,    63,   135,   136,
     142,     4,    41,    90,    91,    92,    93,   262,   155,   194,
       5,    38,    71,    72,    73,     6,     7,    39,    80,    81,
      82,    83,   150,     8,    48,   107,   108,   109,   165,     9,
      51,   126,   127,   128,    10,    50,   115,   116,   117,    11,
      12,    43,    99,   100,    13,    14,    15,    16,    46,   102,
     103,    17,    18,    47,    64,    65,   137,    66,   138,    76,
     146,    67,   139,    68,   141,   226,   207,   208,   209,   210,
     211,   162,    37
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -124
static const short yypact[] =
{
      15,   287,    10,  -124,  -124,  -124,  -124,  -124,  -124,  -124,
    -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,     7,
    -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,
    -124,  -124,  -124,  -124,  -124,  -124,   310,  -124,   298,   238,
    -124,   304,  -124,    -6,  -124,  -124,    -1,     2,    96,  -124,
     252,   261,  -124,  -124,  -124,  -124,  -124,    12,  -124,  -124,
    -124,   210,  -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,
    -124,  -124,   204,  -124,  -124,  -124,  -124,  -124,  -124,  -124,
    -124,   322,  -124,  -124,  -124,  -124,  -124,    14,  -124,  -124,
    -124,   225,  -124,  -124,  -124,  -124,  -124,  -124,   -47,  -124,
    -124,   -13,  -124,  -124,   -13,  -124,  -124,   219,  -124,  -124,
    -124,  -124,  -124,  -124,  -124,  -124,   180,  -124,  -124,  -124,
    -124,  -124,  -124,  -124,  -124,  -124,  -124,   165,  -124,  -124,
    -124,  -124,  -124,  -124,  -124,    21,    23,    26,    29,    36,
     -15,    58,    67,  -124,   310,  -124,    81,  -124,   298,  -124,
      84,  -124,   238,  -124,   104,    97,  -124,   304,  -124,    31,
    -124,  -124,     5,  -124,    70,   110,    96,  -124,  -124,  -124,
     252,  -124,  -124,   261,  -124,    60,    60,   -13,   -13,   -13,
      68,    98,   335,   -13,  -124,   -13,  -124,   -13,  -124,  -124,
    -124,  -124,  -124,  -124,   106,    60,  -124,  -124,  -124,    75,
    -124,    60,  -124,  -124,  -124,  -124,  -124,   128,  -124,   100,
     108,   295,   135,   143,   148,   159,  -124,  -124,  -124,  -124,
    -124,  -124,  -124,  -124,  -124,  -124,   137,   163,   183,   186,
    -124,  -124,   151,  -124,  -124,   156,  -124,    60,    60,    60,
      60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    60,    60,  -124,  -124,  -124,  -124,   -13,  -124,
    -124,  -124,    60,  -124,  -124,   108,  -124,   108,  -124,  -124,
    -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,
    -124,  -124,  -124,   256,   161,  -124,  -124
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
    -124,  -124,  -124,  -124,  -124,  -124,   -55,    -4,  -124,  -124,
    -124,  -124,  -124,  -124,  -124,   -64,   -40,  -124,  -124,  -124,
    -124,  -124,  -124,  -124,   -71,  -124,  -124,  -124,  -124,  -124,
     -78,  -124,  -124,  -124,  -124,  -124,   -95,  -124,  -124,  -124,
    -124,  -124,  -124,  -119,  -124,  -124,  -124,  -124,  -102,  -124,
    -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,
    -124,  -124,  -124,  -124,   -10,   -22,  -124,   -16,  -124,    11,
    -124,    48,  -124,    -8,  -124,  -124,  -123,   123,  -124,    61,
      33,   -97,   -23
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned short yytable[] =
{
      40,   149,    42,   153,    44,    45,   145,   164,   174,    49,
      33,   125,   167,   180,   171,   181,    74,    84,     1,    94,
      34,   159,    75,    85,    35,    95,   111,   158,   119,   130,
      77,    98,   112,    97,   120,   131,   101,   143,   110,   104,
     118,   129,   123,   134,   198,   199,   114,   124,   147,   140,
      74,   154,    96,   212,   204,   161,    75,   151,   175,    84,
     176,   121,   132,   177,    77,    85,   178,   156,   203,    94,
     197,   202,   232,   179,   188,    95,   160,   186,   235,   163,
     213,   214,   215,    97,   168,   111,   227,   125,   228,   184,
     229,   112,   169,   196,   119,   182,   113,   110,   122,   133,
     120,   105,    96,   172,   183,   130,   118,   216,   123,   200,
     199,   131,   114,   189,   190,   191,   192,   129,   185,   134,
      54,   187,    55,   124,    56,    57,    74,   121,   205,   206,
      84,   106,    75,   125,   195,    94,    85,   217,   132,   284,
      77,    95,   193,   233,   111,   230,   231,   201,   119,    97,
     112,   130,   237,   238,   120,   113,   110,   131,   239,   240,
     118,   283,   123,   129,   122,   134,   114,   236,    96,   124,
      52,    53,    86,    87,   254,   133,     0,   234,   258,   241,
     242,   121,   255,   199,   132,    52,    53,   256,   199,    54,
     263,    55,    70,    56,    57,   264,    58,    59,   257,   199,
     286,    89,   259,   199,    54,   173,    55,    70,    56,    57,
       0,    58,    59,     0,   113,    52,    53,     0,   122,    69,
     170,   133,   260,   199,   105,   261,   199,     0,    54,     0,
      55,    70,    86,    87,    54,    58,    55,     0,    56,    57,
      88,    58,    59,    54,   148,    55,     0,    56,    57,    54,
     144,    55,    70,    78,   106,     0,    58,    52,    53,   166,
       0,    89,    54,    79,    55,   157,    52,    53,    86,    87,
     266,   266,   268,   269,   270,   271,    54,     0,    55,    70,
      56,    57,     0,    58,    59,    54,     0,    55,    70,    56,
      57,    19,    58,    59,    20,   285,   199,    89,   265,   267,
      21,    22,     0,    23,    24,    25,    26,    27,    28,    29,
      30,    86,    87,    69,     0,    52,    53,     0,     0,    88,
      31,    32,    54,     0,    55,    70,     0,     0,    54,    58,
      55,    70,     0,     0,    54,    58,    55,    78,    56,    57,
      89,    58,    59,     0,     0,     0,    54,    79,    55,   243,
     244,   245,   246,   247,   248,   249,   250,   251,   252,     0,
       0,     0,   152,     0,     0,   253,   272,   273,   274,   275,
     276,   277,   278,   279,   280,   281,   282,   218,   219,   220,
     221,   222,   223,   224,   225
};

static const short yycheck[] =
{
      23,    72,    25,    81,    27,    28,    61,   104,   127,    32,
       0,    51,   107,    28,   116,    30,    38,    39,     3,    41,
      13,    68,    38,    39,    17,    41,    48,    91,    50,    51,
      38,    37,    48,    41,    50,    51,    37,    60,    48,    37,
      50,    51,    50,    51,    39,    40,    50,    51,    71,    37,
      72,    37,    41,   176,   173,    68,    72,    80,    37,    81,
      37,    50,    51,    37,    72,    81,    37,    90,   170,    91,
      39,   166,   195,    37,   152,    91,    99,   148,   201,   102,
     177,   178,   179,    91,   107,   107,   183,   127,   185,   144,
     187,   107,   115,   157,   116,    37,    48,   107,    50,    51,
     116,     5,    91,   126,    37,   127,   116,    39,   116,    39,
      40,   127,   116,     9,    10,    11,    12,   127,    37,   127,
      24,    37,    26,   127,    28,    29,   148,   116,    68,    69,
     152,    35,   148,   173,    37,   157,   152,    39,   127,   262,
     148,   157,    38,    68,   166,    39,    40,    37,   170,   157,
     166,   173,    52,    53,   170,   107,   166,   173,    50,    51,
     170,   258,   170,   173,   116,   173,   170,    39,   157,   173,
       5,     6,     7,     8,    39,   127,    -1,   200,    41,    71,
      72,   170,    39,    40,   173,     5,     6,    39,    40,    24,
      39,    26,    27,    28,    29,    39,    31,    32,    39,    40,
      39,    36,    39,    40,    24,    40,    26,    27,    28,    29,
      -1,    31,    32,    -1,   166,     5,     6,    -1,   170,    15,
      40,   173,    39,    40,     5,    39,    40,    -1,    24,    -1,
      26,    27,     7,     8,    24,    31,    26,    -1,    28,    29,
      15,    31,    32,    24,    40,    26,    -1,    28,    29,    24,
      40,    26,    27,    15,    35,    -1,    31,     5,     6,    40,
      -1,    36,    24,    25,    26,    40,     5,     6,     7,     8,
     237,   238,   239,   240,   241,   242,    24,    -1,    26,    27,
      28,    29,    -1,    31,    32,    24,    -1,    26,    27,    28,
      29,     4,    31,    32,     7,    39,    40,    36,   237,   238,
      13,    14,    -1,    16,    17,    18,    19,    20,    21,    22,
      23,     7,     8,    15,    -1,     5,     6,    -1,    -1,    15,
      33,    34,    24,    -1,    26,    27,    -1,    -1,    24,    31,
      26,    27,    -1,    -1,    24,    31,    26,    15,    28,    29,
      36,    31,    32,    -1,    -1,    -1,    24,    25,    26,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    -1,
      -1,    -1,    40,    -1,    -1,    70,   243,   244,   245,   246,
     247,   248,   249,   250,   251,   252,   253,    42,    43,    44,
      45,    46,    47,    48,    49
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     3,    74,    75,    84,    93,    98,    99,   106,   112,
     117,   122,   123,   127,   128,   129,   130,   134,   135,     4,
       7,    13,    14,    16,    17,    18,    19,    20,    21,    22,
      23,    33,    34,     0,    13,    17,    76,   155,    94,   100,
     155,    85,   155,   124,   155,   155,   131,   136,   107,   155,
     118,   113,     5,     6,    24,    26,    28,    29,    31,    32,
      77,    78,    79,    80,   137,   138,   140,   144,   146,    15,
      27,    95,    96,    97,   138,   140,   142,   146,    15,    25,
     101,   102,   103,   104,   138,   140,     7,     8,    15,    36,
      86,    87,    88,    89,   138,   140,   142,   146,    37,   125,
     126,    37,   132,   133,    37,     5,    35,   108,   109,   110,
     137,   138,   140,   144,    80,   119,   120,   121,   137,   138,
     140,   142,   144,   146,    80,    89,   114,   115,   116,   137,
     138,   140,   142,   144,   146,    81,    82,   139,   141,   145,
      37,   147,    83,   155,    40,    79,   143,   155,    40,    97,
     105,   155,    40,   103,    37,    91,   155,    40,    88,    68,
     155,    68,   154,   155,   154,   111,    40,   109,   155,   155,
      40,   121,   155,    40,   116,    37,    37,    37,    37,    37,
      28,    30,    37,    37,    79,    37,    97,    37,   103,     9,
      10,    11,    12,    38,    92,    37,    88,    39,    39,    40,
      39,    37,   109,   121,   116,    68,    69,   149,   150,   151,
     152,   153,   149,   154,   154,   154,    39,    39,    42,    43,
      44,    45,    46,    47,    48,    49,   148,   154,   154,   154,
      39,    40,   149,    68,   155,   149,    39,    52,    53,    50,
      51,    71,    72,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    70,    39,    39,    39,    39,    41,    39,
      39,    39,    90,    39,    39,   152,   153,   152,   153,   153,
     153,   153,   150,   150,   150,   150,   150,   150,   150,   150,
     150,   150,   150,   154,   149,    39,    39
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
        case 18:
#line 121 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_parallel,gNode);
		    omptype = e_parallel; 
		  ;}
    break;

  case 31:
#line 147 "ompparser.yy"
    { 
                        ompattribute->addClause(e_if);
                        omptype = e_if;
		     ;}
    break;

  case 32:
#line 151 "ompparser.yy"
    { addExpression("");;}
    break;

  case 33:
#line 153 "ompparser.yy"
    { 
                    ompattribute->addClause(e_num_threads);       
		    omptype = e_num_threads;
		   ;}
    break;

  case 34:
#line 157 "ompparser.yy"
    { addExpression("");;}
    break;

  case 35:
#line 159 "ompparser.yy"
    { ompattribute->addClause(e_copyin);
		    omptype = e_copyin;
		  ;}
    break;

  case 37:
#line 165 "ompparser.yy"
    { 
                   ompattribute = buildOmpAttribute(e_for,gNode); 
                   ;}
    break;

  case 49:
#line 185 "ompparser.yy"
    { 
                           ompattribute->addClause(e_nowait);
                           //Not correct since nowait cannot have expression or var_list
                           //omptype =e_nowait;
			 ;}
    break;

  case 50:
#line 193 "ompparser.yy"
    { 
			    ompattribute->addClause(e_ordered_clause);
			  ;}
    break;

  case 51:
#line 197 "ompparser.yy"
    { 
                    ompattribute->addClause(e_schedule);
                    ompattribute->setScheduleKind(static_cast<omp_construct_enum>(yyvsp[-1].itype));
		    omptype = e_schedule;
		  ;}
    break;

  case 52:
#line 203 "ompparser.yy"
    { 
                    ompattribute->addClause(e_schedule);
		    ompattribute->setScheduleKind(static_cast<omp_construct_enum>(yyvsp[-1].itype));
		    omptype = e_schedule;
		  ;}
    break;

  case 53:
#line 208 "ompparser.yy"
    { addExpression("");;}
    break;

  case 54:
#line 210 "ompparser.yy"
    {
		    ompattribute->addClause(e_collapse);
		    omptype = e_collapse;
		  ;}
    break;

  case 55:
#line 215 "ompparser.yy"
    { addExpression("");;}
    break;

  case 56:
#line 218 "ompparser.yy"
    { yyval.itype = e_schedule_static; ;}
    break;

  case 57:
#line 219 "ompparser.yy"
    { yyval.itype = e_schedule_dynamic; ;}
    break;

  case 58:
#line 220 "ompparser.yy"
    { yyval.itype = e_schedule_guided; ;}
    break;

  case 59:
#line 221 "ompparser.yy"
    { yyval.itype = e_schedule_auto; ;}
    break;

  case 60:
#line 222 "ompparser.yy"
    { yyval.itype = e_schedule_runtime; ;}
    break;

  case 61:
#line 227 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_sections,gNode); ;}
    break;

  case 72:
#line 246 "ompparser.yy"
    { 
			   ompattribute->addClause(e_nowait);
			 ;}
    break;

  case 73:
#line 253 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_section,gNode); ;}
    break;

  case 74:
#line 257 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_single,gNode); 
		    omptype = e_single; ;}
    break;

  case 84:
#line 276 "ompparser.yy"
    { 
                            ompattribute->addClause(e_nowait);
			 ;}
    break;

  case 85:
#line 281 "ompparser.yy"
    { ompattribute->addClause(e_copyprivate);
			  omptype = e_copyprivate; ;}
    break;

  case 87:
#line 286 "ompparser.yy"
    {ompattribute = buildOmpAttribute(e_task,gNode);
		  omptype = e_task; ;}
    break;

  case 97:
#line 304 "ompparser.yy"
    { ompattribute->addClause(e_if);
		    omptype = e_if; ;}
    break;

  case 98:
#line 307 "ompparser.yy"
    { addExpression("");;}
    break;

  case 99:
#line 309 "ompparser.yy"
    {
		   ompattribute->addClause(e_untied);
		  ;}
    break;

  case 100:
#line 315 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_parallel_for,gNode); ;}
    break;

  case 115:
#line 343 "ompparser.yy"
    { ompattribute =buildOmpAttribute(e_parallel_sections,gNode); 
		    omptype = e_parallel_sections; ;}
    break;

  case 129:
#line 370 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_master, gNode);;}
    break;

  case 130:
#line 375 "ompparser.yy"
    {
                  ompattribute = buildOmpAttribute(e_critical, gNode); 
                  ;}
    break;

  case 134:
#line 390 "ompparser.yy"
    { 
                      ompattribute->setCriticalName((const char*)yyvsp[-1].stype);
                    ;}
    break;

  case 135:
#line 397 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_barrier,gNode); ;}
    break;

  case 136:
#line 401 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_taskwait, gNode); ;}
    break;

  case 137:
#line 405 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_atomic,gNode); ;}
    break;

  case 138:
#line 409 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_flush,gNode);
	           omptype = e_flush; ;}
    break;

  case 143:
#line 423 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_ordered_directive,gNode); ;}
    break;

  case 144:
#line 428 "ompparser.yy"
    { ompattribute = buildOmpAttribute(e_threadprivate,gNode); 
                    omptype = e_threadprivate; ;}
    break;

  case 146:
#line 435 "ompparser.yy"
    { 
		    ompattribute->addClause(e_default);
		    ompattribute->setDefaultValue(e_default_shared); 
		  ;}
    break;

  case 147:
#line 440 "ompparser.yy"
    {
		    ompattribute->addClause(e_default);
		    ompattribute->setDefaultValue(e_default_none);
		  ;}
    break;

  case 148:
#line 446 "ompparser.yy"
    { ompattribute->addClause(e_private); omptype = e_private;;}
    break;

  case 150:
#line 451 "ompparser.yy"
    { ompattribute->addClause(e_firstprivate); 
		    omptype = e_firstprivate;;}
    break;

  case 152:
#line 457 "ompparser.yy"
    { ompattribute->addClause(e_lastprivate); 
		    omptype = e_lastprivate;;}
    break;

  case 154:
#line 463 "ompparser.yy"
    { ompattribute->addClause(e_shared); omptype = e_shared; ;}
    break;

  case 156:
#line 468 "ompparser.yy"
    { ompattribute->addClause(e_reduction); omptype = e_reduction;;}
    break;

  case 158:
#line 473 "ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_plus); ;}
    break;

  case 159:
#line 474 "ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_mul);  ;}
    break;

  case 160:
#line 475 "ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_minus); ;}
    break;

  case 161:
#line 476 "ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_bitand);  ;}
    break;

  case 162:
#line 477 "ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_bitxor);  ;}
    break;

  case 163:
#line 478 "ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_bitor);  ;}
    break;

  case 164:
#line 479 "ompparser.yy"
    {ompattribute->setReductionOperator(e_reduction_logand);  ;}
    break;

  case 165:
#line 480 "ompparser.yy"
    { ompattribute->setReductionOperator(e_reduction_logor); ;}
    break;

  case 168:
#line 493 "ompparser.yy"
    {
          current_exp = SageBuilder::buildAssignOp(
                      (SgExpression*)(yyvsp[-2].ptype),
                      (SgExpression*)(yyvsp[0].ptype)); 
          yyval.ptype = current_exp;
	;}
    break;

  case 169:
#line 500 "ompparser.yy"
    {
          current_exp = SageBuilder::buildRshiftAssignOp(
                      (SgExpression*)(yyvsp[-2].ptype),
                      (SgExpression*)(yyvsp[0].ptype)); 
          yyval.ptype = current_exp;
	;}
    break;

  case 170:
#line 507 "ompparser.yy"
    {
          current_exp = SageBuilder::buildLshiftAssignOp(
                      (SgExpression*)(yyvsp[-2].ptype),
                      (SgExpression*)(yyvsp[0].ptype)); 
          yyval.ptype = current_exp;
	;}
    break;

  case 171:
#line 514 "ompparser.yy"
    {
          current_exp = SageBuilder::buildPlusAssignOp(
                      (SgExpression*)(yyvsp[-2].ptype),
                      (SgExpression*)(yyvsp[0].ptype)); 
          yyval.ptype = current_exp;
	;}
    break;

  case 172:
#line 521 "ompparser.yy"
    {
          current_exp = SageBuilder::buildMinusAssignOp(
                      (SgExpression*)(yyvsp[-2].ptype),
                      (SgExpression*)(yyvsp[0].ptype)); 
          yyval.ptype = current_exp;
	;}
    break;

  case 173:
#line 528 "ompparser.yy"
    {
          current_exp = SageBuilder::buildMultAssignOp(
                      (SgExpression*)(yyvsp[-2].ptype),
                      (SgExpression*)(yyvsp[0].ptype)); 
          yyval.ptype = current_exp;
	;}
    break;

  case 174:
#line 536 "ompparser.yy"
    {
          current_exp = SageBuilder::buildDivAssignOp(
                      (SgExpression*)(yyvsp[-2].ptype),
                      (SgExpression*)(yyvsp[0].ptype)); 
          yyval.ptype = current_exp;
	;}
    break;

  case 175:
#line 543 "ompparser.yy"
    {
          current_exp = SageBuilder::buildModAssignOp(
                      (SgExpression*)(yyvsp[-2].ptype),
                      (SgExpression*)(yyvsp[0].ptype)); 
          yyval.ptype = current_exp;
	;}
    break;

  case 176:
#line 550 "ompparser.yy"
    {
          current_exp = SageBuilder::buildAndAssignOp(
                      (SgExpression*)(yyvsp[-2].ptype),
                      (SgExpression*)(yyvsp[0].ptype)); 
          yyval.ptype = current_exp;
	;}
    break;

  case 177:
#line 558 "ompparser.yy"
    {
          current_exp = SageBuilder::buildXorAssignOp(
                      (SgExpression*)(yyvsp[-2].ptype),
                      (SgExpression*)(yyvsp[0].ptype)); 
          yyval.ptype = current_exp;
	;}
    break;

  case 178:
#line 565 "ompparser.yy"
    {
          current_exp = SageBuilder::buildIorAssignOp(
                      (SgExpression*)(yyvsp[-2].ptype),
                      (SgExpression*)(yyvsp[0].ptype)); 
          yyval.ptype = current_exp;
	;}
    break;

  case 180:
#line 576 "ompparser.yy"
    {
          current_exp = SageBuilder::buildEqualityOp(
                      (SgExpression*)(yyvsp[-2].ptype),
                      (SgExpression*)(yyvsp[0].ptype)); 
          yyval.ptype = current_exp;
	;}
    break;

  case 181:
#line 583 "ompparser.yy"
    {
          current_exp = SageBuilder::buildNotEqualOp(
                      (SgExpression*)(yyvsp[-2].ptype),
                      (SgExpression*)(yyvsp[0].ptype)); 
          yyval.ptype = current_exp;
	;}
    break;

  case 183:
#line 594 "ompparser.yy"
    { 
                    current_exp = SageBuilder::buildLessThanOp(
                      (SgExpression*)(yyvsp[-2].ptype),
                      (SgExpression*)(yyvsp[0].ptype)); 
                    yyval.ptype = current_exp; 
                  //  std::cout<<"debug: buildLessThanOp():\n"<<current_exp->unparseToString()<<std::endl;
                  ;}
    break;

  case 184:
#line 602 "ompparser.yy"
    {
                    current_exp = SageBuilder::buildGreaterThanOp(
                      (SgExpression*)(yyvsp[-2].ptype),
                      (SgExpression*)(yyvsp[0].ptype)); 
                    yyval.ptype = current_exp; 
                ;}
    break;

  case 185:
#line 609 "ompparser.yy"
    {
                    current_exp = SageBuilder::buildLessOrEqualOp(
                      (SgExpression*)(yyvsp[-2].ptype),
                      (SgExpression*)(yyvsp[0].ptype)); 
                    yyval.ptype = current_exp; 
                ;}
    break;

  case 186:
#line 616 "ompparser.yy"
    {
                    current_exp = SageBuilder::buildGreaterOrEqualOp(
                      (SgExpression*)(yyvsp[-2].ptype),
                      (SgExpression*)(yyvsp[0].ptype)); 
                    yyval.ptype = current_exp; 
                ;}
    break;

  case 187:
#line 627 "ompparser.yy"
    {current_exp = SageBuilder::buildIntVal(yyvsp[0].itype); 
                    yyval.ptype = current_exp; ;}
    break;

  case 188:
#line 630 "ompparser.yy"
    { current_exp = SageBuilder::buildVarRefExp(
                      (const char*)(yyvsp[0].stype),SageInterface::getScope(gNode)); 
                    yyval.ptype = current_exp; ;}
    break;

  case 189:
#line 642 "ompparser.yy"
    { if (!addVar((const char*)yyvsp[0].stype)) YYABORT; ;}
    break;

  case 190:
#line 643 "ompparser.yy"
    { if (!addVar((const char*)yyvsp[0].stype)) YYABORT; ;}
    break;


    }

/* Line 1000 of yacc.c.  */
#line 2018 "ompparser.cc"

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


#line 649 "ompparser.yy"


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
// pass real expressions as SgExpression, Liao
static bool addExpression(const char* expr) {
        //ompattribute->addExpression(omptype,std::string(expr),NULL);
//  std::cout<<"debug: current expression is:"<<current_exp->unparseToString()<<std::endl;
      ompattribute->addExpression(omptype,std::string(expr),current_exp);
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


