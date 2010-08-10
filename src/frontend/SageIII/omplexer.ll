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

/* Moved from Makefile.am to the source file to work with --with-pch 
Liao 12/10/2009 */
#define YY_NO_TOP_STATE
#define YY_NO_POP_STATE

static const char* ompparserinput = NULL;
static std::string gExpressionString;

/* Liao 6/11/2010,
OpenMP does not preclude the use of clause names as regular variable names.
For example, num_threads could be a clause name or a variable in the variable list.

We introduce a flag to indicate the context: within a variable list like (a,y,y) or outside of it
  We check '(' or ')' to set it to true or false as parsing proceed */
extern bool b_within_variable_list ; /* = false; */

/*conditionally return either a clause token or ID_EXPRESSION, depending on the context.
  We should use it for any OpenMP keyword which could potentially used by users as a variable within a variable list.*/
static int cond_return (int input);
/* pass user specified string to buf, indicate the size using 'result', 
   and shift the current position pointer of user input afterwards 
   to prepare next round of token recognition!!
*/
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
omp             { return cond_return ( OMP); }
parallel        { return cond_return ( PARALLEL); }
task		{ return cond_return ( TASK ); }
taskwait	{ return cond_return ( TASKWAIT ); }
untied          { return cond_return ( UNTIED );}
if		{ return ( IF); } /*if is a keyword in C/C++, no change to be a variable*/
num_threads     { /*Can be either a clause name or a variable name */ 
                  return cond_return (NUM_THREADS);
                  /*
                  if (b_within_variable_list)
                  {
                    omp_lval.stype = strdup(yytext);
                     return ID_EXPRESSION;
                  }
                  else 
                    return ( NUM_THREADS); 
                    */
                } 
ordered         { return cond_return ( ORDERED  ); }
schedule        { return cond_return ( SCHEDULE ); }

static          { return ( STATIC ); }  /*keyword in C/C++ */
dynamic         { return cond_return ( DYNAMIC ); } 
guided          { return cond_return ( GUIDED ); }
runtime         { return cond_return ( RUNTIME ); }
auto            { return ( AUTO ); } /*keyword in C/C++ ?*/

sections        { return cond_return  ( SECTIONS ); }
section         { return cond_return ( SECTION ); }
single          { return cond_return ( SINGLE ); }
nowait          { return cond_return ( NOWAIT); }
for             { return ( FOR ); } /*keyword in C/C++ */
collapse	{ return cond_return ( COLLAPSE ); }
master          { return cond_return ( MASTER ); }
critical        { return cond_return ( CRITICAL ); }
barrier         { return cond_return ( BARRIER ); }
atomic          { return cond_return ( ATOMIC ); }
flush           { return cond_return ( FLUSH ); }
threadprivate   { return cond_return ( THREADPRIVATE ); }
private         { return cond_return ( PRIVATE ); }
copyprivate     { return cond_return ( COPYPRIVATE ); }
firstprivate    { return cond_return ( FIRSTPRIVATE ); }
lastprivate     { return cond_return ( LASTPRIVATE ); }
default         { return cond_return ( DEFAULT ); }
shared          { return cond_return ( SHARED ); } 
none            { return cond_return ( NONE ); } 
reduction       { return cond_return ( REDUCTION ); }
copyin          { return cond_return ( COPYIN ); }

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
"\\"          { /*printf("found a backslash\n"); This does not work properly but can be ignored*/}

{newline}       { /* printf("found a new line\n"); */ /* return (NEWLINE); We ignore NEWLINE since we only care about the pragma string , We relax the syntax check by allowing it as part of line continuation */ }

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


/* yy_push_state can't be called outside of this file, provide a wrapper */
extern void omp_parse_expr() {
	yy_push_state(EXPR);
}

/* entry point invoked by callers to start scanning for a string */
extern void omp_lexer_init(const char* str) {
  ompparserinput = str;
  /* We have omp_ suffix for all flex functions */
  omp_restart(omp_in);
}

static int cond_return (int input)
{
  if (b_within_variable_list)
  {
    omp_lval.stype = strdup(yytext);
    return ID_EXPRESSION;
  }
  else
    return ( input); 
}
/**
 * @file
 * Lexer for OpenMP-pragmas.
 */

