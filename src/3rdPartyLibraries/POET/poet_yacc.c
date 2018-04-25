/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.0.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 26 "poet_yacc.y" /* yacc.c:339  */

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

#line 184 "poet_yacc.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
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

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 495 "poet_yacc.c" /* yacc.c:358  */

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
#else
typedef signed char yytype_int8;
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
# elif ! defined YYSIZE_T
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
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2198

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  131
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  285
/* YYNRULES -- Number of rules.  */
#define YYNRULES  513
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  870

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   385

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
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
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
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
     553,   554,   554,   555,   555,   556,   556,   557,   560,   560,
     561,   562,   563,   564,   565,   567,   567,   568,   568,   569,
     570,   571,   570,   573,   575,   575,   578,   578,   580,   581,
     581,   583,   583,   586,   586,   587,   588,   588,   590,   590,
     593,   594,   595,   597,   597,   599,   602,   602,   604,   604,
     608,   608,   610,   610
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
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
  "SEMICOLON", "IN", "SUCH_THAT", "FOR", "FOREACH", "FOREACH_R", "DO",
  "ENDDO", "BREAK", "CONTINUE", "RETURN", "DEBUG", "IF", "ELSE", "CASE",
  "SWITCH", "DEFAULT", "DOT3", "DOT4", "ASSIGN", "PLUS_ASSIGN",
  "MINUS_ASSIGN", "MULT_ASSIGN", "DIVIDE_ASSIGN", "MOD_ASSIGN", "COMMA",
  "QUESTION", "OR", "TOR", "AND", "NOT", "LT", "LE", "EQ", "GT", "GE",
  "NE", "ARROW", "DARROW", "PLUS", "MINUS", "MULTIPLY", "DIVIDE", "MOD",
  "STR_CONCAT", "DCOLON", "UMINUS", "TILT", "DELAY", "APPLY", "CLEAR",
  "SAVE", "RESTORE", "REVERSE", "PERMUTE", "REPLACE", "ASSERT", "RANGE",
  "DUPLICATE", "REBUILD", "VAR", "MAP", "TUPLE", "LIST", "LIST1", "INT",
  "STRING", "FLOAT", "NAME", "EXP", "TRACE", "ERASE", "COPY", "SPLIT",
  "LEN", "INSERT", "CAR", "CDR", "COLON", "ID", "ICONST", "FCONST", "ANY",
  "DOT", "DOT2", "POND", "LB", "RB", "LBR", "RBR", "LP", "RP", "$accept",
  "poet", "sections", "section", "@1", "@2", "@3", "@4", "@5", "@6", "@7",
  "@8", "@9", "@10", "@11", "inputAttrs", "@12", "inputAttr", "@13", "@14",
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
  "@179", "@180", "@181", "code9", "@182", "@183", "@184", "@185", "@186",
  "codeUnit", "@187", "varInvoke", "@188", "@189", "varInvoke1", "@190",
  "@191", "pond", "@192", "varInvokeType", "@193", "varInvokeType1",
  "@194", "@195", "varInvokePattern", "@196", "varInvokePattern1", "@197",
  "@198", "configOrSubRef", "selectRef", "@199", "configRef", "@200",
  "@201", "xformConfig", "@202", "@203", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
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

#define YYPACT_NINF -658

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-658)))

#define YYTABLE_NINF -257

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -658,    36,    32,  -658,    39,  -658,    10,   -17,    -9,  -658,
    -658,    58,  -658,    66,  -658,    68,    63,   103,  -658,  -658,
    -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  1560,  1560,
    -658,   213,  -658,   357,   127,   132,  -658,  -658,  1754,  -658,
    2020,  -658,  -658,  -658,    90,   111,   119,  -658,  -658,   116,
     124,   131,  -658,  -658,  -658,   139,   149,   153,  -658,  -658,
     163,   164,  -658,  -658,  -658,   168,   169,  -658,   174,   175,
    -658,   -88,  -658,   181,   183,  -658,   188,  -658,   190,  -658,
    -658,  -658,  -658,  -658,  -658,   199,   200,  -658,  -658,  -658,
     304,    52,  -658,  -658,   284,   141,  -658,   -15,  1126,  -658,
     803,  -658,   -39,   296,   339,   -10,   307,   312,   340,   342,
    -658,  -658,    33,   343,   353,   362,   368,   375,   377,   383,
    -658,  -658,   363,    68,  -658,  1754,  1754,  2069,  -658,   292,
     299,   327,  -658,  -658,  -658,  -658,  -658,   389,  -658,  -658,
    -658,  -658,  -658,  -658,  -658,   316,  -658,   390,   328,   335,
    2069,   404,  -658,  -658,  -658,  -658,   259,  -658,   363,   363,
     346,  -658,   347,  -658,   349,  -658,  -658,    -6,  -658,     6,
     352,   341,  -658,  -658,  1922,  1922,  -658,  -658,   363,   363,
     363,  -658,  -658,   363,   363,  -658,   363,   129,   344,   363,
    -658,  -658,   363,  -658,   363,  -658,   363,   363,  -658,  1560,
    -658,  1560,  -658,  1560,  -658,  -658,  -658,  -658,  -658,  -658,
    -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,
    -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,
    -658,  -658,  1971,  -658,  -658,   144,  -658,  -658,   415,   418,
     419,   159,  -658,  -658,  -658,  -658,  -658,   463,   213,   431,
     432,   433,   435,   444,   445,   449,   450,  -658,  -658,  -658,
    -658,    -7,  -658,  -658,   489,  -658,    44,   357,  -658,  -658,
       7,    15,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  1754,
     222,  -658,  -658,   235,  1560,  -658,  -658,  -658,  2069,  -658,
    -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,     6,  1922,
     452,  -658,   471,  -658,  1922,  -658,   385,  -658,     6,  1560,
     867,  -658,   254,  1560,  1560,  -658,  -658,  -658,     6,     6,
    -658,   453,     6,  -658,  -658,   -36,   438,  -658,  2069,  -658,
       6,     6,  -658,     6,  -658,     6,  -658,  -658,   392,   391,
    -658,  1799,  1799,  1922,     6,     6,  1922,  1922,  1922,  1922,
    1922,  1922,  1922,  1922,  1922,  1922,  1922,  1922,  1922,  1015,
       6,     6,     6,     6,     6,     6,  1754,  1754,  -658,  -658,
     363,  -658,  -658,  -658,  -658,   363,   399,   406,  -658,  -658,
    -658,  -658,   -10,   363,   363,   363,   363,  -658,  -658,  -658,
     407,  -658,  -658,   400,  -658,  -658,  -658,   182,    33,   363,
     363,  -658,  -658,   363,   363,  -658,  2020,   363,  -658,  -658,
    -658,  -658,  -658,  -658,  -658,  1754,  1754,  1754,  1754,   130,
     408,   456,  -658,  -658,  -658,  1754,   479,   484,   485,   486,
    -658,   411,  2069,   422,   791,   416,   492,  2020,   519,  -658,
     417,  -658,   252,  -658,   688,  -658,  1560,   430,   446,   436,
     454,   178,   348,   363,   359,   363,  -658,   363,   318,    -8,
      -8,   370,   382,  -658,  -658,  -658,  -658,   739,   511,  -658,
     867,   867,   867,   867,   867,   867,   254,   254,   494,   494,
     494,   494,   494,   464,   467,   524,  -658,  -658,   655,  -658,
    -658,    -8,    -8,    -8,    -8,    -8,   316,   316,  1971,  -658,
     363,  1873,  -658,  -658,   468,  -658,  2069,  1560,  -658,  -658,
    -658,  -658,  -658,   363,   469,   363,  2020,  -658,   240,   363,
     363,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,    93,
    -658,   286,  2069,   316,   324,   325,   316,  -658,  1754,  -658,
    -658,  2069,   475,  -658,  -658,   555,  -658,  -658,  -658,  -658,
    -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  2069,
    -658,  -658,  -658,  -658,  -658,  1015,  -658,  -658,  -658,   875,
    -658,   472,  -658,   474,  -658,  -658,  -658,  -658,   480,  -658,
     528,  -658,  -658,    28,    69,  -658,  -658,  -658,  -658,  -658,
    -658,  1015,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,
    -658,  -658,  -658,   -33,    -8,   477,   545,   575,   498,    78,
     499,   549,   582,  -658,   502,    83,  -658,    93,  -658,  -658,
     296,  -658,  -658,  1560,  -658,  -658,  1754,  -658,  -658,  1754,
     316,  -658,  1754,  -658,   565,    -5,  2069,  -658,  2020,   363,
    2069,  2069,  2069,  2069,  2069,  2069,  2069,   549,  -658,  2069,
       6,  1015,   655,  1015,   507,   497,   587,   867,  1015,  -658,
    1680,  -658,     6,     6,  -658,     6,   363,  2069,  -658,  -658,
    -658,  -658,  -658,  -658,     6,     6,     6,  1015,   483,   500,
     567,  1015,  1015,  1015,  1015,  1015,  1015,  1015,  1971,  -658,
     518,  -658,  -658,  -658,  -658,   468,  -658,   512,   514,   578,
    -658,  -658,   469,  -658,   612,   522,    46,    46,   316,  1754,
     316,  -658,  -658,    93,  -658,  -658,   549,   549,   549,   549,
     549,   549,  2069,   549,   610,   227,  -658,   595,    18,  1922,
    -658,   609,   510,  -658,   616,   608,    70,    -8,   387,  -658,
      24,     6,     6,   121,   122,  -658,   473,  -658,  1015,  -658,
    -658,   261,   195,    75,    75,    75,    75,  -658,  -658,     6,
    -658,   144,   544,   475,  2069,  -658,  -658,  -658,  -658,  2020,
    -658,  -658,  -658,  -658,  -658,   533,   534,  -658,  -658,  -658,
    -658,  -658,  -658,  -658,   490,     6,  -658,  -658,   613,  -658,
      17,  -658,    28,  -658,  -658,   171,   172,  -658,  -658,  1015,
     655,  -658,  1015,    -8,  -658,   546,  -658,   575,   549,  2069,
     499,    93,  -658,  -658,     6,     6,  2069,   507,  -658,  -658,
    1560,     6,  -658,  -658,  1015,  -658,   553,  -658,  -658,     6,
    -658,  -658,   655,  1015,   655,   549,  -658,   536,   541,   549,
     634,   630,   547,  1560,   557,  1015,  -658,   197,  -658,  -658,
    -658,  -658,  -658,  -658,  -658,   637,  1560,  -658,  1680,  1680,
    1680,  1560,  -658,  -658,  -658,  -658,  -658,  -658,  1560,  -658
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       3,     0,   444,     1,     0,     4,     0,     0,     0,     9,
      11,     0,    20,     0,    17,     0,     0,     0,    25,   445,
     455,   454,   446,   456,   452,   453,     5,     7,     0,     0,
      23,    46,    14,    28,   251,     0,   448,   450,     0,    88,
       0,   189,   321,   325,    80,    82,     0,   188,   291,     0,
     274,     0,   318,   317,   319,   442,     0,     0,   352,   384,
       0,     0,   465,   426,   424,     0,     0,   323,     0,     0,
     422,   103,   463,     0,     0,   408,     0,   461,     0,   457,
     459,    76,   186,   187,   472,   288,   468,   473,   474,   471,
       0,   254,   262,   257,   311,   312,   327,   328,   368,   369,
     386,   435,   475,   467,     0,   233,     0,     0,     0,     0,
      21,    44,    57,     0,     0,     0,     0,     0,     0,     0,
      18,    26,     0,     0,    13,     0,     0,     0,   114,   115,
       0,     0,   109,   111,   110,   112,   113,    76,   108,   213,
     486,   116,   205,   107,   208,   447,   206,    89,     0,     0,
       0,    76,   124,   117,   200,   199,     0,   120,     0,     0,
       0,    78,     0,    79,     0,   266,   278,     0,   283,     0,
       0,     0,   258,   271,     0,     0,   413,   420,     0,     0,
       0,   401,   394,     0,     0,   404,     0,     0,    98,     0,
     398,   410,     0,   428,     0,   415,     0,     0,   290,     0,
     470,     0,    10,     0,   313,   292,   315,   345,   350,   348,
     354,   356,   358,   360,   362,   364,   370,   372,   374,   378,
     380,   376,   382,   366,   329,   333,   335,   337,   339,   341,
     331,   343,   390,   431,   432,   479,   477,    12,     0,     0,
       0,     0,   234,    53,    51,    47,    49,     0,    46,     0,
       0,     0,     0,     0,     0,     0,     0,    15,    58,    42,
      40,     0,    38,    33,    36,    29,     0,    28,   252,   253,
       0,     0,   115,   126,   207,   190,   193,   196,   209,     0,
     488,   211,     6,     0,     0,   118,   123,   121,     0,     8,
     204,   203,   201,   322,   326,    81,    83,    77,   296,     0,
      76,   493,     0,   299,     0,   320,     0,   418,     0,     0,
     353,   386,   385,     0,     0,   466,   427,   425,     0,     0,
     324,     0,     0,   423,   102,     0,     0,   101,     0,   464,
       0,     0,   409,     0,   462,     0,   458,   460,     0,     0,
     255,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   387,   388,
       0,   484,   480,   476,   483,     0,     0,     0,   239,   230,
     232,    24,   233,     0,     0,     0,     0,    22,    45,    63,
       0,    72,    67,     0,    70,    65,    61,     0,    57,     0,
       0,    31,    32,     0,     0,    35,     0,     0,   444,    56,
      19,    27,   449,   451,   127,     0,     0,     0,     0,   215,
       0,   217,   491,   489,   487,     0,     0,     0,     0,     0,
      90,     0,     0,   126,   129,     0,   131,     0,     0,   295,
       0,   297,   495,   275,     0,   443,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   104,     0,     0,   399,
     411,     0,     0,   289,   469,   314,   316,     0,   351,   349,
     355,   357,   359,   361,   363,   365,   371,   373,   375,   379,
     381,   377,   383,     0,     0,    76,   157,   153,   367,   156,
     330,   334,   336,   338,   340,   342,   332,   344,   393,   433,
       0,     0,   475,   478,     0,   237,     0,     0,   235,    54,
      52,    48,    50,     0,     0,     0,     0,    69,     0,     0,
       0,   227,   229,    16,    59,    43,    41,    39,    34,    37,
      30,     0,     0,   191,     0,     0,   210,   218,   222,   214,
     220,     0,     0,   212,    91,     0,    95,    93,   119,   122,
     134,   133,   132,   149,   136,   138,   140,   142,   144,   146,
     125,   151,   202,   267,   279,     0,   498,   496,   494,     0,
     284,     0,   259,     0,   414,   421,   402,   395,     0,   405,
       0,   105,    99,     0,     0,   429,   416,   346,   154,   155,
     174,     0,   161,   159,   164,   166,   168,   170,   172,   162,
     389,   391,   485,    76,   500,     0,   502,   501,   241,     0,
       0,   240,     0,    64,    84,     0,    73,    68,    75,    71,
      74,    66,    62,     0,    55,   128,   222,   194,   197,     0,
     223,   216,     0,   492,     0,     0,     0,    97,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   147,   130,     0,
     296,     0,   298,     0,     0,     0,   300,   293,     0,   419,
       0,   272,     0,     0,   434,     0,     0,     0,   437,   436,
     400,   439,   441,   412,     0,     0,     0,     0,   176,     0,
     178,     0,     0,     0,     0,     0,     0,     0,   393,   506,
       0,   481,   503,   508,   242,     0,   236,   248,     0,   247,
     231,    85,     0,    60,     0,     0,     0,     0,   219,   222,
     221,   490,    92,    96,    94,   135,   150,   137,   139,   141,
     143,   145,   146,   152,     0,     0,   499,     0,     0,     0,
     301,     0,     0,   260,     0,     0,     0,   396,     0,   106,
       0,     0,     0,     0,     0,   347,   175,   182,   179,   158,
     184,   160,   165,   167,   169,   171,   173,   163,   392,     0,
     505,   479,     0,     0,     0,   244,   249,   238,   245,     0,
      87,   228,   192,   226,   225,     0,     0,   224,   148,   268,
     280,   510,   512,   497,     0,     0,   276,   285,   263,   306,
       0,   403,     0,   406,   100,     0,     0,   430,   417,     0,
     180,   177,     0,   507,   482,     0,   504,   509,   243,     0,
       0,    86,   195,   198,   296,     0,     0,     0,   294,   302,
       0,     0,   264,   261,     0,   303,     0,   273,   397,     0,
     438,   440,   183,   179,   185,   250,   246,     0,     0,   511,
     513,     0,     0,     0,     0,     0,   309,     0,   181,   269,
     281,   277,   286,   265,   307,     0,     0,   407,     0,     0,
       0,     0,   304,   310,   270,   282,   287,   308,     0,   305
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,
    -658,  -658,  -658,  -658,  -658,   424,  -658,  -658,  -658,  -658,
    -658,  -658,  -658,  -658,   434,  -658,  -658,  -658,  -658,  -658,
    -658,  -658,   280,  -658,  -658,  -658,  -658,  -658,  -658,  -658,
    -658,  -658,    43,  -658,  -658,    -4,  -658,  -658,  -658,  -658,
    -658,  -658,  -658,   724,  -658,  -658,  -658,   798,   -29,  -658,
    -658,  -119,  -658,    59,  -658,  -658,  -658,  -658,  -658,  -658,
    -658,   -23,  -658,  -658,  -658,  -658,  -330,  -658,  -658,  -658,
    -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -133,  -658,
    -658,  -658,  -658,   379,   -38,  -658,  -658,  -658,  -658,  -658,
    -658,  -402,  -658,    27,  -658,  -658,  -658,  -658,  -658,  -658,
    -658,  -658,  -596,  -658,    -2,  -658,  -658,  -658,  -658,   320,
    -658,  -658,  -658,  -658,     8,  -658,  -104,  -658,  -658,  -658,
     585,   -22,  -658,  -134,  -658,  -658,  -658,  -658,  -658,  -657,
    -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,
    -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -640,
    -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,
    -658,  -658,  -658,  -658,  -327,  -658,  -658,  -658,  -658,  -164,
    -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,
     596,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,
    -658,  -132,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,
    -658,  -658,  -658,  -658,    25,  -658,   -28,  -658,  -658,  -658,
    -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,
    -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,  -658,
    -658,   -80,  -658,  -658,  -658,  -658,   306,  -658,  -658,  -658,
    -658,  -658,  -658,  -658,  -658,  -658,  1117,  -658,  -363,  -658,
    -658,   -45,  -658,  -658,  -658,  -658,    14,  -658,  -658,  -658,
    -658,   574,  -658,  -658,  -658,  -658,  -658,   -30,  -658,  -536,
    -658,  -658,   -83,  -658,  -658
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     5,    39,    40,    28,    29,   112,   397,
      33,   266,    31,   247,   105,   120,   267,   121,   407,   404,
     406,   403,   400,   399,   110,   248,   111,   385,   386,   384,
     383,   410,   257,   398,   258,   520,   513,   519,   516,   518,
     515,   619,    87,   161,   163,   615,   769,   147,   283,   430,
     636,   639,   638,    88,   328,   667,   325,   153,   273,   432,
     288,   434,   532,   435,   559,   640,   642,   643,   644,   645,
     646,   648,   722,   436,   641,   649,   800,   591,   681,   687,
     682,   683,   684,   685,   686,   677,   679,   748,   801,   833,
     680,   799,   802,    89,   144,   415,   626,   416,   706,   417,
     707,   156,   437,   630,   418,   425,   279,   420,   538,   421,
     629,   632,   631,   709,   775,   523,   623,   381,   507,   241,
     382,   242,   610,   506,   609,   764,   698,   810,   699,   809,
      35,    90,   203,    91,   308,   660,   788,   823,   843,    92,
     298,   650,   814,   858,   309,   735,   167,   569,   820,   299,
     651,   815,   859,   304,   658,   821,   860,   199,   656,   438,
     302,   565,   731,   785,   790,   845,   868,   824,   861,   856,
      93,    94,   341,   342,    95,   169,   158,   183,   159,    96,
     360,   366,   361,   362,   363,   364,   365,   367,   343,   676,
      97,   345,   344,   174,   346,   347,   348,   349,   350,   351,
     359,    98,   352,   353,   354,   357,   355,   356,   358,   175,
      99,   232,   368,   498,   600,   688,   311,   319,   663,   792,
     330,   583,   318,   662,   322,   665,   829,   192,   331,   584,
     313,   335,   675,   446,   314,   186,   180,   179,   333,   674,
     370,   670,   741,   673,   742,   171,     6,    38,   125,   126,
     101,   196,   197,   194,   189,   178,   102,   201,   103,   235,
     375,   373,   501,   761,   374,   500,   157,   280,   424,   542,
     541,   489,   442,   568,   654,   653,   605,   606,   762,   607,
     759,   763,   728,   816,   817
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     100,   100,   155,   733,   529,   305,   635,   104,   274,    41,
     724,   154,   503,   401,   465,   466,   238,   233,   239,    16,
      17,    44,    18,    19,    20,   689,    45,    46,   455,   488,
     705,   286,    -2,    44,   693,   562,     3,   187,    45,    46,
      47,   188,   310,   312,    21,     7,     8,     9,   249,    41,
     207,   208,   146,   209,    10,     4,    55,   782,   208,   250,
     209,   251,   252,   253,   254,   145,    11,    12,   408,   409,
     825,    13,   826,    44,   281,    58,    14,  -256,    45,    46,
      47,   140,   281,   140,   234,    59,    22,  -256,  -256,   690,
     456,   414,   668,    60,    61,    62,    63,    64,  -256,    65,
      66,    26,    68,    69,    70,  -256,    71,  -256,   240,    27,
      72,   402,   300,   777,   617,    73,    74,    75,    76,    77,
      78,    79,    80,   711,    81,    82,    83,    84,    23,    24,
      25,   255,    41,   671,   439,    86,   208,   412,   209,   146,
     146,   100,   695,   827,   447,   413,   783,   702,    15,   290,
     291,   256,   270,   271,   794,   620,    44,   124,   669,   598,
     292,    45,    46,    47,    81,    82,    83,   440,   140,   140,
     140,   100,   444,   100,   837,   100,    30,   338,  -256,   339,
     205,   340,  -256,   379,    32,   122,    34,   208,   208,   209,
     209,   123,    36,   140,   537,   380,   490,   281,   599,   672,
     791,   864,   865,   866,   369,   206,   521,   522,   696,   458,
     301,   467,   160,   703,   470,   471,   472,   473,   474,   475,
     476,   477,   478,   479,   480,   481,   482,   807,   106,    71,
     107,   108,    37,   162,   109,   652,   713,   208,   208,   209,
     209,   164,   576,    41,   208,   165,   209,    81,    82,    83,
      84,   797,   798,   166,   426,   324,   100,   427,    86,   433,
     168,   678,   431,   208,   428,   209,   170,    44,   371,   618,
     100,   372,    45,    46,    47,   595,   596,   597,   172,   598,
     100,   100,   173,   592,   289,   100,   100,   448,   176,   177,
     429,   449,   450,   146,   593,    16,    17,   181,   182,    19,
      20,   830,   831,   184,   185,   594,   419,   595,   596,   597,
     190,   598,   191,   100,   100,   290,   291,   193,   599,   195,
      21,   725,   140,   726,   624,   198,   292,   857,   732,   202,
     200,   140,   100,   734,   218,   219,   220,   221,   222,   594,
      71,   595,   596,   597,   780,   598,   422,   746,   204,   423,
     599,   751,   752,   753,   754,   755,   756,   757,    81,    82,
      83,    84,    22,   236,   237,   243,    41,   811,   155,    86,
     244,   140,   113,   114,   115,   116,   566,   154,   117,   567,
     146,   146,   582,   281,   599,   414,   118,   611,   627,   628,
      44,   281,   281,   496,   497,    45,    46,    47,   245,   155,
     246,   259,   301,   549,    23,    24,    25,   119,   154,   140,
     140,   260,   577,   625,   208,   282,   209,   143,   100,   143,
     261,   275,   633,   579,   571,   208,   262,   209,   276,   146,
     146,   146,   146,   263,   585,   264,   208,   657,   209,   146,
     647,   265,   533,   534,   535,   536,   586,   278,   208,   140,
     209,   793,   543,   208,   285,   209,   277,   284,   140,   140,
     140,   140,   287,    71,   295,   296,   307,   297,   140,   832,
     601,   306,   834,   376,   327,   140,   377,   378,   155,   100,
     140,    81,    82,    83,    84,   612,   439,   154,   387,   389,
     390,   391,    86,   392,   844,   216,   217,   218,   219,   220,
     221,   222,   393,   394,   143,   143,   143,   395,   396,   405,
     441,   443,   745,   445,   457,   855,   734,   453,   463,   517,
     540,   464,   716,   717,   718,   719,   720,   721,   504,   143,
     723,   734,   734,   734,   564,   505,   514,   544,   539,   592,
     593,   548,   545,   546,   547,   550,   560,   747,   740,   140,
     593,   594,   146,   595,   596,   597,   561,   598,   563,   140,
     572,   594,   574,   595,   596,   597,   592,   598,   216,   217,
     218,   219,   220,   221,   222,   140,   573,   593,   222,   209,
     575,   140,   590,   588,   140,   589,   608,   614,   594,   637,
     595,   596,   597,   634,   598,   100,   599,   784,   659,   661,
     155,   704,   140,   647,   666,   691,   599,   433,   301,   154,
     664,   715,   692,   592,   693,   694,   414,   697,   700,   701,
     818,   819,   100,   689,   593,   727,   729,   787,   730,   766,
     749,   750,   100,   599,   301,   594,   760,   595,   596,   597,
     146,   598,   768,   146,   767,   808,   146,   771,   100,   779,
     439,   838,   772,   781,   786,   205,   708,   842,   143,   710,
     601,   789,   805,   812,   813,   822,   849,   143,   690,   140,
     846,   850,   140,   782,   854,   140,   851,   852,   524,   140,
     599,   140,   388,   140,   140,   140,   140,   140,   140,   140,
     835,   411,   140,   592,   301,   712,   301,   839,   770,   778,
     848,   301,   508,   765,   593,   776,   836,   143,   269,   853,
     140,   592,   828,   758,   531,   594,   804,   595,   596,   597,
     301,   598,   593,   146,   301,   301,   301,   301,   301,   301,
     301,   155,   806,   594,   840,   595,   596,   597,   143,   598,
     154,   303,     0,     0,     0,   143,   143,     0,     0,   773,
     773,     0,   140,     0,   862,     0,     0,   100,     0,     0,
     599,     0,   141,     0,   141,   140,   216,   217,   218,   219,
     220,   221,   222,     0,     0,     0,     0,     0,   599,     0,
       0,     0,     0,     0,     0,   143,   100,   100,     0,     0,
       0,   301,   100,   100,   143,   143,   143,   143,   841,     0,
       0,     0,     0,     0,   143,   570,     0,   140,     0,     0,
       0,   143,   140,     0,     0,   100,   143,   216,   217,   218,
     219,   220,   221,   222,     0,     0,     0,     0,   100,     0,
     100,   100,   100,   100,   863,     0,   142,     0,     0,   867,
     100,     0,   301,     0,     0,   301,   869,   551,   552,   141,
     141,   141,   140,     0,     0,   553,   587,     0,   414,   140,
       0,   224,   225,   226,   227,   228,   229,   301,     0,   554,
       0,   555,   556,   557,   141,   558,   301,     0,    41,   230,
     231,     0,     0,     0,     0,   143,     0,     0,   301,     0,
       0,     0,     0,     0,     0,   143,     0,     0,     0,     0,
       0,     0,    44,     0,     0,     0,     0,    45,    46,    47,
       0,   143,     0,     0,   451,   452,     0,   143,   454,     0,
     143,     0,     0,   142,   142,    55,   459,   460,     0,   461,
       0,   462,     0,     0,     0,     0,     0,     0,   143,     0,
     468,   469,     0,     0,   143,   216,   217,   218,   219,   220,
     221,   222,     0,     0,    59,     0,     0,   491,   492,   493,
     494,   495,    60,    61,    62,    63,    64,   655,    65,    66,
     143,    68,    69,    70,     0,    71,     0,     0,     0,    72,
       0,     0,     0,     0,    73,    74,    75,    76,    77,    78,
      79,    80,     0,    81,    82,    83,    84,     0,     0,     0,
       0,     0,     0,   141,    86,   143,     0,     0,   143,     0,
       0,   143,   141,     0,     0,   143,     0,   143,    41,   143,
     143,   143,   143,   143,   143,   143,     0,     0,   143,     0,
     143,     0,   143,     0,     0,     0,     0,   143,     0,     0,
       0,     0,    44,     0,     0,     0,   143,    45,    46,    47,
       0,     0,   141,     0,     0,     0,   143,     0,     0,     0,
     143,   143,   143,   143,   143,   143,   143,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   142,     0,     0,
       0,     0,     0,   141,     0,   774,   774,     0,   143,     0,
     141,   141,     0,     0,   483,     0,     0,   604,     0,     0,
       0,   143,     0,     0,   484,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   128,    71,   272,     0,     0,   132,
     133,   134,   135,   136,     0,     0,     0,   143,     0,     0,
     141,     0,     0,   485,    82,    83,   138,     0,     0,   141,
     141,   141,   141,   143,   486,     0,     0,     0,   143,   141,
       0,     0,     0,     0,     0,     0,   141,   487,     0,     0,
       0,   141,     0,     0,   142,   142,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   143,     0,
       0,   143,     0,     0,     0,     0,     0,     0,   143,     0,
       0,     0,     0,     0,     0,   143,   210,   211,   212,   213,
     214,   215,     0,   143,   216,   217,   218,   219,   220,   221,
     222,     0,   143,   142,   142,   142,   142,     0,     0,     0,
       0,     0,     0,   142,   143,     0,     0,     0,     0,     0,
     141,     0,     0,     0,     0,     0,     0,     0,     0,   268,
     141,     0,     0,   223,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   141,     0,   736,   737,
       0,   738,   141,     0,     0,   141,     0,     0,     0,     0,
     743,   744,     0,     0,     0,   293,   294,     0,     0,     0,
       0,     0,     0,   141,     0,     0,     0,     0,     0,   141,
       0,     0,     0,     0,     0,   315,   316,   317,     0,     0,
     320,   321,     0,   323,   326,     0,   329,     0,     0,   332,
       0,   334,     0,   336,   337,   141,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   142,   795,   796,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     141,     0,     0,   141,     0,   803,   141,     0,     0,     0,
     141,     0,   141,   487,   141,   141,   141,   141,   141,   141,
     141,     0,     0,   141,     0,   141,     0,   141,     0,     0,
       0,     0,   141,     0,     0,     0,     0,     0,     0,   487,
       0,   141,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   141,     0,     0,     0,   141,   141,   141,   141,   141,
     141,   141,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   142,   847,     0,   142,     0,     0,
     142,     0,     0,   141,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   141,     0,     0,   487,
       0,   487,     0,     0,     0,     0,   487,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   141,     0,     0,   487,     0,     0,     0,   487,
     487,   487,   487,   487,   487,   487,     0,   499,   141,     0,
       0,     0,   502,   141,     0,     0,     0,     0,     0,     0,
     509,   510,   511,   512,     0,     0,     0,   142,     0,     0,
       0,     0,     0,     0,     0,     0,   525,   526,     0,     0,
     527,   528,     0,   141,   530,     0,   141,     0,     0,     0,
       0,     0,     0,   141,     0,     0,     0,     0,     0,     0,
     141,     0,     0,     0,     0,     0,   487,     0,   141,     0,
       0,     0,     0,     0,     0,     0,     0,   141,     0,     0,
       0,     0,     0,    41,    42,     0,     0,     0,     0,   141,
     578,    43,   580,     0,   581,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    44,     0,     0,
       0,     0,    45,    46,    47,     0,     0,   487,     0,    48,
     487,     0,    49,    50,    51,     0,     0,    52,    53,    54,
      55,    56,     0,     0,    57,     0,     0,   602,     0,     0,
       0,     0,   487,     0,     0,     0,     0,     0,     0,    58,
     613,   487,   616,     0,     0,   502,   621,   622,     0,    59,
       0,     0,     0,   487,     0,     0,     0,    60,    61,    62,
      63,    64,     0,    65,    66,    67,    68,    69,    70,     0,
      71,     0,     0,     0,    72,     0,     0,     0,     0,    73,
      74,    75,    76,    77,    78,    79,    80,     0,    81,    82,
      83,    84,     0,    41,    42,    85,     0,     0,     0,    86,
       0,    43,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    44,     0,     0,
       0,     0,    45,    46,    47,     0,     0,     0,     0,    48,
       0,     0,    49,    50,    51,     0,     0,    52,    53,    54,
      55,     0,     0,     0,    57,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    58,
       0,     0,     0,     0,     0,     0,   714,    41,     0,    59,
       0,     0,     0,     0,     0,     0,     0,    60,    61,    62,
      63,    64,     0,    65,    66,    67,    68,    69,    70,     0,
      71,    44,     0,   739,    72,     0,    45,    46,    47,    73,
      74,    75,    76,    77,    78,    79,    80,     0,    81,    82,
      83,    84,    41,    42,     0,    85,     0,     0,     0,    86,
      43,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    44,     0,     0,     0,
       0,    45,    46,    47,     0,     0,     0,     0,     0,     0,
     127,     0,     0,     0,     0,     0,    52,    53,    54,    55,
       0,     0,     0,   128,    71,   129,   130,   131,   132,   133,
     134,   135,   136,     0,     0,     0,     0,     0,    58,     0,
       0,     0,   137,    82,    83,   138,    41,     0,    59,     0,
       0,     0,     0,   139,     0,     0,    60,    61,    62,    63,
      64,     0,    65,    66,    67,    68,    69,    70,     0,    71,
      44,     0,     0,    72,     0,    45,    46,    47,    73,    74,
      75,    76,    77,    78,    79,    80,     0,    81,    82,    83,
      84,     0,     0,    55,     0,    41,     0,     0,    86,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    58,     0,     0,     0,     0,     0,     0,    44,
       0,     0,    59,     0,    45,    46,    47,     0,     0,     0,
      60,    61,    62,    63,    64,     0,    65,    66,     0,    68,
      69,    70,    55,    71,    41,     0,     0,    72,     0,     0,
       0,     0,    73,    74,    75,    76,    77,    78,    79,    80,
       0,   603,    82,    83,    84,     0,     0,     0,    44,     0,
       0,    59,    86,    45,    46,    47,     0,     0,     0,    60,
      61,    62,    63,    64,     0,    65,    66,     0,    68,    69,
      70,    55,    71,    41,     0,     0,    72,     0,   148,     0,
       0,    73,    74,    75,    76,    77,    78,    79,    80,     0,
      81,    82,    83,    84,     0,     0,     0,    44,     0,     0,
       0,    86,    45,    46,    47,     0,     0,     0,    60,    61,
      62,    63,    64,     0,    65,    66,     0,    68,    69,    70,
       0,    71,    41,     0,     0,    72,     0,   148,     0,     0,
      73,    74,    75,    76,    77,    78,    79,    80,     0,    81,
      82,    83,    84,     0,     0,     0,    44,     0,     0,   149,
      86,    45,    46,    47,     0,     0,   150,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   128,
      71,   129,   130,   131,   132,   133,   134,   135,   136,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   151,    82,
      83,   138,     0,     0,     0,     0,     0,     0,   149,   152,
       0,     0,     0,     0,     0,   150,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   128,    71,
     272,     0,     0,   132,   133,   134,   135,   136,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   151,    82,    83,
     138,     0,     0,     0,     0,     0,     0,     0,   152
};

static const yytype_int16 yycheck[] =
{
      28,    29,    40,   660,   406,   169,   542,    29,   127,     3,
     650,    40,   375,    20,   341,   342,    26,    56,    28,     9,
      10,    27,    12,    13,    14,    58,    32,    33,    64,   359,
     626,   150,     0,    27,    39,   437,     0,   125,    32,    33,
      34,   129,   174,   175,    34,     6,     7,     8,    15,     3,
      65,    66,    38,    68,    15,    23,    50,    39,    66,    26,
      68,    28,    29,    30,    31,    38,    27,    28,    24,    25,
      53,    32,    55,    27,    67,    69,    37,    25,    32,    33,
      34,    38,    67,    40,   123,    79,    76,    35,    36,   122,
     126,    67,    64,    87,    88,    89,    90,    91,    46,    93,
      94,   118,    96,    97,    98,    53,   100,    55,   118,   118,
     104,   118,   118,   709,   516,   109,   110,   111,   112,   113,
     114,   115,   116,   128,   118,   119,   120,   121,   118,   119,
     120,    98,     3,    64,   298,   129,    66,   130,    68,   125,
     126,   169,    64,   126,   308,   130,   128,    64,   109,    56,
      57,   118,   125,   126,   130,   518,    27,    25,   130,    84,
      67,    32,    33,    34,   118,   119,   120,   299,   125,   126,
     127,   199,   304,   201,   814,   203,   118,   199,   126,   201,
      39,   203,   130,    24,   118,    58,   118,    66,    66,    68,
      68,    64,   129,   150,    64,    36,   360,    67,   123,   130,
     130,   858,   859,   860,   232,    64,    24,    25,   130,   328,
     167,   343,   122,   130,   346,   347,   348,   349,   350,   351,
     352,   353,   354,   355,   356,   357,   358,   763,    15,   100,
      17,    18,   129,   122,    21,   565,   638,    66,    66,    68,
      68,   122,    64,     3,    66,   129,    68,   118,   119,   120,
     121,   130,   130,   129,    19,   126,   284,    22,   129,   288,
     129,   591,   284,    66,    29,    68,   127,    27,   124,    29,
     298,   127,    32,    33,    34,    80,    81,    82,   129,    84,
     308,   309,   129,    56,    25,   313,   314,   309,   125,   125,
      55,   313,   314,   279,    67,     9,    10,   129,   129,    13,
      14,   130,   130,   129,   129,    78,   279,    80,    81,    82,
     129,    84,   129,   341,   342,    56,    57,   129,   123,   129,
      34,   651,   279,   653,    38,   126,    67,   130,   658,    25,
     130,   288,   360,   660,    80,    81,    82,    83,    84,    78,
     100,    80,    81,    82,   117,    84,   124,   677,    64,   127,
     123,   681,   682,   683,   684,   685,   686,   687,   118,   119,
     120,   121,    76,    67,    25,    58,     3,   769,   406,   129,
      58,   328,    15,    16,    17,    18,   124,   406,    21,   127,
     366,   367,    64,    67,   123,    67,    29,   506,    64,    64,
      27,    67,    67,   366,   367,    32,    33,    34,    58,   437,
      58,    58,   359,   432,   118,   119,   120,    50,   437,   366,
     367,    58,    64,   532,    66,    25,    68,    38,   446,    40,
      58,   129,   541,    64,   446,    66,    58,    68,   129,   415,
     416,   417,   418,    58,    64,    58,    66,   569,    68,   425,
     559,    58,   415,   416,   417,   418,    64,    58,    66,   406,
      68,    64,   425,    66,   119,    68,   129,   129,   415,   416,
     417,   418,    58,   100,   118,   118,   125,   118,   425,   799,
     498,   119,   802,    58,   130,   432,    58,    58,   516,   507,
     437,   118,   119,   120,   121,   507,   650,   516,    25,    58,
      58,    58,   129,    58,   824,    78,    79,    80,    81,    82,
      83,    84,    58,    58,   125,   126,   127,    58,    58,    20,
      58,    40,   676,   128,    76,   845,   843,    64,   126,   119,
      64,   130,   641,   642,   643,   644,   645,   646,   129,   150,
     649,   858,   859,   860,   117,   129,   129,    58,   130,    56,
      67,   130,    58,    58,    58,   123,   130,    64,   667,   506,
      67,    78,   538,    80,    81,    82,    64,    84,    39,   516,
     130,    78,   126,    80,    81,    82,    56,    84,    78,    79,
      80,    81,    82,    83,    84,   532,   130,    67,    84,    68,
     126,   538,    58,   119,   541,   118,   118,   118,    78,    34,
      80,    81,    82,   118,    84,   623,   123,   729,   126,   125,
     638,   623,   559,   722,    76,   128,   123,   636,   565,   638,
     130,   640,    67,    56,    39,   117,    67,   118,    36,   117,
     130,   785,   650,    58,    67,   118,   129,   117,    41,   117,
     130,    64,   660,   123,   591,    78,   118,    80,    81,    82,
     626,    84,    64,   629,   130,   764,   632,    35,   676,    39,
     814,   815,   130,    58,    45,    39,   629,   821,   279,   632,
     688,    53,   118,   130,   130,    52,   130,   288,   122,   626,
     117,   130,   629,    39,   117,   632,    46,   130,   398,   636,
     123,   638,   248,   640,   641,   642,   643,   644,   645,   646,
     809,   267,   649,    56,   651,   636,   653,   816,   702,   722,
     833,   658,   382,   695,    67,   707,   810,   328,   123,   843,
     667,    56,   792,   688,   408,    78,   761,    80,    81,    82,
     677,    84,    67,   709,   681,   682,   683,   684,   685,   686,
     687,   769,   762,    78,   817,    80,    81,    82,   359,    84,
     769,   167,    -1,    -1,    -1,   366,   367,    -1,    -1,   706,
     707,    -1,   709,    -1,   117,    -1,    -1,   785,    -1,    -1,
     123,    -1,    38,    -1,    40,   722,    78,    79,    80,    81,
      82,    83,    84,    -1,    -1,    -1,    -1,    -1,   123,    -1,
      -1,    -1,    -1,    -1,    -1,   406,   814,   815,    -1,    -1,
      -1,   748,   820,   821,   415,   416,   417,   418,   820,    -1,
      -1,    -1,    -1,    -1,   425,   117,    -1,   764,    -1,    -1,
      -1,   432,   769,    -1,    -1,   843,   437,    78,    79,    80,
      81,    82,    83,    84,    -1,    -1,    -1,    -1,   856,    -1,
     858,   859,   860,   861,   856,    -1,    38,    -1,    -1,   861,
     868,    -1,   799,    -1,    -1,   802,   868,    56,    57,   125,
     126,   127,   809,    -1,    -1,    64,   117,    -1,    67,   816,
      -1,    58,    59,    60,    61,    62,    63,   824,    -1,    78,
      -1,    80,    81,    82,   150,    84,   833,    -1,     3,    76,
      77,    -1,    -1,    -1,    -1,   506,    -1,    -1,   845,    -1,
      -1,    -1,    -1,    -1,    -1,   516,    -1,    -1,    -1,    -1,
      -1,    -1,    27,    -1,    -1,    -1,    -1,    32,    33,    34,
      -1,   532,    -1,    -1,   318,   319,    -1,   538,   322,    -1,
     541,    -1,    -1,   125,   126,    50,   330,   331,    -1,   333,
      -1,   335,    -1,    -1,    -1,    -1,    -1,    -1,   559,    -1,
     344,   345,    -1,    -1,   565,    78,    79,    80,    81,    82,
      83,    84,    -1,    -1,    79,    -1,    -1,   361,   362,   363,
     364,   365,    87,    88,    89,    90,    91,    92,    93,    94,
     591,    96,    97,    98,    -1,   100,    -1,    -1,    -1,   104,
      -1,    -1,    -1,    -1,   109,   110,   111,   112,   113,   114,
     115,   116,    -1,   118,   119,   120,   121,    -1,    -1,    -1,
      -1,    -1,    -1,   279,   129,   626,    -1,    -1,   629,    -1,
      -1,   632,   288,    -1,    -1,   636,    -1,   638,     3,   640,
     641,   642,   643,   644,   645,   646,    -1,    -1,   649,    -1,
     651,    -1,   653,    -1,    -1,    -1,    -1,   658,    -1,    -1,
      -1,    -1,    27,    -1,    -1,    -1,   667,    32,    33,    34,
      -1,    -1,   328,    -1,    -1,    -1,   677,    -1,    -1,    -1,
     681,   682,   683,   684,   685,   686,   687,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   279,    -1,    -1,
      -1,    -1,    -1,   359,    -1,   706,   707,    -1,   709,    -1,
     366,   367,    -1,    -1,    79,    -1,    -1,   501,    -1,    -1,
      -1,   722,    -1,    -1,    89,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    99,   100,   101,    -1,    -1,   104,
     105,   106,   107,   108,    -1,    -1,    -1,   748,    -1,    -1,
     406,    -1,    -1,   118,   119,   120,   121,    -1,    -1,   415,
     416,   417,   418,   764,   129,    -1,    -1,    -1,   769,   425,
      -1,    -1,    -1,    -1,    -1,    -1,   432,   359,    -1,    -1,
      -1,   437,    -1,    -1,   366,   367,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   799,    -1,
      -1,   802,    -1,    -1,    -1,    -1,    -1,    -1,   809,    -1,
      -1,    -1,    -1,    -1,    -1,   816,    70,    71,    72,    73,
      74,    75,    -1,   824,    78,    79,    80,    81,    82,    83,
      84,    -1,   833,   415,   416,   417,   418,    -1,    -1,    -1,
      -1,    -1,    -1,   425,   845,    -1,    -1,    -1,    -1,    -1,
     506,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   122,
     516,    -1,    -1,   117,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   532,    -1,   662,   663,
      -1,   665,   538,    -1,    -1,   541,    -1,    -1,    -1,    -1,
     674,   675,    -1,    -1,    -1,   158,   159,    -1,    -1,    -1,
      -1,    -1,    -1,   559,    -1,    -1,    -1,    -1,    -1,   565,
      -1,    -1,    -1,    -1,    -1,   178,   179,   180,    -1,    -1,
     183,   184,    -1,   186,   187,    -1,   189,    -1,    -1,   192,
      -1,   194,    -1,   196,   197,   591,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   538,   741,   742,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     626,    -1,    -1,   629,    -1,   759,   632,    -1,    -1,    -1,
     636,    -1,   638,   565,   640,   641,   642,   643,   644,   645,
     646,    -1,    -1,   649,    -1,   651,    -1,   653,    -1,    -1,
      -1,    -1,   658,    -1,    -1,    -1,    -1,    -1,    -1,   591,
      -1,   667,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   677,    -1,    -1,    -1,   681,   682,   683,   684,   685,
     686,   687,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   626,   829,    -1,   629,    -1,    -1,
     632,    -1,    -1,   709,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   722,    -1,    -1,   651,
      -1,   653,    -1,    -1,    -1,    -1,   658,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   748,    -1,    -1,   677,    -1,    -1,    -1,   681,
     682,   683,   684,   685,   686,   687,    -1,   370,   764,    -1,
      -1,    -1,   375,   769,    -1,    -1,    -1,    -1,    -1,    -1,
     383,   384,   385,   386,    -1,    -1,    -1,   709,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   399,   400,    -1,    -1,
     403,   404,    -1,   799,   407,    -1,   802,    -1,    -1,    -1,
      -1,    -1,    -1,   809,    -1,    -1,    -1,    -1,    -1,    -1,
     816,    -1,    -1,    -1,    -1,    -1,   748,    -1,   824,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   833,    -1,    -1,
      -1,    -1,    -1,     3,     4,    -1,    -1,    -1,    -1,   845,
     453,    11,   455,    -1,   457,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,
      -1,    -1,    32,    33,    34,    -1,    -1,   799,    -1,    39,
     802,    -1,    42,    43,    44,    -1,    -1,    47,    48,    49,
      50,    51,    -1,    -1,    54,    -1,    -1,   500,    -1,    -1,
      -1,    -1,   824,    -1,    -1,    -1,    -1,    -1,    -1,    69,
     513,   833,   515,    -1,    -1,   518,   519,   520,    -1,    79,
      -1,    -1,    -1,   845,    -1,    -1,    -1,    87,    88,    89,
      90,    91,    -1,    93,    94,    95,    96,    97,    98,    -1,
     100,    -1,    -1,    -1,   104,    -1,    -1,    -1,    -1,   109,
     110,   111,   112,   113,   114,   115,   116,    -1,   118,   119,
     120,   121,    -1,     3,     4,   125,    -1,    -1,    -1,   129,
      -1,    11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,
      -1,    -1,    32,    33,    34,    -1,    -1,    -1,    -1,    39,
      -1,    -1,    42,    43,    44,    -1,    -1,    47,    48,    49,
      50,    -1,    -1,    -1,    54,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,
      -1,    -1,    -1,    -1,    -1,    -1,   639,     3,    -1,    79,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,    88,    89,
      90,    91,    -1,    93,    94,    95,    96,    97,    98,    -1,
     100,    27,    -1,   666,   104,    -1,    32,    33,    34,   109,
     110,   111,   112,   113,   114,   115,   116,    -1,   118,   119,
     120,   121,     3,     4,    -1,   125,    -1,    -1,    -1,   129,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,
      -1,    32,    33,    34,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    -1,    -1,    -1,    47,    48,    49,    50,
      -1,    -1,    -1,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,    -1,    -1,    -1,    -1,    -1,    69,    -1,
      -1,    -1,   118,   119,   120,   121,     3,    -1,    79,    -1,
      -1,    -1,    -1,   129,    -1,    -1,    87,    88,    89,    90,
      91,    -1,    93,    94,    95,    96,    97,    98,    -1,   100,
      27,    -1,    -1,   104,    -1,    32,    33,    34,   109,   110,
     111,   112,   113,   114,   115,   116,    -1,   118,   119,   120,
     121,    -1,    -1,    50,    -1,     3,    -1,    -1,   129,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    69,    -1,    -1,    -1,    -1,    -1,    -1,    27,
      -1,    -1,    79,    -1,    32,    33,    34,    -1,    -1,    -1,
      87,    88,    89,    90,    91,    -1,    93,    94,    -1,    96,
      97,    98,    50,   100,     3,    -1,    -1,   104,    -1,    -1,
      -1,    -1,   109,   110,   111,   112,   113,   114,   115,   116,
      -1,   118,   119,   120,   121,    -1,    -1,    -1,    27,    -1,
      -1,    79,   129,    32,    33,    34,    -1,    -1,    -1,    87,
      88,    89,    90,    91,    -1,    93,    94,    -1,    96,    97,
      98,    50,   100,     3,    -1,    -1,   104,    -1,     8,    -1,
      -1,   109,   110,   111,   112,   113,   114,   115,   116,    -1,
     118,   119,   120,   121,    -1,    -1,    -1,    27,    -1,    -1,
      -1,   129,    32,    33,    34,    -1,    -1,    -1,    87,    88,
      89,    90,    91,    -1,    93,    94,    -1,    96,    97,    98,
      -1,   100,     3,    -1,    -1,   104,    -1,     8,    -1,    -1,
     109,   110,   111,   112,   113,   114,   115,   116,    -1,   118,
     119,   120,   121,    -1,    -1,    -1,    27,    -1,    -1,    79,
     129,    32,    33,    34,    -1,    -1,    86,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   118,   119,
     120,   121,    -1,    -1,    -1,    -1,    -1,    -1,    79,   129,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    99,   100,
     101,    -1,    -1,   104,   105,   106,   107,   108,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   118,   119,   120,
     121,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   129
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   132,   133,     0,    23,   134,   377,     6,     7,     8,
      15,    27,    28,    32,    37,   109,     9,    10,    12,    13,
      14,    34,    76,   118,   119,   120,   118,   118,   137,   138,
     118,   143,   118,   141,   118,   261,   129,   129,   378,   135,
     136,     3,     4,    11,    27,    32,    33,    34,    39,    42,
      43,    44,    47,    48,    49,    50,    51,    54,    69,    79,
      87,    88,    89,    90,    91,    93,    94,    95,    96,    97,
      98,   100,   104,   109,   110,   111,   112,   113,   114,   115,
     116,   118,   119,   120,   121,   125,   129,   173,   184,   224,
     262,   264,   270,   301,   302,   305,   310,   321,   332,   341,
     347,   381,   387,   389,   262,   145,    15,    17,    18,    21,
     155,   157,   139,    15,    16,    17,    18,    21,    29,    50,
     146,   148,    58,    64,    25,   379,   380,    86,    99,   101,
     102,   103,   104,   105,   106,   107,   108,   118,   121,   129,
     173,   184,   188,   224,   225,   234,   397,   178,     8,    79,
      86,   118,   129,   188,   189,   225,   232,   397,   307,   309,
     122,   174,   122,   175,   122,   129,   129,   277,   129,   306,
     127,   376,   129,   129,   324,   340,   125,   125,   386,   368,
     367,   129,   129,   308,   129,   129,   366,   125,   129,   385,
     129,   129,   358,   129,   384,   129,   382,   383,   126,   288,
     130,   388,    25,   263,    64,    39,    64,    65,    66,    68,
      70,    71,    72,    73,    74,    75,    78,    79,    80,    81,
      82,    83,    84,   117,    58,    59,    60,    61,    62,    63,
      76,    77,   342,    56,   123,   390,    67,    25,    26,    28,
     118,   250,   252,    58,    58,    58,    58,   144,   156,    15,
      26,    28,    29,    30,    31,    98,   118,   163,   165,    58,
      58,    58,    58,    58,    58,    58,   142,   147,   387,   261,
     234,   234,   101,   189,   192,   129,   129,   129,    58,   237,
     398,    67,    25,   179,   129,   119,   192,    58,   191,    25,
      56,    57,    67,   387,   387,   118,   118,   118,   271,   280,
     118,   173,   291,   402,   284,   310,   119,   125,   265,   275,
     332,   347,   332,   361,   365,   387,   387,   387,   353,   348,
     387,   387,   355,   387,   126,   187,   387,   130,   185,   387,
     351,   359,   387,   369,   387,   362,   387,   387,   262,   262,
     262,   303,   304,   319,   323,   322,   325,   326,   327,   328,
     329,   330,   333,   334,   335,   337,   338,   336,   339,   331,
     311,   313,   314,   315,   316,   317,   312,   318,   343,   347,
     371,   124,   127,   392,   395,   391,    58,    58,    58,    24,
      36,   248,   251,   161,   160,   158,   159,    25,   155,    58,
      58,    58,    58,    58,    58,    58,    58,   140,   164,   154,
     153,    20,   118,   152,   150,    20,   151,   149,    24,    25,
     162,   146,   130,   130,    67,   226,   228,   230,   235,   234,
     238,   240,   124,   127,   399,   236,    19,    22,    29,    55,
     180,   262,   190,   189,   192,   194,   204,   233,   290,   310,
     332,    58,   403,    40,   332,   128,   364,   310,   262,   262,
     262,   321,   321,    64,   321,    64,   126,    76,   192,   321,
     321,   321,   321,   126,   130,   305,   305,   332,   321,   321,
     332,   332,   332,   332,   332,   332,   332,   332,   332,   332,
     332,   332,   332,    79,    89,   118,   129,   188,   207,   402,
     310,   321,   321,   321,   321,   321,   234,   234,   344,   387,
     396,   393,   387,   389,   129,   129,   254,   249,   250,   387,
     387,   387,   387,   167,   129,   171,   169,   119,   170,   168,
     166,    24,    25,   246,   163,   387,   387,   387,   387,   232,
     387,   377,   193,   234,   234,   234,   234,    64,   239,   130,
      64,   401,   400,   234,    58,    58,    58,    58,   130,   189,
     123,    56,    57,    64,    78,    80,    81,    82,    84,   195,
     130,    64,   232,    39,   117,   292,   124,   127,   404,   278,
     117,   262,   130,   130,   126,   126,    64,    64,   387,    64,
     387,   387,    64,   352,   360,    64,    64,   117,   119,   118,
      58,   208,    56,    67,    78,    80,    81,    82,    84,   123,
     345,   347,   387,   118,   321,   407,   408,   410,   118,   255,
     253,   192,   262,   387,   118,   176,   387,   232,    29,   172,
     389,   387,   387,   247,    38,   192,   227,    64,    64,   241,
     234,   243,   242,   192,   118,   410,   181,    34,   183,   182,
     196,   205,   197,   198,   199,   200,   201,   192,   202,   206,
     272,   281,   207,   406,   405,    92,   289,   332,   285,   126,
     266,   125,   354,   349,   130,   356,    76,   186,    64,   130,
     372,    64,   130,   374,   370,   363,   320,   216,   207,   217,
     221,   209,   211,   212,   213,   214,   215,   210,   346,    58,
     122,   128,    67,    39,   117,    64,   130,   118,   257,   259,
      36,   117,    64,   130,   262,   243,   229,   231,   234,   244,
     234,   128,   194,   232,   387,   189,   192,   192,   192,   192,
     192,   192,   203,   192,   290,   207,   207,   118,   413,   129,
      41,   293,   207,   270,   305,   276,   321,   321,   321,   387,
     192,   373,   375,   321,   321,   310,   207,    64,   218,   130,
      64,   207,   207,   207,   207,   207,   207,   207,   345,   411,
     118,   394,   409,   412,   256,   255,   117,   130,    64,   177,
     176,    35,   130,   173,   224,   245,   245,   243,   202,    39,
     117,    58,    39,   128,   332,   294,    45,   117,   267,    53,
     295,   130,   350,    64,   130,   321,   321,   130,   130,   222,
     207,   219,   223,   321,   392,   118,   408,   410,   192,   260,
     258,   232,   130,   130,   273,   282,   414,   415,   130,   310,
     279,   286,    52,   268,   298,    53,    55,   126,   372,   357,
     130,   130,   207,   220,   207,   192,   257,   290,   310,   192,
     413,   262,   310,   269,   207,   296,   117,   321,   219,   130,
     130,    46,   130,   264,   117,   207,   300,   130,   274,   283,
     287,   299,   117,   262,   270,   270,   270,   262,   297,   262
};

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
     381,   384,   381,   385,   381,   386,   381,   381,   388,   387,
     387,   387,   387,   387,   387,   390,   389,   391,   389,   392,
     393,   394,   392,   392,   396,   395,   398,   397,   399,   400,
     399,   401,   399,   403,   402,   404,   405,   404,   406,   404,
     407,   407,   407,   409,   408,   408,   411,   410,   412,   410,
     414,   413,   415,   413
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
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
       3,     0,     3,     0,     3,     0,     3,     1,     0,     4,
       2,     1,     1,     1,     1,     0,     3,     0,     4,     0,
       0,     0,     6,     1,     0,     3,     0,     3,     0,     0,
       4,     0,     3,     0,     3,     0,     0,     4,     0,     3,
       1,     1,     1,     0,     4,     3,     0,     4,     0,     4,
       0,     4,     0,     4
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
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
  int yytoken = 0;
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

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
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
      if (yytable_value_is_error (yyn))
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
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
     '$$ = $1'.

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
#line 151 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=insert_parameter((yyvsp[0]).ptr);}
#line 2544 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 6:
#line 152 "poet_yacc.y" /* yacc.c:1646  */
    {finish_parameter((yyvsp[-2])); }
#line 2550 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 7:
#line 153 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=GLOBAL_SCOPE; }
#line 2556 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 8:
#line 154 "poet_yacc.y" /* yacc.c:1646  */
    {eval_define(make_macroVar((yyvsp[-3]).ptr),(yyvsp[-1]).ptr); }
#line 2562 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 9:
#line 155 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=GLOBAL_VAR; }
#line 2568 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 10:
#line 155 "poet_yacc.y" /* yacc.c:1646  */
    { insert_eval((yyvsp[-1]).ptr); }
#line 2574 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 11:
#line 156 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=GLOBAL_VAR; }
#line 2580 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 12:
#line 156 "poet_yacc.y" /* yacc.c:1646  */
    { insert_cond((yyvsp[-1]).ptr); }
#line 2586 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 13:
#line 157 "poet_yacc.y" /* yacc.c:1646  */
    { insert_trace((yyvsp[-1])); }
#line 2592 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 14:
#line 158 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=insert_code((yyvsp[0]).ptr);(yyval).config=0; }
#line 2598 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 15:
#line 158 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = (yyvsp[-1]).ptr; }
#line 2604 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 17:
#line 159 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=insert_input(); }
#line 2610 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 18:
#line 159 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=(yyvsp[-1]).ptr; }
#line 2616 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 20:
#line 160 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=insert_output(); }
#line 2622 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 21:
#line 160 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=(yyvsp[-1]).ptr; }
#line 2628 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 23:
#line 161 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=insert_xform((yyvsp[0]).ptr); }
#line 2634 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 24:
#line 162 "poet_yacc.y" /* yacc.c:1646  */
    { set_xform_def((yyvsp[-2]).ptr,(yyvsp[0]).ptr); }
#line 2640 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 25:
#line 163 "poet_yacc.y" /* yacc.c:1646  */
    { insert_source((yyvsp[-1]).ptr); }
#line 2646 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 26:
#line 165 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = (yyvsp[-1]).ptr; }
#line 2652 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 29:
#line 167 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=GLOBAL_SCOPE;}
#line 2658 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 30:
#line 167 "poet_yacc.y" /* yacc.c:1646  */
    { set_input_debug((yyvsp[-4]).ptr,(yyvsp[0]).ptr); }
#line 2664 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 31:
#line 168 "poet_yacc.y" /* yacc.c:1646  */
    { set_input_target((yyvsp[-3]).ptr, (yyvsp[-3]).ptr); }
#line 2670 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 32:
#line 169 "poet_yacc.y" /* yacc.c:1646  */
    { set_input_target((yyvsp[-3]).ptr,make_varRef((yyvsp[0]).ptr,GLOBAL_VAR)); }
#line 2676 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 33:
#line 170 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=GLOBAL_SCOPE;}
#line 2682 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 34:
#line 170 "poet_yacc.y" /* yacc.c:1646  */
    {set_input_syntax((yyvsp[-4]).ptr,(yyvsp[0]).ptr);}
#line 2688 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 35:
#line 171 "poet_yacc.y" /* yacc.c:1646  */
    { set_input_codeType((yyvsp[-3]).ptr, (yyvsp[-3]).ptr); }
#line 2694 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 36:
#line 172 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=CODE_OR_XFORM_VAR; }
#line 2700 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 37:
#line 173 "poet_yacc.y" /* yacc.c:1646  */
    { set_input_codeType((yyvsp[-4]).ptr,(yyvsp[0]).ptr); }
#line 2706 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 38:
#line 174 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=GLOBAL_SCOPE;}
#line 2712 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 39:
#line 174 "poet_yacc.y" /* yacc.c:1646  */
    {set_input_files((yyvsp[-4]).ptr,(yyvsp[0]).ptr); }
#line 2718 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 40:
#line 175 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=GLOBAL_SCOPE;}
#line 2724 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 41:
#line 175 "poet_yacc.y" /* yacc.c:1646  */
    { set_input_annot((yyvsp[-4]).ptr,(yyvsp[0]).ptr); }
#line 2730 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 42:
#line 176 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=GLOBAL_SCOPE;}
#line 2736 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 43:
#line 176 "poet_yacc.y" /* yacc.c:1646  */
    { set_input_cond((yyvsp[-4]).ptr,(yyvsp[0]).ptr); }
#line 2742 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 44:
#line 178 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = (yyvsp[-1]).ptr; }
#line 2748 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 47:
#line 180 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=GLOBAL_VAR;}
#line 2754 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 48:
#line 181 "poet_yacc.y" /* yacc.c:1646  */
    { set_output_target((yyvsp[-4]).ptr,(yyvsp[0]).ptr); }
#line 2760 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 49:
#line 182 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=GLOBAL_SCOPE;}
#line 2766 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 50:
#line 183 "poet_yacc.y" /* yacc.c:1646  */
    { set_output_syntax((yyvsp[-4]).ptr,(yyvsp[0]).ptr); }
#line 2772 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 51:
#line 184 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=GLOBAL_VAR;}
#line 2778 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 52:
#line 185 "poet_yacc.y" /* yacc.c:1646  */
    { set_output_file((yyvsp[-4]).ptr,(yyvsp[0]).ptr); }
#line 2784 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 53:
#line 186 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=GLOBAL_SCOPE;}
#line 2790 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 54:
#line 187 "poet_yacc.y" /* yacc.c:1646  */
    { set_output_cond((yyvsp[-4]).ptr,(yyvsp[0]).ptr); }
#line 2796 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 55:
#line 188 "poet_yacc.y" /* yacc.c:1646  */
    { set_input_inline((yyvsp[-3]).ptr,(yyvsp[-1]).ptr);  }
#line 2802 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 58:
#line 191 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=(yyvsp[-1]).ptr; (yyval).config=(yyvsp[0]).config;}
#line 2808 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 60:
#line 192 "poet_yacc.y" /* yacc.c:1646  */
    { set_code_params((yyvsp[-5]).ptr,(yyvsp[-1]).ptr); (yyval).config=(yyvsp[-5]).config; }
#line 2814 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 61:
#line 193 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=ID_DEFAULT;}
#line 2820 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 62:
#line 193 "poet_yacc.y" /* yacc.c:1646  */
    { set_code_attr((yyvsp[-4]).ptr, (yyvsp[-3]).ptr, (yyvsp[0]).ptr); }
#line 2826 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 63:
#line 194 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=ID_DEFAULT;}
#line 2832 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 64:
#line 195 "poet_yacc.y" /* yacc.c:1646  */
    { set_local_static(make_sourceString("cond",4), (yyvsp[0]).ptr,LVAR_ATTR,(yyvsp[0]).ptr,1); (yyval).config=(yyvsp[-4]).config; }
#line 2838 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 65:
#line 196 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=ID_DEFAULT; }
#line 2844 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 66:
#line 197 "poet_yacc.y" /* yacc.c:1646  */
    { set_local_static(make_sourceString("rebuild",7), (yyvsp[0]).ptr,LVAR_ATTR,(yyvsp[0]).ptr,1); (yyval).config=(yyvsp[-4]).config; }
#line 2850 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 67:
#line 198 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=ID_DEFAULT; }
#line 2856 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 68:
#line 199 "poet_yacc.y" /* yacc.c:1646  */
    { set_code_parse((yyvsp[-4]).ptr,(yyvsp[0]).ptr); (yyval).config=(yyvsp[-4]).config; }
#line 2862 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 69:
#line 200 "poet_yacc.y" /* yacc.c:1646  */
    { set_code_lookahead((yyvsp[-3]).ptr, (yyvsp[0]).ptr); (yyval).config=(yyvsp[-3]).config; }
#line 2868 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 70:
#line 201 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=ID_DEFAULT; }
#line 2874 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 71:
#line 202 "poet_yacc.y" /* yacc.c:1646  */
    { set_local_static(make_sourceString("match",5), (yyvsp[0]).ptr,LVAR_ATTR,(yyvsp[0]).ptr,1); (yyval).config=(yyvsp[-4]).config; }
#line 2880 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 72:
#line 203 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=ID_DEFAULT; }
#line 2886 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 73:
#line 204 "poet_yacc.y" /* yacc.c:1646  */
    { set_local_static(make_sourceString("output",6), (yyvsp[0]).ptr,LVAR_ATTR,(yyvsp[0]).ptr,1); (yyval).config=(yyvsp[-4]).config; }
#line 2892 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 74:
#line 206 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[0]); }
#line 2898 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 75:
#line 207 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceString("PARSE",5); }
#line 2904 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 76:
#line 209 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr= make_varRef((yyvsp[0]).ptr,(yyvsp[-1]).config); (yyval).config=(yyvsp[-1]).config; }
#line 2910 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 77:
#line 210 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_varRef((yyvsp[0]).ptr,GLOBAL_SCOPE); (yyval).config=(yyvsp[-3]).config; }
#line 2916 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 78:
#line 211 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval)=(yyvsp[0]); }
#line 2922 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 79:
#line 212 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval)=(yyvsp[0]); }
#line 2928 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 80:
#line 214 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_dummyOperator(POET_OP_XFORM); (yyval).config=(yyvsp[0]).config; }
#line 2934 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 81:
#line 215 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_varRef((yyvsp[0]).ptr,XFORM_VAR); (yyval).config=(yyvsp[-2]).config; }
#line 2940 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 82:
#line 217 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_dummyOperator(POET_OP_CODE); }
#line 2946 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 83:
#line 218 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_varRef((yyvsp[0]).ptr,CODE_VAR); }
#line 2952 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 84:
#line 221 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_localPar((yyvsp[0]).ptr,0,LVAR_CODEPAR); }
#line 2958 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 85:
#line 222 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=CODE_OR_XFORM_VAR; }
#line 2964 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 86:
#line 222 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_localPar((yyvsp[-3]).ptr, (yyvsp[0]).ptr,LVAR_CODEPAR); }
#line 2970 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 87:
#line 223 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = make_sourceVector((yyvsp[-2]).ptr, (yyvsp[0]).ptr); }
#line 2976 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 89:
#line 225 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=(yyvsp[-1]).ptr; }
#line 2982 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 91:
#line 226 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=GLOBAL_SCOPE;}
#line 2988 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 92:
#line 227 "poet_yacc.y" /* yacc.c:1646  */
    {set_param_type((yyvsp[-4]).ptr,(yyvsp[0]).ptr); }
#line 2994 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 93:
#line 228 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=GLOBAL_SCOPE;}
#line 3000 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 94:
#line 228 "poet_yacc.y" /* yacc.c:1646  */
    { set_param_default((yyvsp[-4]).ptr, (yyvsp[0]).ptr); }
#line 3006 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 95:
#line 229 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=GLOBAL_SCOPE; }
#line 3012 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 96:
#line 229 "poet_yacc.y" /* yacc.c:1646  */
    { set_param_parse((yyvsp[-4]).ptr, (yyvsp[0]).ptr); }
#line 3018 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 97:
#line 230 "poet_yacc.y" /* yacc.c:1646  */
    { set_param_message((yyvsp[-3]).ptr, (yyvsp[0]).ptr); }
#line 3024 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 98:
#line 233 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config; }
#line 3030 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 99:
#line 234 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-5]).config; }
#line 3036 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 100:
#line 235 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_MAP, (yyvsp[-4]).ptr, (yyvsp[-1]).ptr); }
#line 3042 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 101:
#line 236 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_dummyOperator(POET_OP_MAP); }
#line 3048 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 102:
#line 237 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_dummyOperator(POET_OP_MAP); }
#line 3054 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 103:
#line 238 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_dummyOperator(POET_OP_MAP); }
#line 3060 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 104:
#line 240 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=make_sourceUop(POET_OP_MAP, (yyvsp[-1]).ptr); }
#line 3066 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 105:
#line 243 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceVector2((yyvsp[-2]).ptr,(yyvsp[0]).ptr); }
#line 3072 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 106:
#line 245 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_inputlist((yyvsp[-4]).ptr,make_sourceVector2((yyvsp[-2]).ptr,(yyvsp[0]).ptr)); }
#line 3078 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 107:
#line 247 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=(yyvsp[0]).ptr; }
#line 3084 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 108:
#line 248 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_any(); }
#line 3090 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 109:
#line 249 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_type(TYPE_INT); }
#line 3096 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 110:
#line 250 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_type(TYPE_FLOAT); }
#line 3102 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 111:
#line 251 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_type(TYPE_STRING); }
#line 3108 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 112:
#line 252 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_type(TYPE_ID); }
#line 3114 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 113:
#line 253 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_dummyOperator(POET_OP_EXP); }
#line 3120 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 114:
#line 254 "poet_yacc.y" /* yacc.c:1646  */
    {  (yyval).ptr = make_dummyOperator(POET_OP_VAR); }
#line 3126 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 115:
#line 255 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_dummyOperator(POET_OP_TUPLE); }
#line 3132 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 116:
#line 256 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval)=(yyvsp[0]); }
#line 3138 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 117:
#line 258 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval)=(yyvsp[0]); }
#line 3144 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 118:
#line 259 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = negate_Iconst((yyvsp[0]).ptr); }
#line 3150 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 119:
#line 260 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_sourceUop(POET_OP_APPLY, (yyvsp[-1]).ptr); }
#line 3156 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 120:
#line 261 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=(yyvsp[0]).ptr; }
#line 3162 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 121:
#line 262 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config; }
#line 3168 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 122:
#line 263 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_sourceAssign(make_varRef((yyvsp[-3]).ptr,ASSIGN_VAR), (yyvsp[0]).ptr); }
#line 3174 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 123:
#line 264 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_typeNot((yyvsp[0])); }
#line 3180 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 124:
#line 265 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 3186 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 125:
#line 265 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[-1]).ptr; }
#line 3192 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 126:
#line 267 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval)=(yyvsp[0]); }
#line 3198 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 127:
#line 268 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3204 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 128:
#line 269 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_typeTor((yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 3210 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 129:
#line 271 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config; }
#line 3216 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 130:
#line 271 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = ((yyvsp[0]).ptr==0)? (yyvsp[-2]).ptr : make_typelist2((yyvsp[-2]).ptr,(yyvsp[0]).ptr); }
#line 3222 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 131:
#line 272 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[0]).ptr; }
#line 3228 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 132:
#line 273 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = make_typeUop(TYPE_LIST1, (yyvsp[-1]).ptr); }
#line 3234 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 133:
#line 274 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = make_typeUop(TYPE_LIST, (yyvsp[-1]).ptr); }
#line 3240 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 134:
#line 275 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3246 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 135:
#line 276 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = make_sourceBop(POET_OP_RANGE, (yyvsp[-3]).ptr,(yyvsp[0]).ptr);}
#line 3252 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 136:
#line 277 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3258 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 137:
#line 278 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_sourceBop(POET_OP_PLUS, (yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 3264 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 138:
#line 279 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3270 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 139:
#line 280 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_MULT,  (yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 3276 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 140:
#line 281 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3282 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 141:
#line 282 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_DIVIDE,  (yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 3288 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 142:
#line 283 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3294 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 143:
#line 283 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_MOD,  (yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 3300 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 144:
#line 284 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3306 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 145:
#line 284 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_CONS,(yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 3312 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 146:
#line 286 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=0; }
#line 3318 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 147:
#line 287 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config; }
#line 3324 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 148:
#line 287 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_typelist2((yyvsp[-2]).ptr,(yyvsp[0]).ptr); }
#line 3330 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 149:
#line 288 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3336 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 150:
#line 289 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceVector2((yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 3342 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 151:
#line 290 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3348 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 152:
#line 290 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceVector((yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 3354 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 153:
#line 292 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval)=(yyvsp[0]); }
#line 3360 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 154:
#line 293 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = negate_Iconst((yyvsp[0]).ptr); }
#line 3366 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 155:
#line 294 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceUop(POET_OP_CLEAR, make_varRef((yyvsp[0]).ptr,ASSIGN_VAR)); }
#line 3372 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 156:
#line 295 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=(yyvsp[0]).ptr; }
#line 3378 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 157:
#line 296 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 3384 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 158:
#line 296 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[-1]).ptr; }
#line 3390 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 159:
#line 297 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3396 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 160:
#line 298 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_typeTor((yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 3402 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 161:
#line 299 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = make_typeUop(TYPE_LIST, (yyvsp[-1]).ptr); }
#line 3408 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 162:
#line 300 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3414 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 163:
#line 301 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = make_sourceBop(POET_OP_RANGE, (yyvsp[-3]).ptr,(yyvsp[0]).ptr);}
#line 3420 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 164:
#line 302 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3426 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 165:
#line 303 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_sourceBop(POET_OP_PLUS, (yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 3432 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 166:
#line 304 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3438 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 167:
#line 305 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_MULT,  (yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 3444 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 168:
#line 306 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3450 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 169:
#line 307 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_DIVIDE,  (yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 3456 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 170:
#line 308 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3462 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 171:
#line 309 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_MOD,  (yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 3468 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 172:
#line 310 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3474 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 173:
#line 310 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_CONS,(yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 3480 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 174:
#line 311 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3486 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 175:
#line 311 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_sourceAssign(make_varRef((yyvsp[-3]).ptr,ASSIGN_VAR), (yyvsp[0]).ptr); }
#line 3492 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 176:
#line 313 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config; }
#line 3498 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 177:
#line 313 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = ((yyvsp[0]).ptr==0)? (yyvsp[-2]).ptr : make_typelist2((yyvsp[-2]).ptr,(yyvsp[0]).ptr); }
#line 3504 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 178:
#line 314 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[0]).ptr; }
#line 3510 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 179:
#line 316 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=0; }
#line 3516 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 180:
#line 317 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config; }
#line 3522 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 181:
#line 317 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_typelist2((yyvsp[-2]).ptr,(yyvsp[0]).ptr); }
#line 3528 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 182:
#line 318 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3534 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 183:
#line 319 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceVector2((yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 3540 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 184:
#line 320 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3546 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 185:
#line 320 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceVector((yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 3552 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 186:
#line 322 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=(yyvsp[0]).ptr; }
#line 3558 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 187:
#line 323 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[0]).ptr; }
#line 3564 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 188:
#line 324 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[0]).ptr; }
#line 3570 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 189:
#line 325 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_empty_list(); }
#line 3576 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 190:
#line 327 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config; }
#line 3582 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 191:
#line 327 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-4]).config;}
#line 3588 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 192:
#line 327 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceUop(POET_OP_TUPLE, make_typelist2((yyvsp[-3]).ptr,(yyvsp[-1]).ptr)); }
#line 3594 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 193:
#line 328 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config; }
#line 3600 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 194:
#line 328 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-5]).config;}
#line 3606 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 195:
#line 328 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_LIST,(yyvsp[-4]).ptr,(yyvsp[-1]).ptr); }
#line 3612 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 196:
#line 329 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config; }
#line 3618 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 197:
#line 329 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-5]).config;}
#line 3624 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 198:
#line 329 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_LIST1,(yyvsp[-4]).ptr,(yyvsp[-1]).ptr); }
#line 3630 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 199:
#line 331 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval)=(yyvsp[0]); }
#line 3636 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 200:
#line 332 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval)=(yyvsp[0]); }
#line 3642 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 201:
#line 333 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3648 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 202:
#line 334 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_typeTor((yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 3654 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 203:
#line 335 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = make_typeUop(TYPE_LIST1, (yyvsp[-1]).ptr); }
#line 3660 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 204:
#line 336 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = make_typeUop(TYPE_LIST, (yyvsp[-1]).ptr); }
#line 3666 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 205:
#line 338 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval)=(yyvsp[0]); }
#line 3672 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 206:
#line 339 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=(yyvsp[0]).ptr; }
#line 3678 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 207:
#line 340 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_typeNot((yyvsp[0])); }
#line 3684 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 208:
#line 341 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[0]); }
#line 3690 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 209:
#line 342 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config; }
#line 3696 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 210:
#line 343 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_sourceAssign(make_varRef((yyvsp[-3]).ptr,ASSIGN_VAR), (yyvsp[0]).ptr); }
#line 3702 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 211:
#line 344 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3708 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 212:
#line 345 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_typeTor((yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 3714 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 213:
#line 346 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 3720 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 214:
#line 346 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[-1]).ptr; }
#line 3726 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 215:
#line 348 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config; }
#line 3732 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 216:
#line 348 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = ((yyvsp[0]).ptr==0)? (yyvsp[-2]).ptr : make_typelist2((yyvsp[-2]).ptr,(yyvsp[0]).ptr); }
#line 3738 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 217:
#line 349 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[0]).ptr; }
#line 3744 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 218:
#line 350 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3750 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 219:
#line 351 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceVector2((yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 3756 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 220:
#line 352 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3762 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 221:
#line 352 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceVector((yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 3768 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 222:
#line 353 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=0; }
#line 3774 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 223:
#line 354 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config; }
#line 3780 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 224:
#line 354 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_typelist2((yyvsp[-2]).ptr,(yyvsp[0]).ptr); }
#line 3786 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 225:
#line 356 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=(yyvsp[0]).ptr; }
#line 3792 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 226:
#line 357 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[0]); }
#line 3798 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 227:
#line 359 "poet_yacc.y" /* yacc.c:1646  */
    {yaccState=YACC_CODE; (yyval).config=ID_DEFAULT;}
#line 3804 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 228:
#line 359 "poet_yacc.y" /* yacc.c:1646  */
    { yaccState=YACC_DEFAULT; set_code_def((yyvsp[-4]).ptr,(yyvsp[-1]).ptr); }
#line 3810 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 229:
#line 360 "poet_yacc.y" /* yacc.c:1646  */
    { set_code_def((yyvsp[-1]).ptr, 0); }
#line 3816 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 230:
#line 361 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=ID_DEFAULT; }
#line 3822 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 231:
#line 361 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=(yyvsp[-1]).ptr; }
#line 3828 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 232:
#line 362 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = 0; }
#line 3834 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 234:
#line 363 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval)=(yyvsp[-1]); }
#line 3840 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 236:
#line 364 "poet_yacc.y" /* yacc.c:1646  */
    {set_xform_params((yyvsp[-5]).ptr,(yyvsp[-1]).ptr); }
#line 3846 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 237:
#line 365 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=0;}
#line 3852 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 239:
#line 366 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=CODE_VAR;}
#line 3858 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 240:
#line 366 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = set_local_static((yyvsp[-3]).ptr,(yyvsp[0]).ptr,LVAR_TUNE,0,1); }
#line 3864 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 241:
#line 367 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_localPar((yyvsp[0]).ptr,0,LVAR_XFORMPAR); }
#line 3870 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 242:
#line 368 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=CODE_VAR;}
#line 3876 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 243:
#line 368 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_localPar((yyvsp[-3]).ptr, (yyvsp[0]).ptr,LVAR_XFORMPAR); }
#line 3882 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 244:
#line 369 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = make_sourceVector((yyvsp[-2]).ptr, (yyvsp[0]).ptr); }
#line 3888 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 245:
#line 370 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = (yyvsp[-2]).ptr + 1; }
#line 3894 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 248:
#line 371 "poet_yacc.y" /* yacc.c:1646  */
    { set_local_static((yyvsp[0]).ptr,make_Iconst1((yyvsp[-1]).ptr), LVAR_OUTPUT,0,1); }
#line 3900 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 249:
#line 372 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=CODE_VAR;}
#line 3906 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 250:
#line 372 "poet_yacc.y" /* yacc.c:1646  */
    { set_local_static((yyvsp[-3]).ptr,make_Iconst1((yyvsp[-4]).ptr), LVAR_OUTPUT,(yyvsp[0]).ptr,1); }
#line 3912 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 251:
#line 373 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_traceVar((yyvsp[0]).ptr,0); }
#line 3918 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 252:
#line 374 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_traceVar((yyvsp[-2]).ptr,0); eval_define((yyval).ptr,(yyvsp[0]).ptr); }
#line 3924 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 253:
#line 376 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_traceVar((yyvsp[-2]).ptr,(yyvsp[0]).ptr);  }
#line 3930 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 254:
#line 378 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 3936 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 255:
#line 378 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_seq((yyvsp[-2]).ptr, (yyvsp[0]).ptr); }
#line 3942 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 256:
#line 379 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=(yyvsp[0]).ptr; }
#line 3948 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 257:
#line 380 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[0]).ptr; }
#line 3954 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 258:
#line 381 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 3960 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 259:
#line 381 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-5]).config;}
#line 3966 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 260:
#line 381 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-7]).config;}
#line 3972 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 261:
#line 382 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_ifElse((yyvsp[-5]).ptr, (yyvsp[-2]).ptr, (yyvsp[0]).ptr); }
#line 3978 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 262:
#line 383 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval)=(yyvsp[0]); }
#line 3984 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 263:
#line 384 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_empty(); }
#line 3990 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 264:
#line 385 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 3996 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 265:
#line 385 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[0]).ptr; }
#line 4002 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 266:
#line 386 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4008 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 267:
#line 387 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-5]).config;}
#line 4014 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 268:
#line 388 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-8]).config;}
#line 4020 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 269:
#line 388 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-11]).config;}
#line 4026 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 270:
#line 389 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceQop(POET_OP_FOR, (yyvsp[-9]).ptr,(yyvsp[-6]).ptr,(yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 4032 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 271:
#line 390 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4038 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 272:
#line 390 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-6]).config;}
#line 4044 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 273:
#line 391 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_CASE, (yyvsp[-5]).ptr,(yyvsp[-1]).ptr); }
#line 4050 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 274:
#line 392 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 4056 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 275:
#line 392 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-4]).config;}
#line 4062 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 276:
#line 392 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-8]).config;}
#line 4068 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 277:
#line 393 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_sourceQop(POET_OP_FOREACH,(yyvsp[-5]).ptr,(yyvsp[-8]).ptr,(yyvsp[-4]).ptr,(yyvsp[-1]).ptr); }
#line 4074 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 278:
#line 394 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4080 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 279:
#line 395 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-5]).config;}
#line 4086 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 280:
#line 396 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-8]).config;}
#line 4092 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 281:
#line 396 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-11]).config;}
#line 4098 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 282:
#line 397 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceQop(POET_OP_FOREACH, (yyvsp[-9]).ptr,(yyvsp[-6]).ptr,(yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 4104 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 283:
#line 398 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4110 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 284:
#line 399 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-5]).config;}
#line 4116 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 285:
#line 400 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-8]).config;}
#line 4122 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 286:
#line 400 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-11]).config;}
#line 4128 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 287:
#line 401 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceQop(POET_OP_FOREACH, make_sourceUop(POET_OP_REVERSE,(yyvsp[-9]).ptr),(yyvsp[-6]).ptr,(yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 4134 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 288:
#line 402 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config; }
#line 4140 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 289:
#line 402 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[-1]).ptr; }
#line 4146 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 290:
#line 403 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_empty(); }
#line 4152 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 291:
#line 404 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_empty(); }
#line 4158 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 292:
#line 405 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceStmt((yyvsp[-1]).ptr); }
#line 4164 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 293:
#line 407 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=(yyvsp[0]).ptr; }
#line 4170 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 294:
#line 408 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_sourceUop(POET_OP_REVERSE,(yyvsp[-1]).ptr); }
#line 4176 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 295:
#line 410 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[0]).ptr; }
#line 4182 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 296:
#line 411 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_empty(); }
#line 4188 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 297:
#line 414 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config; }
#line 4194 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 298:
#line 415 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_sourceAssign(make_varRef((yyvsp[-3]).ptr,ASSIGN_VAR), (yyvsp[0]).ptr); }
#line 4200 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 299:
#line 416 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=(yyvsp[0]).ptr;}
#line 4206 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 300:
#line 418 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_Iconst1(1); }
#line 4212 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 301:
#line 419 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 4218 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 302:
#line 419 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=(yyvsp[0]).ptr; }
#line 4224 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 303:
#line 421 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4230 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 304:
#line 421 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-5]).config;}
#line 4236 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 305:
#line 422 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = make_sourceVector( (yyvsp[-6]).ptr, make_inputlist((yyvsp[-3]).ptr,(yyvsp[0]).ptr)); }
#line 4242 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 306:
#line 423 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 4248 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 307:
#line 423 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-4]).config;}
#line 4254 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 308:
#line 423 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_inputlist((yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 4260 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 309:
#line 424 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-3]).config;}
#line 4266 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 310:
#line 425 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = make_sourceVector( (yyvsp[-4]).ptr, make_inputlist(make_any(),(yyvsp[0]).ptr)); }
#line 4272 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 311:
#line 427 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[0]).ptr; }
#line 4278 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 312:
#line 428 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[0]).ptr; }
#line 4284 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 313:
#line 430 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4290 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 314:
#line 430 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = make_sourceVector((yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 4296 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 315:
#line 431 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4302 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 316:
#line 431 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceVector2((yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 4308 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 317:
#line 434 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_dummyOperator(POET_OP_CONTINUE); }
#line 4314 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 318:
#line 435 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_dummyOperator(POET_OP_BREAK); }
#line 4320 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 319:
#line 436 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 4326 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 320:
#line 436 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_sourceUop(POET_OP_RETURN, (yyvsp[0]).ptr); }
#line 4332 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 321:
#line 437 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 4338 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 322:
#line 437 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceUop(POET_OP_ERROR,(yyvsp[0]).ptr); }
#line 4344 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 323:
#line 438 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 4350 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 324:
#line 438 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceUop(POET_OP_ASSERT,(yyvsp[0]).ptr); }
#line 4356 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 325:
#line 439 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 4362 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 326:
#line 439 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_sourceUop(POET_OP_PRINT,(yyvsp[0]).ptr); }
#line 4368 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 327:
#line 440 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[0]).ptr; }
#line 4374 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 328:
#line 442 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[0]).ptr; }
#line 4380 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 329:
#line 443 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4386 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 330:
#line 443 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_sourceAssign((yyvsp[-3]).ptr, (yyvsp[0]).ptr); }
#line 4392 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 331:
#line 444 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4398 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 332:
#line 444 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_codeMatch((yyvsp[-3]).ptr, (yyvsp[0]).ptr); }
#line 4404 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 333:
#line 445 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4410 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 334:
#line 446 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_sourceAssign((yyvsp[-3]).ptr, make_sourceBop(POET_OP_PLUS, (yyvsp[-3]).ptr,(yyvsp[0]).ptr)); }
#line 4416 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 335:
#line 447 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4422 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 336:
#line 448 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_sourceAssign((yyvsp[-3]).ptr, make_sourceBop(POET_OP_MINUS, (yyvsp[-3]).ptr,(yyvsp[0]).ptr)); }
#line 4428 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 337:
#line 449 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4434 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 338:
#line 450 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_sourceAssign((yyvsp[-3]).ptr, make_sourceBop(POET_OP_MULT, (yyvsp[-3]).ptr,(yyvsp[0]).ptr)); }
#line 4440 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 339:
#line 451 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4446 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 340:
#line 452 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_sourceAssign((yyvsp[-3]).ptr, make_sourceBop(POET_OP_DIVIDE, (yyvsp[-3]).ptr,(yyvsp[0]).ptr)); }
#line 4452 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 341:
#line 453 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4458 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 342:
#line 454 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_sourceAssign((yyvsp[-3]).ptr, make_sourceBop(POET_OP_MOD, (yyvsp[-3]).ptr,(yyvsp[0]).ptr)); }
#line 4464 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 343:
#line 455 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4470 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 344:
#line 455 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_codeMatchQ((yyvsp[-3]).ptr, (yyvsp[0]).ptr); }
#line 4476 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 345:
#line 456 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4482 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 346:
#line 456 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-5]).config;}
#line 4488 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 347:
#line 457 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_ifElse( (yyvsp[-6]).ptr, (yyvsp[-3]).ptr, (yyvsp[0]).ptr); }
#line 4494 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 348:
#line 460 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4500 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 349:
#line 460 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_AND,  (yyvsp[-3]).ptr, (yyvsp[0]).ptr); }
#line 4506 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 350:
#line 461 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4512 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 351:
#line 461 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_OR,   (yyvsp[-3]).ptr, (yyvsp[0]).ptr); }
#line 4518 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 352:
#line 462 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 4524 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 353:
#line 462 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceUop(POET_OP_NOT, (yyvsp[0]).ptr); }
#line 4530 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 354:
#line 463 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4536 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 355:
#line 463 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_LT, (yyvsp[-3]).ptr, (yyvsp[0]).ptr); }
#line 4542 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 356:
#line 464 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4548 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 357:
#line 464 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_LE, (yyvsp[-3]).ptr, (yyvsp[0]).ptr); }
#line 4554 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 358:
#line 465 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4560 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 359:
#line 465 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_EQ, (yyvsp[-3]).ptr, (yyvsp[0]).ptr); }
#line 4566 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 360:
#line 466 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4572 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 361:
#line 467 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_GT,   (yyvsp[-3]).ptr, (yyvsp[0]).ptr); }
#line 4578 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 362:
#line 468 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4584 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 363:
#line 469 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_GE,   (yyvsp[-3]).ptr, (yyvsp[0]).ptr); }
#line 4590 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 364:
#line 470 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4596 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 365:
#line 471 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_NE,   (yyvsp[-3]).ptr, (yyvsp[0]).ptr); }
#line 4602 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 366:
#line 472 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4608 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 367:
#line 472 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_ASTMATCH, (yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 4614 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 368:
#line 473 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[0]).ptr; }
#line 4620 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 369:
#line 475 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[0]).ptr; }
#line 4626 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 370:
#line 476 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4632 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 371:
#line 477 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_sourceBop(POET_OP_PLUS, (yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 4638 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 372:
#line 478 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4644 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 373:
#line 479 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_MINUS,  (yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 4650 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 374:
#line 480 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4656 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 375:
#line 481 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_MULT,  (yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 4662 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 376:
#line 482 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4668 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 377:
#line 483 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_CONCAT, (yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 4674 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 378:
#line 484 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4680 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 379:
#line 485 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_DIVIDE,  (yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 4686 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 380:
#line 486 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4692 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 381:
#line 487 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_MOD,  (yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 4698 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 382:
#line 488 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4704 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 383:
#line 488 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_CONS,(yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 4710 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 384:
#line 489 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 4716 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 385:
#line 489 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceUop(POET_OP_UMINUS,(yyvsp[0]).ptr); }
#line 4722 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 386:
#line 491 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 4728 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 387:
#line 492 "poet_yacc.y" /* yacc.c:1646  */
    { if ((yyvsp[0]).ptr==0) (yyval).ptr=(yyvsp[-2]).ptr; else (yyval).ptr = make_xformList((yyvsp[-2]).ptr, (yyvsp[0]).ptr); }
#line 4734 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 388:
#line 494 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config; }
#line 4740 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 389:
#line 494 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr =make_xformList((yyvsp[-2]).ptr,(yyvsp[0]).ptr);}
#line 4746 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 390:
#line 495 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=0; }
#line 4752 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 391:
#line 496 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config; }
#line 4758 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 392:
#line 496 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr =make_xformList((yyvsp[-2]).ptr,(yyvsp[0]).ptr);}
#line 4764 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 393:
#line 497 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=0; }
#line 4770 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 394:
#line 499 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4776 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 395:
#line 499 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-5]).config;}
#line 4782 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 396:
#line 499 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-7]).config;}
#line 4788 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 397:
#line 500 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=((yyvsp[0]).ptr==0)? make_sourceBop(POET_OP_REPLACE,(yyvsp[-5]).ptr,(yyvsp[-2]).ptr) 
                       : make_sourceTop(POET_OP_REPLACE,(yyvsp[-5]).ptr,(yyvsp[-2]).ptr,(yyvsp[0]).ptr); }
#line 4795 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 398:
#line 502 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4801 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 399:
#line 502 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-4]).config;}
#line 4807 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 400:
#line 503 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=((yyvsp[0]).ptr==0)? make_sourceUop(POET_OP_TRACE,(yyvsp[-2]).ptr)
                      : make_sourceBop(POET_OP_TRACE,(yyvsp[-2]).ptr,(yyvsp[0]).ptr); }
#line 4814 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 401:
#line 505 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4820 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 402:
#line 505 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-5]).config;}
#line 4826 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 403:
#line 506 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_sourceBop(POET_OP_PERMUTE,(yyvsp[-4]).ptr,(yyvsp[-1]).ptr); }
#line 4832 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 404:
#line 507 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4838 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 405:
#line 507 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-5]).config;}
#line 4844 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 406:
#line 507 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-8]).config;}
#line 4850 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 407:
#line 508 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_sourceTop(POET_OP_DUPLICATE,(yyvsp[-7]).ptr,(yyvsp[-4]).ptr,(yyvsp[-1]).ptr); }
#line 4856 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 408:
#line 509 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 4862 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 409:
#line 509 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceUop(POET_OP_COPY, (yyvsp[0]).ptr); }
#line 4868 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 410:
#line 510 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4874 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 411:
#line 511 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-4]).config;(yyval).ptr=(yyvsp[0]).ptr; }
#line 4880 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 412:
#line 512 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = make_sourceBop(POET_OP_ERASE,(yyvsp[-2]).ptr,(yyvsp[0]).ptr); }
#line 4886 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 413:
#line 513 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4892 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 414:
#line 513 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceUop(POET_OP_DELAY, (yyvsp[-1]).ptr); }
#line 4898 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 415:
#line 514 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4904 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 416:
#line 514 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-5]).config;}
#line 4910 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 417:
#line 515 "poet_yacc.y" /* yacc.c:1646  */
    {  (yyval).ptr=make_sourceBop(POET_OP_INSERT, (yyvsp[-4]).ptr,(yyvsp[-1]).ptr); }
#line 4916 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 418:
#line 516 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-3]).config;}
#line 4922 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 419:
#line 517 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_DEBUG,(yyvsp[-4]).ptr,(yyvsp[-1]).ptr); }
#line 4928 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 420:
#line 518 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4934 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 421:
#line 518 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceUop(POET_OP_APPLY, (yyvsp[-1]).ptr); }
#line 4940 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 422:
#line 519 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 4946 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 423:
#line 519 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceUop(POET_OP_REBUILD,(yyvsp[0]).ptr); }
#line 4952 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 424:
#line 520 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 4958 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 425:
#line 520 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceUop(POET_OP_RESTORE,(yyvsp[0]).ptr); }
#line 4964 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 426:
#line 521 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 4970 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 427:
#line 521 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceUop(POET_OP_SAVE,(yyvsp[0]).ptr); }
#line 4976 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 428:
#line 522 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 4982 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 429:
#line 522 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-5]).config;}
#line 4988 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 430:
#line 523 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceBop(POET_OP_SPLIT, (yyvsp[-4]).ptr,(yyvsp[-1]).ptr); }
#line 4994 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 431:
#line 524 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = make_typeUop(TYPE_LIST, (yyvsp[-1]).ptr); }
#line 5000 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 432:
#line 525 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 5006 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 433:
#line 526 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = make_sourceBop(POET_OP_RANGE, (yyvsp[-3]).ptr,(yyvsp[0]).ptr);}
#line 5012 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 434:
#line 528 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = make_sourceBop(POET_OP_RANGE, (yyvsp[-3]).ptr,(yyvsp[0]).ptr);}
#line 5018 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 435:
#line 529 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[0]).ptr; }
#line 5024 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 436:
#line 530 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = 0; }
#line 5030 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 437:
#line 530 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 5036 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 438:
#line 530 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[-1]).ptr; }
#line 5042 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 439:
#line 532 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 5048 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 440:
#line 532 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[-1]).ptr; }
#line 5054 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 441:
#line 533 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[-1]).ptr; }
#line 5060 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 442:
#line 535 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = make_Iconst1(1); }
#line 5066 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 443:
#line 536 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[-1]).ptr; }
#line 5072 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 444:
#line 538 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = 0; }
#line 5078 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 445:
#line 539 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_inputlist((yyvsp[-1]).ptr,(yyvsp[0]).ptr); }
#line 5084 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 446:
#line 540 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=CODE_OR_XFORM_VAR;}
#line 5090 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 447:
#line 540 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_annot_single((yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 5096 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 448:
#line 541 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=CODE_OR_XFORM_VAR; }
#line 5102 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 449:
#line 542 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_annot_lbegin((yyvsp[-5]).ptr,(yyvsp[-1]).ptr); }
#line 5108 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 450:
#line 543 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=CODE_OR_XFORM_VAR; }
#line 5114 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 451:
#line 544 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_inputlist((yyvsp[-5]).ptr,make_sourceUop(POET_OP_ANNOT,(yyvsp[-1]).ptr)); }
#line 5120 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 452:
#line 545 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_inputlist((yyvsp[-1]).ptr, (yyvsp[0]).ptr); }
#line 5126 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 453:
#line 546 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_inputlist((yyvsp[-1]).ptr, (yyvsp[0]).ptr); }
#line 5132 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 454:
#line 547 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_inputlist((yyvsp[-1]).ptr, (yyvsp[0]).ptr); }
#line 5138 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 455:
#line 548 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_inputlist((yyvsp[-1]).ptr, (yyvsp[0]).ptr); }
#line 5144 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 456:
#line 549 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_inputlist((yyvsp[-1]).ptr,make_varRef((yyvsp[0]).ptr,GLOBAL_VAR)); }
#line 5150 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 457:
#line 552 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 5156 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 458:
#line 552 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceUop(POET_OP_CAR, (yyvsp[0]).ptr); }
#line 5162 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 459:
#line 553 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 5168 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 460:
#line 553 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceUop(POET_OP_CDR, (yyvsp[0]).ptr); }
#line 5174 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 461:
#line 554 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 5180 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 462:
#line 554 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceUop(POET_OP_LEN, (yyvsp[0]).ptr); }
#line 5186 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 463:
#line 555 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 5192 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 464:
#line 555 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceUop(TYPE_INT, (yyvsp[0]).ptr); }
#line 5198 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 465:
#line 556 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=ASSIGN_VAR;}
#line 5204 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 466:
#line 556 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceUop(POET_OP_CLEAR, (yyvsp[0]).ptr); }
#line 5210 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 467:
#line 557 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=(yyvsp[0]).ptr; }
#line 5216 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 468:
#line 560 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config;}
#line 5222 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 469:
#line 560 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=(yyvsp[-1]).ptr; }
#line 5228 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 470:
#line 561 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=make_empty(); }
#line 5234 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 471:
#line 562 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr=(yyvsp[0]).ptr; }
#line 5240 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 472:
#line 563 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_any(); }
#line 5246 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 473:
#line 564 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval)=(yyvsp[0]); }
#line 5252 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 474:
#line 565 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval)=(yyvsp[0]); }
#line 5258 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 475:
#line 567 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=(yyvsp[0]).ptr; (yyval).config=(yyvsp[-1]).config; }
#line 5264 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 476:
#line 567 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[0]); }
#line 5270 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 477:
#line 568 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config; }
#line 5276 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 478:
#line 568 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_typeTor((yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 5282 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 479:
#line 569 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval)=(yyvsp[0]); }
#line 5288 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 480:
#line 570 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config; (yyval).ptr=(yyvsp[-1]).ptr; }
#line 5294 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 481:
#line 571 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_tupleAccess((yyvsp[-4]).ptr, (yyvsp[-1]).ptr); (yyval).config=(yyvsp[-4]).config; }
#line 5300 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 482:
#line 572 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval)=(yyvsp[0]); }
#line 5306 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 483:
#line 573 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval)=(yyvsp[0]); }
#line 5312 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 484:
#line 575 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config; }
#line 5318 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 485:
#line 576 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_codeRef((yyvsp[-3]).ptr,(yyvsp[0]).ptr,(yyvsp[-3]).config); (yyval).config=(yyvsp[-3]).config; }
#line 5324 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 486:
#line 578 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=(yyvsp[0]).ptr; (yyval).config=(yyvsp[-1]).config; }
#line 5330 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 487:
#line 578 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[0]); }
#line 5336 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 488:
#line 580 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval)=(yyvsp[0]); }
#line 5342 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 489:
#line 581 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config; (yyval).ptr=(yyvsp[-1]).ptr; }
#line 5348 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 490:
#line 582 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_tupleAccess((yyvsp[-4]).ptr, (yyvsp[-1]).ptr); (yyval).config=(yyvsp[-4]).config; }
#line 5354 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 491:
#line 583 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config; }
#line 5360 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 492:
#line 584 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_codeRef((yyvsp[-3]).ptr,(yyvsp[0]).ptr,(yyvsp[-3]).config);(yyval).config=(yyvsp[-3]).config; }
#line 5366 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 493:
#line 586 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=(yyvsp[0]).ptr; (yyval).config=(yyvsp[-1]).config; }
#line 5372 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 494:
#line 586 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[0]); }
#line 5378 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 495:
#line 587 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval)=(yyvsp[0]); }
#line 5384 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 496:
#line 588 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config; (yyval).ptr=(yyvsp[-1]).ptr; }
#line 5390 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 497:
#line 589 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_tupleAccess((yyvsp[-4]).ptr, (yyvsp[-1]).ptr); (yyval).config=(yyvsp[-4]).config; }
#line 5396 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 498:
#line 590 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-1]).config; }
#line 5402 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 499:
#line 591 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_codeRef((yyvsp[-3]).ptr,(yyvsp[0]).ptr,(yyvsp[-3]).config);(yyval).config=(yyvsp[-3]).config; }
#line 5408 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 500:
#line 593 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=(yyvsp[0]).ptr;}
#line 5414 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 501:
#line 594 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = (yyvsp[0]).ptr; }
#line 5420 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 502:
#line 595 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr = (yyvsp[0]).ptr; }
#line 5426 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 503:
#line 597 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 5432 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 504:
#line 598 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_typeTor((yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 5438 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 505:
#line 599 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_attrAccess(make_varRef((yyvsp[-2]).ptr,CODE_OR_XFORM_VAR), (yyvsp[0]).ptr); }
#line 5444 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 506:
#line 602 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 5450 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 507:
#line 603 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceAssign( make_attrAccess((yyvsp[-4]).ptr,(yyvsp[-3]).ptr),(yyvsp[0]).ptr); }
#line 5456 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 508:
#line 604 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=(yyvsp[-2]).ptr; (yyval).config=(yyvsp[-2]).config;}
#line 5462 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 509:
#line 605 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_seq((yyvsp[-3]).ptr,(yyvsp[0]).ptr); }
#line 5468 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 510:
#line 608 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).config=(yyvsp[-2]).config;}
#line 5474 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 511:
#line 609 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_sourceAssign( make_attrAccess((yyvsp[-4]).ptr,(yyvsp[-3]).ptr),(yyvsp[0]).ptr); }
#line 5480 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 512:
#line 610 "poet_yacc.y" /* yacc.c:1646  */
    {(yyval).ptr=(yyvsp[-2]).ptr; (yyval).config=(yyvsp[-2]).config;}
#line 5486 "poet_yacc.c" /* yacc.c:1646  */
    break;

  case 513:
#line 611 "poet_yacc.y" /* yacc.c:1646  */
    { (yyval).ptr = make_seq((yyvsp[-3]).ptr, (yyvsp[0]).ptr);}
#line 5492 "poet_yacc.c" /* yacc.c:1646  */
    break;


#line 5496 "poet_yacc.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
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

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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

#if !defined yyoverflow || YYERROR_VERBOSE
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
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
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
  return yyresult;
}
#line 612 "poet_yacc.y" /* yacc.c:1906  */

