/* OpenMP C and C++ Grammar */
/* Author: Markus Schordan, 2003 */
/* Modified by Christian Biesinger 2006 for OpenMP 2.0 */
/* Modified by Chunhua Liao for OpenMP 3.0 and connect to OmpAttribute, 2008 */

%name-prefix="omp_"
%defines
%error-verbose

%{
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include "sage3basic.h" // Sage Interface and Builders
#include "sageBuilder.h"
#include "OmpAttribute.h"
using namespace OmpSupport;

/* Parser - BISON */

/*the scanner function*/
extern int omp_lex(); 

/*A customized initialization function for the scanner, str is the string to be scanned.*/
extern void omp_lexer_init(const char* str);

//! Initialize the parser with the originating SgPragmaDeclaration and its pragma text
extern void omp_parser_init(SgNode* aNode, const char* str);

/*Treat the entire expression as a string for now
  Implemented in the scanner*/
extern void omp_parse_expr();

//The result AST representing the annotation
extern OmpAttribute* getParsedDirective();

static int omp_error(const char*);

//Insert variable into var_list of some clause
static bool addVar(const char* var);

//Insert expression into some clause
static bool addExpression(const char* expr);

// The current AST annotation being built
static OmpAttribute* ompattribute = NULL;

// The current OpenMP construct or clause type which is being parsed
// It is automatically associated with the current ompattribute
// Used to indicate the OpenMP directive or clause to which a variable list or an expression should get added for the current OpenMP pragma being parsed.
static omp_construct_enum omptype = e_unknown;

// The node to which vars/expressions should get added
//static OmpAttribute* omptype = 0;

// The context node with the pragma annotation being parsed
//
// We attach the attribute to the pragma declaration directly for now, 
// A few OpenMP directive does not affect the next structure block
// This variable is set by the prefix_parser_init() before prefix_parse() is called.
//Liao
static SgNode* gNode;

// The current expression node being generated 
static SgExpression* current_exp = NULL;
bool b_within_variable_list  = false; 
%}

/* The %union declaration specifies the entire collection of possible data types for semantic values. these names are used in the %token and %type declarations to pick one of the types for a terminal or nonterminal symbol
corresponding C type is union name defaults to YYSTYPE.
*/

%union {  int itype;
          double ftype;
          const char* stype;
          void* ptype; /* For expressions */
        }

/*Some operators have a suffix 2 to avoid name conflicts with ROSE's existing types, We may want to reuse them if it is proper. Liao*/
%token  OMP PARALLEL IF NUM_THREADS ORDERED SCHEDULE STATIC DYNAMIC GUIDED RUNTIME SECTIONS SINGLE NOWAIT SECTION
        FOR MASTER CRITICAL BARRIER ATOMIC FLUSH 
        THREADPRIVATE PRIVATE COPYPRIVATE FIRSTPRIVATE LASTPRIVATE SHARED DEFAULT NONE REDUCTION COPYIN 
        TASK TASKWAIT UNTIED COLLAPSE AUTO
        '(' ')' ',' ':' '+' '*' '-' '&' '^' '|' LOGAND LOGOR
        LE_OP2 GE_OP2 EQ_OP2 NE_OP2 RIGHT_ASSIGN2 LEFT_ASSIGN2 ADD_ASSIGN2
        SUB_ASSIGN2 MUL_ASSIGN2 DIV_ASSIGN2 MOD_ASSIGN2 AND_ASSIGN2 
        XOR_ASSIGN2 OR_ASSIGN2
        LEXICALERROR IDENTIFIER 
/*We ignore NEWLINE since we only care about the pragma string , We relax the syntax check by allowing it as part of line continuation */
%token <itype> ICONSTANT   
%token <stype> EXPRESSION ID_EXPRESSION 

/* nonterminals names, types for semantic values
 */
%type <ptype> expression assignment_expr equality_expr unary_expr 
              relational_expr  
%type <itype> schedule_kind

/* start point for the parsing */
%start openmp_directive

%%

/* NOTE: We can't use the EXPRESSION lexer token directly. Instead, we have
 * to first call omp_parse_expr, because we parse up to the terminating
 * paren.
 */

openmp_directive
                : parallel_directive 
                | for_directive
                | sections_directive
                | single_directive
                | parallel_for_directive
                | parallel_sections_directive
                | task_directive
                | master_directive
                | critical_directive
                | atomic_directive
                | ordered_directive
                | barrier_directive 
                | taskwait_directive
                | flush_directive
                | threadprivate_directive
                | section_directive
                ;

parallel_directive
                : /* # pragma */ OMP PARALLEL
                  { ompattribute = buildOmpAttribute(e_parallel,gNode,true);
                    omptype = e_parallel; 
                  }
                  parallel_clause_optseq 
                ;

parallel_clause_optseq
                : /* empty */
                | parallel_clause_seq
                ;

parallel_clause_seq
                : parallel_clause
                | parallel_clause_seq parallel_clause
                | parallel_clause_seq ',' parallel_clause
                ;

parallel_clause : unique_parallel_clause 
                | data_default_clause
                | data_privatization_clause
                | data_privatization_in_clause
                | data_sharing_clause
                | data_reduction_clause
                ;

unique_parallel_clause
                : IF { 
                        ompattribute->addClause(e_if);
                        omptype = e_if;
                     } '(' expression ')'
                         { addExpression("");}
                | NUM_THREADS 
                  { 
                    ompattribute->addClause(e_num_threads);       
                    omptype = e_num_threads;
                   } '(' expression ')'
                         { addExpression("");}
                | COPYIN
                  { ompattribute->addClause(e_copyin);
                    omptype = e_copyin;
                  } '(' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list =false;}
                ; 

for_directive   : /* # pragma */ OMP FOR
                   { 
                   ompattribute = buildOmpAttribute(e_for,gNode,true); 
                   }
                   for_clause_optseq
                ;

for_clause_optseq: /* empty*/
                | for_clause_seq
                ;

for_clause_seq  : for_clause
                | for_clause_seq for_clause
                | for_clause_seq ',' for_clause
                ;

for_clause      : unique_for_clause 
                | data_privatization_clause
                | data_privatization_in_clause
                | data_privatization_out_clause
                | data_reduction_clause
                | NOWAIT { 
                           ompattribute->addClause(e_nowait);
                           //Not correct since nowait cannot have expression or var_list
                           //omptype =e_nowait;
                         }
                ;

unique_for_clause
                : ORDERED { 
                            ompattribute->addClause(e_ordered_clause);
                          }
                | SCHEDULE '(' schedule_kind ')'
                  { 
                    ompattribute->addClause(e_schedule);
                    ompattribute->setScheduleKind(static_cast<omp_construct_enum>($3));
                    omptype = e_schedule;
                  } 
                | SCHEDULE '(' schedule_kind ','
                  { 
                    ompattribute->addClause(e_schedule);
                    ompattribute->setScheduleKind(static_cast<omp_construct_enum>($3));
                    omptype = e_schedule;
                  } expression ')'
                         { addExpression("");}
                | COLLAPSE 
                  {
                    ompattribute->addClause(e_collapse);
                    omptype = e_collapse;
                  }
                  '(' expression ')'
                         { addExpression("");}
                ;

schedule_kind   : STATIC  { $$ = e_schedule_static; }
                | DYNAMIC { $$ = e_schedule_dynamic; }
                | GUIDED  { $$ = e_schedule_guided; }
                | AUTO    { $$ = e_schedule_auto; }
                | RUNTIME { $$ = e_schedule_runtime; }
                ;

sections_directive
                : /* # pragma */ OMP SECTIONS
                  { ompattribute = buildOmpAttribute(e_sections,gNode, true); }
                  sections_clause_optseq
                ;

sections_clause_optseq
                : /* empty*/
                | sections_clause_seq
                ;

sections_clause_seq     
                : sections_clause
                | sections_clause_seq sections_clause
                | sections_clause_seq ',' sections_clause
                ;

sections_clause : data_privatization_clause
                | data_privatization_in_clause
                | data_privatization_out_clause
                | data_reduction_clause
                | NOWAIT { 
                           ompattribute->addClause(e_nowait);
                         }
                ;

section_directive
                : /* # pragma */  OMP SECTION
                  { ompattribute = buildOmpAttribute(e_section,gNode,true); }
                ;

single_directive: /* # pragma */ OMP SINGLE
                  { ompattribute = buildOmpAttribute(e_single,gNode,true); 
                    omptype = e_single; }
                   single_clause_optseq
                ;

single_clause_optseq
                : /* empty*/
                | single_clause_seq
                ;

single_clause_seq
                : single_clause
                | single_clause_seq single_clause
                | single_clause_seq ',' single_clause
                ;

single_clause   : unique_single_clause
                | data_privatization_clause
                | data_privatization_in_clause
                | NOWAIT { 
                            ompattribute->addClause(e_nowait);
                         }
                ;
unique_single_clause : COPYPRIVATE 
                        { ompattribute->addClause(e_copyprivate);
                          omptype = e_copyprivate; }
                        '(' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list =false;}

task_directive: /* #pragma */ OMP TASK 
                 {ompattribute = buildOmpAttribute(e_task,gNode,true);
                  omptype = e_task; }
                task_clause_optseq
                ;

task_clause_optseq:  /* empty*/
                |task_clause
                | task_clause_optseq task_clause
                | task_clause_optseq ',' task_clause
                ;

task_clause     : unique_task_clause
                | data_default_clause
                | data_privatization_clause
                | data_privatization_in_clause
                | data_sharing_clause
                ;

unique_task_clause : IF 
                  { ompattribute->addClause(e_if);
                    omptype = e_if; }
                    '(' expression ')'
                         { addExpression("");}
                | UNTIED 
                  {
                   ompattribute->addClause(e_untied);
                  }
                ;
parallel_for_directive
                : /* # pragma */ OMP PARALLEL FOR
                  { ompattribute = buildOmpAttribute(e_parallel_for,gNode, true); }
                  parallel_for_clauseoptseq
                ;

parallel_for_clauseoptseq       
                : /* empty*/
                | parallel_for_clause_seq
                ;

parallel_for_clause_seq
                : parallel_for_clause
                | parallel_for_clause_seq parallel_for_clause
                | parallel_for_clause_seq ',' parallel_for_clause
                ;

parallel_for_clause
                : unique_parallel_clause 
                | unique_for_clause 
                | data_default_clause
                | data_privatization_clause
                | data_privatization_in_clause
                | data_privatization_out_clause
                | data_sharing_clause
                | data_reduction_clause
                ;

parallel_sections_directive
                : /* # pragma */ OMP PARALLEL SECTIONS
                  { ompattribute =buildOmpAttribute(e_parallel_sections,gNode, true); 
                    omptype = e_parallel_sections; }
                  parallel_sections_clause_optseq
                ;

parallel_sections_clause_optseq
                : /* empty*/
                | parallel_sections_clause_seq
                ;

parallel_sections_clause_seq
                : parallel_sections_clause
                | parallel_sections_clause_seq parallel_sections_clause
                | parallel_sections_clause_seq ',' parallel_sections_clause
                ;

parallel_sections_clause
                : unique_parallel_clause 
                | data_default_clause
                | data_privatization_clause
                | data_privatization_in_clause
                | data_privatization_out_clause
                | data_sharing_clause
                | data_reduction_clause
                ;

master_directive: /* # pragma */ OMP MASTER
                  { ompattribute = buildOmpAttribute(e_master, gNode, true);}
                ;

critical_directive
                : /* # pragma */ OMP CRITICAL
                  {
                  ompattribute = buildOmpAttribute(e_critical, gNode, true); 
                  }
                  region_phraseopt
                ;

region_phraseopt: /* empty */
                | region_phrase
                ;

/* This used to use IDENTIFIER, but our lexer does not ever return that:
 * Things that'd match it are, instead, ID_EXPRESSION. So use that here.
 * named critical section
 */
region_phrase   : '(' ID_EXPRESSION ')'
                    { 
                      ompattribute->setCriticalName((const char*)$2);
                    }
                ;

barrier_directive
                : /* # pragma */ OMP BARRIER
                 { ompattribute = buildOmpAttribute(e_barrier,gNode, true); }
                ;

taskwait_directive : /* #pragma */ OMP TASKWAIT
                  { ompattribute = buildOmpAttribute(e_taskwait, gNode, true); } 
                ;

atomic_directive: /* # pragma */ OMP ATOMIC
                  { ompattribute = buildOmpAttribute(e_atomic,gNode, true); }
                ;

flush_directive : /* # pragma */ OMP FLUSH
                 { ompattribute = buildOmpAttribute(e_flush,gNode, true);
                   omptype = e_flush; }
                flush_varsopt
                ;

flush_varsopt   : /* empty */
                | flush_vars
                ;

flush_vars      : '(' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list =false;}
                ;

ordered_directive
                : /* # pragma */ OMP ORDERED
                  { ompattribute = buildOmpAttribute(e_ordered_directive,gNode, true); }
                ;

threadprivate_directive
                : /* # pragma */ OMP THREADPRIVATE
                  { ompattribute = buildOmpAttribute(e_threadprivate,gNode, true); 
                    omptype = e_threadprivate; }
                 '(' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list =false;}
                ;

data_default_clause
                :DEFAULT '(' SHARED ')'
                  { 
                    ompattribute->addClause(e_default);
                    ompattribute->setDefaultValue(e_default_shared); 
                  }
                 | DEFAULT '(' NONE ')'
                  {
                    ompattribute->addClause(e_default);
                    ompattribute->setDefaultValue(e_default_none);
                  }
                 ;
data_privatization_clause :  PRIVATE
                  { ompattribute->addClause(e_private); omptype = e_private;}
                  '(' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list =false;}
                ;

data_privatization_in_clause: FIRSTPRIVATE
                  { ompattribute->addClause(e_firstprivate); 
                    omptype = e_firstprivate;}
                  '(' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list =false;}
                ;

data_privatization_out_clause: LASTPRIVATE
                  { ompattribute->addClause(e_lastprivate); 
                    omptype = e_lastprivate;}
                  '(' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list =false;}
                ;

data_sharing_clause: SHARED
                  { ompattribute->addClause(e_shared); omptype = e_shared; }
                   '(' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list =false;}
                ;

data_reduction_clause: REDUCTION
                  { ompattribute->addClause(e_reduction);}
                  '(' reduction_operator ':' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list =false;}
                ;

reduction_operator
                : '+' {
                       ompattribute->setReductionOperator(e_reduction_plus); 
                       omptype = e_reduction_plus; /*variables are stored for each operator*/
                       }
                | '*' {ompattribute->setReductionOperator(e_reduction_mul);  
                       omptype = e_reduction_mul;
                      }
                | '-' {ompattribute->setReductionOperator(e_reduction_minus); 
                       omptype = e_reduction_minus;
                      }
                | '&' {ompattribute->setReductionOperator(e_reduction_bitand);  
                       omptype = e_reduction_bitand;
                      }
                | '^' {ompattribute->setReductionOperator(e_reduction_bitxor);  
                       omptype = e_reduction_bitxor;
                      }
                | '|' {ompattribute->setReductionOperator(e_reduction_bitor);  
                       omptype = e_reduction_bitor;
                      }
                | LOGAND /* && */ {ompattribute->setReductionOperator(e_reduction_logand);  
                                   omptype = e_reduction_logand;
                                  }
                | LOGOR /* || */ { ompattribute->setReductionOperator(e_reduction_logor); 
                                   omptype = e_reduction_logor;
                                 }
                ;

/* parsing real expressions here, Liao, 10/12/2008
   */       
/* expression: { omp_parse_expr(); } EXPRESSION { if (!addExpression((const char*)$2)) YYABORT; }
*/
expression: assignment_expr

/* TODO conditional_expr */
assignment_expr
        : equality_expr 
        | unary_expr '=' assignment_expr 
        {
          current_exp = SageBuilder::buildAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)); 
          $$ = current_exp;
        }
        | unary_expr RIGHT_ASSIGN2 assignment_expr 
        {
          current_exp = SageBuilder::buildRshiftAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)); 
          $$ = current_exp;
        }
        | unary_expr LEFT_ASSIGN2 assignment_expr 
        {
          current_exp = SageBuilder::buildLshiftAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)); 
          $$ = current_exp;
        }
        | unary_expr ADD_ASSIGN2 assignment_expr 
        {
          current_exp = SageBuilder::buildPlusAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)); 
          $$ = current_exp;
        }
        | unary_expr SUB_ASSIGN2 assignment_expr 
        {
          current_exp = SageBuilder::buildMinusAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)); 
          $$ = current_exp;
        }
        | unary_expr MUL_ASSIGN2 assignment_expr 
        {
          current_exp = SageBuilder::buildMultAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)); 
          $$ = current_exp;
        }

        | unary_expr DIV_ASSIGN2 assignment_expr 
        {
          current_exp = SageBuilder::buildDivAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)); 
          $$ = current_exp;
        }
        | unary_expr MOD_ASSIGN2 assignment_expr 
        {
          current_exp = SageBuilder::buildModAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)); 
          $$ = current_exp;
        }
        | unary_expr AND_ASSIGN2 assignment_expr 
        {
          current_exp = SageBuilder::buildAndAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)); 
          $$ = current_exp;
        }

        | unary_expr XOR_ASSIGN2 assignment_expr 
        {
          current_exp = SageBuilder::buildXorAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)); 
          $$ = current_exp;
        }
        | unary_expr OR_ASSIGN2 assignment_expr 
        {
          current_exp = SageBuilder::buildIorAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)); 
          $$ = current_exp;
        }
        ;

equality_expr
        : relational_expr
        | equality_expr EQ_OP2 relational_expr
        {
          current_exp = SageBuilder::buildEqualityOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)); 
          $$ = current_exp;
        }
        | equality_expr NE_OP2 relational_expr
        {
          current_exp = SageBuilder::buildNotEqualOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)); 
          $$ = current_exp;
        }
        ;
              
relational_expr
                : unary_expr
                | relational_expr '<' unary_expr 
                  { 
                    current_exp = SageBuilder::buildLessThanOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)); 
                    $$ = current_exp; 
                  //  std::cout<<"debug: buildLessThanOp():\n"<<current_exp->unparseToString()<<std::endl;
                  }
                | relational_expr '>' unary_expr
                {
                    current_exp = SageBuilder::buildGreaterThanOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)); 
                    $$ = current_exp; 
                }
                | relational_expr LE_OP2 unary_expr
                {
                    current_exp = SageBuilder::buildLessOrEqualOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)); 
                    $$ = current_exp; 
                }
                | relational_expr GE_OP2 unary_expr
                {
                    current_exp = SageBuilder::buildGreaterOrEqualOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)); 
                    $$ = current_exp; 
                }
                ;

/* simplified unary expression, simplest integer constant here */
unary_expr
                : ICONSTANT 
                  {current_exp = SageBuilder::buildIntVal($1); 
                    $$ = current_exp; }
                | ID_EXPRESSION 
                  { current_exp = SageBuilder::buildVarRefExp(
                      (const char*)($1),SageInterface::getScope(gNode)); 
                    $$ = current_exp; }
                ;
/* ----------------------end for parsing expressions ------------------*/
/*  in C
    variable-list: identifier
                 | variable-list , identifier 

*/

/* in C++ (we use the C++ version) */ 
variable_list   : ID_EXPRESSION { if (!addVar((const char*)$1)) YYABORT; }
                | 
                | variable_list ',' ID_EXPRESSION { if (!addVar((const char*)$3)) YYABORT; }
                ;

%%

int yyerror(const char *s) {
        printf("%s!\n", s);
        return 0;
}


OmpAttribute* getParsedDirective() {
        return ompattribute;
}

void omp_parser_init(SgNode* aNode, const char* str) {
        omp_lexer_init(str);
        gNode = aNode;
}

static bool addVar(const char* var) 
{
  ompattribute->addVariable(omptype,var);
  return true;
}

// The ROSE's string-based AST construction is not stable,
// pass real expressions as SgExpression, Liao
static bool addExpression(const char* expr) {
        //ompattribute->addExpression(omptype,std::string(expr),NULL);
//  std::cout<<"debug: current expression is:"<<current_exp->unparseToString()<<std::endl;
      ompattribute->addExpression(omptype,std::string(expr),current_exp);
        return true;
}

