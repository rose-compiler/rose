/* OpenMP C and C++ Grammar */
/* Author: Markus Schordan, 2003 */
/* Modified by Christian Biesinger 2006 */
/* Modified by Chunhua Liao to connect to OmpAttribute and support OpenMP 3.0
   2008 
*/

%name-prefix="omp_"
%defines
%error-verbose

%{
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include "rose.h" // Sage Interface and Builders
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
%}

%union {  int itype;
          double ftype;
          const char* stype;
          void* ptype; /* For expressions */
        }

/*Some operators have a suffix 2 to avoid name conficts with ROSE's existing types, We may want to reuse them if it is proper. Liao*/
%token  OMP PARALLEL IF NUM_THREADS ORDERED SCHEDULE STATIC DYNAMIC GUIDED RUNTIME SECTIONS SINGLE NOWAIT SECTION
        FOR MASTER CRITICAL BARRIER ATOMIC FLUSH 
        THREADPRIVATE PRIVATE COPYPRIVATE FIRSTPRIVATE LASTPRIVATE SHARED DEFAULT NONE REDUCTION COPYIN 
	TASK TASKWAIT UNTIED COLLAPSE AUTO
        '(' ')' ',' ':' '+' '*' '-' '&' '^' '|' LOGAND LOGOR
        LE_OP2 GE_OP2 EQ_OP2 NE_OP2 RIGHT_ASSIGN2 LEFT_ASSIGN2 ADD_ASSIGN2
        SUB_ASSIGN2 MUL_ASSIGN2 DIV_ASSIGN2 MOD_ASSIGN2 AND_ASSIGN2 
        XOR_ASSIGN2 OR_ASSIGN2
        NEWLINE LEXICALERROR IDENTIFIER EXPRESSION ID_EXPRESSION

%token <itype> ICONSTANT   
%token <stype> EXPRESSION ID_EXPRESSION 

/* nonterminals names, types for semantic values
 */
%type <ptype> expression assignment_expr equality_expr unary_expr 
              relational_expr  
%type <itype> schedule_kind

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
		  { ompattribute = buildOmpAttribute(e_parallel,gNode);
		    omptype = e_parallel; 
		  }
		  parallel_clause_optseq new_line
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

parallel_clause	: unique_parallel_clause 
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
		  } '(' variable_list ')'
		; 

for_directive	: /* # pragma */ OMP FOR
		   { 
                   ompattribute = buildOmpAttribute(e_for,gNode); 
                   }
		   for_clause_optseq new_line
		;

for_clause_optseq: /* empty*/
            	| for_clause_seq
            	;

for_clause_seq	: for_clause
		| for_clause_seq for_clause
		| for_clause_seq ',' for_clause
		;

for_clause	: unique_for_clause 
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

schedule_kind	: STATIC  { $$ = e_schedule_static; }
		| DYNAMIC { $$ = e_schedule_dynamic; }
		| GUIDED  { $$ = e_schedule_guided; }
		| AUTO    { $$ = e_schedule_auto; }
		| RUNTIME { $$ = e_schedule_runtime; }
		;

sections_directive
		: /* # pragma */ OMP SECTIONS
		  { ompattribute = buildOmpAttribute(e_sections,gNode); }
		  sections_clause_optseq new_line
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

sections_clause	: data_privatization_clause
		| data_privatization_in_clause
		| data_privatization_out_clause
		| data_reduction_clause
		| NOWAIT { 
			   ompattribute->addClause(e_nowait);
			 }
		;

section_directive
		: /* # pragma */  OMP SECTION new_line 
		  { ompattribute = buildOmpAttribute(e_section,gNode); }
		;

single_directive: /* # pragma */ OMP SINGLE
		  { ompattribute = buildOmpAttribute(e_single,gNode); 
		    omptype = e_single; }
	           single_clause_optseq new_line
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

single_clause	: unique_single_clause
		| data_privatization_clause
		| data_privatization_in_clause
		| NOWAIT { 
                            ompattribute->addClause(e_nowait);
			 }
		;
unique_single_clause : COPYPRIVATE 
			{ ompattribute->addClause(e_copyprivate);
			  omptype = e_copyprivate; }
			'(' variable_list ')'

task_directive: /* #pragma */ OMP TASK 
                 {ompattribute = buildOmpAttribute(e_task,gNode);
		  omptype = e_task; }
		task_clause_optseq new_line
		;

task_clause_optseq:  /* empty*/
		|task_clause
		| task_clause_optseq task_clause
		| task_clause_optseq ',' task_clause
		;

task_clause	: unique_task_clause
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
		  { ompattribute = buildOmpAttribute(e_parallel_for,gNode); }
		  parallel_for_clauseoptseq new_line
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
		  { ompattribute =buildOmpAttribute(e_parallel_sections,gNode); 
		    omptype = e_parallel_sections; }
		  parallel_sections_clause_optseq new_line
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

master_directive: /* # pragma */ OMP MASTER new_line
		  { ompattribute = buildOmpAttribute(e_master, gNode);}
		;

critical_directive
		: /* # pragma */ OMP CRITICAL
		  {
                  ompattribute = buildOmpAttribute(e_critical, gNode); 
                  }
		  region_phraseopt new_line
		;

region_phraseopt: /* empty */
		| region_phrase
		;

/* This used to use IDENTIFIER, but our lexer does not ever return that:
 * Things that'd match it are, instead, ID_EXPRESSION. So use that here.
 * named critical section
 */
region_phrase	: '(' ID_EXPRESSION ')'
		    { 
                      ompattribute->setCriticalName((const char*)$2);
                    }
		;

barrier_directive
		: /* # pragma */ OMP BARRIER new_line
		 { ompattribute = buildOmpAttribute(e_barrier,gNode); }
		;

taskwait_directive : /* #pragma */ OMP TASKWAIT new_line
		  { ompattribute = buildOmpAttribute(e_taskwait, gNode); } 
		;

atomic_directive: /* # pragma */ OMP ATOMIC new_line
		  { ompattribute = buildOmpAttribute(e_atomic,gNode); }
		;

flush_directive	: /* # pragma */ OMP FLUSH
		 { ompattribute = buildOmpAttribute(e_flush,gNode);
	           omptype = e_flush; }
		flush_varsopt new_line
		;

flush_varsopt   : /* empty */
		| flush_vars
		;

flush_vars	: '(' variable_list ')'
		;

ordered_directive
		: /* # pragma */ OMP ORDERED new_line
		  { ompattribute = buildOmpAttribute(e_ordered_directive,gNode); }
		;

threadprivate_directive
		: /* # pragma */ OMP THREADPRIVATE
		  { ompattribute = buildOmpAttribute(e_threadprivate,gNode); 
                    omptype = e_threadprivate; }
		 '(' variable_list ')' new_line
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
		  '(' variable_list ')'
		;

data_privatization_in_clause: FIRSTPRIVATE
                  { ompattribute->addClause(e_firstprivate); 
		    omptype = e_firstprivate;}
		  '(' variable_list ')'
		;

data_privatization_out_clause: LASTPRIVATE
                  { ompattribute->addClause(e_lastprivate); 
		    omptype = e_lastprivate;}
		  '(' variable_list ')'
		;

data_sharing_clause: SHARED
                  { ompattribute->addClause(e_shared); omptype = e_shared; }
		   '(' variable_list ')'
		;

data_reduction_clause: REDUCTION
		  { ompattribute->addClause(e_reduction);}
		  '(' reduction_operator ':' variable_list ')' 
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
variable_list	: ID_EXPRESSION   { if (!addVar((const char*)$1)) YYABORT; }
		| variable_list ',' ID_EXPRESSION { if (!addVar((const char*)$3)) YYABORT; }
		;

new_line	: 
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

/* // do it in OmpAttribute::addVariable() instead
static std::auto_ptr<SgNode> resolveVar(const char* var) {
       
	std::auto_ptr<SgNode> node(gNode->resolveVariable(var));

	if (!node.get())
		printf("No such var <%s>!\n", var);
	return node;
}
*/

static bool addVar(const char* var) {
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


/**
 * @file
 * Parser for OpenMP-pragmas.
 */

#if 0
int main()
{
        yyparse();
        printf("you win!\n");
        return (0);
}
#endif

/* The following rules have been removed from the standard-grammar because
   they specify how to extend the C++ grammar with OpenMP pragmas in some
   allowed contexts only. In ROSE we cannot change the C++ grammar (EDG front-end).
   Therefore, the context-tests, which are specified by the following rules have to
   be performed on the ROSE AST.

// extended in above grammar with all openmp-directives
openmp-directive: barrier-directive
		| flush-directive
		;

statement: .. standard statements ..
	openmp-construct
	;

openmp-construct: parallel-construct 
		| for-construct 
		| sections-construct 
		| single-construct 
		| parallel-for-construct 
		| parallel-sections-construct 
		| master-construct 
		| critical-construct 
		| atomic-construct 
		| ordered-construct
		;

structured-block: statement 
		; 
parallel-construct
		: parallel-directive structured-block 
		;

for-construct	: for-directive iteration-statement
		;

sections-construct
		: sections-directive section-scope
		;

section-scope	: '{' section-sequence '}' 
		;

section-sequence: section-directiveopt structured-block 
		| section-sequence section-directive structured-block 
		;

section-directiveopt
		: // empty 
		| section-directive
		;

single-construct: single-directive structured-block
		;


parallel-for-construct
		: parallel-for-directive iteration-statement
		;

parallel-sections-construct
		: parallel-sections-directive 
		| section-scope
		;

master-construct: master-directive structured-block
		;

critical-construct
		: critical-directive structured-block
		;

atomic-construct: atomic-directive expression-statement
		;

ordered-construct
		: ordered-directive structured-block
		;

declaration	: -- standard declarations --
		| threadprivate-directive
		;

*/

