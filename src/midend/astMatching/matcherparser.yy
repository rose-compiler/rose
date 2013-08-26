/***************************************************************
 * MATCHER Parser                                              *
 * Author: Markus Schordan, 2012.                              *
 ***************************************************************/

%{

#include <stdio.h>
#include <stdlib.h>
#include <list>

#include "matcherparser_decls.h"

char *input_filename = NULL;
char tempfilename[30];
char *output_filename = tempfilename;
MatchOperationList* matchOperationsSequence;

void MSG(char* s)
{
#ifdef TEST 
    printf("%s",s);
#endif
}

#ifndef NDEBUG
#ifndef YYDEBUG
#define YYDEBUG 1
#endif
#endif

%}

%union {
char* idstring; /* IDENT */
char* varstring; /* VARIABLE */
char* sq_string; /* single quoted STRING */
MatchOperationListList* matchoperationlistlist; /* list of (MatchOperation or subclasses) */
MatchOperation* matchoperation; /* MatchOperation or subclasses */
MatchOperationList* matchoperationlist; /* list of (MatchOperation or subclasses) */
long intval; /* INTEGER */
int length; /* #elements */
}

%left ALTERNATION
%right NOT
%left AND
%left OR XOR
%left C_EQ C_NEQ EQ NEQ

%token WHERE TRUE FALSE
%token '^'
%token ';' ',' 
%token '{' '}' '[' ']' '(' ')'
%token <idstring> IDENT 
%token <varstring> VARIABLE 
%token <intval> INTEGER
%token <sq_string> SQ_STRING
%token NULL_NODE
%token DOTDOT

%type <matchoperationlist> match_expression simple_expression term ident_or_any
%type <matchoperationlistlist> expression_list
%type <matchoperation> where_expression where_expression_operand
%start start

%%

start        : match_expression opt_where_clause
        {
            matchOperationsSequence=$1;
        }
        ;

opt_where_clause: /* eps */
        | where_clause
        ;

where_clause: WHERE where_expression
          ;

where_expression:
        where_expression_operand C_EQ where_expression_operand
        { $$=new MatchOpBinaryOp((int)C_EQ,$1,$3); }
        | where_expression_operand C_NEQ where_expression_operand 
        { $$=new MatchOpBinaryOp((int)C_NEQ,$1,$3); }
        | where_expression_operand EQ where_expression_operand
        { $$=new MatchOpBinaryOp((int)EQ,$1,$3); }
        | where_expression_operand NEQ where_expression_operand
        { $$=new MatchOpBinaryOp((int)NEQ,$1,$3); }
        | where_expression_operand AND where_expression_operand
        { $$=new MatchOpBinaryOp((int)AND,$1,$3); }
        | where_expression_operand OR where_expression_operand
        { $$=new MatchOpBinaryOp((int)OR,$1,$3); }
        | where_expression_operand XOR where_expression_operand
        { $$=new MatchOpBinaryOp((int)XOR,$1,$3); }
        | NOT where_expression_operand
        { $$=new MatchOpUnaryOp((int)NOT,$2); }
        | '(' where_expression ')'
        { $$=$2; }
        | TRUE
        { $$=new MatchOpConstant((int)TRUE); }
        | FALSE
        { $$=new MatchOpConstant((int)FALSE); }
        ;

where_expression_operand:
        NULL_NODE
        { $$=new MatchOpConstant((int)NULL_NODE); }
        | VARIABLE
        { $$=new MatchOpAccessVariable($1); }
        | VARIABLE '.' IDENT
        { $$=new MatchOpAccessRoseAstAttribute($1,$3); }
        | VARIABLE '[' IDENT ']'
        { $$=new MatchOpAccessUserAstAttribute($1,$3); }
        | where_expression
        { $$=$1; }
        ;

match_expression: term 
        {
            $$=$1;
        }        | match_expression ALTERNATION match_expression
        { 
            $$=new MatchOperationList();
            $$->push_back(new MatchOpOr($1,$3));
        }
        | VARIABLE '=' IDENT
        {
            $$=new MatchOperationList();
            $$->push_front(new MatchOpVariableAssignment($1));
            $$->push_back(new MatchOpCheckNode($3));
            $$->push_back(new MatchOpSkipChildOnForward());
        }
        | VARIABLE '=' term
        {
            $$=$3;
            $$->push_front(new MatchOpVariableAssignment($1));
        }
        | simple_expression
        {
            $$=$1;
        }
        | '#' simple_expression
        {
            $$=$2;
            $$->push_front(new MatchOpMarkNode());
        }
        | NULL_NODE /* for this node the '#' operator cannot be applied (it could make sense too?)*/
        {
            $$=new MatchOperationList();
            $$->push_back(new MatchOpCheckNull());
            $$->push_back(new MatchOpForward());
        }
        | '(' match_expression ')'
        {
                $$=$2;
        }
        ;

simple_expression:
        VARIABLE
        {
            $$=new MatchOperationList();
            $$->push_back(new MatchOpVariableAssignment($1));
            $$->push_back(new MatchOpSkipChildOnForward());
        }
        | IDENT
        {
            $$=new MatchOperationList();
            $$->push_back(new MatchOpCheckNode($1));
            $$->push_back(new MatchOpSkipChildOnForward());
        }
        | '^' IDENT
        {
            $$=new MatchOperationList();
            $$->push_back(new MatchOpCheckNodeSet($2));
        }
        | '_'
        {
            $$=new MatchOperationList();
            $$->push_back(new MatchOpSkipChildOnForward());
        }
        | DOTDOT
        {
            $$=new MatchOperationList();
            $$->push_back(new MatchOpDotDot());
        }
        ;

ident_or_any    : IDENT
        {
            $$=new MatchOperationList();
            $$->push_back(new MatchOpCheckNode($1));
        }
        | '^' IDENT
        {
            $$=new MatchOperationList();
            $$->push_back(new MatchOpCheckNodeSet($2));
        }
        | '_'
        {
            $$=new MatchOperationList();
        }
        ;

term        : ident_or_any '(' expression_list ')'
        {
            size_t arity=$3->size();
            size_t minarity=arity;
            size_t maxarity=arity;
            $$=$1;

            // search for MatchOpDotDot
            size_t dotdot_pos=0;
            for(MatchOperationListList::iterator i=$3->begin();i!=$3->end();++i) {
              for(MatchOperationList::iterator j=(*i)->begin();j!=(*i)->end();++j) {
                if(dynamic_cast<MatchOpDotDot*>(*j)) {
                  minarity=dotdot_pos; // 
                  maxarity=(size_t)-1; // gives max value                       // error checking
                  if(arity>dotdot_pos+1) {
                    throw "Error: Matcher: no arguments allowed after operator '..'\n";
                  }
                            }
              }
              dotdot_pos++;
            }
            $$->push_back(new MatchOpArityCheck(minarity,maxarity));
            $$->push_back(new MatchOpForward());
            for(MatchOperationListList::iterator i=$3->begin(); i!=$3->end(); ++i) 
            {
               // append list destructively: using splice moves elements efficiently
               $$->splice($$->end(),**i);
               delete *i; // delete aux-list
            }
            delete $3; // delete list
        }
        ;

expression_list    : match_expression
        {
            $$=new MatchOperationListList();
            $$->push_back($1);
        }
        | expression_list ',' match_expression
        {
            
            $$=$1;
            $$->push_back($3);    
        }
        ; 

%%

void InitializeParser(const std::string& inString) {
    InitializeLexer(inString);
}
void FinishParser() {
    FinishLexer();
}

void
matcherparsererror(const char* errorMessage)
{
  /* matcherlineno, errorMessage, matchertext */
  fprintf(stderr,"MATCHER parse error: line %d: %s at %s\n",matcherparserlineno,errorMessage,matcherparsertext);
  exit(2);
}
