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
#define yyparse annparse
#define yylex   annlex
#define yyerror annerror
#define yylval  annlval
#define yychar  annchar
#define yydebug anndebug
#define yynerrs annnerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     tokID = 258,
     tokCONSTANT = 259,
     tokENABLE = 260,
     tokDISABLE = 261,
     tokMAYPROPERTY = 262,
     tokMUSTPROPERTY = 263,
     tokGLOBAL = 264,
     tokINITIALLY = 265,
     tokDIAGNOSTIC = 266,
     tokPROCEDURE = 267,
     tokACCESS = 268,
     tokMODIFY = 269,
     tokANALYZE = 270,
     tokDEFAULT = 271,
     tokIF = 272,
     tokIS_EXACTLY = 273,
     tokIS_ATLEAST = 274,
     tokCOULD_BE = 275,
     tokIS_ATMOST = 276,
     tokIS_BOTTOM = 277,
     tokUNION_SET = 278,
     tokINTERSECT_SET = 279,
     tokUNION_EQUIV = 280,
     tokINTERSECT_EQUIV = 281,
     tokIS_ELEMENT_OF = 282,
     tokIS_EMPTYSET = 283,
     tokADD = 284,
     tokALWAYS = 285,
     tokEVER = 286,
     tokBEFORE = 287,
     tokAFTER = 288,
     tokTRACE = 289,
     tokCONFIDENCE = 290,
     tokFORWARD = 291,
     tokBACKWARD = 292,
     tokIS_ALIASOF = 293,
     tokIS_SAMEAS = 294,
     tokIS_EMPTY = 295,
     tokON_ENTRY = 296,
     tokON_EXIT = 297,
     tokNEW = 298,
     tokARROW = 299,
     tokDELETE = 300,
     tokPATTERN = 301,
     tokANDAND = 302,
     tokOROR = 303,
     tokNOT_EQUALS = 304,
     tokEQUALS_EQUALS = 305,
     tokLESS_EQUALS = 306,
     tokGREATER_EQUALS = 307,
     tokIS_CONSTANT = 308,
     tokASSIGN = 309,
     tokWEAKASSIGN = 310,
     tokEQUALS = 311,
     tokREPORT = 312,
     tokERROR = 313,
     tokAPPEND = 314,
     tokWHEN = 315,
     tokREPLACE = 316,
     tokINLINE = 317,
     tokIO = 318,
     tokSTRING = 319,
     WEAK_REDUCE = 320,
     LOWER_THAN_EXPRESSION = 321,
     LOWER_THAN_UNARY = 322,
     LOWER_THAN_POSTFIX = 323,
     PARENTHESIZED = 324,
     ctokAND = 325,
     ctokSTAR = 326,
     ctokPLUS = 327,
     ctokMINUS = 328,
     ctokTILDE = 329,
     ctokNOT = 330,
     ctokLESS = 331,
     ctokGREATER = 332,
     ctokOR = 333,
     ctokHAT = 334,
     ctokPERCENT = 335,
     ctokDIVIDE = 336,
     ctokLEFTPAREN = 337,
     ctokRIGHTPAREN = 338,
     ctokDOT = 339,
     ctokQUESTION = 340,
     ctokSEMICOLON = 341,
     ctokLEFTBRACE = 342,
     ctokRIGHTBRACE = 343,
     ctokCOMMA = 344,
     ctokLEFTBRACKET = 345,
     ctokRIGHTBRACKET = 346,
     ctokCOLON = 347,
     ctokAUTO = 348,
     ctokDOUBLE = 349,
     ctokINT = 350,
     ctokSTRUCT = 351,
     ctokBREAK = 352,
     ctokELSE = 353,
     ctokLONG = 354,
     ctokSWITCH = 355,
     ctokCASE = 356,
     ctokENUM = 357,
     ctokREGISTER = 358,
     ctokTYPEDEF = 359,
     ctokCHAR = 360,
     ctokEXTERN = 361,
     ctokRETURN = 362,
     ctokUNION = 363,
     ctokCONST = 364,
     ctokFLOAT = 365,
     ctokSHORT = 366,
     ctokUNSIGNED = 367,
     ctokCONTINUE = 368,
     ctokFOR = 369,
     ctokSIGNED = 370,
     ctokVOID = 371,
     ctokDEFAULT = 372,
     ctokGOTO = 373,
     ctokSIZEOF = 374,
     ctokVOLATILE = 375,
     ctokDO = 376,
     ctokIF = 377,
     ctokSTATIC = 378,
     ctokWHILE = 379,
     ctokUPLUS = 380,
     ctokUMINUS = 381,
     ctokINDIR = 382,
     ctokADDRESS = 383,
     ctokPOSTINC = 384,
     ctokPOSTDEC = 385,
     ctokPREINC = 386,
     ctokPREDEC = 387,
     ctokBOGUS = 388,
     IDENTIFIER = 389,
     STRINGliteral = 390,
     FLOATINGconstant = 391,
     INTEGERconstant = 392,
     OCTALconstant = 393,
     HEXconstant = 394,
     WIDECHARconstant = 395,
     CHARACTERconstant = 396,
     TYPEDEFname = 397,
     ctokARROW = 398,
     ctokICR = 399,
     ctokDECR = 400,
     ctokLS = 401,
     ctokRS = 402,
     ctokLE = 403,
     ctokGE = 404,
     ctokEQ = 405,
     ctokNE = 406,
     ctokANDAND = 407,
     ctokOROR = 408,
     ctokELLIPSIS = 409,
     ctokEQUALS = 410,
     ctokMULTassign = 411,
     ctokDIVassign = 412,
     ctokMODassign = 413,
     ctokPLUSassign = 414,
     ctokMINUSassign = 415,
     ctokLSassign = 416,
     ctokRSassign = 417,
     ctokANDassign = 418,
     ctokERassign = 419,
     ctokORassign = 420,
     ctokINLINE = 421,
     ctokATTRIBUTE = 422,
     ctokMETA_TYPE_EXPR = 423,
     ctokMETA_TYPE_STMT = 424,
     ctokMETA_EXPR = 425,
     ctokMETA_STMT = 426,
     metatokID = 427,
     metatokEXPR = 428,
     metatokSTMT = 429,
     metatokTYPE = 430
   };
#endif
/* Tokens.  */
#define tokID 258
#define tokCONSTANT 259
#define tokENABLE 260
#define tokDISABLE 261
#define tokMAYPROPERTY 262
#define tokMUSTPROPERTY 263
#define tokGLOBAL 264
#define tokINITIALLY 265
#define tokDIAGNOSTIC 266
#define tokPROCEDURE 267
#define tokACCESS 268
#define tokMODIFY 269
#define tokANALYZE 270
#define tokDEFAULT 271
#define tokIF 272
#define tokIS_EXACTLY 273
#define tokIS_ATLEAST 274
#define tokCOULD_BE 275
#define tokIS_ATMOST 276
#define tokIS_BOTTOM 277
#define tokUNION_SET 278
#define tokINTERSECT_SET 279
#define tokUNION_EQUIV 280
#define tokINTERSECT_EQUIV 281
#define tokIS_ELEMENT_OF 282
#define tokIS_EMPTYSET 283
#define tokADD 284
#define tokALWAYS 285
#define tokEVER 286
#define tokBEFORE 287
#define tokAFTER 288
#define tokTRACE 289
#define tokCONFIDENCE 290
#define tokFORWARD 291
#define tokBACKWARD 292
#define tokIS_ALIASOF 293
#define tokIS_SAMEAS 294
#define tokIS_EMPTY 295
#define tokON_ENTRY 296
#define tokON_EXIT 297
#define tokNEW 298
#define tokARROW 299
#define tokDELETE 300
#define tokPATTERN 301
#define tokANDAND 302
#define tokOROR 303
#define tokNOT_EQUALS 304
#define tokEQUALS_EQUALS 305
#define tokLESS_EQUALS 306
#define tokGREATER_EQUALS 307
#define tokIS_CONSTANT 308
#define tokASSIGN 309
#define tokWEAKASSIGN 310
#define tokEQUALS 311
#define tokREPORT 312
#define tokERROR 313
#define tokAPPEND 314
#define tokWHEN 315
#define tokREPLACE 316
#define tokINLINE 317
#define tokIO 318
#define tokSTRING 319
#define WEAK_REDUCE 320
#define LOWER_THAN_EXPRESSION 321
#define LOWER_THAN_UNARY 322
#define LOWER_THAN_POSTFIX 323
#define PARENTHESIZED 324
#define ctokAND 325
#define ctokSTAR 326
#define ctokPLUS 327
#define ctokMINUS 328
#define ctokTILDE 329
#define ctokNOT 330
#define ctokLESS 331
#define ctokGREATER 332
#define ctokOR 333
#define ctokHAT 334
#define ctokPERCENT 335
#define ctokDIVIDE 336
#define ctokLEFTPAREN 337
#define ctokRIGHTPAREN 338
#define ctokDOT 339
#define ctokQUESTION 340
#define ctokSEMICOLON 341
#define ctokLEFTBRACE 342
#define ctokRIGHTBRACE 343
#define ctokCOMMA 344
#define ctokLEFTBRACKET 345
#define ctokRIGHTBRACKET 346
#define ctokCOLON 347
#define ctokAUTO 348
#define ctokDOUBLE 349
#define ctokINT 350
#define ctokSTRUCT 351
#define ctokBREAK 352
#define ctokELSE 353
#define ctokLONG 354
#define ctokSWITCH 355
#define ctokCASE 356
#define ctokENUM 357
#define ctokREGISTER 358
#define ctokTYPEDEF 359
#define ctokCHAR 360
#define ctokEXTERN 361
#define ctokRETURN 362
#define ctokUNION 363
#define ctokCONST 364
#define ctokFLOAT 365
#define ctokSHORT 366
#define ctokUNSIGNED 367
#define ctokCONTINUE 368
#define ctokFOR 369
#define ctokSIGNED 370
#define ctokVOID 371
#define ctokDEFAULT 372
#define ctokGOTO 373
#define ctokSIZEOF 374
#define ctokVOLATILE 375
#define ctokDO 376
#define ctokIF 377
#define ctokSTATIC 378
#define ctokWHILE 379
#define ctokUPLUS 380
#define ctokUMINUS 381
#define ctokINDIR 382
#define ctokADDRESS 383
#define ctokPOSTINC 384
#define ctokPOSTDEC 385
#define ctokPREINC 386
#define ctokPREDEC 387
#define ctokBOGUS 388
#define IDENTIFIER 389
#define STRINGliteral 390
#define FLOATINGconstant 391
#define INTEGERconstant 392
#define OCTALconstant 393
#define HEXconstant 394
#define WIDECHARconstant 395
#define CHARACTERconstant 396
#define TYPEDEFname 397
#define ctokARROW 398
#define ctokICR 399
#define ctokDECR 400
#define ctokLS 401
#define ctokRS 402
#define ctokLE 403
#define ctokGE 404
#define ctokEQ 405
#define ctokNE 406
#define ctokANDAND 407
#define ctokOROR 408
#define ctokELLIPSIS 409
#define ctokEQUALS 410
#define ctokMULTassign 411
#define ctokDIVassign 412
#define ctokMODassign 413
#define ctokPLUSassign 414
#define ctokMINUSassign 415
#define ctokLSassign 416
#define ctokRSassign 417
#define ctokANDassign 418
#define ctokERassign 419
#define ctokORassign 420
#define ctokINLINE 421
#define ctokATTRIBUTE 422
#define ctokMETA_TYPE_EXPR 423
#define ctokMETA_TYPE_STMT 424
#define ctokMETA_EXPR 425
#define ctokMETA_STMT 426
#define metatokID 427
#define metatokEXPR 428
#define metatokSTMT 429
#define metatokTYPE 430




/* Copy the first part of user declarations.  */
#line 1 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"


#include "broadway.h"

extern int annlex(void);
extern int annlineno;
extern int line_number_offset;
extern char * anntext;

typedef std::list< enumValueAnn * > enumvalue_list;
typedef std::list< exprAnn * > exprann_list;
typedef std::list< ruleAnn * > rule_list;

// -- Utility functions -- defined in c_breeze/src/main/parser.y

#if 0
declNode::Storage_class merge_sc(declNode::Storage_class sc1,
                                 declNode::Storage_class sc2,
                                 const Coord c = Coord::Unknown);
#endif
struct _TQ merge_tq(struct _TQ ts1,
                    struct _TQ ts2);

// --- Global state variables ----------------

void annerror(const char * msg)
{
  Annotations::Current->Error(annlineno - line_number_offset, std::string(msg) + " near \"" +
                              std::string(anntext) + "\"");
}



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
typedef union YYSTYPE
#line 34 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
{

  /* --- Broadway --- */

  parserID *                  t_id;
  parserid_list *             t_ids;

  int                         t_tok;
  std::string *               t_string;

  structureTreeAnn *          t_structure;
  structuretree_list *        t_structures;

  //  exprNode *                  t_c_expression;

  enumValueAnn *              t_enumvalue;
  enumvalue_list *            t_enumvalue_list;

  exprAnn *                   t_expr;
  exprann_list *              t_exprs;

  analyzeAnn *                t_analyze;

  ruleAnn *                   t_rule;
  rule_list *                 t_rules;

  reportAnn *                 t_report;
  report_element_list *       t_report_elements;
  reportElementAnn *          t_report_element;

  struct {
    int                              line;
    Broadway::Operator                op;
  } t_property_operator;

  struct {
    int                              line;
    Broadway::FlowSensitivity fs;
  } t_property_sensitivity;

  Direction                   t_direction;

  /* --- C Code --- */
#if 0
  primNode *          primN;
  sueNode *           sueN;
  enumNode *          enumN;
  exprNode *          exprN;
  constNode *         constN;
  idNode *            idN;
  initializerNode *   initializerN;
  unaryNode *         unaryN;
  binaryNode *        binaryN;
  stmtNode *          stmtN;
  exprstmtNode *      exprstmtN;
  blockNode *         blockN;
  targetNode *        targetN;
  loopNode *          loopN;
  jumpNode *          jumpN;
  declNode *          declN;
  attribNode *        attribN;
  procNode *          procN;

  def_list  * defL;
  stmt_list * stmtL;
  decl_list * declL;
  attrib_list * attribL;
  expr_list * exprL;

  /* type: type nodes must also carry storage class */
  struct _TYPE typeN;

  /* tq: type qualifiers, storage class */
  struct _TQ tq;

  /* tok: token coordinates */
  simpleCoord  tok;
#endif
}
/* Line 193 of yacc.c.  */
#line 567 "./tempBisonOutput.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 580 "./tempBisonOutput.c"

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
#define YYFINAL  28
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   327

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  198
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  36
/* YYNRULES -- Number of rules.  */
#define YYNRULES  122
/* YYNRULES -- Number of states.  */
#define YYNSTATES  242

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   430

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    69,     2,     2,    83,    79,    78,     2,
      73,    74,    77,    80,    67,    66,    82,    70,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    65,    68,
       2,     2,     2,     2,    84,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   196,     2,   197,    71,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    75,    81,    76,    72,     2,     2,     2,
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
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    12,    15,    18,    20,
      22,    24,    29,    32,    40,    48,    53,    58,    63,    68,
      69,    71,    73,    74,    77,    78,    83,    87,    89,    93,
      95,    98,   103,   107,   113,   118,   120,   123,   125,   127,
     129,   131,   136,   141,   146,   150,   154,   156,   159,   167,
     173,   178,   181,   183,   186,   190,   193,   197,   202,   204,
     208,   213,   218,   223,   226,   231,   234,   240,   246,   248,
     251,   259,   265,   270,   273,   275,   279,   283,   286,   290,
     296,   300,   307,   312,   316,   320,   323,   326,   330,   334,
     337,   339,   342,   346,   350,   353,   357,   359,   361,   363,
     365,   367,   369,   371,   373,   375,   377,   378,   382,   390,
     394,   402,   404,   408,   410,   415,   418,   422,   426,   428,
     430,   434,   436
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     199,     0,    -1,   200,    -1,   199,   200,    -1,   201,    -1,
     202,    -1,     5,     3,    -1,     6,     3,    -1,   209,    -1,
     231,    -1,   219,    -1,     9,    75,   216,    76,    -1,     9,
     217,    -1,     7,     3,    65,   203,   206,   205,   204,    -1,
       8,     3,    65,   203,   206,   205,   204,    -1,     7,     3,
      65,    23,    -1,     8,     3,    65,    24,    -1,     7,     3,
      65,    25,    -1,     8,     3,    65,    26,    -1,    -1,    36,
      -1,    37,    -1,    -1,    10,     3,    -1,    -1,    11,    75,
     232,    76,    -1,    75,   207,    76,    -1,   208,    -1,   207,
      67,   208,    -1,     3,    -1,     3,   206,    -1,   210,    75,
     211,    76,    -1,   210,    75,    76,    -1,    12,     3,    73,
     232,    74,    -1,    12,     3,    73,    74,    -1,   212,    -1,
     211,   212,    -1,   213,    -1,   218,    -1,   219,    -1,   228,
      -1,    41,    75,   216,    76,    -1,    42,    75,   216,    76,
      -1,    42,    75,   214,    76,    -1,    41,    75,    76,    -1,
      42,    75,    76,    -1,   215,    -1,   214,   215,    -1,    17,
      73,   222,    74,    75,   216,    76,    -1,    17,    73,   222,
      74,   217,    -1,    16,    75,   216,    76,    -1,    16,   217,
      -1,   217,    -1,   216,   217,    -1,   216,    67,   217,    -1,
      45,   233,    -1,   216,    45,   233,    -1,   216,    67,    45,
     233,    -1,   233,    -1,     3,    44,   217,    -1,     3,    44,
      43,   217,    -1,     3,    75,   216,    76,    -1,    13,    75,
     232,    76,    -1,    13,     3,    -1,    14,    75,   232,    76,
      -1,    14,     3,    -1,    15,     3,    75,   220,    76,    -1,
      15,     3,    75,   224,    76,    -1,   221,    -1,   220,   221,
      -1,    17,    73,   222,    74,    75,   224,    76,    -1,    17,
      73,   222,    74,   225,    -1,    16,    75,   224,    76,    -1,
      16,   225,    -1,   223,    -1,   222,    48,   222,    -1,   222,
      47,   222,    -1,    69,   222,    -1,    73,   222,    74,    -1,
       3,    65,     3,   227,    22,    -1,     3,   227,    22,    -1,
       3,    65,     3,   227,   226,     3,    -1,     3,   227,   226,
       3,    -1,     3,     3,     3,    -1,     3,    27,     3,    -1,
       3,    28,    -1,     3,    53,    -1,     3,    38,     3,    -1,
       3,    39,     3,    -1,     3,    40,    -1,   225,    -1,   224,
     225,    -1,     3,    54,     3,    -1,     3,    55,     3,    -1,
      29,     3,    -1,     3,     3,     3,    -1,    18,    -1,    19,
      -1,    20,    -1,    21,    -1,    32,    -1,    33,    -1,    30,
      -1,    31,    -1,    34,    -1,    35,    -1,    -1,    57,   229,
      68,    -1,    57,    17,    73,   222,    74,   229,    68,    -1,
      58,   229,    68,    -1,    58,    17,    73,   222,    74,   229,
      68,    -1,   230,    -1,   229,    59,   230,    -1,    64,    -1,
       3,    65,     3,   227,    -1,    84,     3,    -1,   196,     3,
     197,    -1,    81,     3,    81,    -1,    46,    -1,   233,    -1,
     232,    67,   233,    -1,     3,    -1,    63,     3,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   342,   342,   343,   347,   348,   349,   351,   353,   354,
     355,   364,   370,   381,   389,   397,   405,   413,   421,   432,
     436,   441,   449,   453,   460,   461,   466,   473,   479,   487,
     493,   504,   510,   518,   524,   532,   533,   537,   541,   545,
     550,   559,   564,   569,   573,   577,   583,   584,   588,   593,
     600,   605,   615,   621,   627,   633,   645,   652,   661,   668,
     674,   680,   691,   697,   705,   711,   724,   730,   740,   746,
     754,   759,   766,   771,   781,   786,   791,   796,   801,   810,
     818,   825,   833,   842,   850,   858,   868,   875,   882,   889,
     898,   904,   913,   922,   933,   941,   953,   957,   961,   965,
     972,   976,   980,   984,   988,   992,   997,  1005,  1010,  1014,
    1019,  1027,  1033,  1043,  1049,  1059,  1064,  1069,  1080,  1088,
    1094,  1102,  1105
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "tokID", "tokCONSTANT", "tokENABLE",
  "tokDISABLE", "tokMAYPROPERTY", "tokMUSTPROPERTY", "tokGLOBAL",
  "tokINITIALLY", "tokDIAGNOSTIC", "tokPROCEDURE", "tokACCESS",
  "tokMODIFY", "tokANALYZE", "tokDEFAULT", "tokIF", "tokIS_EXACTLY",
  "tokIS_ATLEAST", "tokCOULD_BE", "tokIS_ATMOST", "tokIS_BOTTOM",
  "tokUNION_SET", "tokINTERSECT_SET", "tokUNION_EQUIV",
  "tokINTERSECT_EQUIV", "tokIS_ELEMENT_OF", "tokIS_EMPTYSET", "tokADD",
  "tokALWAYS", "tokEVER", "tokBEFORE", "tokAFTER", "tokTRACE",
  "tokCONFIDENCE", "tokFORWARD", "tokBACKWARD", "tokIS_ALIASOF",
  "tokIS_SAMEAS", "tokIS_EMPTY", "tokON_ENTRY", "tokON_EXIT", "tokNEW",
  "tokARROW", "tokDELETE", "tokPATTERN", "tokANDAND", "tokOROR",
  "tokNOT_EQUALS", "tokEQUALS_EQUALS", "tokLESS_EQUALS",
  "tokGREATER_EQUALS", "tokIS_CONSTANT", "tokASSIGN", "tokWEAKASSIGN",
  "tokEQUALS", "tokREPORT", "tokERROR", "tokAPPEND", "tokWHEN",
  "tokREPLACE", "tokINLINE", "tokIO", "tokSTRING", "':'", "'-'", "','",
  "';'", "'!'", "'/'", "'^'", "'~'", "'('", "')'", "'{'", "'}'", "'*'",
  "'&'", "'%'", "'+'", "'|'", "'.'", "'$'", "'@'", "WEAK_REDUCE",
  "LOWER_THAN_EXPRESSION", "LOWER_THAN_UNARY", "LOWER_THAN_POSTFIX",
  "PARENTHESIZED", "ctokAND", "ctokSTAR", "ctokPLUS", "ctokMINUS",
  "ctokTILDE", "ctokNOT", "ctokLESS", "ctokGREATER", "ctokOR", "ctokHAT",
  "ctokPERCENT", "ctokDIVIDE", "ctokLEFTPAREN", "ctokRIGHTPAREN",
  "ctokDOT", "ctokQUESTION", "ctokSEMICOLON", "ctokLEFTBRACE",
  "ctokRIGHTBRACE", "ctokCOMMA", "ctokLEFTBRACKET", "ctokRIGHTBRACKET",
  "ctokCOLON", "ctokAUTO", "ctokDOUBLE", "ctokINT", "ctokSTRUCT",
  "ctokBREAK", "ctokELSE", "ctokLONG", "ctokSWITCH", "ctokCASE",
  "ctokENUM", "ctokREGISTER", "ctokTYPEDEF", "ctokCHAR", "ctokEXTERN",
  "ctokRETURN", "ctokUNION", "ctokCONST", "ctokFLOAT", "ctokSHORT",
  "ctokUNSIGNED", "ctokCONTINUE", "ctokFOR", "ctokSIGNED", "ctokVOID",
  "ctokDEFAULT", "ctokGOTO", "ctokSIZEOF", "ctokVOLATILE", "ctokDO",
  "ctokIF", "ctokSTATIC", "ctokWHILE", "ctokUPLUS", "ctokUMINUS",
  "ctokINDIR", "ctokADDRESS", "ctokPOSTINC", "ctokPOSTDEC", "ctokPREINC",
  "ctokPREDEC", "ctokBOGUS", "IDENTIFIER", "STRINGliteral",
  "FLOATINGconstant", "INTEGERconstant", "OCTALconstant", "HEXconstant",
  "WIDECHARconstant", "CHARACTERconstant", "TYPEDEFname", "ctokARROW",
  "ctokICR", "ctokDECR", "ctokLS", "ctokRS", "ctokLE", "ctokGE", "ctokEQ",
  "ctokNE", "ctokANDAND", "ctokOROR", "ctokELLIPSIS", "ctokEQUALS",
  "ctokMULTassign", "ctokDIVassign", "ctokMODassign", "ctokPLUSassign",
  "ctokMINUSassign", "ctokLSassign", "ctokRSassign", "ctokANDassign",
  "ctokERassign", "ctokORassign", "ctokINLINE", "ctokATTRIBUTE",
  "ctokMETA_TYPE_EXPR", "ctokMETA_TYPE_STMT", "ctokMETA_EXPR",
  "ctokMETA_STMT", "metatokID", "metatokEXPR", "metatokSTMT",
  "metatokTYPE", "'['", "']'", "$accept", "annotation_file", "annotation",
  "global", "property", "optional_direction", "optional_default",
  "optional_diagnostic", "user_defined_class", "members", "member",
  "procedure", "procedure_declaration", "procedure_annotations",
  "procedure_annotation", "structure_annotation", "pointer_rule_list",
  "pointer_rule", "structures", "structure", "behavior_annotation",
  "analyze_annotation", "rules", "rule", "condition", "test", "effects",
  "effect", "binary_property_operator", "property_sensitivity",
  "report_annotation", "report_element_list", "report_element", "pattern",
  "identifier_list", "qualified_identifier", 0
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
     315,   316,   317,   318,   319,    58,    45,    44,    59,    33,
      47,    94,   126,    40,    41,   123,   125,    42,    38,    37,
      43,   124,    46,    36,    64,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,    91,    93
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   198,   199,   199,   200,   200,   200,   200,   200,   200,
     200,   201,   201,   202,   202,   202,   202,   202,   202,   203,
     203,   203,   204,   204,   205,   205,   206,   207,   207,   208,
     208,   209,   209,   210,   210,   211,   211,   212,   212,   212,
     212,   213,   213,   213,   213,   213,   214,   214,   215,   215,
     215,   215,   216,   216,   216,   216,   216,   216,   217,   217,
     217,   217,   218,   218,   218,   218,   219,   219,   220,   220,
     221,   221,   221,   221,   222,   222,   222,   222,   222,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     224,   224,   225,   225,   225,   225,   226,   226,   226,   226,
     227,   227,   227,   227,   227,   227,   227,   228,   228,   228,
     228,   229,   229,   230,   230,   230,   230,   230,   231,   232,
     232,   233,   233
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     2,     2,     1,     1,
       1,     4,     2,     7,     7,     4,     4,     4,     4,     0,
       1,     1,     0,     2,     0,     4,     3,     1,     3,     1,
       2,     4,     3,     5,     4,     1,     2,     1,     1,     1,
       1,     4,     4,     4,     3,     3,     1,     2,     7,     5,
       4,     2,     1,     2,     3,     2,     3,     4,     1,     3,
       4,     4,     4,     2,     4,     2,     5,     5,     1,     2,
       7,     5,     4,     2,     1,     3,     3,     2,     3,     5,
       3,     6,     4,     3,     3,     2,     2,     3,     3,     2,
       1,     2,     3,     3,     2,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     0,     3,     7,     3,
       7,     1,     3,     1,     4,     2,     3,     3,     1,     1,
       3,     1,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,   118,     0,
       2,     4,     5,     8,     0,    10,     9,     6,     7,     0,
       0,   121,     0,     0,    12,    58,     0,     0,     1,     3,
       0,    19,    19,     0,     0,   122,     0,     0,    52,     0,
       0,     0,     0,     0,     0,     0,     0,    32,     0,    35,
      37,    38,    39,    40,    15,    17,    20,    21,     0,    16,
      18,     0,     0,    59,     0,   121,    55,     0,     0,    11,
      53,    34,     0,   119,     0,     0,     0,     0,     0,    68,
       0,    90,    63,     0,    65,     0,     0,     0,     0,     0,
     113,     0,     0,     0,     0,   111,     0,     0,    31,    36,
       0,    24,    24,    60,    61,    56,     0,    54,     0,    33,
       0,     0,     0,     0,    73,     0,    94,    66,    69,    67,
      91,     0,     0,    44,     0,     0,     0,    45,     0,    46,
       0,     0,     0,     0,   115,     0,     0,   107,     0,   109,
      29,     0,    27,     0,    22,    22,    57,   120,    95,    92,
      93,     0,   106,     0,     0,     0,    74,    62,    64,    41,
       0,    51,     0,    43,    47,    42,   106,     0,   117,   116,
     112,     0,    30,     0,    26,     0,     0,    13,    14,    72,
       0,     0,    85,   102,   103,   100,   101,   104,   105,     0,
       0,    89,    86,     0,     0,    77,     0,     0,     0,     0,
       0,     0,   114,     0,     0,    28,     0,    23,    83,    84,
      87,    88,   106,    96,    97,    98,    99,    80,     0,    78,
      76,    75,     0,    71,    50,     0,     0,     0,    25,     0,
      82,     0,     0,    49,   108,   110,    79,     0,    70,     0,
      81,    48
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     9,    10,    11,    12,    58,   177,   144,   101,   141,
     142,    13,    14,    48,    49,    50,   128,   129,    37,    38,
      51,    15,    78,    79,   155,   156,    80,    81,   218,   194,
      53,    94,    95,    16,    72,    25
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -138
static const yytype_int16 yypact[] =
{
     259,    19,    20,    23,    33,    32,    39,    44,  -138,   246,
    -138,  -138,  -138,  -138,   -54,  -138,  -138,  -138,  -138,   -15,
       5,   -36,    73,   140,  -138,  -138,    68,    30,  -138,  -138,
     151,   187,   236,   141,   140,  -138,    41,    34,  -138,    43,
     165,    28,    64,    83,    86,     0,     1,  -138,   160,  -138,
    -138,  -138,  -138,  -138,  -138,  -138,  -138,  -138,   112,  -138,
    -138,   112,   120,  -138,    45,  -138,  -138,    41,   143,  -138,
    -138,  -138,   102,  -138,   167,    27,   116,   204,   135,  -138,
      12,  -138,  -138,    41,  -138,    41,    55,    17,   125,   158,
    -138,   244,   254,   281,    89,  -138,   166,    94,  -138,  -138,
     290,   214,   214,  -138,  -138,  -138,    41,  -138,    41,  -138,
     291,   292,   299,     9,  -138,    57,  -138,  -138,  -138,  -138,
    -138,   110,   124,  -138,    46,    65,   230,  -138,   138,  -138,
      66,   301,    57,   225,  -138,   111,     2,  -138,    57,  -138,
     112,   152,  -138,   232,   300,   300,  -138,  -138,  -138,  -138,
    -138,    16,   210,    57,    57,   -34,  -138,  -138,  -138,  -138,
     140,  -138,    57,  -138,  -138,  -138,   248,   131,  -138,  -138,
    -138,   182,  -138,   290,  -138,    41,   306,  -138,  -138,  -138,
     308,   309,  -138,  -138,  -138,  -138,  -138,  -138,  -138,   310,
     311,  -138,  -138,   312,   269,  -138,   185,    57,    57,    49,
      69,   222,  -138,     2,     2,  -138,   159,  -138,  -138,  -138,
    -138,  -138,   248,  -138,  -138,  -138,  -138,  -138,   313,  -138,
    -138,  -138,     9,  -138,  -138,    74,   217,   218,  -138,   279,
    -138,    22,   140,  -138,  -138,  -138,  -138,   314,  -138,    71,
    -138,  -138
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -138,  -138,   315,  -138,  -138,   286,   174,   219,   -51,  -138,
     147,  -138,  -138,  -138,   274,  -138,  -138,   195,   -33,    -5,
    -138,    13,  -138,   247,    18,  -138,  -107,   -64,    97,  -137,
    -138,   -44,   191,  -138,   -76,   -12
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      24,    64,    97,    88,    88,    88,   151,   121,    33,   122,
     102,   114,    74,   197,   198,    74,   120,    89,    96,    74,
      21,    30,    17,    18,    66,    74,    19,    73,    63,   202,
      74,    82,    70,   125,   126,    21,    20,    21,    77,    34,
     199,    77,    26,    52,    65,    77,    65,    27,    21,    21,
      31,    77,    74,   124,   130,   105,    77,   103,    21,    70,
     152,    52,    36,   107,    90,    90,    90,    84,    21,    21,
      32,    73,    21,    73,    21,   229,    35,    21,    77,    67,
      22,    91,    91,    91,    92,    92,    92,   120,   119,   172,
      67,    67,   179,   127,   146,    22,   147,    22,   238,   206,
      36,    68,   113,    83,    22,    40,    22,    23,    22,    22,
      69,    67,    68,    68,    67,   231,    67,    71,    22,    70,
     161,   104,   159,    21,   222,    70,   153,   200,    22,    22,
     154,   123,    22,    68,    22,   223,    68,    22,    68,    85,
     160,    39,   165,    21,    21,   224,    21,   241,   136,   232,
     167,    75,    76,   136,   125,   126,   171,   137,    86,   226,
     227,    87,   139,    73,    41,    42,     7,   120,    74,   108,
     110,   195,   196,    41,    42,     7,   109,   108,   197,   198,
     201,    75,    76,    22,    62,    36,   157,   100,   106,   115,
     131,   108,    43,    44,    77,    70,    93,    93,    93,   239,
     158,    43,    44,    22,    22,   203,    22,   116,    45,    46,
      54,   117,    55,   180,   163,   220,   221,    45,    46,   173,
     233,   111,   112,    56,    57,   143,   108,    47,   174,   197,
     198,   132,   197,   198,    70,   228,    98,   181,   182,   138,
     183,   184,   185,   186,   187,   188,    28,   133,   189,   190,
     191,     1,     2,     3,     4,     5,   204,   134,     6,   219,
      59,     7,    60,   192,     1,     2,     3,     4,     5,   197,
     198,     6,    56,    57,     7,   193,   136,   136,   183,   184,
     185,   186,   187,   188,   135,   234,   235,   213,   214,   215,
     216,   217,     8,   140,   148,   149,   225,   213,   214,   215,
     216,   236,   150,   162,   166,     8,   168,   175,   169,   207,
     176,   208,   209,   210,   211,   212,   230,   240,    61,   178,
     205,   145,    99,   164,    29,   118,   237,   170
};

static const yytype_uint8 yycheck[] =
{
       5,    34,    46,     3,     3,     3,   113,    83,    44,    85,
      61,    75,     3,    47,    48,     3,    80,    17,    17,     3,
       3,    75,     3,     3,    36,     3,     3,    39,    33,   166,
       3,     3,    37,    16,    17,     3,     3,     3,    29,    75,
      74,    29,     3,    30,     3,    29,     3,     3,     3,     3,
      65,    29,     3,    86,    87,    67,    29,    62,     3,    64,
       3,    48,    45,    68,    64,    64,    64,     3,     3,     3,
      65,    83,     3,    85,     3,   212,     3,     3,    29,    45,
      63,    81,    81,    81,    84,    84,    84,   151,    76,   140,
      45,    45,    76,    76,   106,    63,   108,    63,    76,   175,
      45,    67,    75,    75,    63,    75,    63,    75,    63,    63,
      76,    45,    67,    67,    45,   222,    45,    74,    63,   124,
     125,    76,    76,     3,    75,   130,    69,   160,    63,    63,
      73,    76,    63,    67,    63,   199,    67,    63,    67,    75,
      75,    73,    76,     3,     3,    76,     3,    76,    59,    75,
     132,    16,    17,    59,    16,    17,   138,    68,    75,   203,
     204,    75,    68,   175,    13,    14,    15,   231,     3,    67,
       3,   153,   154,    13,    14,    15,    74,    67,    47,    48,
     162,    16,    17,    63,    43,    45,    76,    75,    45,    73,
      65,    67,    41,    42,    29,   200,   196,   196,   196,   232,
      76,    41,    42,    63,    63,    74,    63,     3,    57,    58,
      23,    76,    25,     3,    76,   197,   198,    57,    58,    67,
     225,    54,    55,    36,    37,    11,    67,    76,    76,    47,
      48,    73,    47,    48,   239,    76,    76,    27,    28,    73,
      30,    31,    32,    33,    34,    35,     0,     3,    38,    39,
      40,     5,     6,     7,     8,     9,    74,     3,    12,    74,
      24,    15,    26,    53,     5,     6,     7,     8,     9,    47,
      48,    12,    36,    37,    15,    65,    59,    59,    30,    31,
      32,    33,    34,    35,     3,    68,    68,    18,    19,    20,
      21,    22,    46,     3,     3,     3,    74,    18,    19,    20,
      21,    22,     3,    73,     3,    46,    81,    75,   197,     3,
      10,     3,     3,     3,     3,     3,     3,     3,    32,   145,
     173,   102,    48,   128,     9,    78,   229,   136
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     5,     6,     7,     8,     9,    12,    15,    46,   199,
     200,   201,   202,   209,   210,   219,   231,     3,     3,     3,
       3,     3,    63,    75,   217,   233,     3,     3,     0,   200,
      75,    65,    65,    44,    75,     3,    45,   216,   217,    73,
      75,    13,    14,    41,    42,    57,    58,    76,   211,   212,
     213,   218,   219,   228,    23,    25,    36,    37,   203,    24,
      26,   203,    43,   217,   216,     3,   233,    45,    67,    76,
     217,    74,   232,   233,     3,    16,    17,    29,   220,   221,
     224,   225,     3,    75,     3,    75,    75,    75,     3,    17,
      64,    81,    84,   196,   229,   230,    17,   229,    76,   212,
      75,   206,   206,   217,    76,   233,    45,   217,    67,    74,
       3,    54,    55,    75,   225,    73,     3,    76,   221,    76,
     225,   232,   232,    76,   216,    16,    17,    76,   214,   215,
     216,    65,    73,     3,     3,     3,    59,    68,    73,    68,
       3,   207,   208,    11,   205,   205,   233,   233,     3,     3,
       3,   224,     3,    69,    73,   222,   223,    76,    76,    76,
      75,   217,    73,    76,   215,    76,     3,   222,    81,   197,
     230,   222,   206,    67,    76,    75,    10,   204,   204,    76,
       3,    27,    28,    30,    31,    32,    33,    34,    35,    38,
      39,    40,    53,    65,   227,   222,   222,    47,    48,    74,
     216,   222,   227,    74,    74,   208,   232,     3,     3,     3,
       3,     3,     3,    18,    19,    20,    21,    22,   226,    74,
     222,   222,    75,   225,    76,    74,   229,   229,    76,   227,
       3,   224,    75,   217,    68,    68,    22,   226,    76,   216,
       3,    76
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
        case 6:
#line 350 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    { Annotations::Enabled_properties.push_back((yyvsp[(2) - (2)].t_id)->name()); ;}
    break;

  case 7:
#line 352 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    { Annotations::Disabled_properties.push_back((yyvsp[(2) - (2)].t_id)->name()); ;}
    break;

  case 10:
#line 356 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        Annotations::Current->init()->add_analysis((yyvsp[(1) - (1)].t_analyze));
      ;}
    break;

  case 11:
#line 365 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        Annotations::Current->add_globals( (yyvsp[(3) - (4)].t_structures) );
        delete (yyvsp[(3) - (4)].t_structures);
      ;}
    break;

  case 12:
#line 371 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        structuretree_list * temp = new structuretree_list();
        temp->push_back( (yyvsp[(2) - (2)].t_structure));
        Annotations::Current->add_globals( temp );
      ;}
    break;

  case 13:
#line 382 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        Annotations::Current->add_enum_property(new enumPropertyAnn((yyvsp[(2) - (7)].t_id), (yyvsp[(4) - (7)].t_direction), true, (yyvsp[(7) - (7)].t_id), (yyvsp[(5) - (7)].t_enumvalue_list), (yyvsp[(6) - (7)].t_ids)));
        delete (yyvsp[(2) - (7)].t_id);
        delete (yyvsp[(5) - (7)].t_enumvalue_list);
        delete (yyvsp[(6) - (7)].t_ids);
      ;}
    break;

  case 14:
#line 390 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        Annotations::Current->add_enum_property(new enumPropertyAnn((yyvsp[(2) - (7)].t_id), (yyvsp[(4) - (7)].t_direction), false, (yyvsp[(7) - (7)].t_id), (yyvsp[(5) - (7)].t_enumvalue_list), (yyvsp[(6) - (7)].t_ids)));
        delete (yyvsp[(2) - (7)].t_id);
        delete (yyvsp[(5) - (7)].t_enumvalue_list);
        delete (yyvsp[(6) - (7)].t_ids);
      ;}
    break;

  case 15:
#line 398 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        Annotations::Current->add_set_property(new setPropertyAnn((yyvsp[(2) - (4)].t_id), Forward, 
                                                                  setPropertyAnn::Set,
                                                                  setPropertyAnn::Union));
        delete (yyvsp[(2) - (4)].t_id);
      ;}
    break;

  case 16:
#line 406 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        Annotations::Current->add_set_property(new setPropertyAnn((yyvsp[(2) - (4)].t_id), Forward, 
                                                                  setPropertyAnn::Set,
                                                                  setPropertyAnn::Intersect));
        delete (yyvsp[(2) - (4)].t_id);
      ;}
    break;

  case 17:
#line 414 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        Annotations::Current->add_set_property(new setPropertyAnn((yyvsp[(2) - (4)].t_id), Forward, 
                                                                  setPropertyAnn::Equivalence,
                                                                  setPropertyAnn::Union));
        delete (yyvsp[(2) - (4)].t_id);
      ;}
    break;

  case 18:
#line 422 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        Annotations::Current->add_set_property(new setPropertyAnn((yyvsp[(2) - (4)].t_id), Forward, 
                                                                  setPropertyAnn::Equivalence,
                                                                  setPropertyAnn::Intersect));
        delete (yyvsp[(2) - (4)].t_id);
      ;}
    break;

  case 19:
#line 432 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
      (yyval.t_direction) = Forward;
    ;}
    break;

  case 20:
#line 437 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
      (yyval.t_direction) = Forward;
    ;}
    break;

  case 21:
#line 442 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
      (yyval.t_direction) = Backward;
    ;}
    break;

  case 22:
#line 449 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_id) = (parserID *)0;
      ;}
    break;

  case 23:
#line 454 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_id) = (yyvsp[(2) - (2)].t_id);
      ;}
    break;

  case 24:
#line 460 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    { (yyval.t_ids) = NULL; ;}
    break;

  case 25:
#line 462 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    { (yyval.t_ids) = (yyvsp[(3) - (4)].t_ids); ;}
    break;

  case 26:
#line 467 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_enumvalue_list) = (yyvsp[(2) - (3)].t_enumvalue_list);
      ;}
    break;

  case 27:
#line 474 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_enumvalue_list) = new enumvalue_list();
        (yyval.t_enumvalue_list)->push_back((yyvsp[(1) - (1)].t_enumvalue));
      ;}
    break;

  case 28:
#line 480 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_enumvalue_list) = (yyvsp[(1) - (3)].t_enumvalue_list);
        (yyval.t_enumvalue_list)->push_back((yyvsp[(3) - (3)].t_enumvalue));
      ;}
    break;

  case 29:
#line 488 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_enumvalue) = new enumValueAnn((yyvsp[(1) - (1)].t_id), (enumvalue_list *) 0);
        delete (yyvsp[(1) - (1)].t_id);
      ;}
    break;

  case 30:
#line 494 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_enumvalue) = new enumValueAnn((yyvsp[(1) - (2)].t_id), (yyvsp[(2) - (2)].t_enumvalue_list));
        delete (yyvsp[(1) - (2)].t_id);
        delete (yyvsp[(2) - (2)].t_enumvalue_list);
      ;}
    break;

  case 31:
#line 505 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        Annotations::Current->add_procedure(procedureAnn::Current);
        procedureAnn::Current = 0;
      ;}
    break;

  case 32:
#line 511 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        Annotations::Current->add_procedure(procedureAnn::Current);
        procedureAnn::Current = 0;
      ;}
    break;

  case 33:
#line 519 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        procedureAnn::Current = new procedureAnn((yyvsp[(2) - (5)].t_id), (yyvsp[(4) - (5)].t_ids), Annotations::Current, (yyvsp[(1) - (5)].t_tok));
        delete (yyvsp[(2) - (5)].t_id);
      ;}
    break;

  case 34:
#line 525 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        procedureAnn::Current = new procedureAnn((yyvsp[(2) - (4)].t_id), (parserid_list *)0, Annotations::Current, (yyvsp[(1) - (4)].t_tok));
        delete (yyvsp[(2) - (4)].t_id);
      ;}
    break;

  case 37:
#line 538 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
      ;}
    break;

  case 38:
#line 542 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
      ;}
    break;

  case 39:
#line 546 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        procedureAnn::Current->add_analysis((yyvsp[(1) - (1)].t_analyze));
      ;}
    break;

  case 40:
#line 551 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
      ;}
    break;

  case 41:
#line 560 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        procedureAnn::Current->add_on_entry((yyvsp[(3) - (4)].t_structures));
      ;}
    break;

  case 42:
#line 565 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        procedureAnn::Current->add_on_exit(new pointerRuleAnn((exprAnn *)0, (yyvsp[(3) - (4)].t_structures), (yyvsp[(1) - (4)].t_tok)));
      ;}
    break;

  case 43:
#line 570 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
      ;}
    break;

  case 44:
#line 574 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
      ;}
    break;

  case 45:
#line 578 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
      ;}
    break;

  case 48:
#line 589 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        procedureAnn::Current->add_on_exit(new pointerRuleAnn((yyvsp[(3) - (7)].t_expr), (yyvsp[(6) - (7)].t_structures), (yyvsp[(1) - (7)].t_tok)));
      ;}
    break;

  case 49:
#line 594 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        structuretree_list * temp = new structuretree_list();
        temp->push_back((yyvsp[(5) - (5)].t_structure));
        procedureAnn::Current->add_on_exit(new pointerRuleAnn((yyvsp[(3) - (5)].t_expr), temp, (yyvsp[(1) - (5)].t_tok)));
      ;}
    break;

  case 50:
#line 601 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        procedureAnn::Current->add_on_exit(new pointerRuleAnn((exprAnn *)0, (yyvsp[(3) - (4)].t_structures), (yyvsp[(1) - (4)].t_tok)));
      ;}
    break;

  case 51:
#line 606 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        structuretree_list * temp = new structuretree_list();
        temp->push_back((yyvsp[(2) - (2)].t_structure));
        procedureAnn::Current->add_on_exit(new pointerRuleAnn((exprAnn *)0, temp, (yyvsp[(1) - (2)].t_tok)));
      ;}
    break;

  case 52:
#line 616 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_structures) = new structuretree_list();
        (yyval.t_structures)->push_back((yyvsp[(1) - (1)].t_structure));
      ;}
    break;

  case 53:
#line 622 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_structures) = (yyvsp[(1) - (2)].t_structures);
        (yyval.t_structures)->push_back((yyvsp[(2) - (2)].t_structure));
      ;}
    break;

  case 54:
#line 628 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_structures) = (yyvsp[(1) - (3)].t_structures);
        (yyval.t_structures)->push_back((yyvsp[(3) - (3)].t_structure));
      ;}
    break;

  case 55:
#line 634 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    { 
        procedureAnn::Current->add_delete((yyvsp[(2) - (2)].t_id));
        delete (yyvsp[(2) - (2)].t_id);

        /* "delete" doesn't actually add anything to the structure tree, so
           we just pass an empty list back. Similarly for the next two
           productions. */

        (yyval.t_structures) = new structuretree_list();
      ;}
    break;

  case 56:
#line 646 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        procedureAnn::Current->add_delete((yyvsp[(3) - (3)].t_id));
        delete (yyvsp[(3) - (3)].t_id);
        (yyval.t_structures) = (yyvsp[(1) - (3)].t_structures);
      ;}
    break;

  case 57:
#line 653 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        procedureAnn::Current->add_delete((yyvsp[(4) - (4)].t_id));
        delete (yyvsp[(4) - (4)].t_id);
        (yyval.t_structures) = (yyvsp[(1) - (4)].t_structures);
      ;}
    break;

  case 58:
#line 662 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_structure) = new structureTreeAnn((yyvsp[(1) - (1)].t_id), (structuretree_list *)0,
                                  structureTreeAnn::None, false);
        delete (yyvsp[(1) - (1)].t_id);
      ;}
    break;

  case 59:
#line 669 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_structure) = new structureTreeAnn((yyvsp[(1) - (3)].t_id), (yyvsp[(3) - (3)].t_structure), structureTreeAnn::Arrow, false);
        delete (yyvsp[(1) - (3)].t_id);
      ;}
    break;

  case 60:
#line 675 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_structure) = new structureTreeAnn((yyvsp[(1) - (4)].t_id), (yyvsp[(4) - (4)].t_structure), structureTreeAnn::Arrow, true);
        delete (yyvsp[(1) - (4)].t_id);
      ;}
    break;

  case 61:
#line 681 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_structure) = new structureTreeAnn((yyvsp[(1) - (4)].t_id), (yyvsp[(3) - (4)].t_structures), structureTreeAnn::Dot, false);
        delete (yyvsp[(1) - (4)].t_id);
      ;}
    break;

  case 62:
#line 692 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        procedureAnn::Current->add_uses( (yyvsp[(3) - (4)].t_ids) );
        delete (yyvsp[(3) - (4)].t_ids);
      ;}
    break;

  case 63:
#line 698 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        parserid_list temp;
        temp.push_back( * (yyvsp[(2) - (2)].t_id));
        procedureAnn::Current->add_uses( & temp );
        delete (yyvsp[(2) - (2)].t_id);
      ;}
    break;

  case 64:
#line 706 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        procedureAnn::Current->add_defs( (yyvsp[(3) - (4)].t_ids) );
        delete (yyvsp[(3) - (4)].t_ids);
      ;}
    break;

  case 65:
#line 712 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        parserid_list temp;
        temp.push_back( * (yyvsp[(2) - (2)].t_id));
        procedureAnn::Current->add_defs( & temp );
        delete (yyvsp[(2) - (2)].t_id);
      ;}
    break;

  case 66:
#line 725 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_analyze) = new analyzeAnn(Annotations::Current, (yyvsp[(2) - (5)].t_id), (yyvsp[(4) - (5)].t_rules), (yyvsp[(1) - (5)].t_tok));
        delete (yyvsp[(2) - (5)].t_id);
      ;}
    break;

  case 67:
#line 731 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        rule_list * temp_rules = new rule_list();
        temp_rules->push_back(new ruleAnn((exprAnn *)0, (yyvsp[(4) - (5)].t_exprs), (yyvsp[(3) - (5)].t_tok)));
        (yyval.t_analyze) = new analyzeAnn(Annotations::Current, (yyvsp[(2) - (5)].t_id), temp_rules, (yyvsp[(1) - (5)].t_tok));
        delete (yyvsp[(2) - (5)].t_id);
      ;}
    break;

  case 68:
#line 741 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_rules) = new rule_list();
        (yyval.t_rules)->push_back((yyvsp[(1) - (1)].t_rule));
      ;}
    break;

  case 69:
#line 747 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_rules) = (yyvsp[(1) - (2)].t_rules);
        (yyval.t_rules)->push_back((yyvsp[(2) - (2)].t_rule));
      ;}
    break;

  case 70:
#line 755 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_rule) = new ruleAnn((yyvsp[(3) - (7)].t_expr), (yyvsp[(6) - (7)].t_exprs), (yyvsp[(1) - (7)].t_tok));
      ;}
    break;

  case 71:
#line 760 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        exprann_list * temp = new exprann_list();
        temp->push_back((yyvsp[(5) - (5)].t_expr));
        (yyval.t_rule) = new ruleAnn((yyvsp[(3) - (5)].t_expr), temp, (yyvsp[(1) - (5)].t_tok));
      ;}
    break;

  case 72:
#line 767 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_rule) = new ruleAnn((exprAnn *)0, (yyvsp[(3) - (4)].t_exprs), (yyvsp[(1) - (4)].t_tok));
      ;}
    break;

  case 73:
#line 772 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        exprann_list * temp = new exprann_list();
        temp->push_back((yyvsp[(2) - (2)].t_expr));
        (yyval.t_rule) = new ruleAnn((exprAnn *)0, temp, (yyvsp[(1) - (2)].t_tok));
      ;}
    break;

  case 74:
#line 782 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_expr) = (yyvsp[(1) - (1)].t_expr);
      ;}
    break;

  case 75:
#line 787 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_expr) = new connectiveExprAnn(Broadway::Or, (yyvsp[(1) - (3)].t_expr), (yyvsp[(3) - (3)].t_expr), (yyvsp[(2) - (3)].t_tok));
      ;}
    break;

  case 76:
#line 792 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_expr) = new connectiveExprAnn(Broadway::And, (yyvsp[(1) - (3)].t_expr), (yyvsp[(3) - (3)].t_expr), (yyvsp[(2) - (3)].t_tok));
      ;}
    break;

  case 77:
#line 797 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_expr) = new connectiveExprAnn(Broadway::Not, (yyvsp[(2) - (2)].t_expr), (exprAnn *)0, (yyvsp[(1) - (2)].t_tok));
      ;}
    break;

  case 78:
#line 802 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_expr) = (yyvsp[(2) - (3)].t_expr);
      ;}
    break;

  case 79:
#line 811 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_expr) = new enumPropertyExprAnn((yyvsp[(4) - (5)].t_property_sensitivity).fs, (yyvsp[(1) - (5)].t_id), (yyvsp[(3) - (5)].t_id), Broadway::Is_Bottom,
                                     (const parserID *)0, (yyvsp[(4) - (5)].t_property_sensitivity).line);
        delete (yyvsp[(1) - (5)].t_id);
        delete (yyvsp[(3) - (5)].t_id);
      ;}
    break;

  case 80:
#line 819 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_expr) = new enumPropertyExprAnn((yyvsp[(2) - (3)].t_property_sensitivity).fs, (const parserID *)0, (yyvsp[(1) - (3)].t_id), Broadway::Is_Bottom,
                                     (const parserID *)0, (yyvsp[(2) - (3)].t_property_sensitivity).line);
        delete (yyvsp[(1) - (3)].t_id);
      ;}
    break;

  case 81:
#line 826 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_expr) = new enumPropertyExprAnn((yyvsp[(4) - (6)].t_property_sensitivity).fs, (yyvsp[(1) - (6)].t_id), (yyvsp[(3) - (6)].t_id), (yyvsp[(5) - (6)].t_property_operator).op, (yyvsp[(6) - (6)].t_id), (yyvsp[(5) - (6)].t_property_operator).line);
        delete (yyvsp[(1) - (6)].t_id);
        delete (yyvsp[(3) - (6)].t_id);
        delete (yyvsp[(6) - (6)].t_id);
      ;}
    break;

  case 82:
#line 834 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_expr) = new enumPropertyExprAnn((yyvsp[(2) - (4)].t_property_sensitivity).fs, (const parserID *)0, (yyvsp[(1) - (4)].t_id), (yyvsp[(3) - (4)].t_property_operator).op, (yyvsp[(4) - (4)].t_id), (yyvsp[(3) - (4)].t_property_operator).line);
        delete (yyvsp[(1) - (4)].t_id);
        delete (yyvsp[(4) - (4)].t_id);
      ;}
    break;

  case 83:
#line 843 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_expr) = new setPropertyExprAnn(Broadway::Before, (yyvsp[(2) - (3)].t_id), (yyvsp[(1) - (3)].t_id), Broadway::Is_Equivalent, (yyvsp[(3) - (3)].t_id), (yyvsp[(1) - (3)].t_id)->line());
        delete (yyvsp[(1) - (3)].t_id);
        delete (yyvsp[(2) - (3)].t_id);
        delete (yyvsp[(3) - (3)].t_id);
      ;}
    break;

  case 84:
#line 851 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_expr) = new setPropertyExprAnn(Broadway::Before, (yyvsp[(3) - (3)].t_id), (yyvsp[(1) - (3)].t_id), Broadway::Is_Element_Of,
                                    (const parserID *)0, (yyvsp[(2) - (3)].t_tok));
        delete (yyvsp[(1) - (3)].t_id);
        delete (yyvsp[(3) - (3)].t_id);
      ;}
    break;

  case 85:
#line 859 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_expr) = new setPropertyExprAnn(Broadway::Before, (yyvsp[(1) - (2)].t_id), (const parserID *)0,
                                    Broadway::Is_EmptySet, (const parserID *)0, (yyvsp[(2) - (2)].t_tok));

        delete (yyvsp[(1) - (2)].t_id);
      ;}
    break;

  case 86:
#line 869 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_expr) = new numericExprAnn((yyvsp[(1) - (2)].t_id), annlineno);
      ;}
    break;

  case 87:
#line 876 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_expr) = new pointerExprAnn(Broadway::Is_AliasOf, (yyvsp[(1) - (3)].t_id), (yyvsp[(3) - (3)].t_id));
        delete (yyvsp[(1) - (3)].t_id);
        delete (yyvsp[(3) - (3)].t_id);
      ;}
    break;

  case 88:
#line 883 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_expr) = new pointerExprAnn(Broadway::Is_SameAs, (yyvsp[(1) - (3)].t_id), (yyvsp[(3) - (3)].t_id));
        delete (yyvsp[(1) - (3)].t_id);
        delete (yyvsp[(3) - (3)].t_id);
      ;}
    break;

  case 89:
#line 890 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_expr) = new pointerExprAnn(Broadway::Is_Empty, (yyvsp[(1) - (2)].t_id), (const parserID *)0);
        delete (yyvsp[(1) - (2)].t_id);
      ;}
    break;

  case 90:
#line 899 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_exprs) = new exprann_list();
        (yyval.t_exprs)->push_back((yyvsp[(1) - (1)].t_expr));
      ;}
    break;

  case 91:
#line 905 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_exprs) = (yyvsp[(1) - (2)].t_exprs);
        (yyval.t_exprs)->push_back((yyvsp[(2) - (2)].t_expr));
      ;}
    break;

  case 92:
#line 914 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_expr) = new enumPropertyExprAnn(Broadway::None,
                                     (const parserID *)0, (yyvsp[(1) - (3)].t_id), Broadway::Assign,
                                     (yyvsp[(3) - (3)].t_id), (yyvsp[(2) - (3)].t_tok));
        delete (yyvsp[(1) - (3)].t_id);
        delete (yyvsp[(3) - (3)].t_id);
      ;}
    break;

  case 93:
#line 923 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        enumPropertyExprAnn * tmp = new enumPropertyExprAnn(Broadway::None,
                                                            (const parserID *)0, (yyvsp[(1) - (3)].t_id), Broadway::Assign,
                                                            (yyvsp[(3) - (3)].t_id), (yyvsp[(2) - (3)].t_tok));
        tmp->set_weak();
        (yyval.t_expr) = tmp;
        delete (yyvsp[(1) - (3)].t_id);
        delete (yyvsp[(3) - (3)].t_id);
      ;}
    break;

  case 94:
#line 934 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_expr) = new setPropertyExprAnn(Broadway::None,
                                    (const parserID *)0, (yyvsp[(2) - (2)].t_id), Broadway::Add_Elements,
                                    (const parserID *)0, (yyvsp[(1) - (2)].t_tok));
        delete (yyvsp[(2) - (2)].t_id);
      ;}
    break;

  case 95:
#line 942 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_expr) = new setPropertyExprAnn(Broadway::None,
                                    (yyvsp[(2) - (3)].t_id), (yyvsp[(1) - (3)].t_id), Broadway::Add_Equivalences, (yyvsp[(3) - (3)].t_id), (yyvsp[(1) - (3)].t_id)->line());
        delete (yyvsp[(1) - (3)].t_id);
        delete (yyvsp[(2) - (3)].t_id);
        delete (yyvsp[(3) - (3)].t_id);
      ;}
    break;

  case 96:
#line 954 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    { (yyval.t_property_operator).line = (yyvsp[(1) - (1)].t_tok);
        (yyval.t_property_operator).op = Broadway::Is_Exactly; ;}
    break;

  case 97:
#line 958 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    { (yyval.t_property_operator).line = (yyvsp[(1) - (1)].t_tok);
        (yyval.t_property_operator).op = Broadway::Is_AtLeast; ;}
    break;

  case 98:
#line 962 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    { (yyval.t_property_operator).line = (yyvsp[(1) - (1)].t_tok);
        (yyval.t_property_operator).op = Broadway::Could_Be; ;}
    break;

  case 99:
#line 966 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    { (yyval.t_property_operator).line = (yyvsp[(1) - (1)].t_tok);
        (yyval.t_property_operator).op = Broadway::Is_AtMost; ;}
    break;

  case 100:
#line 973 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    { (yyval.t_property_sensitivity).line = (yyvsp[(1) - (1)].t_tok);
         (yyval.t_property_sensitivity).fs = Broadway::Before; ;}
    break;

  case 101:
#line 977 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    { (yyval.t_property_sensitivity).line = (yyvsp[(1) - (1)].t_tok);
         (yyval.t_property_sensitivity).fs = Broadway::After; ;}
    break;

  case 102:
#line 981 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    { (yyval.t_property_sensitivity).line = (yyvsp[(1) - (1)].t_tok);
         (yyval.t_property_sensitivity).fs = Broadway::Always; ;}
    break;

  case 103:
#line 985 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    { (yyval.t_property_sensitivity).line = (yyvsp[(1) - (1)].t_tok);
         (yyval.t_property_sensitivity).fs = Broadway::Ever; ;}
    break;

  case 104:
#line 989 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    { (yyval.t_property_sensitivity).line = (yyvsp[(1) - (1)].t_tok);
         (yyval.t_property_sensitivity).fs = Broadway::Trace; ;}
    break;

  case 105:
#line 993 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    { (yyval.t_property_sensitivity).line = (yyvsp[(1) - (1)].t_tok);
         (yyval.t_property_sensitivity).fs = Broadway::Confidence; ;}
    break;

  case 106:
#line 997 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    { (yyval.t_property_sensitivity).line =  0;
         (yyval.t_property_sensitivity).fs = Broadway::Before; ;}
    break;

  case 107:
#line 1006 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
         procedureAnn::Current->add_report(new reportAnn((exprAnn *)0, false, (yyvsp[(2) - (3)].t_report_elements), (yyvsp[(1) - (3)].t_tok)));
       ;}
    break;

  case 108:
#line 1011 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
         procedureAnn::Current->add_report(new reportAnn((yyvsp[(4) - (7)].t_expr), false, (yyvsp[(6) - (7)].t_report_elements), (yyvsp[(1) - (7)].t_tok)));
       ;}
    break;

  case 109:
#line 1015 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
         procedureAnn::Current->add_report(new reportAnn((exprAnn *)0, true, (yyvsp[(2) - (3)].t_report_elements), (yyvsp[(1) - (3)].t_tok)));
       ;}
    break;

  case 110:
#line 1020 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
         procedureAnn::Current->add_report(new reportAnn((yyvsp[(4) - (7)].t_expr), true, (yyvsp[(6) - (7)].t_report_elements), (yyvsp[(1) - (7)].t_tok)));
       ;}
    break;

  case 111:
#line 1028 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
         (yyval.t_report_elements) = new report_element_list();
         (yyval.t_report_elements)->push_back( (yyvsp[(1) - (1)].t_report_element) );
       ;}
    break;

  case 112:
#line 1034 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
         (yyval.t_report_elements) = (yyvsp[(1) - (3)].t_report_elements);
         (yyval.t_report_elements)->push_back((yyvsp[(3) - (3)].t_report_element));
       ;}
    break;

  case 113:
#line 1044 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
         (yyval.t_report_element) = new literalReportElementAnn((yyvsp[(1) - (1)].t_id));
         delete (yyvsp[(1) - (1)].t_id);
       ;}
    break;

  case 114:
#line 1050 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_report_element) = new expressionReportElementAnn((yyvsp[(4) - (4)].t_property_sensitivity).fs,
                                            new enumPropertyExprAnn((yyvsp[(4) - (4)].t_property_sensitivity).fs, (yyvsp[(1) - (4)].t_id), (yyvsp[(3) - (4)].t_id), Broadway::Report,
                                                                (const parserID *)0, (yyvsp[(1) - (4)].t_id)->line()),
                                            annlineno);
        delete (yyvsp[(1) - (4)].t_id);
        delete (yyvsp[(3) - (4)].t_id);
      ;}
    break;

  case 115:
#line 1060 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_report_element) = new locationReportElementAnn((yyvsp[(2) - (2)].t_id));
      ;}
    break;

  case 116:
#line 1065 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_report_element) = new bindingReportElementAnn((yyvsp[(2) - (3)].t_id), false);
      ;}
    break;

  case 117:
#line 1070 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
        (yyval.t_report_element) = new bindingReportElementAnn((yyvsp[(2) - (3)].t_id), true);
      ;}
    break;

  case 118:
#line 1081 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    {
      ;}
    break;

  case 119:
#line 1089 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    { (yyval.t_ids) = new parserid_list();
        (yyval.t_ids)->push_back( * (yyvsp[(1) - (1)].t_id) );
        delete (yyvsp[(1) - (1)].t_id);
      ;}
    break;

  case 120:
#line 1095 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    { (yyval.t_ids) = (yyvsp[(1) - (3)].t_ids);
        (yyval.t_ids)->push_back( * (yyvsp[(3) - (3)].t_id));
        delete (yyvsp[(3) - (3)].t_id);
      ;}
    break;

  case 121:
#line 1103 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    { (yyval.t_id) = (yyvsp[(1) - (1)].t_id); ;}
    break;

  case 122:
#line 1106 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"
    { (yyval.t_id) = (yyvsp[(2) - (2)].t_id);
        (yyval.t_id)->set_io();
      ;}
    break;


/* Line 1267 of yacc.c.  */
#line 2951 "./tempBisonOutput.c"
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


#line 1113 "../../../../src/midend/programAnalysis/annotationLanguageParser/language.y"


