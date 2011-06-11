/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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
/* Line 1529 of yacc.c.  */
#line 479 "./tempBisonOutput.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE annlval;

