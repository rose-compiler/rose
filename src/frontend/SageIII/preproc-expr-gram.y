//
//(06/11/07: Dan Quinlan and Ramakrishna Upadrasta
//Evaluator for the grammar of the expressions that occur in 
//preprocessing directives.
//

/******************************************************************
After going though some docs, we found out that
that the grammar for the expresions of the #if's that come
in C/C++ are a subset of the ones that occur in the C/C++ language.

This is brazenly-copied from Appendix A5 of Stroustrup. 
pages (798-802 of Stroustrup's book)

Also the relevant material is 
Appendix A.11 of Stroustrup's book and the chapter "Preprocessing 
Directives" (beginning from pages 315) of the standard.

1. This grammar is a strict-subset of the grammar of general 
   C++ expressions.  The start-symbol for this perprocessor-grammar 
   is "constant-expression", which in this context means
   "ones that are definitely found to be constants at preprocessor time"
   tc. There 
   however is a difference in that many items like 
       new-expression, 
       elements of pm-expression
       array indexing expressions
       etc.
   are missing from this.

3. Every sub-expression of this grammar evaluates to an int (no 
   float/string literals allowed).
4. Yes, we agree that this is much like a toy-calculator used in 
   introductory compiler classes. That is why we dared to build it!

Yet to be done: 
   proper extension of the sizeof operator
******************************************************************/

%{
#include <iostream>
#include <string>
//#include "rose_c_cxx_operators.h"

extern int yylex();
int yyerror(char *s);
int get_size_of(int);

#define DEBUG_EXPRESSION_PARSER 0
//Set YYDEBUG to 1 if you want to trace
#if (DEBUG_EXPRESSION_PARSER == 1)
#define YYDEBUG 1
#endif


%}
//The declarations
%error-verbose

//%debug


%token SIZEOF
%token INT_LIT

%token C_CXX_AND
%token C_CXX_ANDAND
%token C_CXX_ASSIGN
%token C_CXX_ANDASSIGN
%token C_CXX_OR
%token C_CXX_ORASSIGN
%token C_CXX_XOR
%token C_CXX_XORASSIGN
%token C_CXX_COMMA
%token C_CXX_COLON
%token C_CXX_DIVIDE
%token C_CXX_DIVIDEASSIGN
%token C_CXX_DOT
//%token C_CXX_DOTSTAR
//%token C_CXX_ELLIPSIS
%token C_CXX_EQUAL
%token C_CXX_GREATER
%token C_CXX_GREATEREQUAL
%token C_CXX_LEFTBRACE
%token C_CXX_LESS
%token C_CXX_LESSEQUAL
%token C_CXX_LEFTPAREN
%token C_CXX_LEFTBRACKET
%token C_CXX_MINUS
%token C_CXX_MINUSASSIGN
%token C_CXX_MINUSMINUS
%token C_CXX_PERCENT
%token C_CXX_PERCENTASSIGN
%token C_CXX_NOT
%token C_CXX_NOTEQUAL
%token C_CXX_OROR
%token C_CXX_PLUS
%token C_CXX_PLUSASSIGN
%token C_CXX_PLUSPLUS
//%token C_CXX_ARROW
//%token C_CXX_ARROWSTAR
%token C_CXX_QUESTION_MARK
%token C_CXX_RIGHTBRACE
%token C_CXX_RIGHTPAREN
%token C_CXX_RIGHTBRACKET
%token C_CXX_COLON_COLON
%token C_CXX_SEMICOLON
%token C_CXX_SHIFTLEFT
%token C_CXX_SHIFTLEFTASSIGN
%token C_CXX_SHIFTRIGHT
%token C_CXX_SHIFTRIGHTASSIGN
%token C_CXX_STAR
%token C_CXX_COMPL
%token C_CXX_STARASSIGN

//
//The following are not needed for now (because of various reasons).
//

//%token C_CXX_POUND_POUND
//%token C_CXX_POUND
/*
%token C_CXX_AND_ALT
%token C_CXX_ANDASSIGN_ALT
%token C_CXX_OR_ALT
%token C_CXX_ORASSIGN_ALT
%token C_CXX_XOR_ALT
%token C_CXX_XORASSIGN_ALT
%token C_CXX_LEFTBRACE_ALT
%token C_CXX_LEFTBRACKET_ALT
%token C_CXX_NOT_ALT
%token C_CXX_NOTEQUAL_ALT
%token C_CXX_RIGHTBRACE_ALT
%token C_CXX_RIGHTBRACKET_ALT
%token C_CXX_COMPL_ALT
%token C_CXX_POUND_POUND_ALT
%token C_CXX_POUND_ALT
%token C_CXX_OR_TRIGRAPH
%token C_CXX_XOR_TRIGRAPH
%token C_CXX_LEFTBRACE_TRIGRAPH
%token C_CXX_LEFTBRACKET_TRIGRAPH
%token C_CXX_RIGHTBRACE_TRIGRAPH
%token C_CXX_RIGHTBRACKET_TRIGRAPH
%token C_CXX_COMPL_TRIGRAPH
%token C_CXX_POUND_POUND_TRIGRAPH
%token C_CXX_POUND_TRIGRAPH
*/

%start constant_expression

%%
//The grammar

literal:
       INT_LIT {$$ = $1;}
       /*
       | CHAR_LIT
       | BOOL_LIT
       */
       ;

//Some hacking has been applied in the rules here
primary_expression:
                  literal
                  //| C_CXX_LEFTPAREN primary_expression C_CXX_RIGHTPAREN
                  | C_CXX_LEFTPAREN expression C_CXX_RIGHTPAREN {$$=$2;}
                  ;

postfix_expression:
                  primary_expression
                  ;

unary_expression:
                postfix_expression
                | SIZEOF unary_expression {$$=get_size_of($2);}
                //This needs fixing at a later stage
                //| SIZEOF L_PAR type_id R_PAR
                ;

cast_expression: 
               unary_expression
               ;

pm_expession:
            cast_expression
            ;

multiplicative_expression:
                         pm_expession
                         | multiplicative_expression C_CXX_STAR pm_expession {$$=$1*$3;}
                         | multiplicative_expression C_CXX_DIVIDE pm_expession  {$$=$1/$3;}
                         | multiplicative_expression C_CXX_PERCENT pm_expession  {$$=$1%$3;}
                         ;
additive_expression:
                   multiplicative_expression
                   | additive_expression C_CXX_PLUS multiplicative_expression  {$$=$1+$3;}
                   | additive_expression C_CXX_MINUS multiplicative_expression {$$=$1-$3;} 
                   ;
shift_expression:
                additive_expression 
                | shift_expression C_CXX_SHIFTLEFT additive_expression  {$$=$1<<$3;}
                | shift_expression C_CXX_SHIFTRIGHT additive_expression {$$=$1>>$3;}
                ;

relational_expression:
                     shift_expression
                     | relational_expression C_CXX_LESS shift_expression         {$$=(($1<$3)?1:0);}
                     | relational_expression C_CXX_GREATER shift_expression      {$$=(($1>$3)?1:0);}
                     | relational_expression C_CXX_LESSEQUAL shift_expression    {$$=(($1<=$3)?1:0);}
                     | relational_expression C_CXX_GREATEREQUAL shift_expression {$$=(($1>=$3)?1:0);}
                     ;

equality_expression:
                   relational_expression
                   | equality_expression C_CXX_EQUAL relational_expression    {$$=(($1==$3)?1:0);}
                   | equality_expression C_CXX_NOTEQUAL relational_expression {$$=(($1!=$3)?1:0);}
                   ;
and_expression:
              equality_expression
              | and_expression C_CXX_AND equality_expression {$$=($1&$3);}
              ;

exclusive_or_expression:
                       and_expression
                       | exclusive_or_expression C_CXX_XOR and_expression {$$=($1^$3);}
                       ;

inclusive_or_expression:
                       exclusive_or_expression
                       | inclusive_or_expression C_CXX_OR and_expression {$$=($1|$3);}
                       ;

//Check if the operators are right again???
logical_and_expression: 
                      inclusive_or_expression
                      | logical_and_expression C_CXX_ANDAND inclusive_or_expression {$$=($1&&$3);}
                      ;

logical_or_expression: 
                     logical_and_expression
                     | logical_or_expression C_CXX_OROR logical_and_expression {$$=($1||$3);}
                     ;

conditional_expression:
                      logical_or_expression
                      | logical_or_expression C_CXX_QUESTION_MARK expression C_CXX_COLON assignment_expression {$$=($1)?$3:$5;}
                      ;

assignment_expression:
                     conditional_expression
                     | logical_or_expression assignment_operator assignment_expression
                     {
                     //???
                     //The code here would assign values to macro-variables
                     //since we have no variable support right now, we can leave the assignment for now
                     }
                     ;

assignment_operator:
                     C_CXX_ASSIGN
                     | C_CXX_ANDASSIGN
                     | C_CXX_ORASSIGN
                     | C_CXX_XORASSIGN
                     | C_CXX_DIVIDEASSIGN
                     | C_CXX_MINUSASSIGN
                     | C_CXX_PERCENTASSIGN
                     | C_CXX_PLUSASSIGN
                     | C_CXX_SHIFTLEFTASSIGN
                     | C_CXX_SHIFTRIGHTASSIGN
                     | C_CXX_STARASSIGN
                     ;
expression:
          assignment_expression
          ;

//This is the start expression
constant_expression:
                   conditional_expression
                   ;
%%
//Other stuff here

int yyerror(char *s)
{ 
    printf("%s\n", s);
    return 0;
}

int get_size_of(int)
{ 
    //Fix this????
    return 0;
}


int ctr = 0;
int yylex()
{ 
//std::string input_lexemes_stream = "0 + 1";

#if 0
int max_ctr = 3;
int tok_stream[] =         {INT_LIT, PLUS, INT_LIT, EOF }; 
std::string lex_stream[] = { "1", "+", "0", "0"};
#endif


int max_ctr = 5;
int tok_stream[] =         {C_CXX_LEFTPAREN, INT_LIT, C_CXX_PLUS, INT_LIT, C_CXX_RIGHTPAREN, EOF }; 
std::string lex_stream[] = {"(", "1", "+", "0", ")", "0"};

#if 0
int max_ctr = 1;
int tok_stream[] =         {INT_LIT, EOF }; 
std::string lex_stream[] = {"1", "0"};
#endif

static int i = 0;
for(; i < max_ctr;)
{
   std::cout<<"returning \""<<lex_stream[i].c_str() 
   << "\" (whose tok_stream val is " <<tok_stream[i]<<") from yylex\n"<<std::endl;
   return tok_stream[i++];
}
return EOF;
}

int main()
{
#if (DEBUG_EXPRESSION_PARSER == 1)
yydebug = 1;
#endif
printf("Hello Ram\n");
yyparse();
return 1;
}
