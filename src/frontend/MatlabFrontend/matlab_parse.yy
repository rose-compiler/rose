/*
SujanK: Octave's parser used for parsing matlab
*/

/*

Copyright (C) 1993-2011 John W. Eaton
Copyright (C) 2009 David Grundberg
Copyright (C) 2009-2010 VZLU Prague

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, see
<http://www.gnu.org/licenses/>.

*/

// Parser for Octave.

// C decarations.

%{
#define YYDEBUG 1

/*#ifdef HAVE_CONFIG_H
#include <config.h>
#endif*/

#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <typeinfo>

#include <iostream>
#include <map>
#include <sstream>
#include <stack>
#include <boost/algorithm/string/join.hpp>
#include <vector>

#include "input.h"
#include "lex.h"
#include "ourtoken.h"

#include "StatementList.h"
#include "MatlabFunctionBuilder.h"

#include "sage3basic.h"

#if defined (GNULIB_NAMESPACE)
// Calls to the following functions appear in the generated output from
// Bison without the namespace tag.  Redefine them so we will use them
// via the gnulib namespace.
#define fclose GNULIB_NAMESPACE::fclose
#define fprintf GNULIB_NAMESPACE::fprintf
#define malloc GNULIB_NAMESPACE::malloc
#endif

int yylex (void);
extern void yyerror (char const *);

// Reset state after parsing function.
static SgExpression*
make_indirect_ref (SgExpression*, SgExpression*);

static void
recover_from_parsing_function ();

int beginParse(SgProject* &p, int argc, char* argv[]);
SgIfStmt* getLastIfClause(SgIfStmt* topIfClause);

// void addVariableDeclarations();
// void manageReturnStatement(SgExprListExp*, SgFunctionDeclaration*);
 SgName extractVarName(SgExpression*);

class token;

bool parsingScriptFile;
SgProject* project;
SgScopeStatement* currentScope; //the current scope on the top of scope stack
int current_function_depth;
//SgFunctionDeclaration* defaultFunction;

#define ABORT_PARSE \
  do \
    { \
      yyerrok; \
        YYABORT; \
    } \
  while (0) 
%}

// Bison declarations.

// Don't add spaces around the = here; it causes some versions of
// bison to fail to properly recognize the directive.

//%name-prefix="matlab_"

%defines "matlab_parse.h"

%union
{
  // The type of the basic tokens returned by the lexer.
  std::string *Name_type;
  PreprocessingInfo *Comment_type;
  char sep_type;
  SgMatrixExp* Matrix_type;
  SgFunctionRefExp* FunctionRefExp_type;
  SgVarRefExp* VarRefExp_type;
  SgExpression* Expression_type;
  SgRangeExp* Colon_Expression_type;
  SgMagicColonExp* Symbols_type; //For magic_colon
  SgFunctionParameterList* Function_Parameter_List_type;
  SgExprListExp* ExprListExp_type;
  SgValueExp* VauleExp_type;
  SgAggregateInitializer* Aggregate_type;
  SgFunctionDeclaration* Function_type;
  SgScopeStatement* Loop_type;
  SgIfStmt* IfStatement_type;
  //SgMatlabDeclarationStatement* DeclarationStatement_type;
  SgStatement* Statement_type;
  SgStatement* Block_type;
  MatlabFunctionBuilder* FunctionBuilder_type;
  StatementList* StatementList_type;
  token* tok_val;
/*
  // Comment strings that we need to deal with mid-rule.
  octave_comment_list *comment_type;

  // Types for the nont`erminals we generate.
  char sep_type;
  tree *tree_type;
  tree_matrix *tree_matrix_type;
  tree_cell *tree_cell_type;
  tree_expression *tree_expression_type;
  tree_constant *tree_constant_type;
  tree_fcn_handle *tree_fcn_handle_type;
  tree_anon_fcn_handle *tree_anon_fcn_handle_type;
  tree_identifier *tree_identifier_type;
  tree_index_expression *tree_index_expression_type;
  tree_colon_expression *tree_colon_expression_type;
  tree_argument_list *tree_argument_list_type;
  tree_parameter_list *tree_parameter_list_type;
  tree_command *tree_command_type;
  tree_if_command *tree_if_command_type;
  tree_if_clause *tree_if_clause_type;
  tree_if_command_list *tree_if_command_list_type;
  tree_switch_command *tree_switch_command_type;
  tree_switch_case *tree_switch_case_type;
  tree_switch_case_list *tree_switch_case_list_type;
  tree_decl_elt *tree_decl_elt_type;
  tree_decl_init_list *tree_decl_init_list_type;
  tree_decl_command *tree_decl_command_type;
  tree_statement *tree_statement_type;
  tree_statement_list *tree_statement_list_type;
  octave_user_function *octave_user_function_type;
  void *dummy_type;*/
}

// Tokens with line and column information.
%token <tok_val> '=' ':' '-' '+' '*' '/'
%token <tok_val> ADD_EQ SUB_EQ MUL_EQ DIV_EQ LEFTDIV_EQ POW_EQ
%token <tok_val> EMUL_EQ EDIV_EQ ELEFTDIV_EQ EPOW_EQ AND_EQ OR_EQ
%token <tok_val> LSHIFT_EQ RSHIFT_EQ LSHIFT RSHIFT
%token <tok_val> EXPR_AND_AND EXPR_OR_OR
%token <tok_val> EXPR_AND EXPR_OR EXPR_NOT
%token <tok_val> EXPR_LT EXPR_LE EXPR_EQ EXPR_NE EXPR_GE EXPR_GT
%token <tok_val> LEFTDIV EMUL EDIV ELEFTDIV EPLUS EMINUS
%token <tok_val> QUOTE TRANSPOSE
%token <tok_val> PLUS_PLUS MINUS_MINUS POW EPOW
%token <tok_val> NUM IMAG_NUM
%token <tok_val> STRUCT_ELT
%token <tok_val> NAME
%token <tok_val> END
%token <tok_val> DQ_STRING SQ_STRING
%token <tok_val> FOR WHILE DO UNTIL
%token <tok_val> IF ELSEIF ELSE
%token <tok_val> SWITCH CASE OTHERWISE
%token <tok_val> BREAK CONTINUE FUNC_RET
%token <tok_val> UNWIND CLEANUP
%token <tok_val> TRY CATCH
%token <tok_val> GLOBAL STATIC
%token <tok_val> FCN_HANDLE
%token <tok_val> PROPERTIES
%token <tok_val> METHODS
%token <tok_val> EVENTS
%token <tok_val> METAQUERY
%token <tok_val> SUPERCLASSREF
%token <tok_val> GET SET
%token <tok_val> TYPE

// Other tokens.
%token END_OF_INPUT LEXICAL_ERROR
%token FCN SCRIPT_FILE FUNCTION_FILE CLASSDEF 
// %token VARARGIN VARARGOUT
%token CLOSE_BRACE
//%token TYPE

%type <Comment_type> stash_comment function_beg
%type <sep_type> sep_no_nl opt_sep_no_nl sep opt_sep
%type <StatementList_type> input
%type <VauleExp_type> string constant
%type <Symbols_type> magic_colon
%type <FunctionRefExp_type> fcn_handle
%type <Matrix_type> matrix_rows matrix_rows1
//%type <ExprListExp_type> matrix_rows matrix_rows1
%type <ExprListExp_type> cell_rows cell_rows1
%type <Matrix_type> matrix
//%type <Expression_type> matrix cell assign_lhs
%type <Expression_type> cell assign_lhs
%type <Expression_type> primary_expr postfix_expr prefix_expr binary_expr
%type <Expression_type> simple_expr colon_expr assign_expr expression
%type <VarRefExp_type> identifier// fcn_name
%type <Name_type> fcn_name
%type <FunctionBuilder_type> function1 function2
%type <Statement_type> word_list_cmd
%type <Colon_Expression_type> colon_expr1
%type <ExprListExp_type> arg_list word_list //assign_lhs
//%type <Aggregate_type> cell_or_matrix_row
%type <ExprListExp_type> cell_or_matrix_row
%type <Function_Parameter_List_type> param_list param_list1 param_list2
%type <ExprListExp_type> return_list return_list1
%type <Block_type> command
%type <StatementList_type> script_file
%type <Loop_type> loop_command
%type <Function_type> function
%type <IfStatement_type> if_command
%type <Statement_type> else_clause
%type <IfStatement_type> elseif_clause if_cmd_list1 if_cmd_list
%type <Expression_type> decl2
%type <ExprListExp_type> decl1
 //%type <DeclarationStatement_type> declaration
%type <Statement_type> statement
//%type <Block_type> simple_list simple_list1 list list1
//%type <Block_type> opt_list input1
%type <StatementList_type> simple_list simple_list1 list list1
%type <StatementList_type> opt_list input1

/*
// Nonterminals we construct.
%type <comment_type> stash_comment function_beg classdef_beg
%type <comment_type> properties_beg methods_beg events_beg
%type <sep_type> sep_no_nl opt_sep_no_nl sep opt_sep
%type <tree_type> input
%type <tree_constant_type> string constant magic_colon
%type <tree_anon_fcn_handle_type> anon_fcn_handle
%type <tree_fcn_handle_type> fcn_handle
%type <tree_matrix_type> matrix_rows matrix_rows1
%type <tree_cell_type> cell_rows cell_rows1
%type <tree_expression_type> matrix cell
%type <tree_expression_type> primary_expr postfix_expr prefix_expr binary_expr
%type <tree_expression_type> simple_expr colon_expr assign_expr expression
%type <tree_identifier_type> identifier fcn_name magic_tilde
%type <tree_identifier_type> superclass_identifier meta_identifier
%type <octave_user_function_type> function1 function2 classdef1
%type <tree_index_expression_type> word_list_cmd
%type <tree_colon_expression_type> colon_expr1
%type <tree_argument_list_type> arg_list word_list assign_lhs
%type <tree_argument_list_type> cell_or_matrix_row
%type <tree_parameter_list_type> param_list param_list1 param_list2
%type <tree_parameter_list_type> return_list return_list1
%type <tree_parameter_list_type> superclasses opt_superclasses
%type <tree_command_type> command select_command loop_command
%type <tree_command_type> jump_command except_command function
%type <tree_command_type> script_file classdef
%type <tree_command_type> function_file function_list
%type <tree_if_command_type> if_command
%type <tree_if_clause_type> elseif_clause else_clause
%type <tree_if_command_list_type> if_cmd_list1 if_cmd_list
%type <tree_switch_command_type> switch_command
%type <tree_switch_case_type> switch_case default_case
%type <tree_switch_case_list_type> case_list1 case_list
%type <tree_decl_elt_type> decl2
%type <tree_decl_init_list_type> decl1
%type <tree_decl_command_type> declaration
%type <tree_statement_type> statement function_end classdef_end
%type <tree_statement_list_type> simple_list simple_list1 list list1
%type <tree_statement_list_type> opt_list input1


// These types need to be specified.
%type <dummy_type> attr
%type <dummy_type> class_event
%type <dummy_type> class_property
%type <dummy_type> properties_list
%type <dummy_type> properties_block
%type <dummy_type> methods_list
%type <dummy_type> methods_block
%type <dummy_type> opt_attr_list
%type <dummy_type> attr_list
%type <dummy_type> events_list
%type <dummy_type> events_block
%type <dummy_type> class_body
//  symrec *tptr;
*/
// Precedence and associativity.
%left ';' ',' '\n'
%right '=' ADD_EQ SUB_EQ MUL_EQ DIV_EQ LEFTDIV_EQ POW_EQ EMUL_EQ EDIV_EQ ELEFTDIV_EQ EPOW_EQ OR_EQ AND_EQ LSHIFT_EQ RSHIFT_EQ
%left EXPR_OR_OR
%left EXPR_AND_AND
%left EXPR_OR
%left EXPR_AND
%left EXPR_LT EXPR_LE EXPR_EQ EXPR_NE EXPR_GE EXPR_GT
%left LSHIFT RSHIFT
%left ':'
%left '-' '+' EPLUS EMINUS
%left '*' '/' LEFTDIV EMUL EDIV ELEFTDIV
%left UNARY PLUS_PLUS MINUS_MINUS EXPR_NOT
%left POW EPOW QUOTE TRANSPOSE
%left '(' '.' '{'

// Where to start.
%start input

%%

// ==============================
// Statements and statement lists
// ==============================

input           : input1
	   	  {                                           
                  std::cout << "END OF current input" << std::endl << std::flush;                                                                                       

                  $1->appendAll();

                  if(parsingScriptFile)
                  {
                    //addVariableDeclarations(); 

                    SageBuilder::popScopeStack(); //pop out the default function

		       //Create a default "run" function
		    SgFunctionDeclaration *defaultFunction = SageBuilder::buildDefiningFunctionDeclaration("run", 
                      SageBuilder::buildVoidType(), SageBuilder::buildFunctionParameterList(), 
                      SageInterface::getFirstGlobalScope(project));

		    SageInterface::removeStatement(defaultFunction->get_definition()->get_body());
		    
		    defaultFunction->get_definition()->set_body(isSgBasicBlock(currentScope));
		    
                    //Add the default function to global scope
                    SageInterface::appendStatement(defaultFunction);
                  }

                  //Pop the global stack
                  SageBuilder::popScopeStack();

		  YYACCEPT;
		  globalCommand = true;
		 }                 
                | function_file
                 {YYACCEPT;}
                | simple_list parse_error
                 {ABORT_PARSE;}
                | parse_error
                 {ABORT_PARSE;}
                ;

input1          : '\n'
                 {$$ = 0;}
                | END_OF_INPUT
                 {
                    std::cout << "END OF FILE" << std::endl << std::flush;
                    parser_end_of_input = 1;
                    $$ = 0;
                 }
                | simple_list
                 {$$ = $1;}
                | simple_list '\n'
                 {$$ = $1;}
                | simple_list END_OF_INPUT
                 {$$ = $1;}
                ;

simple_list     : simple_list1 opt_sep_no_nl
                  { $$ = $1;}
                  /*{ $$ = set_stmt_print_flag ($1, $2, false);}*/
                ;

simple_list1    : statement
                  //{$$ = buildBasicBlock($1);}                  
                  { 
                    $$ = dynamic_cast<StatementList*>($1); 

                    assert($$ != NULL);

                    //ROSE_ASSERT($$ != NULL);
                  }
                | simple_list1 sep_no_nl statement
                  { 
                    ROSE_ASSERT(false);
                    /*SgBasicBlock *block;
                    if(!(block = dynamic_cast<SgBasicBlock*>($1)))
                    {
                       block = buildBasicBlock($1);
                    }

                    block->append_statement($3);
                    $$ = block;assert($$ != NULL);*/
                  }                  
                ;

opt_list        : // empty
                  { $$ = new StatementList();}
                  /*{ $$ = new tree_statement_list ();}*/
                | list
                  { 
                    $$ = $1;
                    //SageInterface::appendStatement($1);
                  }
                ;

list            : list1 opt_sep
                  { $$ = $1;}
                  /*{ $$ = set_stmt_print_flag ($1, $2, true);}*/
                ;

list1           : statement
                  {
            		    ROSE_ASSERT($1 != NULL);
            		    $$ = new StatementList($1);                    
            		  }                  
                | list1 sep statement
                  { 
                    $1->appendStatement($3);
                    $$ = $1;

                    assert($$ != NULL);
                  }                  
                ;

statement       : expression
                  {$$ = SageBuilder::buildExprStatement($1);}                  
                | command
                  {$$ = $1;}                  
                | word_list_cmd
                  {
            		    $$ = $1;
            		    ROSE_ASSERT($$ != NULL);
                  }
                ;

// =================
// Word-list command
// =================

// These are not really like expressions since they can't appear on
// the RHS of an assignment.  But they are also not like commands (IF,
// WHILE, etc.

word_list_cmd   : identifier word_list
                  { 
                    std::cout << "Making word_list_cmd" << std::endl << std::flush;                    
		    
		    //$$ = SageMatlabBuilder::buildWordListStatement($1->get_symbol()->get_name(), $2);

		                ROSE_ASSERT($$ != NULL);
                  }
                  /*{ $$ = make_index_expression ($1, $2, '(');}*/
                ;

word_list       : string
                  { $$ = SageBuilder::buildExprListExp($1);}                  
                | word_list string
                  {
		                ROSE_ASSERT(false);
                    std::cout << "Appending " <<   $2 << std::endl << std::flush;                  
                    SageInterface::appendExpression($1, $2);
                    $$ = $1;
                  }                  
                ;

// ===========
// Expressions
// ===========

identifier      : NAME
      		 {
		   std::string varName = $1->text();

		   SgScopeStatement *activeScope = SageBuilder::topScopeStack();

		   SgVariableSymbol *varSymbol = SageInterface::lookupVariableSymbolInParentScopes(varName, activeScope);
		   // if(activeScope->symbol_exists(varName) == false)
		   if(varSymbol == NULL)
                 {
		   SgVarRefExp *varRef = SageBuilder::buildVarRefExp(varName, activeScope);

		   /*If there is no explicit variable declaration, we have to manually insert the symbol to the symbol table*/
                   activeScope->get_symbol_table()->insert(varName, varRef->get_symbol());

		   std::cout << "Symbol " << varName << " inserted" << std::endl << std::flush;
		   
		   $$ = varRef;
                 }
		 else
		   {
		     /*The symbol already exists. The varref will point to the existing symbol.*/
		     $$ = SageBuilder::buildVarRefExp(varName, activeScope);
		   }
      		 }
                ;

superclass_identifier
                : SUPERCLASSREF
                  /*{ $$ = new tree_identifier ($1->line (), $1->column ());}*/
                ;

meta_identifier : METAQUERY
                  /*{ $$ = new tree_identifier ($1->line (), $1->column ());}*/
                ;

string          : DQ_STRING
                  { $$ = SageBuilder::buildStringVal($1->text());}
                  /*{ $$ = make_constant (DQ_STRING, $1);}*/
                | SQ_STRING
                  { $$ = SageBuilder::buildStringVal($1->text());}
                  /*{ $$ = make_constant (SQ_STRING, $1);}*/
                ;

constant        : NUM
                 { $$ = SageBuilder::buildDoubleVal($1->number());}
                  /*{ $$ = make_constant (NUM, $1);}*/
                | IMAG_NUM
		{ $$ = SageBuilder::buildImaginaryVal ($1->number()); }
                  /*{ $$ = make_constant (IMAG_NUM, $1);}*/                
                | string
                  { $$ = $1;}
                ;

matrix          : '[' ']'
                  {
                    //$$ = new tree_constant (octave_null_matrix::instance);
                    lexer_flags.looking_at_matrix_or_assign_lhs = false;
                    lexer_flags.pending_local_variables.clear ();
                 }
                | '[' ';' ']'
                  {
                    //$$ = new tree_constant (octave_null_matrix::instance);
                    lexer_flags.looking_at_matrix_or_assign_lhs = false;
                    lexer_flags.pending_local_variables.clear ();
                 }
                | '[' ',' ']'
                  {
                    //$$ = new tree_constant (octave_null_matrix::instance);
                    lexer_flags.looking_at_matrix_or_assign_lhs = false;
                    lexer_flags.pending_local_variables.clear ();
                 }
                | '[' matrix_rows ']'
                  {
                    $$ = $2;//SageBuilder::buildMatrix($2);//buildAggregateInitializer($2);assert($$ != NULL);
                    //$$ = finish_matrix ($2);
                    lexer_flags.looking_at_matrix_or_assign_lhs = false;
                    lexer_flags.pending_local_variables.clear ();
                 }
                ;

matrix_rows     : matrix_rows1
                  { $$ = $1;}
                | matrix_rows1 ';'      // Ignore trailing semicolon.
                  { $$ = $1;}
                ;

matrix_rows1    : cell_or_matrix_row
                  {
                    //$$ = SageMatlabBuilder::buildMatrix($1);
		    $$ = SageBuilder::buildMatrixExp($1);
                    //$$ = buildExprListExp($1);assert($$ != NULL);
                  }
                  /*{ $$ = new tree_matrix ($1);}*/
                | matrix_rows1 ';' cell_or_matrix_row
                  {
                    $1->append_expression($3);
                    //SageInterface::appendExpression($1, $3);
                    //$1->append_expression($3);
                    $$ = $1;
		    assert($$ != NULL);
                  }
                  /*{
                    $1->append ($3);
                    $$ = $1;
                 }*/
                ;

cell            : '{' '}'
                  /*{ $$ = new tree_constant (octave_value (Cell ()));}*/
                | '{' ';' '}'
                  /*{ $$ = new tree_constant (octave_value (Cell ()));}*/
                | '{' cell_rows '}'
                   {
		                  $$ = SageBuilder::buildAggregateInitializer($2);
                   }
		  /*{$$ = finish_cell ($2);}*/
                ;

cell_rows       : cell_rows1
                 {$$ = $1;}
                | cell_rows1 ';'        // Ignore trailing semicolon.
                 {$$ = $1;}
                ;

cell_rows1      : cell_or_matrix_row
                  {
		    $$ = SageBuilder::buildExprListExp($1);
                  }
            		  /*{$$ = new tree_cell ($1);}*/
                  | cell_rows1 ';' cell_or_matrix_row
            		  {
            		    SageInterface::appendExpression($1, $3);
            		    $$ = $1;
            		  }
		  /*{
                    $1->append ($3);
                    $$ = $1;
                 }*/
                ;

cell_or_matrix_row
                : arg_list
                {
                  $$ = $1;
                }
                 //{$$ = buildAggregateInitializer($1);assert($$ != NULL);}                
                | arg_list ','  // Ignore trailing comma.
                 {
                   $$ = $1;
                 }
                 //{$$ = buildAggregateInitializer($1);assert($$ != NULL);}                 
                ;

fcn_handle      : '@' FCN_HANDLE
                 {
            	  SgName fcnName($2->text());
            		   
            	  SgFunctionDeclaration* fcnDeclaration = SageBuilder::buildNondefiningFunctionDeclaration(fcnName, SageBuilder::buildUnknownType(), SageBuilder::buildFunctionParameterList(), currentScope);
            		   
            	   SgFunctionSymbol* fcnSymbol = new SgFunctionSymbol(fcnDeclaration);
                   $$ = SageBuilder::buildFunctionRefExp(fcnSymbol); 
		
                    lexer_flags.looking_at_function_handle--;
                 }
                ;

anon_fcn_handle : '@' param_list statement
                 /*{$$ = make_anon_fcn_handle ($2, $3);}*/
                ;

primary_expr    : identifier
                 {$$ = $1;}
                | constant
                 {$$ = $1;}
                | fcn_handle
                 /*{$$ = $1;}*/
                | matrix
                 {$$ = $1;}
                | cell
                 {$$ = $1;}
                | meta_identifier
                 /*{$$ = $1;}*/
                | superclass_identifier
                 /*{$$ = $1;}*/
                | '(' expression ')' //TODO
                  { $$ = $2; }
                 /*{$$ = $2->mark_in_parens ();}*/                  
                ;

magic_colon     : ':'
                  {
		    $$ = SageBuilder::buildMagicColonExp();
                    // $$ = new SgMagicColonExpression();
                    // SageMatlabBuilder::setOneSourcePositionForTransformation($$);
                  }
                 /*{
                    octave_value tmp (octave_value::magic_colon_t);
                    $$ = new tree_constant (tmp);
                 }*/
                ;

magic_tilde     : EXPR_NOT
                 /*{
                    $$ = new tree_black_hole ();
                 }*/
                ;

arg_list        : expression
                  {$$ = SageBuilder::buildExprListExp($1);}
                 /*{$$ = new tree_argument_list ($1);}*/
                | magic_colon
		{$$ = SageBuilder::buildExprListExp ($1);}
                | magic_tilde
                 /*{$$ = new tree_argument_list ($1);}*/
                | arg_list ',' magic_colon
                 {
                    $1->append_expression ($3);
                    $$ = $1;
                 }
                | arg_list ',' magic_tilde
                 /*{
                    $1->append ($3);
                    $$ = $1;
                 }*/
                | arg_list ',' expression
                 {
                    $1->append_expression ($3);
                    $$ = $1;
                 }
                ;

indirect_ref_op : '.'
                 {lexer_flags.looking_at_indirect_ref = true;}
                ;

postfix_expr    : primary_expr
                 {$$ = $1;}
                | postfix_expr '(' ')'
                  { $$ = SageBuilder::buildFunctionCallExp($1);}
                 /*{$$ = make_index_expression ($1, 0, '(');}*/                  
                | postfix_expr '(' arg_list ')'
                  { 
                    //Treat every indexed operation as a function call
                    $$ = SageBuilder::buildFunctionCallExp($1, $3);
                  } 
                 /*{$$ = make_index_expression ($1, $3, '(');}*/
                | postfix_expr '{' '}'
		  { $$ = SageBuilder::buildFunctionCallExp($1);}
                 /*{$$ = make_index_expression ($1, 0, '{');}*/
                | postfix_expr '{' arg_list '}'
		  { $$ = SageBuilder::buildFunctionCallExp($1, $3); }
		  /*{$$ = make_index_expression ($1, $3, '{');}*/
                | postfix_expr PLUS_PLUS
                  {$$ = SageBuilder::buildPlusPlusOp($1, SgUnaryOp::postfix);}
                 /*{$$ = make_postfix_op (PLUS_PLUS, $1, $2);}*/                  
                | postfix_expr MINUS_MINUS
                  {$$ = SageBuilder::buildMinusMinusOp($1, SgUnaryOp::postfix);}
                 /*{$$ = make_postfix_op (MINUS_MINUS, $1, $2);}*/
                | postfix_expr QUOTE
                  {
		    SgMatrixTransposeOp *transposeOp = SageBuilder::buildMatrixTransposeOp($1);
		    transposeOp->set_is_conjugate(true);

		    $$ = transposeOp;
		  } 
                | postfix_expr TRANSPOSE
		{
		  $$ = SageBuilder::buildMatrixTransposeOp($1);
		}
                | postfix_expr indirect_ref_op STRUCT_ELT                                                
                 {$$ = make_indirect_ref ($1, SageBuilder::buildVarRefExp($3->text()));}
                | postfix_expr indirect_ref_op '(' expression ')'
                 {$$ = make_indirect_ref ($1, $4);}                 
                ;

prefix_expr     : postfix_expr
                 {$$ = $1;}
                | binary_expr
                 {$$ = $1;}
                | PLUS_PLUS prefix_expr %prec UNARY
                {
                  $$ = SageBuilder::buildPlusPlusOp($2, SgUnaryOp::prefix);
                 /*Turns out that Matlab does not support ++,-- operators*/
                }
                | MINUS_MINUS prefix_expr %prec UNARY
                {
                  $$ = SageBuilder::buildMinusMinusOp($2, SgUnaryOp::prefix);
                 /*{$$ = make_prefix_op (MINUS_MINUS, $2, $1);}*/
                }
                | EXPR_NOT prefix_expr %prec UNARY
                { $$ = SageBuilder::buildNotOp($2);}
                 /*{$$ = make_prefix_op (EXPR_NOT, $2, $1);}*/                
                | '+' prefix_expr %prec UNARY
                { $$ = $2;}
                 /*{$$ = make_prefix_op ('+', $2, $1);}*/                
                | '-' prefix_expr %prec UNARY
                { $$ = SageBuilder::buildMinusOp($2, SgUnaryOp::prefix);}
                 /*{$$ = make_prefix_op ('-', $2, $1);}*/                
                ;

binary_expr     : prefix_expr POW prefix_expr
                 {$$ = SageBuilder::buildPowerOp($1, $3);}
                | prefix_expr EPOW prefix_expr
                 {$$ = SageBuilder::buildElementwisePowerOp($1, $3);}
                | prefix_expr '+' prefix_expr
                   {$$ = SageBuilder::buildAddOp($1, $3);}                 
                | prefix_expr '-' prefix_expr
                 {$$ = SageBuilder::buildSubtractOp($1, $3);}
                | prefix_expr '*' prefix_expr
                 {$$ = SageBuilder::buildMultiplyOp($1, $3);}
                | prefix_expr '/' prefix_expr
                 {$$ = SageBuilder::buildDivideOp($1, $3);}
                | prefix_expr EPLUS prefix_expr                
                { $$ = SageBuilder::buildElementwiseAddOp($1, $3);}                 
                | prefix_expr EMINUS prefix_expr
                { $$ = SageBuilder::buildElementwiseSubtractOp($1, $3);}                
                | prefix_expr EMUL prefix_expr
                { $$ = SageBuilder::buildElementwiseMultiplyOp($1, $3);}                
                | prefix_expr EDIV prefix_expr
                { $$ = SageBuilder::buildElementwiseDivideOp($1, $3);}                
                | prefix_expr LEFTDIV prefix_expr
                 { $$ = SageBuilder::buildLeftDivideOp($1, $3);}                
                | prefix_expr ELEFTDIV prefix_expr
                 { $$ = SageBuilder::buildElementwiseLeftDivideOp($1, $3);}
                ;

colon_expr      : prefix_expr
                  {
                    $$ = $1;
                  }
                  | colon_expr1					       
                  {
                    $$ = $1;
                  }
                 /*{$$ = finish_colon_expression ($1);}*/
                ;

colon_expr1     : prefix_expr					       
                 {
		   $$ = SageBuilder::buildRangeExp($1);
                   /* SgColonExpression* colonexp = new SgColonExpression ($1);
		    SageMatlabBuilder::setOneSourcePositionForTransformation(colonexp);

                    $$ = colonexp;

                    std::cout << "setOneSourcePositionForTransformation" << std::endl << std::flush;*/
                 }
                | colon_expr1 ':' prefix_expr                 
                 {
                    if(($$ = $1->append($3)) == NULL)
                      ABORT_PARSE;                    
                 }
                ;

simple_expr     : colon_expr
                 {$$ = $1;}
                | simple_expr LSHIFT simple_expr
                 {
                  $$ = SageBuilder::buildLshiftOp($1, $3);
                 }                 
                | simple_expr RSHIFT simple_expr
                {
                  $$ = SageBuilder::buildRshiftOp($1, $3);
                }
                 /*{$$ = make_binary_op (RSHIFT, $1, $2, $3);}*/              
                | simple_expr EXPR_LT simple_expr
                {
                  $$ = SageBuilder::buildLessThanOp($1, $3);
                }
                 /*{$$ = make_binary_op (EXPR_LT, $1, $2, $3);}*/
                | simple_expr EXPR_LE simple_expr
                {
                  $$ = SageBuilder::buildLessOrEqualOp($1, $3);
                }
                 /*{$$ = make_binary_op (EXPR_LE, $1, $2, $3);}*/
                | simple_expr EXPR_EQ simple_expr
                 {
                  $$ = SageBuilder::buildEqualityOp($1, $3);
                 }
                 /*{$$ = make_binary_op (EXPR_EQ, $1, $2, $3);}*/
                 
                | simple_expr EXPR_GE simple_expr
                {
                  $$ = SageBuilder::buildGreaterOrEqualOp($1, $3);
                 }
                 /*{$$ = make_binary_op (EXPR_GE, $1, $2, $3);}*/
                | simple_expr EXPR_GT simple_expr
                {
                  $$ = SageBuilder::buildGreaterThanOp($1, $3);
                 }
                 /*{$$ = make_binary_op (EXPR_GT, $1, $2, $3);}*/
                | simple_expr EXPR_NE simple_expr
                {
                  $$ = SageBuilder::buildNotEqualOp($1, $3);
                 }
                 /*{$$ = make_binary_op (EXPR_NE, $1, $2, $3);}*/
                | simple_expr EXPR_AND simple_expr
                {
                  $$ = SageBuilder::buildBitAndOp($1, $3);
                 }
                 /*{$$ = make_binary_op (EXPR_AND, $1, $2, $3);}*/
                | simple_expr EXPR_OR simple_expr
                 {
                  $$ = SageBuilder::buildBitOrOp($1, $3);
                 }
                 /*{$$ = make_binary_op (EXPR_OR, $1, $2, $3);}*/
                | simple_expr EXPR_AND_AND simple_expr
                 {
                  $$ = SageBuilder::buildAndOp($1, $3);
                 }
                 /*{$$ = make_boolean_op (EXPR_AND_AND, $1, $2, $3);}*/
                | simple_expr EXPR_OR_OR simple_expr
                {
                  $$ = SageBuilder::buildOrOp($1, $3);
                 }
                 /*{$$ = make_boolean_op (EXPR_OR_OR, $1, $2, $3);}*/
                ;

// Arrange for the lexer to return CLOSE_BRACE for `]' by looking ahead
// one token for an assignment op.

assign_lhs      : simple_expr
				        {					         
                   $$ = $1;                          
				        }
                 /*{
                    $$ = new tree_argument_list ($1);
                    $$->mark_as_simple_assign_lhs ();
                 }*/
                | '[' arg_list CLOSE_BRACE
                 {                                        
                    $$ = $2;
                    lexer_flags.looking_at_matrix_or_assign_lhs = false;
                    /*for (std::set<std::string>::const_iterator p = lexer_flags.pending_local_variables.begin ();
                         p != lexer_flags.pending_local_variables.end ();
                         p++)
                     {
                        symbol_table::force_variable (*p);
                     }*/
                    lexer_flags.pending_local_variables.clear ();
                 }
                ;

assign_expr     : assign_lhs '=' expression
                 {                   
                   $$ = SageBuilder::buildAssignOp($1, $3);                     
                 }
                 /*{$$ = make_assign_op ('=', $1, $2, $3);}*/
                | assign_lhs ADD_EQ expression
                 /*{$$ = make_assign_op (ADD_EQ, $1, $2, $3);}*/
                | assign_lhs SUB_EQ expression
                 /*{$$ = make_assign_op (SUB_EQ, $1, $2, $3);}*/
                | assign_lhs MUL_EQ expression
                 /*{$$ = make_assign_op (MUL_EQ, $1, $2, $3);}*/
                | assign_lhs DIV_EQ expression
                 /*{$$ = make_assign_op (DIV_EQ, $1, $2, $3);}*/
                | assign_lhs LEFTDIV_EQ expression
                 /*{$$ = make_assign_op (LEFTDIV_EQ, $1, $2, $3);}*/
                | assign_lhs POW_EQ expression
                 /*{$$ = make_assign_op (POW_EQ, $1, $2, $3);}*/
                | assign_lhs LSHIFT_EQ expression
                 /*{$$ = make_assign_op (LSHIFT_EQ, $1, $2, $3);}*/
                | assign_lhs RSHIFT_EQ expression
                 /*{$$ = make_assign_op (RSHIFT_EQ, $1, $2, $3);}*/
                | assign_lhs EMUL_EQ expression
                 /*{$$ = make_assign_op (EMUL_EQ, $1, $2, $3);}*/
                | assign_lhs EDIV_EQ expression
                 /*{$$ = make_assign_op (EDIV_EQ, $1, $2, $3);}*/
                | assign_lhs ELEFTDIV_EQ expression
                 /*{$$ = make_assign_op (ELEFTDIV_EQ, $1, $2, $3);}*/
                | assign_lhs EPOW_EQ expression
                 /*{$$ = make_assign_op (EPOW_EQ, $1, $2, $3);}*/
                | assign_lhs AND_EQ expression
                 /*{$$ = make_assign_op (AND_EQ, $1, $2, $3);}*/
                | assign_lhs OR_EQ expression
                 /*{$$ = make_assign_op (OR_EQ, $1, $2, $3);}*/
                ;

expression      : simple_expr
                 {$$ = $1;}
                | assign_expr
                 {$$ = $1;}
                | anon_fcn_handle
                 /*{$$ = $1;}*/
                ;

// ================================================
// Commands, declarations, and function definitions
// ================================================

command         :// declaration
// {$$ = $1;}
                 // |
		select_command
                 /*{$$ = $1;}*/
                | loop_command
                 /*{$$ = $1;}*/
                | jump_command
                 /*{$$ = $1;}*/
                | except_command
                 /*{$$ = $1;}*/
                | function
                 {$$ = $1;}
                | script_file
                   {$$ = $1;}
                | classdef
                 /*{$$ = $1;}*/                       
                ;

// =====================
// Declaration statemnts
// =====================

parsing_decl_list
                : // empty
                 {lexer_flags.looking_at_decl_list = true;}

declaration     : GLOBAL parsing_decl_list decl1
                 {
                   //$$ = SageMatlabBuilder::buildGlobalDeclarationStatement($3); 
		   //$$ = make_decl_command (GLOBAL, $1, $3);
                    lexer_flags.looking_at_decl_list = false;
                 }
                | STATIC parsing_decl_list decl1
                 {
		   //$$ = SageMatlabBuilder::buildStaticDeclarationStatement($3);
                    //$$ = make_decl_command (STATIC, $1, $3);
                    lexer_flags.looking_at_decl_list = false;
                 }
                ;

decl1           : decl2
                  {
		    $$ = SageBuilder::buildExprListExp($1);
                  }
                 /*{$$ = new tree_decl_init_list ($1);}*/
                | decl1 decl2
            	  {
            	    SageInterface::appendExpression($1, $2);
            	  }
                ;

decl_param_init : // empty
               {lexer_flags.looking_at_initializer_expression = true;}

decl2           : identifier
                  { $$ = $1;}
                 /*{$$ = new tree_decl_elt ($1);}*/
                | identifier '=' decl_param_init expression
                 {
                    lexer_flags.looking_at_initializer_expression = false;
                     $$ = SageBuilder::buildAssignOp($1, $4);
                 }
                | magic_tilde
                 /*{
                    $$ = new tree_decl_elt ($1);
                 }*/
                ;

// ====================
// Selection statements
// ====================

select_command  : if_command
                 /*{$$ = $1;}*/
                | switch_command
                 /*{$$ = $1;}*/
                ;

// ============
// If statement
// ============

if_command      : IF stash_comment if_cmd_list END
                 {                   
                    $$ = $3;
                 }                 
                ;

if_cmd_list     : if_cmd_list1
                  {$$ = $1;}
                | if_cmd_list1 else_clause
                 {
                  SgIfStmt* lastIfClause = getLastIfClause($1);

                  lastIfClause->set_false_body($2);
                  $$ = $1;                    
                 }
                ;

if_cmd_list1    : expression opt_sep opt_list
                 {
		   $$ = SageBuilder::buildIfStmt($1, $3->getBasicBlock(), NULL);                    
                 }                 
                | if_cmd_list1 elseif_clause
                {                         
                  SgIfStmt* lastIfClause = getLastIfClause($1);

                  lastIfClause->set_false_body($2);
                  $$ = $1;
                }                 
                ;

elseif_clause   : ELSEIF stash_comment opt_sep expression opt_sep opt_list
                 {                  
		   $$ = SageBuilder::buildIfStmt($4, $6->getBasicBlock(), NULL);
                 }                 
                ;

else_clause     : ELSE stash_comment opt_sep opt_list
                  {
                    $$ = $4->getBasicBlock(); //Ignore the comments
                  }                 
                ;

// ================
// Switch statement
// ================

switch_command  : SWITCH stash_comment expression opt_sep case_list END
                 /*{
                    if (! ($$ = finish_switch_command ($1, $3, $5, $6, $2)))
                      ABORT_PARSE;
                 }*/
                ;

case_list       : // empty
                 /*{$$ = new tree_switch_case_list ();}*/
                | default_case
                 /*{$$ = new tree_switch_case_list ($1);}*/
                | case_list1
                 /*{$$ = $1;}*/
                | case_list1 default_case
                 /*{
                    $1->append ($2);
                    $$ = $1;
                 }*/
                ;

case_list1      : switch_case
                 /*{$$ = new tree_switch_case_list ($1);}*/
                | case_list1 switch_case
                 /*{
                    $1->append ($2);
                    $$ = $1;
                 }*/
                ;

switch_case     : CASE stash_comment opt_sep expression opt_sep opt_list
                 /*{$$ = make_switch_case ($1, $4, $6, $2);}*/
                ;

default_case    : OTHERWISE stash_comment opt_sep opt_list
                 /*{
                    $$ = new tree_switch_case ($4, $2);
                 }*/
                ;

// =======
// Looping
// =======

loop_command    : WHILE stash_comment expression opt_sep opt_list END
                 /*{
                    $3->mark_braindead_shortcircuit (curr_fcn_file_full_name);

                    if (! ($$ = make_while_command ($1, $3, $5, $6, $2)))
                      ABORT_PARSE;
                 }*/
                | DO stash_comment opt_sep opt_list UNTIL expression
                 /*{
                    if (! ($$ = make_do_until_command ($5, $4, $6, $2)))
                      ABORT_PARSE;
                 }*/
                | FOR stash_comment assign_lhs '=' expression opt_sep opt_list END
                 {
		   $$ = SageBuilder::buildMatlabForStatement($3, $5, $7->getBasicBlock());
                 }
                | FOR stash_comment '(' assign_lhs '=' expression ')' opt_sep opt_list END
                 {
		   $$ = SageBuilder::buildMatlabForStatement($4, $6, $9->getBasicBlock());
                 }
                ;

// =======
// Jumping
// =======

jump_command    : BREAK
                 /*{
                    if (! ($$ = make_break_command ($1)))
                      ABORT_PARSE;
                 }*/
                | CONTINUE
                 /*{
                    if (! ($$ = make_continue_command ($1)))
                      ABORT_PARSE;
                 }*/
                | FUNC_RET
                 /*{
                    if (! ($$ = make_return_command ($1)))
                      ABORT_PARSE;
                 }*/
                ;

// ==========
// Exceptions
// ==========

except_command  : UNWIND stash_comment opt_sep opt_list CLEANUP
                  stash_comment opt_sep opt_list END
                 /*{
                    if (! ($$ = make_unwind_command ($1, $4, $8, $9, $2, $6)))
                      ABORT_PARSE;
                 }*/
                | TRY stash_comment opt_sep opt_list CATCH
                  stash_comment opt_sep opt_list END
                 /*{
                    if (! ($$ = make_try_command ($1, $4, $8, $9, $2, $6)))
                      ABORT_PARSE;
                 }*/
                | TRY stash_comment opt_sep opt_list END
                 /*{
                    if (! ($$ = make_try_command ($1, $4, 0, $5, $2, 0)))
                      ABORT_PARSE;
                 }*/
                ;

// ===========================================
// Some `subroutines' for function definitions
// ===========================================

push_fcn_symtab : // empty
                  {
                    std::cout << "FUNCTION SYMTAB" << std::endl << std::flush;                    

                    //The file contains at least one function means it will contain
                    //only functions. Switch scope to global scope instead of main method scope
                    

                    current_function_depth++;

                    if(current_function_depth == 1)
                    {
		      //Pop out the default function used for script files
                      SageBuilder::popScopeStack();
		      
		      SageInterface::removeStatement(currentScope);

		      //Set the scope to be the global scope since we need to add functions
                      currentScope = SageInterface::getFirstGlobalScope(project);
                      
                      parsingScriptFile = false;
                    }
                    //The new basic block will serve as the scope for the function
                    SageBuilder::pushScopeStack(SageBuilder::buildBasicBlock());

                    
                  }
                 /*{
                    current_function_depth++;

                    if (max_function_depth < current_function_depth)
                      max_function_depth = current_function_depth;

                    symtab_context.push (symbol_table::current_scope ());
                    symbol_table::set_scope (symbol_table::alloc_scope ());

                    if (! reading_script_file && current_function_depth == 1
                        && ! parsing_subfunctions)
                      primary_fcn_scope = symbol_table::current_scope ();

                    if (reading_script_file && current_function_depth > 1)
                      yyerror ("nested functions not implemented in this context");
                 }*/
                ;

// ===========================
// List of function parameters
// ===========================

param_list_beg  : '('
                 {
                    lexer_flags.looking_at_parameter_list = true;

                    if (lexer_flags.looking_at_function_handle)
                     {
                        //symtab_context.push (symbol_table::current_scope ());
                        //symbol_table::set_scope (symbol_table::alloc_scope ());
                        lexer_flags.looking_at_function_handle--;
                     }
                 }
                ;

param_list_end  : ')'
                 {
                    lexer_flags.looking_at_parameter_list = false;
                    lexer_flags.looking_for_object_index = false;
                 }
                ;

param_list      : param_list_beg param_list1 param_list_end
                 {
                    lexer_flags.quote_is_transpose = false;
                    $$ = $2;
                 }
                | param_list_beg error
                 {
                    yyerror ("invalid parameter list");
                    $$ = 0;
                    ABORT_PARSE;
                 }
                ;

param_list1     : // empty
                 {$$ = SageBuilder::buildFunctionParameterList();}
                | param_list2
                {         
                  $$ = $1;                  
                }
                ;

param_list2     : decl2
                 {
		   SgInitializedName *initializedName = ((SgVarRefExp*)$1)->get_symbol()->get_declaration();
		   $$ = SageBuilder::buildFunctionParameterList(initializedName);
                 }
                | param_list2 ',' decl2
                  {
		    SgInitializedName *initializedName = ((SgVarRefExp*)$3)->get_symbol()->get_declaration();
		    SageInterface::appendArg($1, initializedName);
                    $$ = $1;
                  }
                ;

// ===================================
// List of function return value names
// ===================================

return_list     : '[' ']'
                 {
                    lexer_flags.looking_at_return_list = false;
                    $$ = SageBuilder::buildExprListExp();
                    //$$ = new tree_parameter_list ();
                 }
                | return_list1
                 {
                    lexer_flags.looking_at_return_list = false;

                    $$ = $1;
                    /*if ($1->validate (tree_parameter_list::out))
                      $$ = $1;
                    else
                      ABORT_PARSE;*/
                 }
                | '[' return_list1 ']'
                 {
                    lexer_flags.looking_at_return_list = false;
                    
                    $$ = $2;

                    /*if ($2->validate (tree_parameter_list::out))
                      $$ = $2;
                    else
                      ABORT_PARSE;*/
                 }
                ;

return_list1    : identifier
                  { $$ = SageBuilder::buildExprListExp($1);}
                 
                | return_list1 ',' identifier
                  {
                    SageInterface::appendExpression($1, $3);
                    $$ = $1;
                  }
                ;

// ===========
// Script file
// ===========

script_file     : SCRIPT_FILE opt_list END_OF_INPUT
                  { $$ = $2; }
                 /*{
                    tree_statement *end_of_script
                      = make_end ("endscript", input_line_number,
                                  current_input_column);

                    make_script ($2, end_of_script);

                    $$ = 0;
                 }*/
                ;

// =============
// Function file
// =============

function_file   : FUNCTION_FILE function_list opt_sep END_OF_INPUT
                 /*{$$ = 0;}*/
                ;

function_list   : function
                | function_list sep function
                ;

// ===================
// Function definition
// ===================

function_beg    : push_fcn_symtab FCN stash_comment
                 {
                    //$$ = $3;
                    //TODO: May be set the current scope                                        

                    if (reading_classdef_file || lexer_flags.parsing_classdef)
                      lexer_flags.maybe_classdef_get_set_method = true;                        
                 }
                ;

function        : function_beg function1
                 {                    
		   $$ = $2->build_function();
		   recover_from_parsing_function ();

                    SageBuilder::popScopeStack();
                    lexer_flags.variableTypes.clear();
                 }
                | function_beg return_list '=' function1
                 {                                        
		   $4->set_return_list($2);

		   $$ = $4->build_function();
		   
                   recover_from_parsing_function ();
                 
                    SageBuilder::popScopeStack();
                    lexer_flags.variableTypes.clear();

                 }
                ;

fcn_name        : identifier
                 {                    
                    lexer_flags.parsed_function_name.top () = true;
                    lexer_flags.maybe_classdef_get_set_method = false;

		    $$ = new std::string(extractVarName($1).getString());

		    /*
		      This identifier did not turn out to be a variable symbol.
		      It will be added later on as a function symbol.
		     */
		    SgVariableSymbol *symbol = $1->get_symbol();
		    SageBuilder::topScopeStack()->remove_symbol(symbol);
                 }
                | GET '.' identifier
                 {
                    lexer_flags.maybe_classdef_get_set_method = false;
                    $$ = new std::string(extractVarName($3).getString());

		    SageBuilder::topScopeStack()->remove_symbol($3->get_symbol());
                 }
                | SET '.' identifier
                 {
                    lexer_flags.maybe_classdef_get_set_method = false;
                    $$ = new std::string(extractVarName($3).getString());
		    SageBuilder::topScopeStack()->remove_symbol($3->get_symbol());
                 }
                ;

function1       : fcn_name function2
                  {
                    $2->set_function_name(*$1);

                    $$ = $2;
                  }
                ;

function2       : param_list opt_sep opt_list function_end
                  {

		   MatlabFunctionBuilder *functionBuilder = new MatlabFunctionBuilder($1, $3, SageInterface::getFirstGlobalScope(project));
                    $$ = functionBuilder;

                  }                 
                | opt_sep opt_list function_end
                  {
		   MatlabFunctionBuilder *functionBuilder = new MatlabFunctionBuilder($2, SageInterface::getFirstGlobalScope(project));
                    $$ = functionBuilder;
											       
                  }
                ;

function_end    : END
                 {                    
                    //endfunction_found = true;
                    // if (end_token_ok ($1, token::function_end))
                    //   $$ = make_end ("endfunction", $1->line (), $1->column ());
                    // else
                    //   ABORT_PARSE;
                 }
                | END_OF_INPUT
                 /*{
// A lot of tests are based on the assumption that this is OK
//                  if (reading_script_file)
//                   {
//                      yyerror ("function body open at end of script");
//                      YYABORT;
//                   }

                    if (endfunction_found)
                     {
                        yyerror ("inconsistent function endings -- "
                                 "if one function is explicitly ended, "
                                 "so must all the others");
                        YYABORT;
                     }

                    if (! (reading_fcn_file || reading_script_file
                           || get_input_from_eval_string))
                     {
                        yyerror ("function body open at end of input");
                        YYABORT;
                     }

                    if (reading_classdef_file)
                     {
                        yyerror ("classdef body open at end of input");
                        YYABORT;
                     }

                    $$ = make_end ("endfunction", input_line_number,
                                   current_input_column);
                 }*/
                ;

// ========
// Classdef
// ========

classdef_beg    : CLASSDEF stash_comment
                 {
                    //$$ = 0;
                    lexer_flags.parsing_classdef = true;
                 }
                ;

classdef_end    : END
                 {
                    lexer_flags.parsing_classdef = false;

                    /*if (end_token_ok ($1, token::classdef_end))
                      $$ = make_end ("endclassdef", $1->line (), $1->column ());
                    else
                      ABORT_PARSE;*/
                 }
                ;

classdef1       : classdef_beg opt_attr_list identifier opt_superclasses
                 /*{$$ = 0;}*/
                ;

classdef        : classdef1 '\n' class_body '\n' stash_comment classdef_end
                 /*{$$ = 0;}*/
                ;

opt_attr_list   : // empty
                 /*{$$ = 0;}*/
                | '(' attr_list ')'
                 /*{$$ = 0;}*/
                ;

attr_list       : attr
                 /*{$$ = 0;}*/
                | attr_list ',' attr
                 /*{$$ = 0;}*/
                ;

attr            : identifier
                 /*{$$ = 0;}*/
                | identifier '=' decl_param_init expression
                 /*{$$ = 0;}*/
                | EXPR_NOT identifier
                 /*{$$ = 0;}*/
                ;

opt_superclasses
                : // empty
                 /*{$$ = 0;}*/
                | superclasses
                 /*{$$ = 0;}*/
                ;

superclasses    : EXPR_LT identifier '.' identifier
                 /*{$$ = 0;}*/
                | EXPR_LT identifier
                 /*{$$ = 0;}*/
                | superclasses EXPR_AND identifier '.' identifier
                 /*{$$ = 0;}*/
                | superclasses EXPR_AND identifier
                 /*{$$ = 0;}*/
                ;

class_body      : properties_block
                 /*{$$ = 0;}*/
                | methods_block
                 /*{$$ = 0;}*/
                | events_block
                 /*{$$ = 0;}*/
                | class_body '\n' properties_block
                 /*{$$ = 0;}*/
                | class_body '\n' methods_block
                 /*{$$ = 0;}*/
                | class_body '\n' events_block
                 /*{$$ = 0;}*/
                ;

properties_beg  : PROPERTIES stash_comment
                 /*{$$ = 0;}*/
                ;

properties_block
                : properties_beg opt_attr_list '\n' properties_list '\n' END
                 /*{$$ = 0;}*/
                ;

properties_list
                : class_property
                 /*{$$ = 0;}*/
                | properties_list '\n' class_property
                 /*{$$ = 0;}*/
                ;

class_property  : identifier
                 /*{$$ = 0;}*/
                | identifier '=' decl_param_init expression ';'
                 /*{$$ = 0;}*/
                ;

methods_beg     : METHODS stash_comment
                 /*{$$ = 0;}*/
                ;

methods_block   : methods_beg opt_attr_list '\n' methods_list '\n' END
                 /*{$$ = 0;}*/
                ;

methods_list    : function
                 /*{$$ = 0;}*/
                | methods_list '\n' function
                 /*{$$ = 0;}*/
                ;

events_beg      : EVENTS stash_comment
                 /*{$$ = 0;}*/
                ;

events_block    : events_beg opt_attr_list '\n' events_list '\n' END
                 /*{$$ = 0;}*/
                ;

events_list     : class_event
                 /*{$$ = 0;}*/
                | events_list '\n' class_event
                 /*{$$ = 0;}*/
                ;

class_event     : identifier
                 /*{$$ = 0;}*/
                ;

// =============
// Miscellaneous
// =============

stash_comment   : // empty
                  { $$ = NULL;}
                 /*{$$ = octave_comment_buffer::get_comment ();}*/
                ;

parse_error     : LEXICAL_ERROR
                 {yyerror ("parse error");}
                | error
                ;

sep_no_nl       : ','
                 {$$ = ',';}
                | ';'
                 {$$ = ';';}
                | sep_no_nl ','
                 {$$ = $1;}
                | sep_no_nl ';'
                 {$$ = $1;}
                ;

opt_sep_no_nl   : // empty
                 {$$ = 0;}
                | sep_no_nl
                 {$$ = $1;}
                ;

sep             : ','
                 {$$ = ',';}
                | ';'
                 {$$ = ';';}
                | '\n'
                 {$$ = '\n';}
                | sep ','
                 {$$ = $1;}
                | sep ';'
                 {$$ = $1;}
                | sep '\n'
                 {$$ = $1;}
                ;

opt_sep         : // empty
                 {$$ = 0;}
                | sep
                 {$$ = $1;}
                ;

%%

// Generic error messages.

/*void
yyerror (const char *s)
{
  int err_col = current_input_column - 1;

  std::ostringstream output_buf;

  if (reading_fcn_file || reading_script_file || reading_classdef_file)
    output_buf << "parse error near line " << input_line_number
               << " of file " << curr_fcn_file_full_name;
  else
    output_buf << "parse error:";

  if (s && strcmp (s, "parse error") != 0)
    output_buf << "\n\n  " << s;

  output_buf << "\n\n";

  if (! current_input_line.empty ())
   {
      size_t len = current_input_line.length ();

      if (current_input_line[len-1] == '\n')
        current_input_line.resize (len-1);

      // Print the line, maybe with a pointer near the error token.

      output_buf << ">>> " << current_input_line << "\n";

      if (err_col == 0)
        err_col = len;

      for (int i = 0; i < err_col + 3; i++)
        output_buf << " ";

      output_buf << "^";
   }

  output_buf << "\n";

  //std::string msg = output_buf.str ();

  parse_error ("%s", msg.c_str ());
}*/

// Error mesages for mismatched end tokens.
/*
static voidstatic tree_index_expression *
make_indirect_ref (tree_expression *expr, const std::string& elt)
{
  tree_index_expression *retval = 0;

  int l = expr->line ();
  int c = expr->column ();

  if (expr->is_index_expression ())
    {
      tree_index_expression *tmp = static_cast<tree_index_expression *> (expr);

      tmp->append (elt);

      retval = tmp;
    }
  else
    retval = new tree_index_expression (expr, elt, l, c);

  lexer_flags.looking_at_indirect_ref = false;

  return retval;
}
end_error (const char *type, token::end_tok_type ettype, int l, int c)
{
  static const char *fmt
    = "`%s' command matched by `%s' near line %d column %d";

  switch (ettype)
   {
    case token::simple_end:
      error (fmt, type, "end", l, c);
      break;

    case token::for_end:
      error (fmt, type, "endfor", l, c);
      break;

    case token::function_end:
      error (fmt, type, "endfunction", l, c);
      break;

    case token::classdef_end:
      error (fmt, type, "endclassdef", l, c);
      break;

    case token::if_end:
      error (fmt, type, "endif", l, c);
      break;

    case token::switch_end:
      error (fmt, type, "endswitch", l, c);
      break;

    case token::while_end:
      error (fmt, type, "endwhile", l, c);
      break;

    case token::try_catch_end:
      error (fmt, type, "end_try_catch", l, c);
      break;

    case token::unwind_protect_end:
      error (fmt, type, "end_unwind_protect", l, c);
      break;

    default:
      panic_impossible ();
      break;
   }
}

// Check to see that end tokens are properly matched.

static bool
end_token_ok (token *tok, token::end_tok_type expected)
{
  bool retval = true;

  token::end_tok_type ettype = tok->ettype ();

  if (ettype != expected && ettype != token::simple_end)
   {
      retval = false;

      yyerror ("parse error");

      int l = tok->line ();
      int c = tok->column ();

      switch (expected)
       {
        case token::classdef_end:
          end_error ("classdef", ettype, l, c);
          break;

        case token::for_end:
          end_error ("for", ettype, l, c);
          break;

        case token::function_end:
          end_error ("function", ettype, l, c);
          break;

        case token::if_end:
          end_error ("if", ettype, l, c);
          break;

        case token::try_catch_end:
          end_error ("try", ettype, l, c);
          break;

        case token::switch_end:
          end_error ("switch", ettype, l, c);
          break;

        case token::unwind_protect_end:
          end_error ("unwind_protect", ettype, l, c);
          break;

        case token::while_end:
          end_error ("while", ettype, l, c);
          break;

        default:
          panic_impossible ();
          break;
       }
   }

  return retval;
}

// Maybe print a warning if an assignment expression is used as the
// test in a logical expression.

static void
maybe_warn_assign_as_truth_value (tree_expression *expr)
{
  if (expr->is_assignment_expression ()
      && expr->paren_count () < 2)
   {
      if (curr_fcn_file_full_name.empty ())
        warning_with_id
          ("Octave:assign-as-truth-value",
           "suggest parenthesis around assignment used as truth value");
      else
        warning_with_id
          ("Octave:assign-as-truth-value",
           "suggest parenthesis around assignment used as truth value near line %d, column %d in file `%s'",
           expr->line (), expr->column (), curr_fcn_file_full_name.c_str ());
   }
}

// Maybe print a warning about switch labels that aren't constants.

static void
maybe_warn_variable_switch_label (tree_expression *expr)
{
  if (! expr->is_constant ())
   {
      if (curr_fcn_file_full_name.empty ())
        warning_with_id ("Octave:variable-switch-label",
                         "variable switch label");
      else
        warning_with_id
          ("Octave:variable-switch-label",
           "variable switch label near line %d, column %d in file `%s'",
           expr->line (), expr->column (), curr_fcn_file_full_name.c_str ());
   }
}*/

static SgExpression*
make_indirect_ref (SgExpression *className, SgExpression *member)
{
  /*tree_index_expression *retval = 0;

  int l = expr->line ();
  int c = expr->column ();

  if (expr->is_index_expression ())
    {
      tree_index_expression *tmp = static_cast<tree_index_expression *> (expr);

      tmp->append (elt);

      retval = tmp;
    }
  else
    retval = new tree_index_expression (expr, elt, l, c);*/


  SgExpression *dotExpression = SageBuilder::buildDotExp(className, member);
  lexer_flags.looking_at_indirect_ref = false;

  return dotExpression;
  //return retval;
}

static void
recover_from_parsing_function ()
{
  /*if (symtab_context.empty ())
    panic_impossible ();

  symbol_table::set_scope (symtab_context.top ());
  symtab_context.pop ();*/

  /*if (reading_fcn_file && current_function_depth == 1
      && ! parsing_subfunctions)
    parsing_subfunctions = true;*/

  lexer_flags.defining_func--;
  lexer_flags.parsed_function_name.pop ();
  lexer_flags.looking_at_return_list = false;
  lexer_flags.looking_at_parameter_list = false;
}

/*static SgStatementList*
append_statement_list (SgStatementList* list, SgStatement* stmt)
{  
  list->Append (stmt);

  return list;
}*/

int beginParse(SgProject* &p, int argc, char* argv[])
{ 
  parsingScriptFile = true;
  current_function_depth = 0;

 if(project == NULL)
 {
   std::cout << "                          Initializing project: \n" << std::flush;

   std::string workingFile(argv[1]);
   workingFile += ".cc";

   std::ofstream workingFileStream(workingFile.c_str());
   workingFileStream.close();
   
   argv[1] = strdup(workingFile.c_str());

   p = frontend(argc, argv);

   SageBuilder::pushScopeStack(SageInterface::getFirstGlobalScope(p));
   currentScope = SageBuilder::buildBasicBlock();
   SageBuilder::pushScopeStack(currentScope);
 }
  project = p; 

 return yyparse();
}

SgIfStmt* getLastIfClause(SgIfStmt* topIfClause)
{
  SgIfStmt *currentIf = topIfClause,
           *tmpIf = topIfClause;

  while( (tmpIf = (SgIfStmt*)tmpIf->get_false_body()))
  {      
      currentIf = tmpIf;
  }

  return currentIf;
}

/*void addVariableDeclarations()
{

  std::cout << "Variable declarations" << std::endl << std::flush;
  //std::cout << "GETTYPE: " << lexer_flags.getType("c") << std::endl << std::flush;

  for(const auto &declarations : lexer_flags.variableTypes)
  {
    std::string typeString = declarations.first;

    std::cout << declarations.first << std::endl << std::flush;

    SgType *type = SageBuilder::buildOpaqueType(typeString, SageBuilder::topScopeStack());

    // if(typeString == "int")
    // {
    //   type = SageBuilder::buildIntType();
    // }
    // else if(typeString == "double")
    // {
    //   type = SageBuilder::buildDoubleType();
    // }

    for(std::string var : declarations.second)
    {
      SgVariableDeclaration* variableDeclaration = SageBuilder::buildVariableDeclaration(var, type);
      SageInterface::prependStatement(variableDeclaration);

      std::cout << var << "\t" << std::flush;      
    }

    std::cout << std::endl << std::flush;
  }  
}*/

/*
*Creates a return std::make_tuple statement for multiple return items
*For a single return item, a return var statement is created
*/
/*void manageReturnStatement(SgExprListExp* returnList, SgFunctionDeclaration* fcn)
{
  std::cout << "RETURN" << std::endl << std::flush;
  //std::cout << "size" << returnList->get_expressions().size() << std::endl << std::flush;

  int returnParamsCount = returnList->get_expressions().size();

  std::string returnTypeString;
  SgType* returnType;

  SgExpression *returnExp;

  if(returnParamsCount == 1)
  {
    SgExpression* retvar = returnList->get_expressions()[0];

    std::string retvarName = extractVarName(retvar);

    returnTypeString = lexer_flags.getType(retvarName);

    returnExp = retvar;
  }
  else if(returnParamsCount > 1)
  {
    //Create a return tuple statement
    std::vector<std::string> returnListString;    

    for(auto currentRetVar : returnList->get_expressions())
    {
      std::string retvarName = extractVarName(currentRetVar);

      returnListString.push_back(lexer_flags.getType(retvarName));      
    }

    returnTypeString = "std::tuple<" + boost::algorithm::join(returnListString, ", ") + ">";

    returnExp = SageBuilder::buildFunctionCallExp(SageBuilder::buildVarRefExp("std::make_tuple"), returnList);
  }

  returnType = SageBuilder::buildOpaqueType(returnTypeString, SageBuilder::topScopeStack());

  SgFunctionType *updatedFunctionType = SageBuilder::buildFunctionType(returnType, fcn->get_parameterList());
  fcn->set_type(updatedFunctionType);

  

  SageInterface::appendStatement(SageBuilder::buildReturnStmt(returnExp));
}
*/
SgName extractVarName(SgExpression* varRefExp)
{
  return ((SgVarRefExp*)varRefExp)->get_symbol()->get_name();
}
