#!/usr/bin/pl -t testrun -f
/* -*- prolog -*- */

:- module(astproperties,
	  [ast_equiv/2,
	   is_transp/3,
	   is_complex_statement/1,
	   guarantee/2,

	   strip_file_info/5, % deprecated
	   var_stripped/2,
	   term_stripped/2,
	   var_interval/3,
	   term_interval/3,

	   var_type/2,
	   var_typemod/2,
	   
	   isIntVal/2,
	   isVar/2,
	   isBinNode/7,
	   isBinOpLhs/2,
	   isBinOpRhs/2,

	   scope_statement/1,
	   analysis_info/2,
	   file_info/2,

	   function_signature/4,
	   is_function_call/3,
	   is_function_call_exp/3,
	   function_body/2,
	   
	   pragma_text/2,
	   get_annot/3,
	   get_annot_term/3,
	   
	   get_preprocessing_infos/2]).

:- use_module(library(utils)).

%-----------------------------------------------------------------------
/** <module> Properties of abstract syntax trees

  This module defines commonly-used queries about C/C++/Objective C
  ASTs given in the TERMITE term representation as exported by SATIrE.

  Depenedencies:
  
  The user has to define the type predicates [type_info/3, type_interval/2]

  
@version   @PACKAGE_VERSION@
@copyright Copyright (C) 2008-2009 Adrian Prantl
@author    Adrian Prantl <adrian@complang.tuwien.ac.at>
@license 

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 3 of the License.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

*/
%-----------------------------------------------------------------------

%% ast_equiv(+Expr1, +Expr2) is semidet.
%
% Compare two expressions disregarding the file information
% Expects compact form.
%
% Todo: rewrite this!
ast_equiv(Expr, Expr).

ast_equiv(I1, I2) :-
  isIntVal(I1, X),
  isIntVal(I2, Y),
  !,
  X = Y.

ast_equiv(ExprA, ExprB) :-
  functor(ExprA, F, 2),
  functor(ExprB, F, 2),
  arg(1, ExprA, Annot),
  arg(1, ExprB, Annot).
ast_equiv(ExprA, ExprB) :-
  functor(ExprA, F, 3),
  functor(ExprB, F, 3),
  arg(1, ExprA, E1A),
  arg(1, ExprB, E1B),
  ast_equiv(E1A, E1B),
  arg(2, ExprA, Annot),
  arg(2, ExprB, Annot).
ast_equiv(ExprA, ExprB) :-
  functor(ExprA, F, 4),
  functor(ExprB, F, 4),
  arg(1, ExprA, E1A),
  arg(1, ExprB, E1B),
  ast_equiv(E1A, E1B),
  arg(2, ExprA, E2A),
  arg(2, ExprB, E2B),
  ast_equiv(E2A, E2B),
  arg(3, ExprA, Annot),
  arg(3, ExprB, Annot).

ast_equiv(ExprA, ExprB) :-
  functor(ExprA, F, 5),
  functor(ExprB, F, 5),
  arg(1, ExprA, E1A),
  arg(1, ExprB, E1B),
  ast_equiv(E1A, E1B),
  arg(2, ExprA, E2A),
  arg(2, ExprB, E2B),
  ast_equiv(E2A, E2B),
  ast_equiv(E1A, E1B),
  arg(3, ExprA, E3A),
  arg(3, ExprB, E3B),
  ast_equiv(E3A, E3B),
  arg(4, ExprA, Annot),
  arg(4, ExprB, Annot).

ast_equiv(ExprA, ExprB) :-
  functor(ExprA, F, 6),
  functor(ExprB, F, 6),
  arg(1, ExprA, E1A),
  arg(1, ExprB, E1B),
  ast_equiv(E1A, E1B),
  arg(2, ExprA, E2A),
  arg(2, ExprB, E2B),
  ast_equiv(E2A, E2B),
  ast_equiv(E1A, E1B),
  arg(3, ExprA, E3A),
  arg(3, ExprB, E3B),
  ast_equiv(E3A, E3B),
  arg(4, ExprA, E4A),
  arg(4, ExprB, E4B),
  ast_equiv(E4A, E4B),
  arg(5, ExprA, Annot),
  arg(5, ExprB, Annot).

ast_equiv([], []).
ast_equiv([A|As], [B|Bs]) :-
  ast_equiv(A, B),
  ast_equiv(As, Bs).
%-----------------------------------------------------------------------


%% is_transp(+Expr, +Var, +Scope).
% Goal succeeds if Var is not written to by Expr.
% * Expr must be an Expression.
% * Scope specifies whether Var is global or local.

is_transp(function_call_exp(_,_,_,_,_), _, global) :- !, fail.
is_transp(assign_op(LHS,_,_,_,_), Var, _) :- LHS = Var, !, fail.
is_transp(plus_assign_op(Var,_,_,_,_), Var, _) :- !, fail.
is_transp(minus_assign_op(Var,_,_,_,_), Var, _) :- !, fail.
is_transp(and_assign_op(Var,_,_,_,_), Var, _) :- !, fail.
is_transp(ior_assign_op(Var,_,_,_,_), Var, _) :- !, fail.
is_transp(mult_assign_op(Var,_,_,_,_), Var, _) :- !, fail.
is_transp(div_assign_op(Var,_,_,_,_), Var, _) :- !, fail.
is_transp(mod_assign_op(Var,_,_,_,_), Var, _) :- !, fail.
is_transp(xor_assign_op(Var,_,_,_,_), Var, _) :- !, fail.
is_transp(lshift_assign_op(Var,_,_,_,_), Var, _) :- !, fail.
is_transp(rshift_assign_op(Var,_,_,_,_), Var, _) :- !, fail.

is_transp(plus_plus_op(Var,_,_,_), Var, _) :- !, fail.
is_transp(minus_minus_op(Var,_,_,_), Var, _) :- !, fail.
is_transp(address_of_op(Var,_,_,_), Var, _) :- !, fail.
is_transp(_, _, _).
%-----------------------------------------------------------------------

%% is_complex_statement(+Node).
% Goal succeeds if Node introduces new edges into the control flow
% graph (CFG).
is_complex_statement(expr_statement(N, _, _, _)) :- 
  guarantee(N, is_complex_statement).
is_complex_statement(expression_root(N, _, _, _)) :- 
  guarantee(N, is_complex_statement).
is_complex_statement(function_call_exp(_, _, _, _)).
is_complex_statement(S) :- is_scope_statement(S).
is_complex_statement(return_statement(_, _, _, _)).
is_complex_statement(goto_statement(_, _, _, _)).
is_complex_statement(break_statement(_, _, _, _)).
is_complex_statement(continue_statement(_, _, _, _)).

is_scope_statement(basic_block(_, _, _, _)).
is_scope_statement(catch_option_stmt(_, _, _, _)).
is_scope_statement(class_definition(_, _, _, _)).
is_scope_statement(template_instantiation_defn(_, _, _, _)).
is_scope_statement(go_while_stmt(_, _, _, _)).
is_scope_statement(for_statement(_, _, _, _)).
is_scope_statement(function_definition(_, _, _, _)).
is_scope_statement(global(_, _, _, _)).
is_scope_statement(if_stmt(_, _, _, _)).
is_scope_statement(namespace_definition_statement(_, _, _, _)).
is_scope_statement(switch_statement(_, _, _, _)).
is_scope_statement(while_stmt(_, _, _, _)).
is_scope_statement(do_while_stmt(_, _, _, _)).
%-----------------------------------------------------------------------

%% guarantee(+Node, +Pred).
% Recursively test a predicate Pred on an AST Node.
%
guarantee(Node, Pred) :-
  ast_node(Node, _, Children, _, _, _),
  do_check(Node, Pred),
  guarantee_children(Children, Pred).

guarantee_children([], _).
guarantee_children([C|Cs], Pred) :-
  do_check(C, Pred),
  guarantee_children(Cs, Pred).

do_check(Node, F) :-
  %simple_form_of(Node, NodeSimple),
  F =.. [Functor|Params],
  Check =.. [Functor|[Node|Params]],
  %writeln('checking'), writeln(Check),
  Check.%,   writeln('succeeds').

% FIXME -> guarantee(Node, X->is_transp(sdf, foo, X)).

%-----------------------------------------------------------------------
% Handy Helper Functions
%-----------------------------------------------------------------------

%% strip_file_info(_,_,_,+Term1,-Term2)
% Repace file_info(...) with null in all VarRefExps. This facilitates
% the comparison of AST nodes.
%
% Use this with transformed_with/4
strip_file_info(_,_,_,
  var_ref_exp(var_ref_exp_annotation(Type, Name, Val, An, PPI), _Ai, _Fi),
  var_ref_exp(var_ref_exp_annotation(Type, Name, Val, An, PPI),null, null)).
strip_file_info(_,_,_, Term, Term).

%% var_stripped(+VarRefExp, -VarRefExpStripped)
%
% Non-traversal version of strip_file_info/5.
var_stripped(
  var_ref_exp(var_ref_exp_annotation(Type, Name, Val, A1, PPI), _Ai, _Fi),
  var_ref_exp(var_ref_exp_annotation(Type, Name, Val, A1, PPI),null,null)) :- !.

var_stripped(cast_exp(V, _, _A, _Ai, _Fi), V1) :- !,
  var_stripped(V, V1).

var_stripped(Term, Term).

%% var_interval(+AnalysisInfo, +VarRefExp, -Interval)
% Employ the analysis result/type info to yield an interval for the
% VarRefExp.
var_interval(analysis_info(AnalysisInfo),
	     var_ref_exp(var_ref_exp_annotation(Type, Name, _Val, _, _),
			 _Ai, _Fi),
	     interval(Min,Max)) :-
  member(pre_info(interval, Contexts), AnalysisInfo),
  member(merged:map(_,Intvls),Contexts),
  member(Name->[Min1,Max1],Intvls),
  (  number(Min1)
  -> Min = Min1
  ;  type_interval(Type, interval(Min, _))),
  (  number(Max1)
  -> Max = Max1
  ;  type_interval(Type, interval(_, Max))).

var_interval(_,
	     var_ref_exp(var_ref_exp_annotation(Type, _, _, _, _), _, _),
	     Interval) :- 
  type_interval(Type, Interval).
var_interval(_, Term, Term).

%% term_stripped(+Term, -StrippedTerm)
% Recursively strip all VarRefExps in Term
term_stripped(Term, Stripped) :-
  term_mod(Term, var_stripped, Stripped), !.

term_interval(AnalysisInfo, Term, TermC) :-
  term_mod(Term, var_interval(AnalysisInfo), TermC), !.

%% isIntVal(?IntVal, ?Value) is noned.
% Convert between int_val nodes and integer values.
isIntVal(                   int_val(_,value_annotation(Value,_), _, _), Value).
isIntVal(		  short_val(_,value_annotation(Value,_), _, _), Value).
isIntVal(	      short_int_val(_,value_annotation(Value,_), _, _), Value).
isIntVal(	       long_int_val(_,value_annotation(Value,_), _, _), Value).
isIntVal(	  long_long_int_val(_,value_annotation(Value,_), _, _), Value).
isIntVal(	   unsigned_int_val(_,value_annotation(Value,_), _, _), Value).
isIntVal(    unsigned_short_int_val(_,value_annotation(Value,_), _, _), Value).
isIntVal(	 unsigned_short_val(_,value_annotation(Value,_), _, _), Value).
isIntVal(	  unsigned_long_val(_,value_annotation(Value,_), _, _), Value).
isIntVal(unsigned_long_long_int_val(_,value_annotation(Value,_), _, _), Value).
isIntVal(	       bool_val_exp(_,value_annotation(Value,_), _, _), Value).

isIntVal(char_val(_,value_annotation(Value, _), _, _), Value) :-
  integer(Value).

isIntVal(unsigned_char_val(_,value_annotation(Value, _), _, _), Value) :-
  integer(Value).

isIntVal(char_val(_,value_annotation(String, _), _, _), Value) :-
  string(String),
  string_to_atom(String, Atom),
  atom_codes(Atom, [Value]).

isIntVal(unsigned_char_val(_,value_annotation(String, _), _, _), Value) :-
  string(String),
  string_to_atom(String, Atom),
  atom_codes(Atom, [Value]).

%% isVar(?VarRefExp, ?Name) is nondet.
% True if VarRefExp is a var_ref_exp or a cast_exp. Name is the name
% of the variable.
isVar(var_ref_exp(var_ref_exp_annotation(_, Name, _, _, _), _, _), Name).
isVar(cast_exp(var_ref_exp(var_ref_exp_annotation(_, Name, _, _, _), _, _),
	       null, _, _, _),
      Name).

%% var_type(?VarRefExp, ?Type) is nondet.
%  Allows access to the Type of VarRefExp.
var_type(var_ref_exp(var_ref_exp_annotation(Type, _, _, _, _), _, _), Type).

%% var_typemod(?VarRefExp, ?ConstVolatile) is nondet.
%  Allows access to the ConstVolatile modifier of VarRefExp.
%  Values for ConstVolatile are 'const' and 'volatil' (sic!).
var_typemod(VarRefExp, ConstVolatile) :-
  VarRefExp = var_ref_exp(var_ref_exp_annotation(Type, _, _, _, _), _, _),
  Type = modifier_type(_, type_modifier(_Restrict, 1, CV, 1)),
  ( (ConstVolatile = const,    CV = 2)
  ; (ConstVolatile = volatil,  CV = 3)
  ).

%% isBinNode(+Node, -Name, -E1, -E2, -Annot, -Ai, -Fi) is semidet.
% Decompose a binary node Node.
%% isBinNode(-Node, +Name, +E1, +E2, +Annot, +Ai, +Fi) is det.
% Compose a binary node Node.
isBinNode(N, Name, E1, E2, Annot, Ai, Fi) :-
  functor(N, Name, 5),
  arg(1, N, E1),
  arg(2, N, E2),
  arg(3, N, Annot),
  arg(4, N, Ai),
  arg(5, N, Fi).

%% isBinOpLhs(?BinOp, ?Lhs)
% Bind Lhs to the left-hand-side (1) operator of BinOp.
isBinOpLhs(BinOp, Lhs) :- arg(1, BinOp, Lhs).

%% isBinOpRhs(?BinOp, ?Rhs)
% Bind Rhs to the right-hand-side (2) operator of BinOp.
isBinOpRhs(BinOp, Rhs) :- arg(2, BinOp, Rhs).

%% scope_statement(+Node)
% True, if Node is a scope statement.
%
%Scope statements are
%    basic_block, catch_option_stmt, class_definition, do_while_stmt,
%    for_statement,function_definition, global, if_stmt,
%    namespace_definition_statment, switch_statement, while_stmt
scope_statement(Node) :-
  functor(Node, F, _),
  member(F, [basic_block, catch_option_stmt, class_definition,
	     do_while_stmt, for_statement,function_definition, global, if_stmt,
	     namespace_definition_statment, switch_statement, while_stmt]).
% scope_statement(basic_block(_, _, _, _)).
% scope_statement(catch_option_stmt(_, _, _, _)).
% scope_statement(class_definition(_, _, _, _)).
% scope_statement(do_while_stmt(_, _, _, _, _)).
% scope_statement(for_statement(_, _, _, _, _, _, _)).
% scope_statement(function_definition(_, _, _, _, _)).
% scope_statement(global(_, _, _, _)).
% scope_statement(if_stmt(_, _, _, _, _, _)).
% scope_statement(namespace_definition_statment(_, _, _, _)).
% scope_statement(switch_statement(_, _, _, _, _)).
% scope_statement(while_stmt(_, _, _, _, _)).

%% analysis_info(+Term, -Ai)
% Extract the analysis info Ai from Term.
analysis_info(N, Ai) :-
  functor(N, _, NArgs),
  NArgs1 is NArgs - 1,
  arg(NArgs1, N, Ai).

%% file_info(+Term, -Fi)
% Extract the file info Fi from Term.
file_info(N, Fi) :-
  functor(N, _, NArgs),
  Fi = file_info(_, _, _),
  arg(NArgs, N, Fi).

%% function_signature(?FunctionDecl, ?Type, ?Name, ?Modifier)
% Convert between signatures and terms.
function_signature(FunctionDecl, Type, Name, Modifier) :-
  FunctionDecl = function_declaration(_Params, _Def, DeclAnnot, _AI, _FI),
  DeclAnnot = function_declaration_annotation(Type, Name, Modifier, _PPI).

%% is_function_call(?Term, ?Name, ?Type).
% (De-)construct a function call.
is_function_call(expr_statement(function_call_exp(
                   function_ref_exp(
                     function_ref_exp_annotation(Name, Type, _), _Ai1, _Fi1), 
                   _ExprListExp, _A2, _Ai2, _Fi2),
                 _A3, _Ai3, _Fi3), Name, Type).

%% is_function_call_exp(?Term, -Name, -Type)
% (De-)construct a function call expression.
is_function_call_exp(function_call_exp(function_ref_exp(
                     function_ref_exp_annotation(Name, Type, _), _Ai1, _Fi1), 
				       _ExprListExp, _A2, _Ai2, _Fi2),
		     Name, Type).

%% function_body(?FuncDecl, ?Body)
% Get the function body Body from a function declaration FuncDecl.
function_body(function_declaration(_, function_definition(Body, _, _,_),
				   _, _, _),
	      Body).

%% pragma_text(?Pragma, ?Text)
% pragma_text/2 defines the relation between a pragma statement
% and the String Text inside the "#pragma Text" Statement.

% -Pragma +Text
pragma_text(Pragma, Text) :-
  var(Pragma), !,
  Pragma = pragma_declaration(
               pragma(pragma_annotation(Text),
		      analysis_result(null), null),
			      default_annotation(null, preprocessor_info([])),
			      analysis_result(null, null), null).

% +Pragma -Text
pragma_text(pragma_declaration(pragma(pragma_annotation(String), _,_), _,_,_),
	    String).

%% get_annot(+Stmts, -Annotterm, -Pragma)
% Find Pragma in Stmts and treat its contents as a Prolog term.
% If successuful, unify AnnotTerm with the parsed term.
get_annot(Stmts, AnnotTerm, Pragma) :-
  member(Pragma, Stmts),
  pragma_text(Pragma, Text),
  (atom(Text)
  -> atom_to_term(Text, AnnotTerm, _)
  ;  AnnotTerm = Text).

%% get_annot_term(?Stmts, ?Annotterm, ?Pragma)
% Quicker version of get_annot/3 without term parsing.
get_annot_term(Stmts, AnnotTerm, Pragma) :-
  pragma_text(Pragma, AnnotTerm),
  member(Pragma, Stmts).



%% get_preprocessing_infos(+Node, -PPIs)
% Todo: move to annot.pl!
get_preprocessing_infos(Node, PPIs) :- 
  functor(Node, _, Arity1), Arity1 > 2,
  N2 is Arity1-2, arg(N2, Node, Annot),
  (var(Annot)
  -> PPIs = []
  ; functor(Annot, _, Arity2),
   arg(Arity2, Annot, preprocessing_info(PPIs))
  ), !.
get_preprocessing_infos(_, []).

