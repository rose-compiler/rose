%token ERROR SYNERROR PARAMETER DEFINE EVAL LBEGIN RBEGIN PRINT INPUT_ENDFILE INPUT_ENDL INPUT_CONT FALSE TOKEN INHERIT
%token COND ANNOT TO FROM TYPE POETTYPE SYNTAX MESSAGE 
%token LTAG RTAG ENDTAG PARS XFORM OUTPUT   
%token PARSE LOOKAHEAD MATCH CODE GLOBAL SOURCE ENDCODE ENDXFORM INPUT ENDINPUT 
%right SEMICOLON  
%nonassoc FOR FOREACH FOREACH_R BREAK CONTINUE RETURN DEBUG
%nonassoc IF ELSE CASE SWITCH DEFAULT DOT3 DOT4
%right ASSIGN PLUS_ASSIGN MINUS_ASSIGN MULT_ASSIGN DIVIDE_ASSIGN MOD_ASSIGN
%left COMMA  QUESTION
%left OR TOR
%left AND
%right NOT
%nonassoc LT LE EQ GT GE NE ARROW DARROW 
%left PLUS MINUS
%left MULTIPLY DIVIDE MOD STR_CONCAT
%right DCOLON 
%nonassoc UMINUS TILT
%nonassoc DELAY APPLY CLEAR SAVE RESTORE PERMUTE REPLACE RANGE DUPLICATE  REBUILD VAR MAP TUPLE LIST LIST1 INT STRING NAME EXP TRACE ERASE COPY SPLIT LEN INSERT
%right CAR CDR COLON
%nonassoc ID ICONST ANY
%left DOT  DOT2
%left POND
%nonassoc LB RB LBR RBR
%nonassoc LP RP

%{
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
%}

%%

poet :  sections

sections :  
    | sections section

section : LTAG PARAMETER ID {$$.ptr=insert_parameter($3.ptr);} paramAttrs ENDTAG 
            {finish_parameter($4); }
    | LTAG DEFINE ID {$$.config=GLOBAL_SCOPE; } typeMulti ENDTAG  
            {eval_define(make_macroVar($3.ptr),$5); }
    | LTAG EVAL {$$.config=GLOBAL_VAR; } code ENDTAG { insert_eval($4.ptr); } 
    | LTAG TRACE traceVars ENDTAG { insert_trace($3); } 
    | LTAG CODE ID {$$.ptr=insert_code($3.ptr);$$.config=0; } codeAttrs {$$.ptr = $4.ptr; } codeRHS
    | LTAG INPUT {$$.ptr=insert_input(); } inputAttrs {$$.ptr=$3.ptr; } inputRHS 
    | LTAG OUTPUT {$$.ptr=insert_output(); } outputAttrs {$$.ptr=$3.ptr; } ENDTAG
    | LTAG XFORM ID { $$.ptr=insert_xform($3.ptr); } xformAttrs xformRHS 
            { set_xform_def($4.ptr,$6.ptr); } 
    | inputBase inputCodeList INPUT_ENDFILE 
         { if ($2.ptr==0) insert_source($1.ptr); else insert_source(make_inputlist2($1.ptr,$2.ptr)); }

inputAttrs : inputAttr {$$.ptr = $0.ptr; } inputAttrs
    | ;
inputAttr : DEBUG ASSIGN {$$.config=GLOBAL_SCOPE;} codeUnit { set_input_debug($0.ptr,$4.ptr); }
    |       TO ASSIGN POETTYPE { set_input_target($0.ptr, $0.ptr); }
    |       TO ASSIGN ID { set_input_target($0.ptr,make_varRef($3.ptr,GLOBAL_VAR)); }
    |       SYNTAX ASSIGN {$$.config=GLOBAL_SCOPE;} codeUnit {set_input_syntax($0.ptr,$4.ptr);}
    |       PARSE ASSIGN  POETTYPE { set_input_codeType($0.ptr, $0.ptr); } 
    |       PARSE ASSIGN  {$$.config=CODE_OR_XFORM_VAR; } parseType 
                   { set_input_codeType($0.ptr,$4.ptr); } 
    |       FROM ASSIGN {$$.config=GLOBAL_SCOPE;} codeUnit {set_input_files($0.ptr,$4.ptr); } 
    |       ANNOT ASSIGN {$$.config=GLOBAL_SCOPE;} codeUnit { set_input_annot($0.ptr,$4.ptr); }
    |       COND ASSIGN {$$.config=GLOBAL_SCOPE;} codeUnit { set_input_cond($0.ptr,$4.ptr); }

outputAttrs : outputAttr {$$.ptr = $0.ptr; } outputAttrs
    | ;
outputAttr : FROM ASSIGN {$$.config=GLOBAL_VAR;} codeUnit  
                  { set_output_target($0.ptr,$4.ptr); }
    |       SYNTAX ASSIGN {$$.config=GLOBAL_SCOPE;} codeUnit 
                  { set_output_syntax($0.ptr,$4.ptr); }
    |       TO ASSIGN {$$.config=GLOBAL_VAR;} codeUnit 
                  { set_output_file($0.ptr,$4.ptr); } 
    |       COND ASSIGN {$$.config=GLOBAL_SCOPE;} codeUnit 
                  { set_output_cond($0.ptr,$4.ptr); } 
inputRHS : RTAG inputCodeList ENDINPUT  { set_input_inline($0.ptr,$2.ptr);  }
    |  ENDTAG  

codeAttrs :  | codeAttr {$$.ptr=$0.ptr; $$.config=$1.config;} codeAttrs 
codeAttr : PARS ASSIGN LP codePars RP { set_code_params($0.ptr,$4.ptr); $$.config=$0.config; }
    | ID ASSIGN {$$.config=GLOBAL_SCOPE;} typeSpec { set_code_attr($0.ptr, $1.ptr, $4.ptr); }
    | ID ASSIGN INHERIT { set_code_inherit($0.ptr, $1.ptr); }
    | COND ASSIGN {$$.config=ID_DEFAULT;} codeUnit 
      { set_local_static(make_sourceString("cond",4), $4.ptr,LVAR_ATTR,$4.ptr,1); $$.config=$0.config; }
    | REBUILD ASSIGN {$$.config=ID_DEFAULT; } codeUnit
      { set_local_static(make_sourceString("rebuild",7), $4.ptr,LVAR_ATTR,$4.ptr,1); $$.config=$0.config; }
    | PARSE ASSIGN {$$.config=ID_DEFAULT; } parseType
      { set_code_parse($0.ptr,$4.ptr); $$.config=$0.config; }
    | LOOKAHEAD ASSIGN ICONST { set_code_lookahead($0.ptr, $3.ptr); $$.config=$0.config; }
    | MATCH ASSIGN {$$.config=ID_DEFAULT; } typeSpec
      { set_local_static(make_sourceString("match",5), $4.ptr,LVAR_ATTR,$4.ptr,1); $$.config=$0.config; }
    | OUTPUT ASSIGN {$$.config=ID_DEFAULT; } codeUnit
      { set_local_static(make_sourceString("output",6), $4.ptr,LVAR_ATTR,$4.ptr,1); $$.config=$0.config; }

varRef: ID { $$.ptr= make_varRef($1.ptr,$0.config); $$.config=$0.config; }
    | GLOBAL DOT ID { $$.ptr=make_varRef($3.ptr,GLOBAL_SCOPE); $$.config=$0.config; }
    | XFORM xformRef {$$=$2; }
    | CODE codeRef {$$=$2; }

xformRef :  { $$.ptr = make_dummyOperator(POET_OP_XFORM); $$.config=$0.config; }
    |  DOT ID { $$.ptr=make_varRef($2.ptr,XFORM_VAR); $$.config=$0.config; }

codeRef : { $$.ptr = make_dummyOperator(POET_OP_CODE); }
    |  DOT ID { $$.ptr=make_varRef($2.ptr,CODE_VAR); }


codePars :  ID { $$.ptr=make_localPar($1.ptr,0,LVAR_CODEPAR); } 
    | ID COLON {$$.config=CODE_OR_XFORM_VAR; } parseType { $$.ptr = make_localPar($1.ptr, $4.ptr,LVAR_CODEPAR); }
    | codePars COMMA codePars {$$.ptr = make_sourceVector($1.ptr, $3.ptr); } 

paramAttrs :  | paramAttrs {$$.ptr=$0.ptr; } paramAttr 
paramAttr : TYPE ASSIGN {$$.config=GLOBAL_SCOPE;} typeMulti 
                {set_param_type($0.ptr,$4.ptr); }
    |  DEFAULT ASSIGN {$$.config=GLOBAL_SCOPE;} codeUnit { set_param_default($0.ptr, $4.ptr); }
    |  PARSE ASSIGN {$$.config=GLOBAL_SCOPE; } parseType { set_param_parse($0.ptr, $4.ptr); }
    |  MESSAGE ASSIGN SOURCE { set_param_message($0.ptr, $3.ptr); }


map:  MAP LP {$$.config=$0.config; } typeSpec COMMA 
              {$$.config=$0.config; } typeSpec RP 
          { $$.ptr = make_sourceBop(POET_OP_MAP, $4.ptr, $7.ptr); }
    | MAP LP RP { $$.ptr=make_sourceUop(POET_OP_MAP,make_empty()); } 
    | MAP LB RB { $$.ptr=make_sourceUop(POET_OP_MAP,make_empty()); } 
    | MAP LB mapEntries RB 
        {$$.ptr=make_sourceUop(POET_OP_MAP, $3.ptr); }

mapEntries : codeUnit ARROW codeUnit 
      { $$.ptr = make_inputlist(make_inputlist($1.ptr,$3.ptr),0); } 
    | codeUnit ARROW codeUnit COMMA mapEntries 
      { $$.ptr = make_inputlist2(make_inputlist($1.ptr,$3.ptr),$5); } 
      
singletype : constant {$$.ptr=$1.ptr; }
    | ANY { $$.ptr = make_any(); }
    | INT { $$.ptr = make_type(TYPE_INT); }
    | STRING { $$.ptr = make_type(TYPE_STRING); }
    | NAME { $$.ptr = make_type(TYPE_ID); }
    | EXP { $$.ptr = make_dummyOperator(POET_OP_EXP); }
    | VAR  {  $$.ptr = make_dummyOperator(POET_OP_VAR); }
    | TUPLE { $$.ptr = make_dummyOperator(POET_OP_TUPLE); }
    | map { $$=$1; }

typeSpec1: singletype {$$=$1; }
    | MINUS ICONST { $$.ptr = negate_Iconst($2.ptr); }
    | varInvokeType { $$.ptr=$1.ptr; }
    | TILT typeSpec { $$.ptr = make_typeNot($2); }
    | LP {$$.config=$0.config;} typeMulti RP { $$.ptr = $3.ptr; }

typeSpec: typeSpec1 {$$=$1; }
    | typeSpec TOR {$$.config=$0.config;} typeSpec
            { $$.ptr = make_typeTor($1.ptr,$4.ptr); }

typeMulti : typeSpec {$$.config=$0.config; } typeList  { $$.ptr = ($3.ptr==0)? $1.ptr : make_typelist2($1.ptr,$3.ptr); }
    |  typeTuple  { $$.ptr = $1.ptr; }
    | typeSpec DOT4 {$$.ptr = make_typeUop(TYPE_LIST1, $1.ptr); }
    | typeSpec DOT3 {$$.ptr = make_typeUop(TYPE_LIST, $1.ptr); }
    | typeSpec1 DOT2 {$$.config=$0.config;} typeSpec1
           {$$.ptr = make_sourceBop(POET_OP_RANGE, $1.ptr,$4.ptr);}
    | typeSpec PLUS {$$.config=$0.config;} typeSpec
           { $$.ptr=make_sourceBop(POET_OP_PLUS, $1.ptr,$4.ptr); }
    | typeSpec MULTIPLY {$$.config=$0.config;} typeSpec
           { $$.ptr = make_sourceBop(POET_OP_MULT,  $1.ptr,$4.ptr); }
    | typeSpec DIVIDE {$$.config=$0.config;} typeSpec
           { $$.ptr = make_sourceBop(POET_OP_DIVIDE,  $1.ptr,$4.ptr); }
    | typeSpec MOD {$$.config=$0.config;} typeSpec { $$.ptr = make_sourceBop(POET_OP_MOD,  $1.ptr,$4.ptr); }
     | typeSpec DCOLON {$$.config=$0.config;} typeSpec { $$.ptr = make_sourceBop(POET_OP_CONS,$1.ptr,$4.ptr); }

typeList :  {$$.ptr=0; }
    |  typeSpec {$$.config=$0.config; } typeList { $$.ptr = make_typelist2($1.ptr,$3.ptr); }
typeTuple :  typeSpec COMMA {$$.config=$0.config;} typeSpec 
                   { $$.ptr = make_sourceVector2($1.ptr,$4.ptr); }
    | typeTuple COMMA {$$.config=$0.config;} typeSpec { $$.ptr = make_sourceVector($1.ptr,$4.ptr); }

patternSpec: singletype {$$=$1; }
    | MINUS ICONST { $$.ptr = negate_Iconst($2.ptr); }
    | CLEAR ID { $$.ptr = make_sourceUop(POET_OP_CLEAR, make_varRef($2.ptr,ASSIGN_VAR)); }
    | varInvokePattern { $$.ptr=$1.ptr; }
    | LP {$$.config=$0.config;} patternMulti RP { $$.ptr = $3.ptr; }
    | patternSpec TOR {$$.config=$0.config;} patternSpec 
            { $$.ptr = make_typeTor($1.ptr,$4.ptr); }
    | patternSpec DOT3 {$$.ptr = make_typeUop(TYPE_LIST, $1.ptr); }
    | patternSpec DOT2 {$$.config=$0.config;} patternSpec
           {$$.ptr = make_sourceBop(POET_OP_RANGE, $1.ptr,$4.ptr);}
    | patternSpec PLUS {$$.config=$0.config;} patternSpec
           { $$.ptr=make_sourceBop(POET_OP_PLUS, $1.ptr,$4.ptr); }
    | patternSpec MULTIPLY {$$.config=$0.config;} patternSpec
           { $$.ptr = make_sourceBop(POET_OP_MULT,  $1.ptr,$4.ptr); }
    | patternSpec DIVIDE {$$.config=$0.config;} patternSpec 
           { $$.ptr = make_sourceBop(POET_OP_DIVIDE,  $1.ptr,$4.ptr); }
    | patternSpec MOD {$$.config=$0.config;} patternSpec 
           { $$.ptr = make_sourceBop(POET_OP_MOD,  $1.ptr,$4.ptr); }
    | patternSpec DCOLON {$$.config=$0.config;} patternSpec { $$.ptr = make_sourceBop(POET_OP_CONS,$1.ptr,$4.ptr); }

patternMulti : patternSpec {$$.config=$0.config; } patternList  { $$.ptr = ($3.ptr==0)? $1.ptr : make_typelist2($1.ptr,$3.ptr); }
    |  patternTuple  { $$.ptr = $1.ptr; }
    | ID ASSIGN {$$.config=$0.config;} patternSpec { $$.ptr=make_sourceAssign(make_varRef($1.ptr,ASSIGN_VAR), $4.ptr); }

patternList :  {$$.ptr=0; }
    |  patternSpec {$$.config=$0.config; } patternList { $$.ptr = make_typelist2($1.ptr,$3.ptr); }
patternTuple :  patternSpec COMMA {$$.config=$0.config;} patternSpec 
                   { $$.ptr = make_sourceVector2($1.ptr,$4.ptr); }
    | patternTuple COMMA {$$.config=$0.config;} patternSpec { $$.ptr = make_sourceVector($1.ptr,$4.ptr); }

constant : ICONST { $$.ptr=$1.ptr; } 
    | SOURCE  { $$.ptr = $1.ptr; }

parseType1 :  TUPLE LP {$$.config=$0.config; } parseElem {$$.config=$0.config;} parseElemList RP { $$.ptr = make_sourceUop(POET_OP_TUPLE, make_typelist2($4.ptr,$6.ptr)); }
    | LIST LP {$$.config=$0.config; } parseElem COMMA {$$.config=$0.config;} constantOrVar RP { $$.ptr = make_sourceBop(POET_OP_LIST,$4.ptr,$7.ptr); }
    | LIST1 LP {$$.config=$0.config; } parseElem COMMA {$$.config=$0.config;} constantOrVar RP { $$.ptr = make_sourceBop(POET_OP_LIST1,$4.ptr,$7.ptr); }
    | TOKEN { $$.ptr = make_empty(); }

parseType : parseType1 {$$=$1; }
    | typeSpec1 { $$=$1; }
    | parseType TOR {$$.config=$0.config;} parseType
            { $$.ptr = make_typeTor($1.ptr,$4.ptr); }
    | parseType DOT4 {$$.ptr = make_typeUop(TYPE_LIST1, $1.ptr); }
    | parseType DOT3 {$$.ptr = make_typeUop(TYPE_LIST, $1.ptr); }

parseElem : singletype { $$=$1; }
      | varInvokeType { $$.ptr=$1.ptr; }
      | TILT typeSpec { $$.ptr = make_typeNot($2); }
      | parseType1 { $$ = $1; }
      | ID ASSIGN {$$.config=$0.config;} parseElem { $$.ptr=make_sourceAssign(make_varRef($1.ptr,ASSIGN_VAR), $4.ptr); }
      | parseElem TOR {$$.config=$0.config;} parseElem
            { $$.ptr = make_typeTor($1.ptr,$4.ptr); }
      | LP {$$.config=$0.config;} parseElemMulti RP { $$.ptr = $3.ptr; }
      
parseElemMulti : parseElem {$$.config=$0.config; } parseElemList  { $$.ptr = ($3.ptr==0)? $1.ptr : make_typelist2($1.ptr,$3.ptr); }
    |  parseElemTuple  { $$.ptr = $1.ptr; }
parseElemTuple :  parseElem COMMA {$$.config=$0.config;} parseElem
                   { $$.ptr = make_sourceVector2($1.ptr,$4.ptr); }
    | parseElemTuple COMMA {$$.config=$0.config;} parseElem { $$.ptr = make_sourceVector($1.ptr,$4.ptr); }
parseElemList : {$$.ptr=0; }
      | parseElem {$$.config=$0.config; } parseElemList  { $$.ptr = make_typelist2($1.ptr,$3.ptr); }

constantOrVar : constant {$$.ptr=$1.ptr; }
     | varRef { $$ = $1; }

codeRHS : RTAG {yaccState=YACC_CODE; $$.config=ID_DEFAULT;} code ENDCODE { yaccState=YACC_DEFAULT; set_code_def($0.ptr,$3.ptr); }
    | ENDTAG { set_code_def($0.ptr, 0); }
xformRHS : RTAG {$$.config=ID_DEFAULT; } code ENDXFORM {$$.ptr=$3.ptr; }
    | ENDXFORM {$$.ptr = 0; } 
xformAttrs : | xformAttr {$$=$0; } xformAttrs
xformAttr: PARS ASSIGN LP  xformPars RP {set_xform_params($0.ptr,$4.ptr); }
    | OUTPUT ASSIGN LP {$$.ptr=0;} outputPars RP 
    | ID ASSIGN {$$.config=CODE_VAR;} typeSpec { $$.ptr = set_local_static($1.ptr,$4.ptr,LVAR_TUNE,0,1); }
xformPars :  ID { $$.ptr=make_localPar($1.ptr,0,LVAR_XFORMPAR); } 
    | ID COLON {$$.config=CODE_VAR;} typeSpec { $$.ptr = make_localPar($1.ptr, $4.ptr,LVAR_XFORMPAR); }
    | xformPars COMMA xformPars {$$.ptr = make_sourceVector($1.ptr, $3.ptr); } 
outputPars : outputPar COMMA {$$.ptr = $0.ptr + 1; } outputPars | outputPar
outputPar : ID { set_local_static($1.ptr,make_Iconst1($0.ptr), LVAR_OUTPUT,0,1); }
    | ID COLON {$$.config=CODE_VAR;} typeSpec { set_local_static($1.ptr,make_Iconst1($0.ptr), LVAR_OUTPUT,$4.ptr,1); }
traceVars : ID { $$.ptr = make_traceVar($1.ptr,0); }
    | ID  COMMA  traceVars  
           { $$.ptr = make_traceVar($1.ptr,$3.ptr);  }

code : codeIf {$$.config=$0.config;} code { $$.ptr=make_sourceBop(POET_OP_SEQ,$1.ptr, $3.ptr);} 
    | codeIf {$$.ptr=$1.ptr; }
    | code23 { $$.ptr = $1.ptr; }
codeIf : IF LP {$$.config=$0.config;} code4 RP {$$.config=$0.config;} code1 {$$.config=$0.config;} codeIfHelp 
        { $$.ptr = make_ifElse($4.ptr, $7.ptr, $9.ptr); }
    | code1 { $$=$1; }
codeIfHelp:  { $$.ptr=make_empty(); }
     |  ELSE {$$.config=$0.config;} codeIf { $$.ptr = $3.ptr; }
code1 : FOR LP {$$.config=$0.config;} e_code4 SEMICOLON 
               {$$.config=$0.config;} e_code4 SEMICOLON 
               {$$.config=$0.config;} e_code4 RP {$$.config=$0.config;} code1 
        { $$.ptr = make_sourceQop(POET_OP_FOR, $4.ptr,$7.ptr,$10.ptr,$13.ptr); } 
     | SWITCH LP {$$.config=$0.config;} code RP LB {$$.config=$0.config;} switchHelp RB
        { $$.ptr = make_sourceBop(POET_OP_CASE, $4.ptr,$8.ptr); }
     | FOREACH LP {$$.config=$0.config;} code6 COLON 
                  {$$.config=$0.config;} patternSpec COLON  
                  {$$.config=$0.config;} code4 RP {$$.config=$0.config;} code1
        { $$.ptr = make_sourceQop(POET_OP_FOREACH, $4.ptr,$7.ptr,$10.ptr,$13.ptr); } 
     | FOREACH_R LP {$$.config=$0.config;} code6 COLON 
                    {$$.config=$0.config;} patternSpec COLON 
                    {$$.config=$0.config;} code4 RP {$$.config=$0.config;} code1
        { $$.ptr = make_sourceQop(POET_OP_FOREACHR, $4.ptr,$7.ptr,$10.ptr,$13.ptr); } 
     | LB {$$.config=$0.config; } code RB { $$.ptr = $3.ptr; }
     | LB RB { $$.ptr=make_empty(); }
     | code3 SEMICOLON { $$.ptr = make_sourceStmt($1.ptr); }

e_code4 : code4 { $$.ptr = $1.ptr; }
    | { $$.ptr = make_empty(); }

switchHelp : switchHelp CASE {$$.config=$0.config;} patternSpec COLON {$$.config=$0.config;} code
           {$$.ptr = make_sourceVector( $1.ptr, make_inputlist2($4.ptr,$7.ptr)); }
     | CASE {$$.config=$0.config;} patternSpec COLON  {$$.config=$0.config;} code  { $$.ptr = make_inputlist2($3.ptr,$6.ptr); }
     | switchHelp DEFAULT COLON {$$.config=$0.config;} code
           {$$.ptr = make_sourceVector( $1.ptr, make_inputlist2(make_any(),$5.ptr)); }

code23 : code2 { $$.ptr = $1.ptr; }
     | code3 { $$.ptr = $1.ptr; }

code2 : code2 COMMA {$$.config=$0.config;} code3 {$$.ptr = make_sourceVector($1.ptr,$4.ptr); }
     | code3 COMMA {$$.config=$0.config;} code3 { $$.ptr = make_sourceVector2($1.ptr,$4.ptr); }

code3 : 
       CONTINUE { $$.ptr=make_dummyOperator(POET_OP_CONTINUE); }
     | BREAK { $$.ptr=make_dummyOperator(POET_OP_BREAK); }
     | RETURN {$$.config=$0.config;} codeUnit { $$.ptr=make_sourceUop(POET_OP_RETURN, $3.ptr); }
     | ERROR {$$.config=$0.config;} codeUnit { $$.ptr = make_sourceUop(POET_OP_ERROR,$3.ptr); }
     | PRINT {$$.config=$0.config;} codeUnit { $$.ptr=make_sourceUop(POET_OP_PRINT,$3.ptr); }
     | code4 { $$.ptr = $1.ptr; }

code4 : code5 { $$.ptr = $1.ptr; }
     | code8 ASSIGN {$$.config=$0.config;} code4 { $$.ptr=make_sourceAssign($1.ptr, $4.ptr); }
     | code8 ARROW {$$.config=$0.config;} parseElem { $$.ptr = make_codeMatch($1.ptr, $4.ptr); }
     | code8 PLUS_ASSIGN {$$.config=$0.config;} code5
           { $$.ptr=make_sourceAssign($1.ptr, make_sourceBop(POET_OP_PLUS, $1.ptr,$4.ptr)); }
     | code8 MINUS_ASSIGN {$$.config=$0.config;} code5
           { $$.ptr=make_sourceAssign($1.ptr, make_sourceBop(POET_OP_MINUS, $1.ptr,$4.ptr)); }
     | code8 MULT_ASSIGN {$$.config=$0.config;} code5
           { $$.ptr=make_sourceAssign($1.ptr, make_sourceBop(POET_OP_MULT, $1.ptr,$4.ptr)); }
     | code8 DIVIDE_ASSIGN {$$.config=$0.config;} code5 
           { $$.ptr=make_sourceAssign($1.ptr, make_sourceBop(POET_OP_DIVIDE, $1.ptr,$4.ptr)); }
     | code8 MOD_ASSIGN {$$.config=$0.config;} code5 
           { $$.ptr=make_sourceAssign($1.ptr, make_sourceBop(POET_OP_MOD, $1.ptr,$4.ptr)); }
     | code8 DARROW {$$.config=$0.config;} parseElem { $$.ptr = make_codeMatchQ($1.ptr, $4.ptr); }
     | code8 QUESTION {$$.config=$0.config;} code6 COLON {$$.config=$0.config;} code4
        { $$.ptr = make_ifElse( $1.ptr, $4.ptr, $7.ptr); }

code5 :  
       code5 AND {$$.config=$0.config;} code5 { $$.ptr = make_sourceBop(POET_OP_AND,  $1.ptr, $4.ptr); }
     | code5 OR {$$.config=$0.config;} code5 { $$.ptr = make_sourceBop(POET_OP_OR,   $1.ptr, $4.ptr); }
     | NOT {$$.config=$0.config;} code6 { $$.ptr = make_sourceUop(POET_OP_NOT, $3.ptr); }
     | code6 LT {$$.config=$0.config;} code6 { $$.ptr = make_sourceBop(POET_OP_LT, $1.ptr, $4.ptr); }
     | code6 LE {$$.config=$0.config;} code6 { $$.ptr = make_sourceBop(POET_OP_LE, $1.ptr, $4.ptr); }
     | code6 EQ {$$.config=$0.config;} code6 { $$.ptr = make_sourceBop(POET_OP_EQ, $1.ptr, $4.ptr); }
     | code6 GT {$$.config=$0.config;} code6
           { $$.ptr = make_sourceBop(POET_OP_GT,   $1.ptr, $4.ptr); }
     | code6 GE {$$.config=$0.config;} code6
           { $$.ptr = make_sourceBop(POET_OP_GE,   $1.ptr, $4.ptr); }
     | code6 NE {$$.config=$0.config;} code6
           { $$.ptr = make_sourceBop(POET_OP_NE,   $1.ptr, $4.ptr); }
     | code6 COLON {$$.config=$0.config;} patternSpec { $$.ptr = make_sourceBop(POET_OP_ASTMATCH, $1.ptr,$4.ptr); }
     | MINUS {$$.config=$0.config;} code7 { $$.ptr = make_sourceUop(POET_OP_UMINUS,$3.ptr); } 
     | code6 { $$.ptr = $1.ptr; }

code6 : code7 { $$.ptr = $1.ptr; }
     | code6 PLUS {$$.config=$0.config;} code6
           { $$.ptr=make_sourceBop(POET_OP_PLUS, $1.ptr,$4.ptr); }
     | code6 MINUS {$$.config=$0.config;} code6
           { $$.ptr = make_sourceBop(POET_OP_MINUS,  $1.ptr,$4.ptr); }
     | code6 MULTIPLY {$$.config=$0.config;} code6
           { $$.ptr = make_sourceBop(POET_OP_MULT,  $1.ptr,$4.ptr); }
     | code6 STR_CONCAT {$$.config=$0.config;} code6
           { $$.ptr = make_sourceBop(POET_OP_CONCAT, $1.ptr,$4.ptr); }
     | code6 DIVIDE {$$.config=$0.config;} code6 
           { $$.ptr = make_sourceBop(POET_OP_DIVIDE,  $1.ptr,$4.ptr); }
     | code6 MOD {$$.config=$0.config;} code6 
           { $$.ptr = make_sourceBop(POET_OP_MOD,  $1.ptr,$4.ptr); }
     | code6 DCOLON {$$.config=$0.config;} code6 { $$.ptr = make_sourceBop(POET_OP_CONS,$1.ptr,$4.ptr); }

code7 :  code8 {$$.config=$0.config;} code7Help 
            { if ($3.ptr==0) $$.ptr=$1.ptr; else $$.ptr = make_xformList($1.ptr, $3.ptr); }

code7Help: code8 {$$.config=$0.config; } code7Help2 { $$.ptr =make_xformList($1.ptr,$3.ptr);}
      | { $$.ptr=0; }
code7Help2 : code8 {$$.config=$0.config; } code7Help2 { $$.ptr =make_xformList($1.ptr,$3.ptr);}
      | { $$.ptr=0; }

code8 : REPLACE LP {$$.config=$0.config;} code5 COMMA {$$.config=$0.config;} code5 {$$.config=$0.config;} paramRHS 
         { $$.ptr=($9.ptr==0)? make_sourceBop(POET_OP_REPLACE,$4.ptr,$7.ptr) 
                       : make_sourceTop(POET_OP_REPLACE,$4.ptr,$7.ptr,$9.ptr); } 
     | TRACE LP {$$.config=$0.config;} code5 {$$.config=$0.config;} paramRHS
         { $$.ptr=($6.ptr==0)? make_sourceUop(POET_OP_TRACE,$4.ptr)
                      : make_sourceBop(POET_OP_TRACE,$4.ptr,$6.ptr); } 
     | PERMUTE LP {$$.config=$0.config;} code5 COMMA {$$.config=$0.config;} code5 RP 
         { $$.ptr=make_sourceBop(POET_OP_PERMUTE,$4.ptr,$7.ptr); }
     | DUPLICATE LP {$$.config=$0.config;} code5 COMMA {$$.config=$0.config;} code5 COMMA {$$.config=$0.config;} code5 RP 
         { $$.ptr=make_sourceTop(POET_OP_DUPLICATE,$4.ptr,$7.ptr,$10.ptr); }
     | COPY {$$.config=$0.config;} codeUnit { $$.ptr = make_sourceUop(POET_OP_COPY, $3.ptr); }
     | ERASE LP {$$.config=$0.config;} code5 
           {$$.config=$0.config;$$.ptr=$4.ptr; } eraseRHS
         {$$.ptr = make_sourceBop(POET_OP_ERASE,$4.ptr,$6.ptr); }
     | DELAY LB {$$.config=$0.config;} code RB { $$.ptr = make_sourceUop(POET_OP_DELAY, $4.ptr); }
     | INSERT LP {$$.config=$0.config;} code5 COMMA {$$.config=$0.config;} code5 RP 
           {  $$.ptr=make_sourceBop(POET_OP_INSERT, $4.ptr,$7.ptr); }
     | DEBUG debugConfig LB {$$.config=$0.config;} code RB 
         { $$.ptr = make_sourceBop(POET_OP_DEBUG,$2.ptr,$5.ptr); }
     | APPLY LB {$$.config=$0.config;} code RB { $$.ptr = make_sourceUop(POET_OP_APPLY, $4.ptr); }
     | REBUILD {$$.config=$0.config;} codeUnit { $$.ptr = make_sourceUop(POET_OP_REBUILD,$3.ptr); } 
     | RESTORE  {$$.config=$0.config;} codeUnit  { $$.ptr = make_sourceUop(POET_OP_RESTORE,$3.ptr); }
     | SAVE  {$$.config=$0.config;} codeUnit  { $$.ptr = make_sourceUop(POET_OP_SAVE,$3.ptr); }
     | SPLIT LP {$$.config=$0.config;} code5 COMMA {$$.config=$0.config;} code5 RP 
         { $$.ptr = make_sourceBop(POET_OP_SPLIT, $4.ptr,$7.ptr); }
     | codeUnit DOT3 {$$.ptr = make_typeUop(TYPE_LIST, $1.ptr); }
     | codeUnit DOT2 {$$.config=$0.config;} codeUnit
           {$$.ptr = make_sourceBop(POET_OP_RANGE, $1.ptr,$4.ptr);}
     | RANGE LP codeUnit COMMA codeUnit RP
           {$$.ptr = make_sourceBop(POET_OP_RANGE, $3.ptr,$6.ptr);}
     | code9 { $$.ptr = $1.ptr; }
paramRHS : RP { $$.ptr = 0; } | COMMA {$$.config=$0.config;} code5 RP { $$.ptr = $3.ptr; }

eraseRHS : COMMA {$$.config=$0.config;} code5 RP  { $$.ptr = $3.ptr; }
         | RP { $$.ptr = $0.ptr; }

debugConfig : {$$.ptr = make_Iconst1(1); }
     | LBR ICONST RBR { $$.ptr = $2.ptr; }

inputCodeList : inputCode inputCodeList2 { $$.ptr = make_inputlist2($1.ptr,$2.ptr); }

inputCodeList2 :  { $$.ptr = 0; }
     | inputCode inputCodeList2 { $$.ptr = make_inputlist2($1.ptr,$2.ptr); }

inputCode :  
       INPUT_ENDL inputBase { $$.ptr = ($2.ptr==0)? $1.ptr : make_inputlist2($1.ptr,$2.ptr); } lineAnnot { $$.ptr = $4.ptr; }
     | RBEGIN LP {$$.config=CODE_OR_XFORM_VAR; } parseElem RP 
            { $$.ptr = make_annot($4.ptr); }

inputBase:  { $$.ptr = 0; }
     | ICONST inputBase { $$.ptr = make_inputlist2($1.ptr, $2.ptr); }
     | SOURCE inputBase { $$.ptr = make_inputlist2($1.ptr, $2.ptr); }
     | INPUT_CONT inputBase { $$.ptr = make_inputlist2($1.ptr, $2.ptr); }
     | ID inputBase { $$.ptr = make_inputlist2(make_varRef($1.ptr,GLOBAL_VAR), $2.ptr); }

lineAnnot : 
      ARROW {$$.config=CODE_OR_XFORM_VAR;} parseElem { $$.ptr = make_codeMatch($0.ptr, $3.ptr); } typeMatch { $$.ptr = $5.ptr; }
     | typeMatch { $$.ptr = $1.ptr; }

typeMatch : { $$.ptr = $0.ptr; }
     |  LBEGIN LP {$$.config=CODE_OR_XFORM_VAR; } parseElem RP {$$.ptr=$0.ptr; }
         typeMatch { $$.ptr = make_inputlist2(make_annot($4.ptr), $7.ptr); } 

code9 : 
       CAR {$$.config=$0.config;} codeUnit { $$.ptr = make_sourceUop(POET_OP_CAR, $3.ptr); }
     | CDR {$$.config=$0.config;} codeUnit { $$.ptr = make_sourceUop(POET_OP_CDR, $3.ptr); }
     | LEN {$$.config=$0.config;} codeUnit { $$.ptr = make_sourceUop(POET_OP_LEN, $3.ptr); }
     | INT {$$.config=$0.config;} codeUnit { $$.ptr = make_sourceUop(TYPE_INT, $3.ptr); }
     | CLEAR ID { $$.ptr = make_sourceUop(POET_OP_CLEAR, make_varRef($2.ptr,ASSIGN_VAR)); }
     | varInvoke { $$.ptr=$1.ptr; }

codeUnit: LP {$$.config=$0.config;} code RP { $$.ptr=$3.ptr; }
     | constant { $$.ptr=$1.ptr; }
     | ANY { $$.ptr = make_any(); }
     | varRef { $$=$1; }
     | map {$$=$1; }

varInvoke : codeUnit {$$.ptr=$1.ptr; $$.config=$0.config; } varInvoke1 { $$ = $3; }
     | varInvoke TOR {$$.config=$0.config; } varInvoke { $$.ptr = make_typeTor($1.ptr,$4.ptr); }
varInvoke1 : {$$=$0; }
    | LBR {$$.config=$0.config; $$.ptr=$0.ptr; } configOrSubRef RBR 
          { $$.ptr = make_tupleAccess($0.ptr, $3.ptr); $$.config=$0.config; }
      varInvoke1 { $$=$6; }
    | pond { $$=$1; }

pond: POND {$$.config=$0.config; } codeUnit 
       { $$.ptr = make_codeRef($0.ptr,$3.ptr,$0.config); $$.config=$0.config; }

varInvokeType : varRef {$$.ptr=$1.ptr; $$.config=$0.config; } varInvokeType1 { $$ = $3; }
varInvokeType1 : {$$=$0; }
    | LBR {$$.config=$0.config; $$.ptr=$0.ptr; } configRef RBR 
          { $$.ptr = make_tupleAccess($0.ptr, $3.ptr); $$.config=$0.config; }
    | POND {$$.config=$0.config; } typeSpec 
         { $$.ptr = make_codeRef($0.ptr,$3.ptr,$0.config);$$.config=$0.config; }

varInvokePattern : varRef {$$.ptr=$1.ptr; $$.config=$0.config; } varInvokePattern1 { $$ = $3; }
varInvokePattern1 : {$$=$0; }
    | LBR {$$.config=$0.config; $$.ptr=$0.ptr; } xformConfig RBR 
          { $$.ptr = make_tupleAccess($0.ptr, $3.ptr); $$.config=$0.config; }
    | POND {$$.config=$0.config; } patternSpec 
         { $$.ptr = make_codeRef($0.ptr,$3.ptr,$0.config);$$.config=$0.config; }

configOrSubRef : code5 {$$.ptr=$1.ptr;}
   |  configRef { $$.ptr = $1.ptr; } 
   | selectRef {$$.ptr = $1.ptr; }

selectRef : selectRef TOR {$$.config=$0.config;} selectRef 
                    { $$.ptr = make_typeTor($1.ptr,$4.ptr); }
   |  ID DOT ID { $$.ptr = make_attrAccess(make_varRef($1.ptr,CODE_OR_XFORM_VAR), $3.ptr); }

configRef :  
     ID ASSIGN {$$.config=$0.config;} code5 
         { $$.ptr = make_sourceAssign( make_attrAccess($0.ptr,$1.ptr),$4.ptr); }
   | configRef SEMICOLON {$$.ptr=$0.ptr; $$.config=$0.config;} configRef 
       { $$.ptr = make_sourceBop(POET_OP_SEQ,$1.ptr, $4.ptr);}

xformConfig :  
     ID ASSIGN {$$.config=$0.config;} typeSpec 
         { $$.ptr = make_sourceAssign( make_attrAccess($0.ptr,$1.ptr),$4.ptr); }
   | xformConfig SEMICOLON {$$.ptr=$0.ptr; $$.config=$0.config;} xformConfig 
       { $$.ptr = make_sourceBop(POET_OP_SEQ,$1.ptr, $4.ptr);}
%%
