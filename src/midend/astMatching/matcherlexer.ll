/*********************************
* MATCHER Scanner                *
* Author: Markus Schordan, 2012. *
**********************************/

%{
#include "sage3basic.h"

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
%option never-interactive
/* make ylwrap happy */
%option outfile="lex.yy.c"

digit           [0-9]
letter          [a-zA-Z_]

%%
".."            { return DOTDOT; }
"null"            { return NULL_NODE; }
"where"            { return WHERE; }
"and"            { return AND; }
"or"            { return OR; }
"xor"            { return XOR; }
"not"            { return NOT; }
"eq"            { return EQ; }
"neq"            { return NEQ; }
"=="            { return C_EQ; }
"!="            { return C_NEQ; }
"true"            { return TRUE; }
"false"            { return FALSE; }
"|"                { return ALTERNATION; }
"("                { return '('; }
")"                { return ')'; }
","                { return ','; }
"#"                { return '#'; }
"_"                { return '_'; }
"^"                { return '^'; }
"="                { return '='; }
"$"{letter}({letter}|{digit})*  { matcherparserlval.varstring = strdup(matcherparsertext); /* = lookup(matcherparsertext); */ return VARIABLE; }
{letter}({letter}|{digit})*  { matcherparserlval.idstring = strdup(matcherparsertext); /* = lookup(matcherparsertext); */ return IDENT; }
"'"[^']"'"        {
                        int purelen=strlen(matcherparsertext)-2;
                        char* purestring=(char*)malloc(purelen+1);
                        strncpy(purestring,matcherparsertext+1,purelen)[purelen]=0;
                        matcherparserlval.sq_string = purestring; 
                        return SQ_STRING; 
                }
[ \t\r]         ; /*white space*/
\n              ; /* we are using #option yylineno*/

.               { printf("ERROR 1: Lexical error! : <%s>\n",matcherparsertext); exit(1);}

%%

YY_BUFFER_STATE gInputBuffer;

void
InitializeLexer(const std::string& tokenizeString)
{
    gInputBuffer=yy_scan_string(tokenizeString.c_str());
}

void
FinishLexer()
{
    yy_delete_buffer(gInputBuffer);
}

/*
\"[^"\n]**["\n]    { int slen=strlen(matcherparsertext+1); char* snew=(char*)malloc(slen-1+1);strncpy(snew,matcherparsertext+1,slen-1);snew[slen-1]=0; matcherparserlval.string = snew; return STRING; }
{digit}{digit}*              { matcherparserlval.intval = atoi(matcherparsertext); return INTEGER; }
*/
