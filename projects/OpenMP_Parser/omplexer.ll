%option prefix="omp_"
%option outfile="lex.yy.c"
%option stack
%x EXPR

%{

/* lex requires me to use extern "C" here */
extern "C" int omp_wrap() { return 1; }

extern int omp_lex();

#include <stdio.h>
#include <string>
#include <string.h>
#include "ompparser.h"

static const char* ompparserinput = NULL;
static std::string gExpressionString;

#define YY_INPUT(buf, result, max_size) { \
		if (*ompparserinput == '\0') result = 0; \
		else { strncpy(buf, ompparserinput, max_size); \
			buf[max_size] = 0; \
			result = strlen(buf); \
			ompparserinput += result; \
		} \
		}

%}

blank		[ ]
newline         [\n]
digit           [0-9]

id              [a-zA-Z_][a-zA-Z0-9_]*

%%
{digit}{digit}* { omp_lval.itype = atoi(strdup(yytext)); return (ICONSTANT); }
omp             { return ( OMP); }
parallel        { return ( PARALLEL); }
task		{ return ( TASK ); }
taskwait	{ return ( TASKWAIT ); }
untied          { return ( UNTIED );}
if		{ return ( IF); }
num_threads     { return ( NUM_THREADS); }
ordered         { return ( ORDERED  ); }
schedule        { return ( SCHEDULE ); }
static          { return ( STATIC ); }
dynamic         { return ( DYNAMIC ); }
guided          { return ( GUIDED ); }
runtime         { return ( RUNTIME ); }
auto            { return ( AUTO ); }
sections        { return ( SECTIONS ); }
section         { return ( SECTION ); }
single          { return ( SINGLE ); }
nowait          { return ( NOWAIT); }
for             { return ( FOR ); }
collapse	{ return ( COLLAPSE ); }
master          { return ( MASTER ); }
critical        { return ( CRITICAL ); }
barrier         { return ( BARRIER ); }
atomic          { return ( ATOMIC ); }
flush           { return ( FLUSH ); }
threadprivate   { return ( THREADPRIVATE ); }
private         { return ( PRIVATE ); }
copyprivate     { return ( COPYPRIVATE ); }
firstprivate    { return ( FIRSTPRIVATE ); }
lastprivate     { return ( LASTPRIVATE ); }
shared          { return ( SHARED ); }
default         { return ( DEFAULT ); }
none            { return ( NONE ); }
reduction       { return ( REDUCTION ); }
copyin          { return ( COPYIN ); }

"="             { return('='); }
"("		{ return ('('); }
")"		{ return (')'); }
","		{ return (','); }
":"		{ return (':'); }
"+"		{ return ('+'); }
"*"		{ return ('*'); }
"-"		{ return ('-'); }
"&"		{ return ('&'); }
"^"		{ return ('^'); }
"|"		{ return ('|'); }
"&&"		{ return (LOGAND); }
"||"		{ return (LOGOR); }

">>="            {return(RIGHT_ASSIGN2); }
"<<="            {return(LEFT_ASSIGN2); }
"+="             {return(ADD_ASSIGN2); }
"-="             {return(SUB_ASSIGN2); }
"*="             {return(MUL_ASSIGN2); }
"/="             {return(DIV_ASSIGN2); }
"%="             {return(MOD_ASSIGN2); }
"&="             {return(AND_ASSIGN2); }
"^="             {return(XOR_ASSIGN2); }
"|="             {return(OR_ASSIGN2); }

"<"		{ return ('<'); }
">"		{ return ('>'); }
"<="		{ return (LE_OP2);}
">="		{ return (GE_OP2);}
"=="		{ return (EQ_OP2);}
"!="		{ return (NE_OP2);}

{newline}       { return (NEWLINE); }

<EXPR>.         { int c = yytext[0];
		  int parenCount = 1;
		  for (;;) {
			if (c == EOF)
				return LEXICALERROR;
			if (c == ')')
				--parenCount;
			if (parenCount == 0) {
				unput(')');
				omp_lval.stype =strdup(gExpressionString.c_str()); 
				gExpressionString = "";
				BEGIN(INITIAL);
				return EXPRESSION;
			}
			gExpressionString += c;
			if (c == '(')
				parenCount++;
		  	c = yyinput();
		  }
			
		}

expr		{ return (EXPRESSION); }
identifier      { return (IDENTIFIER); /*not in use for now*/ }
{id}		{ omp_lval.stype = strdup(yytext); 
                  return (ID_EXPRESSION); }

{blank}*	;
.		{ return (LEXICALERROR);}

%%


// yy_push_state can't be called outside of this file, provide a wrapper
extern void omp_parse_expr() {
	yy_push_state(EXPR);
}

extern void omp_lexer_init(const char* str) {
  ompparserinput = str;
  omp_restart(omp_in);
}

/**
 * @file
 * Lexer for OpenMP-pragmas.
 */

