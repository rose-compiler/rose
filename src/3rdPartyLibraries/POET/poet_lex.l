letter [A-Za-z]
digit  [0-9]
id   ({letter}|_)({letter}|{digit}|_)*
integer {digit}({digit})*  

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
/*  Lex scanner for reading POET specefications                      */
/*********************************************************************/

#include <poet_config.h>
#include <poet_yacc.h>

LexState lexState;

extern void* make_sourceString();
extern void* make_Iconst();
extern void* make_Iconst1();
extern void* make_any();

#define CHECK_INPUT if (lexState&LEX_INPUT) { BEGIN(IN_INPUT); yylval.ptr=make_sourceString(yytext,yyleng); return SOURCE; }
#define CHECK_ANNOT if (lexState&LEX_NOANNOT) { yylval.ptr=make_sourceString(yytext,yyleng); return SOURCE; }
%}

%s IN_CODE_TAG IN_INPUT_TAG IN_TAG IN_TAG_BODY IN_OUTPUT_TAG IN_XFORM_TAG IN_XFORM IN_ARG IN_IARG IN_IARG1 IN_IARG_R IN_IARG1_R IN_IARG_PARSE IN_IARG1_PARSE  
%x IN_CODE IN_INPUT 
%option noyywrap
%option yylineno

%%

"<*"([^\*]|\*+[^\*\>])*\*+\> { CHECK_INPUT; }
"<<*"(.)*[\n\r] { CHECK_INPUT; }

<INITIAL>"<" { CHECK_INPUT; BEGIN(IN_TAG); return LTAG; }
<INITIAL>[\n\r \t] { CHECK_INPUT; }
<INITIAL>{id}  { if (lexState&LEX_INPUT) { BEGIN(IN_INPUT);}  yylval.ptr = make_sourceString(yytext, yyleng); return SOURCE; }
<INITIAL>{integer} { if (lexState&LEX_INPUT) { BEGIN(IN_INPUT);} yylval.ptr = make_Iconst(yytext,yyleng); return ICONST; }
<INITIAL>. {  if (lexState&LEX_INPUT) { BEGIN(IN_INPUT); } yylval.ptr = make_sourceString(yytext, yyleng); return SOURCE; }


<IN_TAG>"define" {  BEGIN(IN_TAG_BODY); return DEFINE; }
<IN_TAG>"eval" {  BEGIN(IN_TAG_BODY); return EVAL; }
<IN_TAG>"trace" {  BEGIN(IN_TAG_BODY); return TRACE; }
<IN_TAG>"parameter" { BEGIN(IN_TAG_BODY); return PARAMETER; }
<IN_TAG>"output" { BEGIN(IN_OUTPUT_TAG); return OUTPUT; }
<IN_TAG>"xform" { BEGIN(IN_XFORM_TAG); return XFORM; }
<IN_TAG>"code" { BEGIN(IN_CODE_TAG); return CODE; }
<IN_TAG>"input" { BEGIN(IN_INPUT_TAG); return INPUT; }
<IN_TAG_BODY,IN_INPUT_TAG,IN_OUTPUT_TAG,IN_CODE_TAG>"/>" { BEGIN(0); return ENDTAG; }

<IN_INPUT_TAG,IN_OUTPUT_TAG>"syntax" { return SYNTAX; }
<IN_INPUT_TAG,IN_OUTPUT_TAG>"POET" { return POETTYPE; }
<IN_INPUT_TAG,IN_OUTPUT_TAG>"to" { return TO; }
<IN_OUTPUT_TAG,IN_INPUT_TAG>"from" { return FROM; }
<IN_INPUT_TAG>"debug" { return DEBUG; }
<IN_INPUT_TAG,IN_TAG_BODY>"type" { return TYPE; }
<IN_INPUT_TAG>"annot" { return ANNOT; }
<IN_INPUT_TAG,IN_OUTPUT_TAG,IN_CODE_TAG>"cond" { return COND; }
<IN_TAG_BODY>"message" { return MESSAGE; }

<IN_CODE_TAG,IN_XFORM_TAG>"pars" { return PARS; }
<IN_CODE_TAG>"rebuild" { return REBUILD; }
<IN_CODE_TAG,IN_TAG_BODY,IN_INPUT_TAG>"parse" { return PARSE; }
<IN_CODE_TAG>"lookahead" { return LOOKAHEAD; }
<IN_TAG_BODY,IN_INPUT_TAG,IN_CODE_TAG>"token" { return TOKEN; }
<IN_CODE_TAG,IN_TAG_BODY,IN_INPUT_TAG>"match" { return MATCH; }
<IN_XFORM_TAG,IN_CODE_TAG>"output" { return OUTPUT; }


<IN_IARG,IN_IARG1>"BEGIN" { return LBEGIN; }
<IN_IARG_R,IN_IARG1_R>"BEGIN" { return RBEGIN; }
<IN_IARG>";" { BEGIN(IN_IARG_R); }
<IN_IARG1>";" { BEGIN(IN_IARG1_R); }
<IN_CODE_TAG>">"[\t ]*[\n\r]?([\t ]*@@)? { BEGIN(IN_CODE); lexState=LEX_INPUT; return RTAG; }
<IN_XFORM_TAG>">" { BEGIN(IN_XFORM); return RTAG; }
<IN_XFORM_TAG>"/>" { BEGIN(0); return ENDXFORM; }

<IN_CODE,IN_INPUT>{id}  { yylval.ptr = make_sourceString(yytext, yyleng); return SOURCE; }
<IN_CODE,IN_INPUT>{integer} {  yylval.ptr = make_Iconst(yytext,yyleng); return ICONST; }

<IN_CODE>[\n\r]?[\t ]*"</code>" { BEGIN(0); lexState=LEX_DEFAULT; return ENDCODE; }
<IN_CODE>[\n\r]([ \t]*"@@")? { yylval.ptr = make_sourceString(yytext,1); return SOURCE; }
<IN_CODE>"@"|"@\n" { BEGIN(IN_ARG); lexState=LEX_DEFAULT; }
<IN_ARG>"@" { BEGIN(IN_CODE); lexState=LEX_INPUT; }

<IN_INPUT_TAG>">"[\t ]* { BEGIN(IN_IARG1); return RTAG; }
<IN_INPUT>[\t ]*"</input>" { CHECK_ANNOT; BEGIN(0); lexState=LEX_DEFAULT; return ENDINPUT; }
<IN_INPUT>[\n\r]?[\t ]*"//@</input>" { CHECK_ANNOT; BEGIN(0); lexState=LEX_DEFAULT; return ENDINPUT; }
<IN_INPUT>([\n\r]?[\t ]*"/*@") { CHECK_ANNOT; BEGIN(IN_IARG); lexState=LEX_DEFAULT;  }
<IN_INPUT>([\n\r]?[\t ]*"//@") { CHECK_ANNOT; BEGIN(IN_IARG1); lexState=LEX_DEFAULT; }
<IN_INPUT><<EOF>> { BEGIN(0); lexState=LEX_DEFAULT; return INPUT_ENDFILE;}
<IN_INPUT>(\\\n) { yylval.ptr=make_sourceString("\\\n",2); return INPUT_CONT;}
<IN_INPUT>[\n\r] { yylval.ptr=make_sourceString("\n", 1); return INPUT_ENDL;}

<IN_CODE,IN_INPUT>. {  yylval.ptr = make_sourceString(yytext, yyleng); return SOURCE; }

<IN_IARG>"</input>" { BEGIN(0); lexState=LEX_DEFAULT; return ENDINPUT;  }
<IN_IARG,IN_IARG_R>("@") { BEGIN(IN_INPUT); lexState=LEX_INPUT; }
<IN_IARG,IN_IARG_R>("@*/")[ \t]* { BEGIN(IN_INPUT);  lexState=LEX_INPUT; }
<IN_IARG1,IN_IARG1_R>[\n\r] { BEGIN(IN_INPUT); lexState=LEX_INPUT; yylval.ptr = make_sourceString("\n", 1); return INPUT_ENDL; }

<IN_XFORM>"</xform>" { BEGIN(0); return ENDXFORM; }

<IN_XFORM_TAG>-?{integer} { yylval.ptr = make_Iconst(yytext,yyleng); return ICONST; }

<IN_CODE_TAG,IN_INPUT_TAG,IN_TAG,IN_TAG_BODY,IN_XFORM_TAG,IN_XFORM,IN_ARG,IN_IARG,IN_IARG_R,IN_IARG_PARSE,IN_OUTPUT_TAG>[\n\r]+ {}

[\t ] {}
"ERROR" { return ERROR; }
"PERMUTE" { return PERMUTE; }
"REPLACE" { return REPLACE; }
"RANGE" { return RANGE; }
"INSERT" { return INSERT; }
"MAP" { return MAP; }
"XFORM" { return XFORM; }
"CODE" { return CODE; }
"GLOBAL" { return GLOBAL; }
"REBUILD" { return REBUILD; }
"DELAY" { return DELAY; }
"APPLY" { return APPLY; }
"PRINT" { return PRINT; }
"SAVE" { return SAVE; }
"RESTORE" { return RESTORE; }
"VAR"  { return VAR; }
"TRACE" { return TRACE; }
"ERASE" { return ERASE; }
"COPY" { return COPY; }
"CLEAR" { return CLEAR; }
"DUPLICATE" { return DUPLICATE; }
"SPLIT" { return SPLIT; }
"LEN"  { return LEN; }
"INT" { return INT; }
"STRING" { return STRING; }
"ID" { return NAME; }
"EXP" { return EXP; }
"TUPLE" { return TUPLE; }
"LIST" { return LIST; }
"LIST1" { return LIST1; }
"ENDL" { yylval.ptr = make_sourceString("\n",1); return SOURCE; }
"case" { return CASE; }
"default" { return DEFAULT; }
"switch" { return SWITCH; }
"BREAK" { return BREAK; }
"CONTINUE" { return CONTINUE; }
"RETURN" { return RETURN; }
"DEBUG" { return DEBUG; }
"INHERIT" { return INHERIT; }
"for" { return FOR; }
"foreach" { return FOREACH; }
"foreach_r" { return FOREACH_R; }
"if" { return IF; }
"else" { return ELSE; }
"car" { return CAR; }
"cdr" { return CDR; }
"HEAD" { return CAR; }
"TAIL" { return CDR; }
"TRUE" { yylval.ptr = make_Iconst1(1); return ICONST; }
"FALSE" { yylval.ptr = make_Iconst1(0); return ICONST; }
"_"  { yylval.ptr = make_any(); return ANY; }
{id} { yylval.ptr = make_sourceString(yytext, yyleng); return ID; }
\"(\\\")?([^\"]|[^\\]\\\")*\"  { yylval.ptr = make_sourceString(yytext+1,yyleng-2); return SOURCE; }
{integer} { yylval.ptr = make_Iconst(yytext,yyleng); return ICONST; }
"#" { return POND; }
"{" { return LB; }
"}" { return RB; }
"(" { return LP; }
"[" { return LBR; }
")" { return RP; }
"]" { return RBR; }
"&&" { return AND; }
"||" { return OR; }
"|" { return TOR; }
"!" { return NOT; }
">=" { return GE; }
"?"  { return QUESTION; }
">" { return GT; }
"<=" { return LE; }
"<" { return LT; }
"==>" { return DARROW; }
"=>" { return ARROW; }
"==" { return EQ; }
"=" { return ASSIGN; }
"!=" { return NE; }
"," { return COMMA; }
";" { return SEMICOLON; }
"+=" { return PLUS_ASSIGN; }
"-=" { return MINUS_ASSIGN; }
"*=" { return MULT_ASSIGN; }
"%=" { return MOD_ASSIGN; }
"/=" { return DIVIDE_ASSIGN; }
"^"  { return STR_CONCAT; }
"+" { return PLUS; }
"-" { return MINUS; }
"*" { return MULTIPLY; }
"%" { return MOD; }
"/" { return DIVIDE; }
"::" { return DCOLON; }
":" { return COLON; }
"..." { return DOT3; }
"...." { return DOT4; }
".." { return DOT2; }
"." { return DOT; }
"~" { return TILT; }

. { return SYNERROR; }
