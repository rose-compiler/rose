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
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         dapparse
#define yylex           daplex
#define yyerror         daperror
#define yylval          daplval
#define yychar          dapchar
#define yydebug         dapdebug
#define yynerrs         dapnerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 10 "dap.y"
#include "dapparselex.h"

/* Line 189 of yacc.c  */
#line 83 "dap.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
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
     SCAN_ALIAS = 258,
     SCAN_ARRAY = 259,
     SCAN_ATTR = 260,
     SCAN_BYTE = 261,
     SCAN_CODE = 262,
     SCAN_DATASET = 263,
     SCAN_ERROR = 264,
     SCAN_FLOAT32 = 265,
     SCAN_FLOAT64 = 266,
     SCAN_GRID = 267,
     SCAN_INT16 = 268,
     SCAN_INT32 = 269,
     SCAN_MAPS = 270,
     SCAN_MESSAGE = 271,
     SCAN_SEQUENCE = 272,
     SCAN_STRING = 273,
     SCAN_STRUCTURE = 274,
     SCAN_UINT16 = 275,
     SCAN_UINT32 = 276,
     SCAN_URL = 277,
     SCAN_WORD = 278,
     SCAN_PTYPE = 279,
     SCAN_PROG = 280
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 150 "dap.tab.c"

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
#define YYFINAL  11
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   294

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  34
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  32
/* YYNRULES -- Number of rules.  */
#define YYNRULES  98
/* YYNRULES -- Number of states.  */
#define YYNSTATES  191

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   280

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    33,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    29,    28,
       2,    32,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    30,     2,    31,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    26,     2,    27,     2,     2,     2,     2,
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
      25
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,    10,    13,    15,    21,    22,    25,
      30,    38,    45,    57,    59,    61,    63,    65,    67,    69,
      71,    73,    75,    77,    78,    81,    85,    91,    93,    95,
      97,    99,   100,   104,   106,   107,   110,   113,   118,   123,
     128,   133,   138,   143,   148,   153,   158,   163,   165,   167,
     171,   173,   177,   179,   183,   185,   189,   191,   195,   197,
     201,   203,   207,   209,   213,   215,   219,   221,   223,   227,
     235,   236,   241,   242,   247,   248,   253,   254,   259,   261,
     263,   265,   267,   269,   271,   273,   275,   277,   279,   281,
     283,   285,   287,   289,   291,   293,   295,   297,   299
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      35,     0,    -1,     8,    36,    -1,     5,    44,    45,    -1,
       9,    60,    -1,     1,    -1,    26,    37,    27,    42,    28,
      -1,    -1,    37,    38,    -1,    39,    43,    40,    28,    -1,
      19,    26,    37,    27,    43,    40,    28,    -1,    17,    26,
      37,    27,    43,    28,    -1,    12,    26,     4,    29,    38,
      15,    29,    37,    27,    43,    28,    -1,     1,    -1,     6,
      -1,    13,    -1,    20,    -1,    14,    -1,    21,    -1,    10,
      -1,    11,    -1,    22,    -1,    18,    -1,    -1,    40,    41,
      -1,    30,    23,    31,    -1,    30,    65,    32,    23,    31,
      -1,     1,    -1,    43,    -1,     1,    -1,    65,    -1,    -1,
      26,    46,    27,    -1,     1,    -1,    -1,    46,    47,    -1,
      59,    28,    -1,     6,    65,    48,    28,    -1,    13,    65,
      49,    28,    -1,    20,    65,    50,    28,    -1,    14,    65,
      51,    28,    -1,    21,    65,    52,    28,    -1,    10,    65,
      53,    28,    -1,    11,    65,    54,    28,    -1,    18,    65,
      55,    28,    -1,    22,    65,    56,    28,    -1,    65,    26,
      46,    27,    -1,     1,    -1,    23,    -1,    48,    33,    23,
      -1,    23,    -1,    49,    33,    23,    -1,    23,    -1,    50,
      33,    23,    -1,    23,    -1,    51,    33,    23,    -1,    23,
      -1,    52,    33,    23,    -1,    23,    -1,    53,    33,    23,
      -1,    23,    -1,    54,    33,    23,    -1,    58,    -1,    55,
      33,    58,    -1,    57,    -1,    56,    33,    57,    -1,    23,
      -1,    23,    -1,     3,    23,    23,    -1,    26,    61,    62,
      63,    64,    27,    28,    -1,    -1,     7,    32,    23,    28,
      -1,    -1,    16,    32,    23,    28,    -1,    -1,    24,    32,
      23,    28,    -1,    -1,    25,    32,    23,    28,    -1,    23,
      -1,     3,    -1,     4,    -1,     5,    -1,     6,    -1,     8,
      -1,     9,    -1,    10,    -1,    11,    -1,    12,    -1,    13,
      -1,    14,    -1,    15,    -1,    17,    -1,    18,    -1,    19,
      -1,    20,    -1,    21,    -1,    22,    -1,     7,    -1,    16,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    45,    45,    46,    47,    48,    53,    58,    59,    65,
      67,    69,    71,    74,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    92,    93,    97,    98,    99,   104,   105,
     109,   111,   114,   115,   120,   121,   125,   126,   128,   130,
     132,   134,   136,   138,   140,   142,   144,   145,   150,   151,
     155,   156,   160,   161,   165,   166,   170,   171,   174,   175,
     178,   179,   182,   183,   187,   188,   192,   196,   206,   210,
     214,   214,   215,   215,   216,   216,   217,   217,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SCAN_ALIAS", "SCAN_ARRAY", "SCAN_ATTR",
  "SCAN_BYTE", "SCAN_CODE", "SCAN_DATASET", "SCAN_ERROR", "SCAN_FLOAT32",
  "SCAN_FLOAT64", "SCAN_GRID", "SCAN_INT16", "SCAN_INT32", "SCAN_MAPS",
  "SCAN_MESSAGE", "SCAN_SEQUENCE", "SCAN_STRING", "SCAN_STRUCTURE",
  "SCAN_UINT16", "SCAN_UINT32", "SCAN_URL", "SCAN_WORD", "SCAN_PTYPE",
  "SCAN_PROG", "'{'", "'}'", "';'", "':'", "'['", "']'", "'='", "','",
  "$accept", "start", "datasetbody", "declarations", "declaration",
  "base_type", "array_decls", "array_decl", "datasetname", "var_name",
  "dassetup", "attributebody", "attr_list", "attribute", "bytes", "int16",
  "uint16", "int32", "uint32", "float32", "float64", "strs", "urls", "url",
  "str_or_id", "alias", "errorbody", "errorcode", "errormsg", "errorptype",
  "errorprog", "name", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   123,   125,    59,    58,
      91,    93,    61,    44
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    34,    35,    35,    35,    35,    36,    37,    37,    38,
      38,    38,    38,    38,    39,    39,    39,    39,    39,    39,
      39,    39,    39,    40,    40,    41,    41,    41,    42,    42,
      43,    44,    45,    45,    46,    46,    47,    47,    47,    47,
      47,    47,    47,    47,    47,    47,    47,    47,    48,    48,
      49,    49,    50,    50,    51,    51,    52,    52,    53,    53,
      54,    54,    55,    55,    56,    56,    57,    58,    59,    60,
      61,    61,    62,    62,    63,    63,    64,    64,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     3,     2,     1,     5,     0,     2,     4,
       7,     6,    11,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     2,     3,     5,     1,     1,     1,
       1,     0,     3,     1,     0,     2,     2,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     1,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     1,     3,     7,
       0,     4,     0,     4,     0,     4,     0,     4,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     5,    31,     0,     0,     0,     0,     7,     2,    70,
       4,     1,    33,    34,     3,     0,     0,    72,     0,    13,
      14,    19,    20,     0,    15,    17,     0,    22,     0,    16,
      18,    21,     0,     8,     0,     0,     0,    74,    47,    79,
      80,    81,    82,    97,    83,    84,    85,    86,    87,    88,
      89,    90,    98,    91,    92,    93,    94,    95,    96,    78,
      32,    35,     0,     0,     0,     7,     7,    29,    79,    82,
      85,    86,    88,    89,    92,    94,    95,    96,     0,    28,
      30,    23,     0,     0,     0,    76,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,    34,     0,     0,
       0,     6,     0,    71,     0,     0,     0,     0,    68,    48,
       0,    58,     0,    60,     0,    50,     0,    54,     0,    67,
       0,    62,    52,     0,    56,     0,    66,     0,    64,     0,
       0,     0,     0,    27,     9,     0,    24,    73,     0,     0,
       0,    37,     0,    42,     0,    43,     0,    38,     0,    40,
       0,    44,     0,    39,     0,    41,     0,    45,     0,    46,
       0,     0,    23,    78,     0,    75,     0,    69,    49,    59,
      61,    51,    55,    63,    53,    57,    65,     0,    11,     0,
      25,     0,    77,     7,    10,     0,     0,    26,     0,     0,
      12
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     5,     8,    15,    33,    34,   102,   136,    78,    79,
       6,    14,    18,    61,   110,   116,   123,   118,   125,   112,
     114,   120,   127,   128,   121,    62,    10,    17,    37,    85,
     107,    80
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -65
static const yytype_int16 yypact[] =
{
       5,   -65,   -65,   -18,    -4,    37,     3,   -65,   -65,    -2,
     -65,   -65,   -65,   -65,   -65,   111,    36,    24,    44,   -65,
     -65,   -65,   -65,    43,   -65,   -65,    67,   -65,    74,   -65,
     -65,   -65,   207,   -65,   250,    72,    70,    77,   -65,    80,
     -65,   -65,   250,   -65,   -65,   -65,   250,   250,   -65,   250,
     250,   -65,   -65,   -65,   250,   -65,   250,   250,   250,   -65,
     -65,   -65,    76,    79,   102,   -65,   -65,   -65,   -65,   -65,
     -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,    81,   -65,
     -65,   -65,    82,    84,    83,    86,    85,    90,    93,    95,
     103,   104,   112,   113,   114,   117,   -65,   -65,   105,   138,
     160,   -65,     8,   -65,   115,   118,   110,   119,   -65,   -65,
     -21,   -65,   -17,   -65,   -13,   -65,   -10,   -65,    -9,   -65,
      -7,   -65,   -65,    -3,   -65,     0,   -65,    66,   -65,    69,
     230,   250,   250,   -65,   -65,   271,   -65,   -65,   125,   122,
     134,   -65,   124,   -65,   140,   -65,   141,   -65,   144,   -65,
     145,   -65,   112,   -65,   146,   -65,   152,   -65,   117,   -65,
     161,   156,   -65,   154,   157,   -65,   158,   -65,   -65,   -65,
     -65,   -65,   -65,   -65,   -65,   -65,   -65,   162,   -65,    16,
     -65,   167,   -65,   -65,   -65,   166,   182,   -65,   250,   170,
     -65
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -65,   -65,   -65,   -64,    75,   -65,    45,   -65,   -65,   -34,
     -65,   -65,   109,   -65,   -65,   -65,   -65,   -65,   -65,   -65,
     -65,   -65,   -65,    87,    94,   -65,   -65,   -65,   -65,   -65,
     -65,   -15
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      81,    99,   100,    63,    12,    16,     1,   141,     7,   133,
       2,   143,   142,     3,     4,   145,   144,   133,   147,   149,
     146,   151,     9,   148,   150,   153,   152,    87,   155,    13,
     154,    88,    89,   156,    90,    91,   134,    11,   135,    92,
      36,    93,    94,    95,   184,    38,   135,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    35,    64,
      38,    60,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    65,   157,    82,   159,   161,   162,   158,
      66,    84,    83,    86,    96,    97,    98,   104,   108,   101,
     103,   106,    19,   109,    63,   105,   111,    20,   113,   186,
     164,    21,    22,    23,    24,    25,   115,   117,    26,    27,
      28,    29,    30,    31,   130,   119,   122,   124,    32,    19,
     126,   138,   139,   137,    20,   166,   140,   168,    21,    22,
      23,    24,    25,   165,   189,    26,    27,    28,    29,    30,
      31,    19,   167,   169,   170,   131,    20,   171,   172,   174,
      21,    22,    23,    24,    25,   175,   177,    26,    27,    28,
      29,    30,    31,    19,   178,   180,   182,   132,    20,   181,
     185,   183,    21,    22,    23,    24,    25,   187,   190,    26,
      27,    28,    29,    30,    31,   160,   129,   179,    67,   188,
      68,    40,    41,    69,    43,    44,    45,    70,    71,    48,
      72,    73,    51,    52,    53,    74,    55,    75,    76,    77,
      59,    19,     0,     0,     0,     0,    20,     0,     0,     0,
      21,    22,    23,    24,    25,   176,   173,    26,    27,    28,
      29,    30,    31,    68,    40,    41,    69,    43,    44,    45,
      70,    71,    48,    72,    73,    51,    52,    53,    74,    55,
      75,    76,    77,    59,    68,    40,    41,    69,    43,    44,
      45,    70,    71,    48,    72,    73,    51,    52,    53,    74,
      55,    75,    76,    77,   163
};

static const yytype_int16 yycheck[] =
{
      34,    65,    66,    18,     1,     7,     1,    28,    26,     1,
       5,    28,    33,     8,     9,    28,    33,     1,    28,    28,
      33,    28,    26,    33,    33,    28,    33,    42,    28,    26,
      33,    46,    47,    33,    49,    50,    28,     0,    30,    54,
      16,    56,    57,    58,    28,     1,    30,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    32,    26,
       1,    27,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    26,    28,    23,    27,   131,   132,    33,
      26,    24,    32,    23,    28,    26,     4,    23,    23,    28,
      28,    25,     1,    23,   129,    32,    23,     6,    23,   183,
     135,    10,    11,    12,    13,    14,    23,    23,    17,    18,
      19,    20,    21,    22,    29,    23,    23,    23,    27,     1,
      23,    23,    32,    28,     6,    23,    27,    23,    10,    11,
      12,    13,    14,    28,   188,    17,    18,    19,    20,    21,
      22,     1,    28,    23,    23,    27,     6,    23,    23,    23,
      10,    11,    12,    13,    14,    23,    15,    17,    18,    19,
      20,    21,    22,     1,    28,    31,    28,    27,     6,    32,
      23,    29,    10,    11,    12,    13,    14,    31,    28,    17,
      18,    19,    20,    21,    22,   130,    97,   162,     1,    27,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,     1,    -1,    -1,    -1,    -1,     6,    -1,    -1,    -1,
      10,    11,    12,    13,    14,   158,   152,    17,    18,    19,
      20,    21,    22,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     5,     8,     9,    35,    44,    26,    36,    26,
      60,     0,     1,    26,    45,    37,     7,    61,    46,     1,
       6,    10,    11,    12,    13,    14,    17,    18,    19,    20,
      21,    22,    27,    38,    39,    32,    16,    62,     1,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      27,    47,    59,    65,    26,    26,    26,     1,     3,     6,
      10,    11,    13,    14,    18,    20,    21,    22,    42,    43,
      65,    43,    23,    32,    24,    63,    23,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    28,    26,     4,    37,
      37,    28,    40,    28,    23,    32,    25,    64,    23,    23,
      48,    23,    53,    23,    54,    23,    49,    23,    51,    23,
      55,    58,    23,    50,    23,    52,    23,    56,    57,    46,
      29,    27,    27,     1,    28,    30,    41,    28,    23,    32,
      27,    28,    33,    28,    33,    28,    33,    28,    33,    28,
      33,    28,    33,    28,    33,    28,    33,    28,    33,    27,
      38,    43,    43,    23,    65,    28,    23,    28,    23,    23,
      23,    23,    23,    58,    23,    23,    57,    15,    28,    40,
      31,    32,    28,    29,    28,    23,    37,    31,    27,    43,
      28
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
      yyerror (parsestate, YY_("syntax error: cannot back up")); \
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
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, parsestate)
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
		  Type, Value, parsestate); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, DAPparsestate* parsestate)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, parsestate)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    DAPparsestate* parsestate;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (parsestate);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, DAPparsestate* parsestate)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, parsestate)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    DAPparsestate* parsestate;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, parsestate);
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
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, DAPparsestate* parsestate)
#else
static void
yy_reduce_print (yyvsp, yyrule, parsestate)
    YYSTYPE *yyvsp;
    int yyrule;
    DAPparsestate* parsestate;
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
		       		       , parsestate);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule, parsestate); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, DAPparsestate* parsestate)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, parsestate)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    DAPparsestate* parsestate;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (parsestate);

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
int yyparse (DAPparsestate* parsestate);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */





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
yyparse (DAPparsestate* parsestate)
#else
int
yyparse (parsestate)
    DAPparsestate* parsestate;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

    /* Number of syntax errors so far.  */
    int yynerrs;

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
#line 49 "dap.y"
    {(yyval)=unrecognizedresponse(parsestate);;}
    break;

  case 6:

/* Line 1464 of yacc.c  */
#line 54 "dap.y"
    {(yyval)=datasetbody(parsestate,(yyvsp[(4) - (5)]),(yyvsp[(2) - (5)]));;}
    break;

  case 7:

/* Line 1464 of yacc.c  */
#line 58 "dap.y"
    {(yyval)=declarations(parsestate,null,null);;}
    break;

  case 8:

/* Line 1464 of yacc.c  */
#line 59 "dap.y"
    {(yyval)=declarations(parsestate,(yyvsp[(1) - (2)]),(yyvsp[(2) - (2)]));;}
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 66 "dap.y"
    {(yyval)=makebase(parsestate,(yyvsp[(2) - (4)]),(yyvsp[(1) - (4)]),(yyvsp[(3) - (4)]));;}
    break;

  case 10:

/* Line 1464 of yacc.c  */
#line 68 "dap.y"
    {if(((yyval) = makestructure(parsestate,(yyvsp[(5) - (7)]),(yyvsp[(6) - (7)]),(yyvsp[(3) - (7)])))==null) {YYABORT;};}
    break;

  case 11:

/* Line 1464 of yacc.c  */
#line 70 "dap.y"
    {if(((yyval) = makesequence(parsestate,(yyvsp[(5) - (6)]),(yyvsp[(3) - (6)])))==null) {YYABORT;};}
    break;

  case 12:

/* Line 1464 of yacc.c  */
#line 73 "dap.y"
    {if(((yyval) = makegrid(parsestate,(yyvsp[(10) - (11)]),(yyvsp[(5) - (11)]),(yyvsp[(8) - (11)])))==null) {YYABORT;};}
    break;

  case 13:

/* Line 1464 of yacc.c  */
#line 75 "dap.y"
    {daperror(parsestate,"Unrecognized type"); YYABORT;;}
    break;

  case 14:

/* Line 1464 of yacc.c  */
#line 80 "dap.y"
    {(yyval)=(Object)SCAN_BYTE;;}
    break;

  case 15:

/* Line 1464 of yacc.c  */
#line 81 "dap.y"
    {(yyval)=(Object)SCAN_INT16;;}
    break;

  case 16:

/* Line 1464 of yacc.c  */
#line 82 "dap.y"
    {(yyval)=(Object)SCAN_UINT16;;}
    break;

  case 17:

/* Line 1464 of yacc.c  */
#line 83 "dap.y"
    {(yyval)=(Object)SCAN_INT32;;}
    break;

  case 18:

/* Line 1464 of yacc.c  */
#line 84 "dap.y"
    {(yyval)=(Object)SCAN_UINT32;;}
    break;

  case 19:

/* Line 1464 of yacc.c  */
#line 85 "dap.y"
    {(yyval)=(Object)SCAN_FLOAT32;;}
    break;

  case 20:

/* Line 1464 of yacc.c  */
#line 86 "dap.y"
    {(yyval)=(Object)SCAN_FLOAT64;;}
    break;

  case 21:

/* Line 1464 of yacc.c  */
#line 87 "dap.y"
    {(yyval)=(Object)SCAN_URL;;}
    break;

  case 22:

/* Line 1464 of yacc.c  */
#line 88 "dap.y"
    {(yyval)=(Object)SCAN_STRING;;}
    break;

  case 23:

/* Line 1464 of yacc.c  */
#line 92 "dap.y"
    {(yyval)=arraydecls(parsestate,null,null);;}
    break;

  case 24:

/* Line 1464 of yacc.c  */
#line 93 "dap.y"
    {(yyval)=arraydecls(parsestate,(yyvsp[(1) - (2)]),(yyvsp[(2) - (2)]));;}
    break;

  case 25:

/* Line 1464 of yacc.c  */
#line 97 "dap.y"
    {(yyval)=arraydecl(parsestate,null,(yyvsp[(2) - (3)]));;}
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 98 "dap.y"
    {(yyval)=arraydecl(parsestate,(yyvsp[(2) - (5)]),(yyvsp[(4) - (5)]));;}
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 100 "dap.y"
    {daperror(parsestate,"Illegal dimension declaration"); YYABORT;;}
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 104 "dap.y"
    {(yyval)=(yyvsp[(1) - (1)]);;}
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 106 "dap.y"
    {daperror(parsestate,"Illegal dataset declaration"); YYABORT;;}
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 109 "dap.y"
    {(yyval)=(yyvsp[(1) - (1)]);;}
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 111 "dap.y"
    {dassetup(parsestate);;}
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 114 "dap.y"
    {(yyval)=attributebody(parsestate,(yyvsp[(2) - (3)]));;}
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 116 "dap.y"
    {daperror(parsestate,"Illegal DAS body"); YYABORT;;}
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 120 "dap.y"
    {(yyval)=attrlist(parsestate,null,null);;}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 121 "dap.y"
    {(yyval)=attrlist(parsestate,(yyvsp[(1) - (2)]),(yyvsp[(2) - (2)]));;}
    break;

  case 36:

/* Line 1464 of yacc.c  */
#line 125 "dap.y"
    {(yyval)=null;;}
    break;

  case 37:

/* Line 1464 of yacc.c  */
#line 127 "dap.y"
    {(yyval)=attribute(parsestate,(yyvsp[(2) - (4)]),(yyvsp[(3) - (4)]),(Object)SCAN_BYTE);;}
    break;

  case 38:

/* Line 1464 of yacc.c  */
#line 129 "dap.y"
    {(yyval)=attribute(parsestate,(yyvsp[(2) - (4)]),(yyvsp[(3) - (4)]),(Object)SCAN_INT16);;}
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 131 "dap.y"
    {(yyval)=attribute(parsestate,(yyvsp[(2) - (4)]),(yyvsp[(3) - (4)]),(Object)SCAN_UINT16);;}
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 133 "dap.y"
    {(yyval)=attribute(parsestate,(yyvsp[(2) - (4)]),(yyvsp[(3) - (4)]),(Object)SCAN_INT32);;}
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 135 "dap.y"
    {(yyval)=attribute(parsestate,(yyvsp[(2) - (4)]),(yyvsp[(3) - (4)]),(Object)SCAN_UINT32);;}
    break;

  case 42:

/* Line 1464 of yacc.c  */
#line 137 "dap.y"
    {(yyval)=attribute(parsestate,(yyvsp[(2) - (4)]),(yyvsp[(3) - (4)]),(Object)SCAN_FLOAT32);;}
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 139 "dap.y"
    {(yyval)=attribute(parsestate,(yyvsp[(2) - (4)]),(yyvsp[(3) - (4)]),(Object)SCAN_FLOAT64);;}
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 141 "dap.y"
    {(yyval)=attribute(parsestate,(yyvsp[(2) - (4)]),(yyvsp[(3) - (4)]),(Object)SCAN_STRING);;}
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 143 "dap.y"
    {(yyval)=attribute(parsestate,(yyvsp[(2) - (4)]),(yyvsp[(3) - (4)]),(Object)SCAN_URL);;}
    break;

  case 46:

/* Line 1464 of yacc.c  */
#line 144 "dap.y"
    {(yyval)=attrset(parsestate,(yyvsp[(1) - (4)]),(yyvsp[(3) - (4)]));;}
    break;

  case 47:

/* Line 1464 of yacc.c  */
#line 146 "dap.y"
    {daperror(parsestate,"Illegal attribute"); YYABORT;;}
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 150 "dap.y"
    {(yyval)=attrvalue(parsestate,null,(yyvsp[(1) - (1)]),(Object)SCAN_BYTE);;}
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 152 "dap.y"
    {(yyval)=attrvalue(parsestate,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]),(Object)SCAN_BYTE);;}
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 155 "dap.y"
    {(yyval)=attrvalue(parsestate,null,(yyvsp[(1) - (1)]),(Object)SCAN_INT16);;}
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 157 "dap.y"
    {(yyval)=attrvalue(parsestate,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]),(Object)SCAN_INT16);;}
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 160 "dap.y"
    {(yyval)=attrvalue(parsestate,null,(yyvsp[(1) - (1)]),(Object)SCAN_UINT16);;}
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 162 "dap.y"
    {(yyval)=attrvalue(parsestate,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]),(Object)SCAN_UINT16);;}
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 165 "dap.y"
    {(yyval)=attrvalue(parsestate,null,(yyvsp[(1) - (1)]),(Object)SCAN_INT32);;}
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 167 "dap.y"
    {(yyval)=attrvalue(parsestate,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]),(Object)SCAN_INT32);;}
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 170 "dap.y"
    {(yyval)=attrvalue(parsestate,null,(yyvsp[(1) - (1)]),(Object)SCAN_UINT32);;}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 171 "dap.y"
    {(yyval)=attrvalue(parsestate,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]),(Object)SCAN_UINT32);;}
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 174 "dap.y"
    {(yyval)=attrvalue(parsestate,null,(yyvsp[(1) - (1)]),(Object)SCAN_FLOAT32);;}
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 175 "dap.y"
    {(yyval)=attrvalue(parsestate,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]),(Object)SCAN_FLOAT32);;}
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 178 "dap.y"
    {(yyval)=attrvalue(parsestate,null,(yyvsp[(1) - (1)]),(Object)SCAN_FLOAT64);;}
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 179 "dap.y"
    {(yyval)=attrvalue(parsestate,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]),(Object)SCAN_FLOAT64);;}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 182 "dap.y"
    {(yyval)=attrvalue(parsestate,null,(yyvsp[(1) - (1)]),(Object)SCAN_STRING);;}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 183 "dap.y"
    {(yyval)=attrvalue(parsestate,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]),(Object)SCAN_STRING);;}
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 187 "dap.y"
    {(yyval)=attrvalue(parsestate,null,(yyvsp[(1) - (1)]),(Object)SCAN_URL);;}
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 188 "dap.y"
    {(yyval)=attrvalue(parsestate,(yyvsp[(1) - (3)]),(yyvsp[(3) - (3)]),(Object)SCAN_URL);;}
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 192 "dap.y"
    {(yyval)=(yyvsp[(1) - (1)]);;}
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 196 "dap.y"
    {(yyval)=(yyvsp[(1) - (1)]);;}
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 206 "dap.y"
    {(yyval)=(yyvsp[(2) - (3)]); (yyval)=(yyvsp[(3) - (3)]); (yyval)=null;;}
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 211 "dap.y"
    {(yyval)=errorbody(parsestate,(yyvsp[(2) - (7)]),(yyvsp[(3) - (7)]),(yyvsp[(4) - (7)]),(yyvsp[(5) - (7)]));;}
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 214 "dap.y"
    {(yyval)=null;;}
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 214 "dap.y"
    {(yyval)=(yyvsp[(3) - (4)]);;}
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 215 "dap.y"
    {(yyval)=null;;}
    break;

  case 73:

/* Line 1464 of yacc.c  */
#line 215 "dap.y"
    {(yyval)=(yyvsp[(3) - (4)]);;}
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 216 "dap.y"
    {(yyval)=null;;}
    break;

  case 75:

/* Line 1464 of yacc.c  */
#line 216 "dap.y"
    {(yyval)=(yyvsp[(3) - (4)]);;}
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 217 "dap.y"
    {(yyval)=null;;}
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 217 "dap.y"
    {(yyval)=(yyvsp[(3) - (4)]);;}
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 223 "dap.y"
    {(yyval)=(yyvsp[(1) - (1)]);;}
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 224 "dap.y"
    {(yyval)=strdup("alias");;}
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 225 "dap.y"
    {(yyval)=strdup("array");;}
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 226 "dap.y"
    {(yyval)=strdup("attributes");;}
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 227 "dap.y"
    {(yyval)=strdup("byte");;}
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 228 "dap.y"
    {(yyval)=strdup("dataset");;}
    break;

  case 84:

/* Line 1464 of yacc.c  */
#line 229 "dap.y"
    {(yyval)=strdup("error");;}
    break;

  case 85:

/* Line 1464 of yacc.c  */
#line 230 "dap.y"
    {(yyval)=strdup("float32");;}
    break;

  case 86:

/* Line 1464 of yacc.c  */
#line 231 "dap.y"
    {(yyval)=strdup("float64");;}
    break;

  case 87:

/* Line 1464 of yacc.c  */
#line 232 "dap.y"
    {(yyval)=strdup("grid");;}
    break;

  case 88:

/* Line 1464 of yacc.c  */
#line 233 "dap.y"
    {(yyval)=strdup("int16");;}
    break;

  case 89:

/* Line 1464 of yacc.c  */
#line 234 "dap.y"
    {(yyval)=strdup("int32");;}
    break;

  case 90:

/* Line 1464 of yacc.c  */
#line 235 "dap.y"
    {(yyval)=strdup("maps");;}
    break;

  case 91:

/* Line 1464 of yacc.c  */
#line 236 "dap.y"
    {(yyval)=strdup("sequence");;}
    break;

  case 92:

/* Line 1464 of yacc.c  */
#line 237 "dap.y"
    {(yyval)=strdup("string");;}
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 238 "dap.y"
    {(yyval)=strdup("structure");;}
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 239 "dap.y"
    {(yyval)=strdup("uint16");;}
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 240 "dap.y"
    {(yyval)=strdup("uint32");;}
    break;

  case 96:

/* Line 1464 of yacc.c  */
#line 241 "dap.y"
    {(yyval)=strdup("url");;}
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 242 "dap.y"
    {(yyval)=strdup("code");;}
    break;

  case 98:

/* Line 1464 of yacc.c  */
#line 243 "dap.y"
    {(yyval)=strdup("message");;}
    break;



/* Line 1464 of yacc.c  */
#line 2202 "dap.tab.c"
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
      yyerror (parsestate, YY_("syntax error"));
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
	    yyerror (parsestate, yymsg);
	  }
	else
	  {
	    yyerror (parsestate, YY_("syntax error"));
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
		      yytoken, &yylval, parsestate);
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
		  yystos[yystate], yyvsp, parsestate);
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
  yyerror (parsestate, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, parsestate);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, parsestate);
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
#line 246 "dap.y"


