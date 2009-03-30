/*********************************
* ARAL Scanner                   *
* Author: Markus Schordan, 2009. *
**********************************/

%{
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include <string>
#include <cstring>
#include "parser_decls.h"
#include "parser.H"

YY_BUFFER_STATE gInputBuffer;

//static char *lookup(char *);    /* lookup function for identifiers */

%}

%option yylineno
%option noyywrap
%option 8bit
%option noreject
%option never-interactive
%option outfile="lexer.C"

digit                   [0-9]
letter                  [a-zA-Z_]
hexdigit                [a-fA-F0-9]
single			[{}<>(),;]

%%
"//".*                  ; /* comment */

{single}		{  return *yytext; }
\"(digit|letter|" ")*\"	{ yylval.string = strdup(yytext+1); return STRING; }
"["			{ return '['; }
"]"			{ return ']'; }

{letter}({letter}|{digit})*  { yylval.idstring = strdup(yytext); /* = lookup(yytext); */ return IDENT; }
{digit}{digit}*              { yylval.intval = atoi(yytext); return INTEGER; }

[ \t\r]                      ; /* white space */
\n                           ; /* we are using #option yylineno */ 
.                            printf("ERROR 1: Lexical error! : <%s>\n",yytext); exit(1);

%%

void
InitializeLexer(const std::string& tokenizeString)
{
  yylineno = 1;
  const char* inputBytes = tokenizeString.c_str();
  gInputBuffer = yy_scan_bytes(inputBytes, tokenizeString.length());
}

void
FinishLexer()
{
  yy_delete_buffer(gInputBuffer);
}

#if 0
/******************************************************************/
/* hash table management                                          */
/******************************************************************/

#define PRIME 211               /* hash table size */

static struct hash_entry {
   struct hash_entry *link;     /* link to next hash entry */
   char *ident;                 /* identifier */
} *hash_table[PRIME];           /* hash table */

/* 
 * hash function
 * algorithm by P.J.Weinberg
 */

static int hashpjw(char *s)
{
  char *p;
  unsigned h=0, g;

  for(p=s;*p;p++) {
    h = (h << 4)+ (*p);
    if(g=h&0xf0000000) {
      h ^= g>>24;
      h ^= g;
    }
  }
  return h % PRIME;
}

/* 
 * look up a identifier 
 */

char *lookup(char *ident)
{
  struct hash_entry *p;
  int i = hashpjw(ident);

  /* find identifier */
  for(p=hash_table[i];p!=NULL;p=p->link) 
    if(!strcmp(p->ident,ident)) 
      return p->ident; 

  /* no entry found => create new one */
  p = (struct hash_entry *)malloc(sizeof(struct hash_entry));
  assert(p!=NULL);
  p->ident = strdup(ident);
  assert(p->ident != NULL);
  p->link = hash_table[i];
  hash_table[i] = p; 

  return p->ident;
}

#endif
