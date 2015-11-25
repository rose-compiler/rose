/* OpenMP C and C++ Grammar */
/* Author: Markus Schordan, 2003 */
/* Modified by Christian Biesinger 2006 for OpenMP 2.0 */
/* Modified by Chunhua Liao for OpenMP 3.0 and connect to OmpAttribute, 2008 */

%name-prefix="omp_"
%defines
%error-verbose

%{
/* DQ (2/10/2014): IF is conflicting with Boost template IF. */
#undef IF

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include "sage3basic.h" // Sage Interface and Builders
#include "sageBuilder.h"
#include "OmpAttribute.h"

#ifdef _MSC_VER
  #undef IN
  #undef OUT
#endif

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
bool b_within_variable_list  = false;  // a flag to indicate if the program is now processing a list of variables

// We now follow the OpenMP 4.0 standard's C-style array section syntax: [lower-bound:length] or just [length]
// the latest variable symbol being parsed, used to help parsing the array dimensions associated with array symbol
// such as a[0:n][0:m]
static SgVariableSymbol* array_symbol; 
static SgExpression* lower_exp = NULL;
static SgExpression* length_exp = NULL;

%}

/* The %union declaration specifies the entire collection of possible data types for semantic values. these names are used in the %token and %type declarations to pick one of the types for a terminal or nonterminal symbol
corresponding C type is union name defaults to YYSTYPE.
*/

%union {  int itype;
          double ftype;
          const char* stype;
          void* ptype; /* For expressions */
        }

/*Some operators have a suffix 2 to avoid name conflicts with ROSE's existing types, We may want to reuse them if it is proper. 
  experimental BEGIN END are defined by default, we use TARGET_BEGIN TARGET_END instead. 
  Liao*/
%token  OMP PARALLEL IF NUM_THREADS ORDERED SCHEDULE STATIC DYNAMIC GUIDED RUNTIME SECTIONS SINGLE NOWAIT SECTION
        FOR MASTER CRITICAL BARRIER ATOMIC FLUSH TARGET UPDATE DIST_DATA BLOCK DUPLICATE CYCLIC
        THREADPRIVATE PRIVATE COPYPRIVATE FIRSTPRIVATE LASTPRIVATE SHARED DEFAULT NONE REDUCTION COPYIN 
        TASK TASKWAIT UNTIED COLLAPSE AUTO DECLARE DATA DEVICE MAP ALLOC TO FROM TOFROM
        SIMD SAFELEN ALIGNED LINEAR UNIFORM INBRANCH NOTINBRANCH MPI MPI_ALL MPI_MASTER TARGET_BEGIN TARGET_END
        '(' ')' ',' ':' '+' '*' '-' '&' '^' '|' LOGAND LOGOR SHLEFT SHRIGHT PLUSPLUS MINUSMINUS PTR_TO '.'
        LE_OP2 GE_OP2 EQ_OP2 NE_OP2 RIGHT_ASSIGN2 LEFT_ASSIGN2 ADD_ASSIGN2
        SUB_ASSIGN2 MUL_ASSIGN2 DIV_ASSIGN2 MOD_ASSIGN2 AND_ASSIGN2 
        XOR_ASSIGN2 OR_ASSIGN2
        LEXICALERROR IDENTIFIER 
/*We ignore NEWLINE since we only care about the pragma string , We relax the syntax check by allowing it as part of line continuation */
%token <itype> ICONSTANT   
%token <stype> EXPRESSION ID_EXPRESSION 

/* nonterminals names, types for semantic values
 */
%type <ptype> expression assignment_expr conditional_expr 
              logical_or_expr logical_and_expr
              inclusive_or_expr exclusive_or_expr and_expr
              equality_expr relational_expr
              shift_expr additive_expr multiplicative_expr 
              primary_expr incr_expr unary_expr
              device_clause if_clause num_threads_clause
              simd_clause

%type <itype> schedule_kind

/* start point for the parsing */
%start openmp_directive

%%

/* NOTE: We can't use the EXPRESSION lexer token directly. Instead, we have
 * to first call omp_parse_expr, because we parse up to the terminating
 * paren.
 */

openmp_directive : parallel_directive 
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
                 | target_directive
                 | target_data_directive
                 | simd_directive
                 ;

parallel_directive : /* #pragma */ OMP PARALLEL {
                       ompattribute = buildOmpAttribute(e_parallel,gNode,true);
                       omptype = e_parallel; 
                     }
                     parallel_clause_optseq 
                   ;

parallel_clause_optseq : /* empty */
                       | parallel_clause_seq
                       ;

parallel_clause_seq : parallel_clause
                    | parallel_clause_seq parallel_clause
                    | parallel_clause_seq ',' parallel_clause
                    ;

parallel_clause : unique_parallel_clause 
                | data_default_clause
                | data_privatization_clause
                | data_privatization_in_clause
                | data_sharing_clause
                | data_reduction_clause
                | if_clause
                | num_threads_clause
                ;

unique_parallel_clause : IF { 
                           ompattribute->addClause(e_if);
                           omptype = e_if;
                         } '(' expression ')' { 
                           addExpression("");
                         }
                       | NUM_THREADS { 
                           ompattribute->addClause(e_num_threads);
                           omptype = e_num_threads;
                         } '(' expression ')' { 
                           addExpression("");
                         }
                       | COPYIN { 
                           ompattribute->addClause(e_copyin);
                           omptype = e_copyin;
                         } '(' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list = false;}
                ; 

for_directive : /* #pragma */ OMP FOR { 
                  ompattribute = buildOmpAttribute(e_for,gNode,true); 
                }
                for_clause_optseq
              ;

for_clause_optseq : /* empty */
                  | for_clause_seq
                  ;

for_clause_seq : for_clause
               | for_clause_seq for_clause
               | for_clause_seq ',' for_clause
               ;

for_clause : unique_for_clause 
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

unique_for_clause : ORDERED { 
                      ompattribute->addClause(e_ordered_clause);
                    }
                  | SCHEDULE '(' schedule_kind ')' { 
                      ompattribute->addClause(e_schedule);
                      ompattribute->setScheduleKind(static_cast<omp_construct_enum>($3));
                      omptype = e_schedule;
                    } 
                  | SCHEDULE '(' schedule_kind ',' { 
                      ompattribute->addClause(e_schedule);
                      ompattribute->setScheduleKind(static_cast<omp_construct_enum>($3));
                      omptype = e_schedule;
                    } expression ')' { 
                      addExpression("");
                    }
                  | COLLAPSE {
                      ompattribute->addClause(e_collapse);
                      omptype = e_collapse;
                    } '(' expression ')' { 
                      addExpression("");
                    }
                  ;

schedule_kind : STATIC  { $$ = e_schedule_static; }
              | DYNAMIC { $$ = e_schedule_dynamic; }
              | GUIDED  { $$ = e_schedule_guided; }
              | AUTO    { $$ = e_schedule_auto; }
              | RUNTIME { $$ = e_schedule_runtime; }
              ;

sections_directive : /* #pragma */ OMP SECTIONS { 
                       ompattribute = buildOmpAttribute(e_sections,gNode, true); 
                     } sections_clause_optseq
                   ;

sections_clause_optseq : /* empty */
                       | sections_clause_seq
                       ;

sections_clause_seq : sections_clause
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

section_directive : /* #pragma */  OMP SECTION { 
                      ompattribute = buildOmpAttribute(e_section,gNode,true); 
                    }
                  ;

single_directive : /* #pragma */ OMP SINGLE { 
                     ompattribute = buildOmpAttribute(e_single,gNode,true); 
                     omptype = e_single; 
                   } single_clause_optseq
                 ;

single_clause_optseq : /* empty */
                     | single_clause_seq
                     ;

single_clause_seq : single_clause
                  | single_clause_seq single_clause
                  | single_clause_seq ',' single_clause
                  ;

single_clause : unique_single_clause
              | data_privatization_clause
              | data_privatization_in_clause
              | NOWAIT { 
                  ompattribute->addClause(e_nowait);
                }
              ;
unique_single_clause : COPYPRIVATE { 
                         ompattribute->addClause(e_copyprivate);
                         omptype = e_copyprivate; 
                       }
                       '(' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list =false;}

task_directive : /* #pragma */ OMP TASK {
                   ompattribute = buildOmpAttribute(e_task,gNode,true);
                   omptype = e_task; 
                 } task_clause_optseq
               ;

task_clause_optseq :  /* empty */ 
                   | task_clause
                   | task_clause_optseq task_clause
                   | task_clause_optseq ',' task_clause
                   ;

task_clause : unique_task_clause
            | data_default_clause
            | data_privatization_clause
            | data_privatization_in_clause
            | data_sharing_clause
            ;

unique_task_clause : IF { 
                       ompattribute->addClause(e_if);
                       omptype = e_if; 
                     } '(' expression ')' { 
                       addExpression("");
                     }
                   | UNTIED {
                       ompattribute->addClause(e_untied);
                     }
                   ;
                   
parallel_for_directive : /* #pragma */ OMP PARALLEL FOR { 
                           ompattribute = buildOmpAttribute(e_parallel_for,gNode, true); 
                         } parallel_for_clauseoptseq
                       ;

parallel_for_clauseoptseq : /* empty */
                          | parallel_for_clause_seq
                          ;

parallel_for_clause_seq : parallel_for_clause
                        | parallel_for_clause_seq parallel_for_clause
                        | parallel_for_clause_seq ',' parallel_for_clause
                        ;

parallel_for_clause : unique_parallel_clause 
                    | unique_for_clause 
                    | data_default_clause
                    | data_privatization_clause
                    | data_privatization_in_clause
                    | data_privatization_out_clause
                    | data_sharing_clause
                    | data_reduction_clause
                    | if_clause
                    | num_threads_clause
                    ;

parallel_sections_directive : /* #pragma */ OMP PARALLEL SECTIONS { 
                                ompattribute =buildOmpAttribute(e_parallel_sections,gNode, true); 
                                omptype = e_parallel_sections; 
                              } parallel_sections_clause_optseq
                            ;

parallel_sections_clause_optseq : /* empty */
                                | parallel_sections_clause_seq
                                ;

parallel_sections_clause_seq : parallel_sections_clause
                             | parallel_sections_clause_seq parallel_sections_clause
                             | parallel_sections_clause_seq ',' parallel_sections_clause
                             ;

parallel_sections_clause : unique_parallel_clause 
                         | data_default_clause
                         | data_privatization_clause
                         | data_privatization_in_clause
                         | data_privatization_out_clause
                         | data_sharing_clause
                         | data_reduction_clause
                         | if_clause
                         | num_threads_clause
                         ;

master_directive : /* #pragma */ OMP MASTER { 
                     ompattribute = buildOmpAttribute(e_master, gNode, true);}
                 ;

critical_directive : /* #pragma */ OMP CRITICAL {
                       ompattribute = buildOmpAttribute(e_critical, gNode, true); 
                     } region_phraseopt
                   ;

region_phraseopt : /* empty */
                 | region_phrase
                 ;

/* This used to use IDENTIFIER, but our lexer does not ever return that:
 * Things that'd match it are, instead, ID_EXPRESSION. So use that here.
 * named critical section
 */
region_phrase : '(' ID_EXPRESSION ')' { 
                  ompattribute->setCriticalName((const char*)$2);
                }
              ;

barrier_directive : /* #pragma */ OMP BARRIER { 
                      ompattribute = buildOmpAttribute(e_barrier,gNode, true); }
                  ;

taskwait_directive : /* #pragma */ OMP TASKWAIT { 
                       ompattribute = buildOmpAttribute(e_taskwait, gNode, true); } 
                   ;

atomic_directive : /* #pragma */ OMP ATOMIC { 
                     ompattribute = buildOmpAttribute(e_atomic,gNode, true); }
                 ;

flush_directive : /* #pragma */ OMP FLUSH {
                    ompattribute = buildOmpAttribute(e_flush,gNode, true);
                    omptype = e_flush; 
                  } flush_varsopt
                ;

flush_varsopt : /* empty */
              | flush_vars
              ;

flush_vars : '(' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list = false;}
           ;

ordered_directive : /* #pragma */ OMP ORDERED { 
                      ompattribute = buildOmpAttribute(e_ordered_directive,gNode, true); 
                    }
                  ;

threadprivate_directive : /* #pragma */ OMP THREADPRIVATE {
                            ompattribute = buildOmpAttribute(e_threadprivate,gNode, true); 
                            omptype = e_threadprivate; 
                          } '(' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list = false;}
                        ;

data_default_clause : DEFAULT '(' SHARED ')' { 
                        ompattribute->addClause(e_default);
                        ompattribute->setDefaultValue(e_default_shared); 
                      }
                    | DEFAULT '(' NONE ')' {
                        ompattribute->addClause(e_default);
                        ompattribute->setDefaultValue(e_default_none);
                      }
                    ;
                    
data_privatization_clause : PRIVATE {
                              ompattribute->addClause(e_private); omptype = e_private;
                            } '(' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list = false;}
                          ;

data_privatization_in_clause : FIRSTPRIVATE { 
                                 ompattribute->addClause(e_firstprivate); 
                                 omptype = e_firstprivate;
                               } '(' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list = false;}
                             ;

data_privatization_out_clause : LASTPRIVATE { 
                                  ompattribute->addClause(e_lastprivate); 
                                  omptype = e_lastprivate;
                                } '(' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list = false;}
                              ;

data_sharing_clause : SHARED {
                        ompattribute->addClause(e_shared); omptype = e_shared; 
                      } '(' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list = false;}
                    ;

data_reduction_clause : REDUCTION { 
                          ompattribute->addClause(e_reduction);
                        } '(' reduction_operator ':' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list = false;}
                      ;

reduction_operator : '+' {
                       ompattribute->setReductionOperator(e_reduction_plus); 
                       omptype = e_reduction_plus; /*variables are stored for each operator*/
                     }
                   | '*' {
                       ompattribute->setReductionOperator(e_reduction_mul);  
                       omptype = e_reduction_mul;
                     }
                   | '-' {
                       ompattribute->setReductionOperator(e_reduction_minus); 
                       omptype = e_reduction_minus;
                      }
                   | '&' {
                       ompattribute->setReductionOperator(e_reduction_bitand);  
                       omptype = e_reduction_bitand;
                      }
                   | '^' {
                       ompattribute->setReductionOperator(e_reduction_bitxor);  
                       omptype = e_reduction_bitxor;
                      }
                   | '|' {
                       ompattribute->setReductionOperator(e_reduction_bitor);  
                       omptype = e_reduction_bitor;
                      }
                   | LOGAND /* && */ {
                       ompattribute->setReductionOperator(e_reduction_logand);  
                       omptype = e_reduction_logand;
                     }
                   | LOGOR /* || */ {
                       ompattribute->setReductionOperator(e_reduction_logor); 
                       omptype = e_reduction_logor;
                     }
                   ;

target_data_directive: /* pragma */ OMP TARGET DATA {
                       ompattribute = buildOmpAttribute(e_target_data, gNode,true);
                       omptype = e_target_data;
                     }
                      target_data_clause_seq
                    ;

target_data_clause_seq : target_data_clause
                    | target_data_clause_seq target_data_clause
                    | target_data_clause_seq ',' target_data_clause
                    ;

target_data_clause : device_clause 
                | map_clause
                | if_clause
                ;

target_directive: /* #pragma */ OMP TARGET {
                       ompattribute = buildOmpAttribute(e_target,gNode,true);
                       omptype = e_target;
                     }
                     target_clause_optseq 
                   ;

target_clause_optseq : /* empty */
                       | target_clause_seq
                       ;

target_clause_seq : target_clause
                    | target_clause_seq target_clause
                    | target_clause_seq ',' target_clause
                    ;

target_clause : device_clause 
                | map_clause
                | if_clause
                | num_threads_clause
                | begin_clause
                | end_clause
                ;
/*
device_clause : DEVICE {
                           ompattribute->addClause(e_device);
                           omptype = e_device;
                         } '(' expression ')' {
                           addExpression("");
                         }
                ;
*/
device_clause : DEVICE {
                           ompattribute->addClause(e_device);
                           omptype = e_device;
                         } '(' expression_or_star_or_mpi 
                ;
/* Experimental extensions to support multiple devices and MPI */
expression_or_star_or_mpi: 
                  MPI ')' { // special mpi device for supporting MPI code generation
                            current_exp= SageBuilder::buildStringVal("mpi");
                            addExpression("mpi");
                          }
                  | MPI_ALL ')' { // special mpi device for supporting MPI code generation
                            current_exp= SageBuilder::buildStringVal("mpi:all");
                            addExpression("mpi:all");
                          }
                  | MPI_MASTER ')' { // special mpi device for supporting MPI code generation
                            current_exp= SageBuilder::buildStringVal("mpi:master");
                            addExpression("mpi:master");
                          }
                  | expression ')' { //normal expression
                           addExpression("");
                          }
                  | '*' ')' { // our extension device (*) 
                            current_exp= SageBuilder::buildCharVal('*'); 
                            addExpression("*");  }; 
begin_clause: TARGET_BEGIN {
                           ompattribute->addClause(e_begin);
                           omptype = e_begin;
                    }
                    ;

end_clause: TARGET_END {
                           ompattribute->addClause(e_end);
                           omptype = e_end;
                    }
                    ;

                    
if_clause: IF {
                           ompattribute->addClause(e_if);
                           omptype = e_if;
             } '(' expression ')' {
                            addExpression("");
             }
             ;

num_threads_clause: NUM_THREADS {
                           ompattribute->addClause(e_num_threads);
                           omptype = e_num_threads;
                         } '(' expression ')' {
                            addExpression("");
                         }
                      ;
map_clause: MAP {
                          ompattribute->addClause(e_map);
                           omptype = e_map; // use as a flag to see if it will be reset later
                     } '(' map_clause_optseq 
                     { 
                       b_within_variable_list = true;
                       if (omptype == e_map) // map data directions are not explicitly specified
                       {
                          ompattribute->setMapVariant(e_map_tofrom);  omptype = e_map_tofrom;  
                       }
                     } 
                     variable_list ')' { b_within_variable_list =false;} 

map_clause_optseq: /* empty, default to be tofrom*/ { ompattribute->setMapVariant(e_map_tofrom);  omptype = e_map_tofrom; /*No effect here???*/ }
                    | ALLOC ':' { ompattribute->setMapVariant(e_map_alloc);  omptype = e_map_alloc; } 
                    | TO     ':' { ompattribute->setMapVariant(e_map_to); omptype = e_map_to; } 
                    | FROM    ':' { ompattribute->setMapVariant(e_map_from); omptype = e_map_from; } 
                    | TOFROM  ':' { ompattribute->setMapVariant(e_map_tofrom); omptype = e_map_tofrom; } 
                    ;

simd_directive: /* # pragma */ OMP SIMD
                  { ompattribute = buildOmpAttribute(e_simd,gNode,true); 
                    omptype = e_simd; }
                   simd_clause_optseq
                ;

simd_clause_optseq
                : /* empty*/
                | simd_clause_seq
                ;

simd_clause_seq
                : simd_clause
                | simd_clause_seq simd_clause
                | simd_clause_seq ',' simd_clause
                ;

simd_clause : SAFELEN {
                        ompattribute->addClause(e_safelen);
                        omptype = e_safelen;
                      } '(' expression ')' {
                        addExpression("");
                      }
                | data_reduction_clause
                | uniform_clause
                | aligned_clause
                | linear_clause
              ;

uniform_clause : UNIFORM { 
                         ompattribute->addClause(e_uniform);
                         omptype = e_uniform; 
                       }
                       '(' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list =false;}
                ;

aligned_clause : ALIGNED { 
                         ompattribute->addClause(e_aligned);
                         omptype = e_aligned; 
                       }
                       '(' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list =false;}
                | ALIGNED
                  { ompattribute->addClause(e_reduction);}
                  '(' reduction_operator ':' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list =false;}
                ;

linear_clause : LINEAR { 
                         ompattribute->addClause(e_linear);
                         omptype = e_linear; 
                       }
                       '(' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list =false;}
                | LINEAR
                  { ompattribute->addClause(e_reduction);}
                  '(' reduction_operator ':' {b_within_variable_list = true;} variable_list ')' {b_within_variable_list =false;}
                ;
/* parsing real expressions here, Liao, 10/12/2008
   */       
/* expression: { omp_parse_expr(); } EXPRESSION { if (!addExpression((const char*)$2)) YYABORT; }
*/
/* Sara Royuela, 04/27/2012
 * Extending grammar to accept conditional expressions, arithmetic and bitwise expressions and member accesses
 */
expression : assignment_expr

assignment_expr : conditional_expr
                | logical_or_expr 
                | unary_expr '=' assignment_expr  {
                    current_exp = SageBuilder::buildAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                | unary_expr RIGHT_ASSIGN2 assignment_expr {
                    current_exp = SageBuilder::buildRshiftAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                | unary_expr LEFT_ASSIGN2 assignment_expr {
                    current_exp = SageBuilder::buildLshiftAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                | unary_expr ADD_ASSIGN2 assignment_expr {
                    current_exp = SageBuilder::buildPlusAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                | unary_expr SUB_ASSIGN2 assignment_expr {
                    current_exp = SageBuilder::buildMinusAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                | unary_expr MUL_ASSIGN2 assignment_expr {
                    current_exp = SageBuilder::buildMultAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                | unary_expr DIV_ASSIGN2 assignment_expr {
                    current_exp = SageBuilder::buildDivAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                | unary_expr MOD_ASSIGN2 assignment_expr {
                    current_exp = SageBuilder::buildModAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                | unary_expr AND_ASSIGN2 assignment_expr {
                    current_exp = SageBuilder::buildAndAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                | unary_expr XOR_ASSIGN2 assignment_expr {
                    current_exp = SageBuilder::buildXorAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                | unary_expr OR_ASSIGN2 assignment_expr {
                    current_exp = SageBuilder::buildIorAssignOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp;
                  }
                ;

conditional_expr : logical_or_expr '?' assignment_expr ':' assignment_expr {
                     current_exp = SageBuilder::buildConditionalExp(
                       (SgExpression*)($1),
                       (SgExpression*)($3),
                       (SgExpression*)($5)
                     );
                     $$ = current_exp;
                   }
                 ;

logical_or_expr : logical_and_expr
                | logical_or_expr LOGOR logical_and_expr {
                    current_exp = SageBuilder::buildOrOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    );
                    $$ = current_exp;
                  }
                ;

logical_and_expr : inclusive_or_expr
                 | logical_and_expr LOGAND inclusive_or_expr {
                     current_exp = SageBuilder::buildAndOp(
                       (SgExpression*)($1),
                       (SgExpression*)($3)
                     );
                   $$ = current_exp;
                 }
                 ;

inclusive_or_expr : exclusive_or_expr
                  | inclusive_or_expr '|' exclusive_or_expr {
                      current_exp = SageBuilder::buildBitOrOp(
                        (SgExpression*)($1),
                        (SgExpression*)($3)
                      );
                      $$ = current_exp;
                    }
                  ;

exclusive_or_expr : and_expr
                  | exclusive_or_expr '^' and_expr {
                      current_exp = SageBuilder::buildBitXorOp(
                        (SgExpression*)($1),
                        (SgExpression*)($3)
                      );
                      $$ = current_exp;
                    }
                  ;

and_expr : equality_expr
         | and_expr '&' equality_expr {
             current_exp = SageBuilder::buildBitAndOp(
               (SgExpression*)($1),
               (SgExpression*)($3)
             );
             $$ = current_exp;
           }
         ;  

equality_expr : relational_expr
              | equality_expr EQ_OP2 relational_expr {
                  current_exp = SageBuilder::buildEqualityOp(
                    (SgExpression*)($1),
                    (SgExpression*)($3)
                  ); 
                  $$ = current_exp;
                }
              | equality_expr NE_OP2 relational_expr {
                  current_exp = SageBuilder::buildNotEqualOp(
                    (SgExpression*)($1),
                    (SgExpression*)($3)
                  ); 
                  $$ = current_exp;
                }
              ;
              
relational_expr : shift_expr
                | relational_expr '<' shift_expr { 
                    current_exp = SageBuilder::buildLessThanOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp; 
                  // std::cout<<"debug: buildLessThanOp():\n"<<current_exp->unparseToString()<<std::endl;
                  }
                | relational_expr '>' shift_expr {
                    current_exp = SageBuilder::buildGreaterThanOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp; 
                  }
                | relational_expr LE_OP2 shift_expr {
                    current_exp = SageBuilder::buildLessOrEqualOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    ); 
                    $$ = current_exp; 
                  }
                | relational_expr GE_OP2 shift_expr {
                    current_exp = SageBuilder::buildGreaterOrEqualOp(
                      (SgExpression*)($1),
                      (SgExpression*)($3)
                    );
                    $$ = current_exp; 
                  }
                ;

shift_expr : additive_expr
           | shift_expr SHRIGHT additive_expr {
               current_exp = SageBuilder::buildRshiftOp(
                 (SgExpression*)($1),
                 (SgExpression*)($3)
               ); 
               $$ = current_exp; 
             }
           | shift_expr SHLEFT additive_expr {
               current_exp = SageBuilder::buildLshiftOp(
                 (SgExpression*)($1),
                 (SgExpression*)($3)
               ); 
               $$ = current_exp; 
             }
           ;

additive_expr : multiplicative_expr
              | additive_expr '+' multiplicative_expr {
                  current_exp = SageBuilder::buildAddOp(
                    (SgExpression*)($1),
                    (SgExpression*)($3)
                  ); 
                  $$ = current_exp; 
                }
              | additive_expr '-' multiplicative_expr {
                  current_exp = SageBuilder::buildSubtractOp(
                    (SgExpression*)($1),
                    (SgExpression*)($3)
                  ); 
                  $$ = current_exp; 
                }
              ;

multiplicative_expr : primary_expr
                    | multiplicative_expr '*' additive_expr {
                        current_exp = SageBuilder::buildMultiplyOp(
                          (SgExpression*)($1),
                          (SgExpression*)($3)
                        ); 
                        $$ = current_exp; 
                      }
                    | multiplicative_expr '/' additive_expr {
                        current_exp = SageBuilder::buildDivideOp(
                          (SgExpression*)($1),
                          (SgExpression*)($3)
                        ); 
                        $$ = current_exp; 
                      }
                    | multiplicative_expr '%' additive_expr {
                        current_exp = SageBuilder::buildModOp(
                          (SgExpression*)($1),
                          (SgExpression*)($3)
                        ); 
                        $$ = current_exp; 
                      }
                    ;

primary_expr : unary_expr
             | '(' expression ')' {
                 $$ = current_exp;
               } 
             | incr_expr;
             ;

incr_expr : PLUSPLUS unary_expr {
              current_exp = SageBuilder::buildPlusPlusOp(
                (SgExpression*)($2),
                SgUnaryOp::prefix
              ); 
              $$ = current_exp; 
            }
          | unary_expr PLUSPLUS {
              current_exp = SageBuilder::buildPlusPlusOp(
                (SgExpression*)($1),
                SgUnaryOp::postfix
              ); 
              $$ = current_exp; 
            }
          | MINUSMINUS unary_expr {
              current_exp = SageBuilder::buildMinusMinusOp(
                (SgExpression*)($2),
                SgUnaryOp::prefix
              ); 
              $$ = current_exp; 
            }
          | unary_expr MINUSMINUS {
              current_exp = SageBuilder::buildMinusMinusOp(
                (SgExpression*)($1),
                SgUnaryOp::postfix
              ); 
              $$ = current_exp; 
            }
          ;

unary_expr : ICONSTANT {
               current_exp = SageBuilder::buildIntVal($1); 
               $$ = current_exp; 
             }
           | ID_EXPRESSION { 
               current_exp = SageBuilder::buildVarRefExp(
                 (const char*)($1),SageInterface::getScope(gNode)
               ); 
               $$ = current_exp; 
             }
           ;
                
/* ----------------------end for parsing expressions ------------------*/

/*  in C
variable-list : identifier
              | variable-list , identifier 
*/

/* in C++ (we use the C++ version) */ 
variable_list : ID_EXPRESSION { if (!addVar((const char*)$1)) YYABORT; }
              | variable_list ',' ID_EXPRESSION { if (!addVar((const char*)$3)) YYABORT; }
              ;

/* */ 
variable_list : id_expression_opt_dimension
              | variable_list ',' id_expression_opt_dimension
              ;

id_expression_opt_dimension: ID_EXPRESSION { if (!addVar((const char*)$1)) YYABORT; } dimension_field_optseq id_expression_opt_dist_data
                           ;

/* Parse optional dimension information associated with map(a[0:n][0:m]) Liao 1/22/2013 */
dimension_field_optseq: /* empty */
                      | dimension_field_seq
                      ;
/* sequence of dimension fields */
dimension_field_seq : dimension_field
                    | dimension_field_seq dimension_field
                    ;
dimension_field: '[' expression {lower_exp = current_exp; } 
                 ':' expression { length_exp = current_exp;
                      assert (array_symbol != NULL);
                      SgType* t = array_symbol->get_type();
                      bool isPointer= (isSgPointerType(t) != NULL );
                      bool isArray= (isSgArrayType(t) != NULL);
                      if (!isPointer && ! isArray )
                      {
                        std::cerr<<"Error. ompparser.yy expects a pointer or array type."<<std::endl;
                        std::cerr<<"while seeing "<<t->class_name()<<std::endl;
                      }
                      ompattribute->array_dimensions[array_symbol].push_back( std::make_pair (lower_exp, length_exp));
                      } 
                  ']'
               ;
/*Optional data distribution clause: dist_data(dim1_policy, dim2_policy, dim3_policy)*/
/* mixed keyword or variable parsing is tricky TODO */
id_expression_opt_dist_data: /* empty */
                           | DIST_DATA '(' dist_policy_seq ')'
                           ;
/* one or more dimensions, each has a policy*/
dist_policy_seq: dist_policy_per_dim
               | dist_policy_seq ',' dist_policy_per_dim
               ;
/*reset current_exp to avoid leaving stale values*/
dist_policy_per_dim: DUPLICATE  { ompattribute->appendDistDataPolicy(array_symbol, e_duplicate, NULL); }
                   | BLOCK dist_size_opt { ompattribute->appendDistDataPolicy(array_symbol, e_block, current_exp );  current_exp = NULL;}
                   | CYCLIC dist_size_opt { ompattribute->appendDistDataPolicy(array_symbol, e_cyclic, current_exp ); current_exp = NULL;}
                   ;
/*Optional (exp) for some policy */                   
dist_size_opt: /*empty*/ {current_exp = NULL;}
             | '(' expression ')'
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

static bool addVar(const char* var)  {
    array_symbol = ompattribute->addVariable(omptype,var);
    return true;
}

// The ROSE's string-based AST construction is not stable,
// pass real expressions as SgExpression, Liao
static bool addExpression(const char* expr) {
    // ompattribute->addExpression(omptype,std::string(expr),NULL);
    // std::cout<<"debug: current expression is:"<<current_exp->unparseToString()<<std::endl;
    assert (current_exp != NULL);
    ompattribute->addExpression(omptype,std::string(expr),current_exp);
    return true;
}

