/*********************************
* MATCHER Scanner                *
* Author: Markus Schordan, 2012. *
**********************************/

%{
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include <string>
#include <cstring>
#include "matcherparser_decls.h"
#include "matcherparser.h"

%}

%option yylineno
%option noyywrap
%option 8bit
%option noreject
%option never-interactive
/* make ylwrap happy */
%option outfile="lex.yy.c"

digit                   [0-9]
letter                  [a-zA-Z_]
single			[(),|#_^=.]

%%
"'"[^']"'"		{
						int purelen=strlen(yytext)-2;
						char* purestring=(char*)malloc(purelen+1);
						strncpy(purestring,yytext+1,purelen)[purelen]=0;
						matcherlval.sq_string = purestring; 
						return SQ_STRING; 
				}
"//".*                  ; /* comment */
".."			{ return DOTDOT; }
"null"			{ return NULL_NODE; }
"where"			{ return WHERE; }
"and"			{ return AND; }
"or"			{ return OR; }
"xor"			{ return XOR; }
"not"			{ return NOT; }
"eq"			{ return EQ; }
"neq"			{ return NEQ; }
"=="			{ return C_EQ; }
"!="			{ return C_NEQ; }
"true"			{ return TRUE; }
"false"			{ return FALSE; }

{single}		{  return *matchertext; }

"$"{letter}({letter}|{digit})*  { matcherlval.varstring = strdup(matchertext); /* = lookup(matchertext); */ return VARIABLE; }
{letter}({letter}|{digit})*  { matcherlval.idstring = strdup(matchertext); /* = lookup(matchertext); */ return IDENT; }

[ \t\r]                      ; /* white space */
\n                           ; /* we are using #option yylineno */ 
.                            printf("ERROR 1: Lexical error! : <%s>\n",matchertext); exit(1);

%%

YY_BUFFER_STATE gInputBuffer;

void
InitializeLexer(const std::string& tokenizeString)
{
  //yylineno = 1;
  const char* inputBytes = tokenizeString.c_str();
  gInputBuffer = yy_scan_bytes(inputBytes, tokenizeString.length());
}

void
FinishLexer()
{
  yy_delete_buffer(gInputBuffer);
}

/*
\"[^"\n]**["\n]	{ int slen=strlen(matchertext+1); char* snew=(char*)malloc(slen-1+1);strncpy(snew,matchertext+1,slen-1);snew[slen-1]=0; matcherlval.string = snew; return STRING; }
{digit}{digit}*              { matcherlval.intval = atoi(matchertext); return INTEGER; }
*/
