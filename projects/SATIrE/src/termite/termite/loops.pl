/* -*- prolog -*- */

:- module(loops,
	  [
	   isSimpleForInit/3,
	   isEmptyForInit/1,
	   isForTestLE/2,
	   isForTestGE/2,
	   isForTestOp/2,
	   isWhileStatement/7,
	   isDoWhileStatement/7,
	   isMin2Func/3,

	   isStepsize/3,

	   is_fortran_for_loop/6,
	   is_fortran_multicond_for_loop/6,
	   is_const_val/2,

	   max_nesting_level/2,
	   nested/3, nestmax/3
]).

:- use_module(library(astproperties)).
:- use_module(library(asttransform)).
:- use_module(library(clpfd)).

%-----------------------------------------------------------------------
/** <module> Properties of loops

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


%% is_const_val(+Term, -Val) is semidet.
%% is_const_val(-Term, +Val) is nondet.
%% @todo implement constant analysis result for VarRefExp
is_const_val(Term, Val) :- isIntVal(Term, Val).

%% isStepsize(+Term, -InductionVar, -Val) is semidet.
%% isStepsize(-Term, +InductionVar, +Val) is nondet.
isStepsize(plus_assign_op(InductionVar, Increment, _, _, _),
	   InductionVar, Val) :-
  is_const_val(Increment, Val).

isStepsize(minus_assign_op(InductionVar, Decrement, _, _, _),
	   InductionVar, -Val):-
  is_const_val(Decrement, Val).

isStepsize(plus_plus_op(InductionVar, _, _, _), InductionVar, 1).
isStepsize(minus_minus_op(InductionVar, _, _, _), InductionVar, -1).

isStepsize(assign_op(InductionVar, Increment, _, _, _), InductionVar, Val) :-
  is_const_val(Increment, Val).

isStepsize(comma_op_exp(E1, E2, _, _, _), InductionVar, Val) :-
  ( (Op1 = E1, Op2 = E2)
  ; (Op1 = E2, Op2 = E1)),
  isStepsize(Op1, InductionVar, Val),
  guarantee(Op2, is_transp(InductionVar, local)).

%% is_fortran_multicond_for_loop(+ForStatement, +I, ForInit,
%% ForTest, ForStep, Body)
% generate multiple ForTest on backtracking if multiple conditions are
% combined with logical and operators
is_fortran_multicond_for_loop(for_statement(ForInit,
			     ForTest,
			     ForStep,
			     Body,
			     _, _, _),
		   I, ForInit, ForTest, ForStep, Body) :-
  % ForInit may be empty
  isSimpleForInit(ForInit, I, _Base),
  isForMultiTestOp(ForTest, TestOp),
  isBinOpRhs(TestOp, _End), 
  % Assure we only have one induction variable
  isBinOpLhs(TestOp, I2), var_stripped(I2, I),
  isStepsize(ForStep, I3, _Increment), var_stripped(I3, I),

  guarantee(Body, is_transp(I, local)).

is_fortran_multicond_for_loop(for_statement(ForInit,
			     ForTest,
			     ForStep,
			     Body,
			     _, _, _),
		   I, ForInit, ForTest, ForStep, Body) :-
  % ForInit may be empty
  isEmptyForInit(ForInit),
  isForTestLE(ForTest, LeOp),
  isBinOpRhs(LeOp, _End),
  % Assure we only have one induction variable
  isBinOpLhs(LeOp, I1), var_stripped(I1, I),
  isStepsize(ForStep, I2, _Increment), var_stripped(I2, I),

  guarantee(Body, is_transp(I, local)).

%% is_fortran_for_loop(+ForStatement, -I, -ForInit, -ForTest, -ForStep, -Body)
is_fortran_for_loop(for_statement(ForInit,
			     ForTest,
			     ForStep,
			     Body,
			     _, _, _),
		   I, ForInit, ForTest, ForStep, Body) :-
  % ForInit may be empty
  isSimpleForInit(ForInit, I, _Base),
  isForTestOp(ForTest, TestOp),
  isBinOpRhs(TestOp, _End), 
  % Assure we only have one induction variable
  isBinOpLhs(TestOp, I2), var_stripped(I2, I),
  isStepsize(ForStep, I3, _Increment), var_stripped(I3, I),

  guarantee(Body, is_transp(I, local)).

is_fortran_for_loop(for_statement(ForInit,
			     ForTest,
			     ForStep,
			     Body,
			     _, _, _),
		   I, ForInit, ForTest, ForStep, Body) :-
  % ForInit may be empty
  isEmptyForInit(ForInit),
  isForTestLE(ForTest, LeOp),
  isBinOpRhs(LeOp, _End),
  % Assure we only have one induction variable
  isBinOpLhs(LeOp, I1), var_stripped(I1, I),
  isStepsize(ForStep, I2, _Increment), var_stripped(I2, I),

  guarantee(Body, is_transp(I, local)).


%% isSimpleForInit(+InitStatement, -InductionVar, -InitVal) is semidet.
% Extracts the induction variable and the initial value from InitStatement
isSimpleForInit(for_init_statement(ExprStmt, _, _, _), InductionVar, InitVal) :-
  ExprStmt = [expr_statement(Expr, _, _, _)],
  isSimpleForInit(Expr, InductionVar, InitVal).

isSimpleForInit(AssignOp, InductionVar, InitVal) :-
  AssignOp = assign_op(_,_,_,_,_),
  isBinOpLhs(AssignOp, I),
  var_stripped(I, InductionVar),
  isBinOpRhs(AssignOp, InitVal).

isSimpleForInit(comma_op_exp(E1, E2, _, _, _), InductionVar, InitVal) :-
  ( (Op1 = E1, Op2 = E2)
  ; (Op1 = E2, Op2 = E1)),
  isSimpleForInit(Op1, InductionVar, InitVal),
  guarantee(Op2, is_transp(InductionVar, local)).

%% isEmptyForInit(+InitStatement) is semidet.
isEmptyForInit(for_init_statement(_, _, _)).
isEmptyForInit(for_init_statement([], _, _, _)).

%% isForTestLE(+TestOp, -LeOp) is semidet.
% Any < test will be converted into a =<
isForTestLE(expr_statement(TestOp, _, _, _), Op) :-
  isForTestLE(TestOp, Op).

isForTestLE(LeOp, LeOp) :-
  LeOp = less_or_equal_op(_, _, _, _, _).

isForTestLE(LtOp, LeOp) :-
  LtOp = less_than_op(Var, Val, A, Ai, Fi),
  isVar(Var, _),
  isIntVal(One, 1),
  LeOp = less_or_equal_op(Var, subtract_op(Val, One, _, _, _), A, Ai, Fi).

%% isForTestGE(+TestOp, -GeOp) is semidet.
% Any > test will be converted into a >=
isForTestGE(expr_statement(TestOp, _, _, _), Op) :-
  isForTestGE(TestOp, Op).

isForTestGE(GeOp, GeOp) :-
  GeOp = greater_or_equal_op(_, _, _, _, _).

isForTestGE(GtOp, GeOp) :-
  GtOp = greater_than_op(Var, Val, A, Ai, Fi),
  isVar(Var, _),
  isIntVal(One, 1),
  GeOp = greater_or_equal_op(Var, add_op(Val, One, _), A, Ai, Fi).

%% isForTestNE(+NeOp, -NeOp) is semidet.
isForTestNE(NeOp, NeOp) :- NeOp = not_equal_op(_, _, _, _, _).

%% isForTestOp(+TestOp, -TestOp) is nondet.
% removes the sourrounding expression statement
isForTestOp(expr_statement(TestOp, _, _, _), Op) :- isForTestOp(TestOp, Op).
isForTestOp(N, Op) :- isForTestLE(N, Op).
isForTestOp(N, Op) :- isForTestGE(N, Op).
isForTestOp(N, Op) :- isForTestNE(N, Op).

%% isForMultiTestOp(+TestOp, -TestOp) is nondet.
% generate two solutions for an AND condition
isForMultiTestOp(expr_statement(TestOp, _, _, _), Op) :-
  isForMultiTestOp(TestOp, Op).
isForMultiTestOp(N, Op) :- isForTestLE(N, Op).
isForMultiTestOp(N, Op) :- isForTestGE(N, Op).
isForMultiTestOp(N, Op) :- isForTestNE(N, Op).
isForMultiTestOp(and_op(N1, _, _, _, _), Op) :- isForMultiTestOp(N1, Op).
isForMultiTestOp(and_op(_, N2, _, _, _), Op) :- isForMultiTestOp(N2, Op).

%% isWhileStatement(+WhileStmt,-Condition,-Var,-Body,-Annot,-Ai,-Fi) is semidet.
%% isWhileStatement(-WhileStmt,+Condition,+Var,+Body,+Annot,+Ai,+Fi) is det.
%% FIXME rename this!
isWhileStatement(while_stmt(Condition, Body, Annot, Ai, Fi),
		 Condition, Var, Body, Annot, Ai, Fi) :- 
  isForTestOp(Condition, Op),
  isBinOpLhs(Op, Var),
  isVar(Var, _).

%% isDoWhileStatement(+DoWhileStmt,-Condition,-Var,-Body,-Annot,-Ai-Fi)
%% is semidet.
%% isDoWhileStatement(-DoWhileStmt,+Condition,+Var,+Body,+Annot,+Ai,+Fi) is det.
isDoWhileStatement(do_while_stmt(Body, Condition, Annot, Ai, Fi),
		 Condition, Var, Body, Annot, Ai, Fi) :-
  isForTestOp(Condition, Op),
  isBinOpLhs(Op, Var),
  isVar(Var, _).

%% isMin2Func(+MinFunc, -Expr1, -Expr2) is semidet.
%% isMin2Func(-MinFunc, +Expr1, +Expr2) is det.
% FIXME move to annot.pl
isMin2Func(
    function_call_exp(
	function_ref_exp(
	    function_ref_exp_annotation(MIN2,
					function_type(type_int,
						      1,
						      [type_int,type_int])),
			    _Ai1, _Fi1),
			 expr_list_exp([Expr1, Expr2], _, _Ai2, _Fi2),
			 _, _Ai3, _Fi3), Expr1, Expr2) :-
  string_to_atom(MIN2, min2).

%% max_nesting_level(+Loop, -N)
% return the maximum number of loops nested inside Loop
max_nesting_level(Loop, N) :-
  collate_ast(loops:nested, loops:nestmax, 0, Loop, N).

nestmax(N1, N2, N) :- N is max(N1, N2).
nested(Node, N0, N) :-
  functor(Node, F, _),
  (   (	  F = for_statement
      ;	  F = while_stmt
      ;	  F = do_while_stmt
      )
  ->  N is N0+1
  ;   N = N0).


