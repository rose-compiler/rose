%option noyywrap
/* Note that yylineno is an undocumented feature that we shouldn't be using */
%option yylineno
%{
#include "parse_header.h"
#include "parseDotGrammar.h"
#include <cstring>
int yyerror(const char *s);
%}

inquote          \\\"

%%
\[              {return LBRK;}
\]              {return RBRK;}
\;              {return COLUMN;}
\=              {return ASSGN;}
"label"         {return LABEL;}
"digraph"       {return DIGRAPH;}
"subgraph"      {return SUBGRAPH;}
"color"         {return COLOR;}
"shape"         {return SHAPE;}
"diamond"       {return DIAMOND;}
"dir"           {return DIR;}
"none"          {return NONE;}
"both"          {return BOTH;}
"null"          {return VNULL;}
"{"             {return LBRACE;}
"}"             {return RBRACE;}
"->"            {return LARROW;}
[a-zA-Z0-9_]+   {
                  // returning the (unquoted) string
                  yylval.sVal = strdup(yytext);
                  return NAME;
                }

((\"(0x|"")[0-9a-fA-F]+\")|(\"(0x|""x)[0-9a-fA-F]+__[a-zA-Z0-9]+__null\"))   {
                      // eliminating quotes, returning ID or string
                      yylval.sVal = strdup(yytext + 1);
                      yylval.sVal[yyleng - 2] = 0;
                      return ID;
                    }

[ \t]           ;
\"([^"\n]|{inquote})*\"    {
                  // returning a string (may have inside quotes)
                  yylval.sVal = strdup(yytext + 1);
                  yylval.sVal[yyleng - 2] = 0;
                  return STRING;
                }

[\n]            { yylineno++;}
.               ;

%%
