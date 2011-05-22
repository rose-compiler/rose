#!/usr/bin/pl -q  -O -L0 -G0 -T0 -t main -s 
% -*- prolog -*-

%-----------------------------------------------------------------------
/** <module> Loop analysis

This program analyzes loop bounds of C (and some C++) programs.
This source code was extraced from the TuBound WCET analysis tool
[1] and is now distributed with SATIrE to serve as an example of
what you can do with TERMITE.

A detailed description of the algorithm can be found in the appendix
of [2].


@see

References

[1] http://www.complang.tuwien.ac.at/adrian/

[2] A. Prantl, J. Knoop, M. Schordan and M. Triska.
    Constraint solving for high-level WCET analysis.
    The 18th Workshop on Logic-based methods in Programming
    Environments (WLPE 2008). Udine, Italy, December 12, 2008.

    http://costa.tuwien.ac.at/papers/wlpe08.pdf

@author 

Copyright (C) 2007-2010, Adrian Prantl <adrian@complang.tuwien.ac.at>

@license 

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/
%-----------------------------------------------------------------------

:- getenv('PWD', CurDir),
   asserta(library_directory(CurDir)),
   prolog_load_context(directory, SrcDir),
   asserta(library_directory(SrcDir)),
   (getenv('TERMITE_LIB', TermitePath)
   ; (print_message(error, 'Please set the environment variable TERMITE_LIB'),
      halt(1))
   ),
   asserta(library_directory(TermitePath)).

:- use_module([library(apply_macros),
	       library(asttransform),
	       library(astproperties),
	       library(loops),
	       library(markers),
	       library(utils)]).
:- use_module([while2for,comefrom]).
:- use_module(library(assoc)),
   use_module(library(clpfd)).
:- use_module(library(types)).
%   use_module(library(swi/pce_profile)).

% Before doing anything else, set up handling to halt if warnings or errors
% are encountered.
:- dynamic prolog_reported_problems/0.
% If the Prolog compiler encounters warnings or errors, record this fact in
% a global flag. We let message_hook fail because that signals Prolog to
% format output as usual (otherwise, we would have to worry about formatting
% error messages).
user:message_hook(_Term, warning, _Lines) :-
    assert(prolog_reported_problems), !, fail.
user:message_hook(_Term, error, _Lines) :-
    assert(prolog_reported_problems), !, fail.


%-----------------------------------------------------------------------
% Symbolic & Numeric Loop bounds
%-----------------------------------------------------------------------

%-----------------------------------------------------------------------
%% merge_info(+Info, +(Var-Interval), -Merged) is det.
% replaces has() and equiv() info with new data
%
merge_info(Info, Var-Interval, Merged) :-
  put_assoc(Var, Info, Interval, Merged).
merge_info(Info1, Info2, Merged) :- 
  assoc_to_list(Info2, List), 
  foldl(List, merge_info, Info1, Merged).

interval_of(AI-_, Var, Interval) :-
  term_interval(AI, Var, Interval).

interval_of(_-Info, Var, Interval) :-
  (   get_assoc(Var, Info, Interval)
  ;
      %write('INFO: Could not analyze interval_of('), unparse(Var), writeln(')'),
      fail
  ), !.

%-----------------------------------------------------------------------
%% reduce(+Info, +Expr, -ExprRed) is det.
% try to reduce an AST expression into an interval Min..Max
% analysis information Info

reduce(_, Min..Max, Min..Max).

reduce(_, Int, I..I) :-
  isIntVal(Int, I).

% If evaluation doesn't yield a result, try to rewrite the expression
reduce(Info, Expr, X) :-
  simplify(Expr, ExprT),
  Expr \= ExprT,
  % This line is most powerful to debug the simplification rules:
  %write('simplifyed '), unparse(Expr), write(' INTO '), unparse(ExprT), nl,
  reduce(Info, ExprT, X).

reduce(Info, add_op(Expr1, Expr2, _, _, _), I) :-
  reduce(Info, Expr1, (I1lo..I1hi)),
  reduce(Info, Expr2, (I2lo..I2hi)),
  Ilo is I1lo + I2lo,
  Ihi is I1hi + I2hi,
  I = (Ilo..Ihi).

% The subtraction of two intervals is (xmin-ymax, xmax-ymin)
reduce(Info, subtract_op(Expr1, Expr2, _, _, _), I) :-
  reduce(Info, Expr1, (I1lo..I1hi)),
  reduce(Info, Expr2, (I2lo..I2hi)),
  Ilo is I1lo - I2hi,
  Ihi is I1hi - I2lo,
  I = (Ilo..Ihi).

% Interval Arithmetic
reduce(_, add_op((Min1..Max1), (Min2..Max2), _, _, _),
          (   Min3..Max3)) :-
  Min3 is Min1 + Min2,
  Max3 is Max1 + Max2.

reduce(_, subtract_op((Min1..Max1), (Min2..Max2), _, _, _),
	       (Min3..Max3)) :-
  Min3 is Min1 - Max2,
  Max3 is Max1 - Min2.

reduce(_, minus_op((Min1..Max1), _, _, _), (Min2..Max2)) :-
  Min2 is -Max1,
  Max2 is -Min1.

reduce(_, and_op((Min1..Max1), E1, _, _, _), (Min2..Max2)) :-
  isIntVal(E1, Mask),
  Min2 is Min1 /\ Mask,
  Max2 is Max1 /\ Mask.

reduce(_, or_op((Min1..Max1), E1, _, _, _), (Min2..Max2)) :-
  isIntVal(E1, Mask),
  Min2 is Min1 \/ Mask,
  Max2 is Max1 \/ Mask.

reduce(Info, Var, Interval) :-
   % Use this rule only if we cannot determine a result otherwise
  Var = var_ref_exp(_, _, _),
  interval_of(Info, Var, Interval).

%-----------------------------------------------------------------------
%% simplification_fixpoint(+Expr, -Interval).
% transform until we reach a fixpoint
%
simplification_fixpoint(E, Efix) :-
  %write('simplification( '), write(E), write(') '), nl,
  simplification(E, Etrans), !,
  %write('$transformed '), unparse(E), write(' INTO '), unparse(Etrans), nl,
  simplification_fixpoint(Etrans, Efix).
simplification_fixpoint(E, E).
  %\+ simplification(E, _).

%-----------------------------------------------------------------------
%% simplify(+Expr, -ExprSimple).
% recurse simplification
simplify(E, Et) :-
  simplification_fixpoint(E, Ewk),
  isBinNode(Ewk, F, E1, E2, A, Ai, Fi), 
  simplify(E1, E1t),
  simplify(E2, E2t),
  isBinNode(Ewk2, F, E1t, E2t, A, Ai, Fi),
  simplification_fixpoint(Ewk2, Et).
  %write('#simplifyed '), unparse(E), write(' INTO '), unparse(Ewk2), nl,
  %simplify(Ewk2, Et).
  %(E \= Et -> write('>simplifyed2 '), unparse(E), write(' INTO '), unparse(Et), nl)

simplify(E, Et) :- simplification_fixpoint(E, Et).
   %(E \= Et -> write('>simplifyed3 '), unparse(E), write(' INTO '), unparse(Et), nl).
simplify(E, E).

%-----------------------------------------------------------------------
%% simplification(+Expr, -ExprSimple).
% In here, we define a set of simplification rules that allow us to reason
% about loop bound information.
% All simplification rules have to satisfy the following conditions:
%   (*) Correctness (duh)
%   (*) Confluence - No cycles!
%         Ideally, simplifications should shrink the expression

% Strip Vars
simplification(var_ref_exp(var_ref_exp_annotation(Type1,Name,T,S,PPI), Ai, Fi),
	       var_ref_exp(var_ref_exp_annotation(Type1,Name,T,S,PPI), Ai, _)):-
  compound(Fi).

% Basic Arithmetic
simplification(add_op(E1, E2, _, _, _), E3) :-
  isIntVal(E1, X), isIntVal(E2, Y), Z is X+Y, isIntVal(E3, Z).

simplification(subtract_op(E1, E2, _, _, _), E3) :-
  isIntVal(E1, X), isIntVal(E2, Y), Z is X-Y, isIntVal(E3, Z).

simplification(minus_op(E1, _, _, _), E2) :-
  isIntVal(E1, X), Z is -X, isIntVal(E2, Z).

simplification(and_op(E1, E2, _, _, _), E3) :-
  isIntVal(E1, X), isIntVal(E2, Y), Z is X/\Y, isIntVal(E3, Z).

simplification(or_op(E1, E2, _, _, _), E3) :-
  isIntVal(E1, X), isIntVal(E2, Y), Z is X\/Y, isIntVal(E3, Z).

simplification(xor_op(E1, E2, _, _, _), E3) :-
  isIntVal(E1, X), isIntVal(E2, Y), Z is X xor Y, isIntVal(E3, Z).

simplification(arrow_exp(_,(_.._),binary_op_annotation(Type,_), _, _), I):-
  type_interval(Type, I). % Fallback, we don't know anything

% Type Casting
simplification(assign_op(E1,
         cast_exp(E2, _, unary_op_annotation(_, Type, _, _, _), _, _),
			 binary_op_annotation(Type, _), _, _),
	       assign_op(E1, E2, _, _, _)).

simplification(cast_exp(var_ref_exp(var_ref_exp_annotation(Type1,Name,T,S,PPI),A,_),
		        _, unary_op_annotation(_, Type2, _, _), _, _),
	       var_ref_exp(var_ref_exp_annotation(Type1,Name,T,S,PPI),A,_)) :-
  type_info(Type1, Signed1, Size1),
  type_info(Type2, Signed2, Size2),
  Size1 =< Size2,
  ( Signed1 = Signed2
  ; (Signed1 = unsigned,
     Signed2 = signed)).
  
simplification(CastExp, and_op(E1, Mask, _, _, _)) :- 
  CastExp = cast_exp(E1, _, unary_op_annotation(_, Type, _, _, _), _, _),
  type_info(Type, _, Size),
  M is (2**Size) -1,
  isIntVal(Mask, M).


% FXIME: allow the ast_equiv only for constants!

% cancellation of '-
simplification(subtract_op(E1, E2, _, _, _), Z) :-
  ast_equiv(E1, E2), isIntVal(Z, 0).
% (a+b)-b = a
simplification(subtract_op(add_op(E1, E2, _, _, _), E3, _, _, _), E1) :-
  ast_equiv(E2, E3).
% (a+b)-a = b
simplification(subtract_op(add_op(E1, E2, _, _, _), E3, _, _, _), E2) :-
  ast_equiv(E1, E3).
% (v+i1)-i2 = v+i'
simplification(subtract_op(add_op(E1, E2, _, _, _), E3, _, _, _),
	       add_op(E1, E4, _, _, _)) :-
  isIntVal(E2, X), isIntVal(E3, Y), Z is X-Y, isIntVal(E4, Z).
% (v-i1)-i2 = v-i'
simplification(subtract_op(subtract_op(E1, E2, _, _, _), E3, _, _, _),
	       subtract_op(E1, E4, _, _, _)) :-
  isIntVal(E2, X), isIntVal(E3, Y), Z is X-Y, isIntVal(E4, Z).

% a-(b+a) = -a
simplification(subtract_op(E1, add_op(E2, E3, _, _, _), _, _, _),
	       minus_op(E2, _, _, _)) :- ast_equiv(E1, E3).
% a-(a+b) = -b
simplification(subtract_op(E1, add_op(E2, E3, _, _, _), _, _, _),
	       minus_op(E3, _, _, _)) :- ast_equiv(E1, E2).

% (i1+v)-i2 = v+i'
simplification(subtract_op(add_op(E1, E2, _, _, _), E3, _, _, _),
	       add_op(E4, E2, _, _, _)) :- 
  isIntVal(E1, X), isIntVal(E3, Y), Z is X-Y, isIntVal(E4, Z).

% Some associativity of '+'
%simplification(subtract_op(add_op(E1, E2, _, _, _), E3, _, _, _),
 %              add_op(E1, subtract_op(E2, E3, _, _, _), _, _, _)) :-
  % Hints on when to apply this rule
 % isVar(E3,_).
%simplification(subtract_op(add_op(E1, E2, _, _, _), E3, _, _, _),
%               add_op(subtract_op(E1, E3, _, _, _), E2, _, _, _)) :-
  % Hints on when to apply this rule
%  isVar(E3,_).

simplification(add_op(add_op(E1, E2, _, _, _), E3, _, _, _),
               add_op(R, E2, _, _, _)) :-
  % Hints on when to apply this rule
  isIntVal(E1,I1), isIntVal(E3,I2), I is I1+I2, isIntVal(R, I).

simplification(assign_op(V1, subtract_op(V1, E1, _, _, _), _, _, _),
	       minus_assign_op(V1, E1, _, _, _)).

simplification(assign_op(V1, add_op(V1, E1, _, _, _), _, _, _),
	       plus_assign_op(V1, E1, _, _, _)).

% Worstcase minimal stepsize for unsigned variable increments
isUnsignedVarStep(Info, plus_assign_op(InductionVar, Var, _, _, _),
		  InductionVar, 1) :-
  var_interval(Info, Var, (Min.._)),
  Min >= 0.

%-----------------------------------------------------------------------
%% is_real_for_loop(+ForStmt,+Info,-AnalysisInfo,-Body,-IterationVar,-Interval,-Step).
% Determine wether a given for loop is a "simple", counter-based loop and
% extract its parameters
is_real_for_loop(for_statement(ForInit,
			       ForTest,
			       ForStep,
			       Body,
			       _An,
			       AnalysisInfo % Merged preinfo for all 4 children
			      , _Fi),
		 Info, AnalysisInfo,
		 Body, IterationVar,
		 (Min..Max),
		 Step) :-
%  write('% '), unparse(for_statement(ForInit,ForTest,ForStep,[], _, _, _)), nl,   gtrace,
  (isSimpleForInit(ForInit, IterationVar, B1v)
  -> (
      term_interval(AnalysisInfo, B1v, B1),

      isForTestOp(ForTest, TestOp), 
      isBinOpRhs(TestOp, B2v),
      term_interval(AnalysisInfo, B2v, B2))
  ; ( % ForInit is empty
     isEmptyForInit(ForInit), 
     isForTestOp(ForTest, TestOp),
     isBinOpLhs(TestOp, I1),
     var_stripped(I1, IterationVar),
     ( interval_of(Info, IterationVar, B1)
     ; term_interval(AnalysisInfo, IterationVar, B1)
       % This will often (=janne_complex.c) fail, because due to the
       % way the ICFG is constructed we have no preinfo available if
       % the for_initializer is missing
     ),
     isBinOpRhs(TestOp, B2v),
     term_interval(AnalysisInfo, B2v, B2)
  )), 
    %unparse(B1v), write('->'), unparse(B1), nl,
    %writeln(B2v),
    %unparse(B2v), write('->'), unparse(B2), nl,

  % Assure we only have one induction variable
  isBinOpLhs(TestOp, I2),   var_stripped(I2, IterationVar),
  simplify(ForStep, SimpleForStep),
  ( isStepsize(SimpleForStep, I3, Step)
  ; isUnsignedVarStep(Info, SimpleForStep, I3, Step) ),
  var_stripped(I3, IterationVar),
  
  !,
  
  ((Step < 0) ->
    (MinX = B2l, MaxX = B1h, MinV = B2v, MaxV = B1v)
  ; (MinX = B1l, MaxX = B2h, MinV = B1v, MaxV = B2v)
  ),

  ( ( % Symbolic
      ground(MinV),
      ground(MaxV),
      Min = MinV, 
      Max = MaxV)
  ; ( % Evaluated
      reduce(Info, B1, (B1l..B1h)),
      reduce(Info, B2, (B2l..B2h)),
      isIntVal(Min, MinX), 
      isIntVal(Max, MaxX))
  ), 

  guarantee(Body, is_transp(IterationVar, local)),
  write('% **WARNING: Assuming that '), unparse(IterationVar),
  writeln(' is local'), write('% ').
  %write('is transp: '), writeln(I),


find_iv_interval(Info, InfoInner, PostCondition, I, Base, End) :- 
  % Find out the interval of IV
  reduce(Info, Base, StartR),
  reduce(Info, End, StopR),
  
  %unparse(for_statement(ForInit,ForTest,ForStep, [], _, _, _)), nl,
  StartR = (StartMin.._),          % write('StartR='), writeln(StartR),
  StopR = (StopMin..StopMax),      %  write('StopR='), writeln(StopR),
  IVinterval2 = (StartMin..StopMax), write('% IVinterval='), writeln(IVinterval2),
  merge_info(Info, I-IVinterval2, InfoInner), %write('inner '), writeln(InfoInner),

  PCmin is StopMin /*+Increment*/,
  PCmax is StopMax /*+Increment*/,
  merge_info(Info, I-(PCmin..PCmax), PostCondition).
  %write('post '), writeln(PostCondition)
  %merge_info(PostCond1, equiv(I, End),
    %add_op(End, int_val(_, value_annotation(Increment, _), _, _), _)),
 %	     PostCondition).%,   write('post '), writeln(PostCondition).

%-----------------------------------------------------------------------
%% get_loopbound(+Fs, -Bound, -I, +Info, -InfoInner, -PostCondition).
% This is the heart of the loop_bounds/4 predicate.
% Algorithm:
%   1. transform(loop_end - loop_start)
%   2. divided by step
%   resulting in: either an Integer
%                 or at least an interval (min..max)
%

get_loopbound(Fs, Bound, I, Info, InfoInner, PostCondition) :-
  is_real_for_loop(Fs, Info, AnInfo, _, I,
		   (Base..End),
		   Increment),

  % Find out the iteration space
  %unparse(subtract_op(EndExpr, BaseExpr, _, _, _)), nl,

  term_stripped(subtract_op(End, Base, null, null, null), Term),
  reduce(AnInfo-Info, Term, LoopBoundR), %write('LoopBoundR= '),writeln(LoopBoundR),
  LoopBoundR = (IV_low..IV_high),

  % Find the loop bounds
  (   is_fortran_for_loop(Fs, _, _, _, _, _)
  ->  Low is floor((IV_low + 1 /*LE*/) / abs(Increment))
  ;   Low = 0  ), % Early exits or multiple exit conditions not handled yet
  High is ceil((IV_high + 1 /*LE*/) / abs(Increment)),
  Bound=Low..High, 

  !,
  (   High #> 65536
  ->  format('% ** Ignoring overly large loop bound ~w.~n', [Bound]), fail
  ;   true),
  
  write(' --> '), write('Bound= '), writeln(Bound),

  % Try to find the Induction Variable interval;
  % this information is used by the inner loop
  (find_iv_interval(Info, InfoInner, PostCondition, I, Base, End) -> true
  ; (Info = InfoInner,
     Info = PostCondition)
  ).

% special case for while(True)
%get_loopbound(InfiniteLoop, Bound, _, Info, Info, Info) :-
%  InfiniteLoop = while_stmt(expr_statement(Expr, _, _, _), _ ,_, _, _),
%  is_const_val(Expr, Val),
%  Val \= 0,
%  Bound is 1.

% shift-loop
get_loopbound(ShiftLoop, Bound, _, Info, Info, Info) :-
  ShiftLoop = for_statement(_,
			    expr_statement(not_equal_op(Var1, End, _, _, _), _, _, _),
			    rshift_assign_op(Var2, Amount, _, _, _),
			    Body, _, _, _), 
  guarantee(Body, is_transp(Var, local)), 
  isIntVal(End, 0),
  isIntVal(Amount, AmountVal),
  var_stripped(Var1, Var),
  var_stripped(Var2, Var),
  Var = var_ref_exp(var_ref_exp_annotation(Type, _, _, _, _), _, _),
  type_info(Type, _, Size),
  Bound is ceil(Size/AmountVal).


%-----------------------------------------------------------------------
% Insert an annotation as the last Statement of a for loop's inner basic block
insert_annot(for_statement(Init,Test,Incr, X, An, Ai, Fi),
	     Annot,
	     for_statement(Init,Test,Incr, Y, An, Ai, Fi)) :-
  (   X = basic_block(Stmts, An1, Ai1, Fi1)
  ->  append(Stmts, [Annot], Stmts1),
      Y = basic_block(Stmts1, An1, Ai1, Fi1)
  ;   default_values(_PPI, An1, Ai1, Fi1),
      Y = basic_block([X,Annot], An1, Ai1, Fi1)
  ).

% insert_annot(while_stmt(Expr,basic_block(A1, Fi1), A2, Fi2),
% 	     Annot,
% 	     while_stmt(Expr,basic_block([Annot], A1, Fi1), A2, Fi2)).

% insert_annot(while_stmt(Expr,basic_block(XS, A1, Fi1), A2, Fi2),
% 	     Annot,
% 	     while_stmt(Expr,basic_block(XS1, A1, Fi1), A2, Fi2)) :-
%   append(XS, [Annot], XS1).

replace_loopbody(for_statement(Init, Test, Incr,
			       _, A, Ai, Fi),
		 Body,
		 for_statement(Init, Test, Incr,
			       basic_block(Body, A, Ai, Fi), A, Ai, Fi)).

%-----------------------------------------------------------------------
% Annotate the easy loop bounds

%% loop_bounds(+Info, -InfoInner, -InfoPost, +Fs, -Fs_Annot).
loop_bounds(Info, InfoInner, InfoPost, Fs, Fs_Annot) :- 
  get_loopbound(Fs, Bound, /*InductionVar*/_, Info, InfoIn, _InfoPo),
  A = wcet_loopbound(Bound), write('% '), pragma_text(Annot, A), writeln(A),

  (   Fs = for_statement(_,_,_,basic_block(Stmts, _, _, _), _, _, _),
      get_annot(Stmts, wcet_loopbound(_), _),
      Fs = Fs_Annot
  ;   % only insert new loop bounds - don't overwrite manual annotations
      insert_annot(Fs, Annot, Fs_Annot)
  ),

  empty_assoc(InfoPost),
  merge_info(Info, InfoIn, InfoInner), !.
%  merge_info(Info, InfoPo, InfoPost).
  %writeln(InfoIn),
  %writeln(InfoInner),
  %writeln(InfoPost),nl.
loop_bounds(I, I, I, Term, Term).

% Return the label of the first non-loopbound node
get_bb_marker(Stmts, Marker) :-
  member(Stmt, Stmts),
  \+ pragma_text(Stmt, wcet_loopbound(_)),
  analysis_info(Stmt, analysis_info(Ai)),
  member(entry_exit_labels(Id-_), Ai),
  atom_concat(label, Id, Marker).

%-----------------------------------------------------------------------
% Constraints ...
%-----------------------------------------------------------------------

%% expr_constr(+Expr, +AM, -Expr1) is det.
% AM is (Analysisresult-Map)
expr_constr(add_op(E1, E2, _, _, _), Map, Expr) :-
  expr_constr(E1, Map, Expr1),
  expr_constr(E2, Map, Expr2),
  Expr #= Expr1 + Expr2. % FIXME  mod type (Expression)

expr_constr(subtract_op(E1, E2, _, _, _), Map, Expr) :-
  expr_constr(E1, Map, Expr1),
  expr_constr(E2, Map, Expr2),
  Expr #= Expr1 - Expr2.    

expr_constr(and_op(E1, E2, _, _, _), Map, Expr) :-
  expr_constr(E1, Map, Expr1),
  expr_constr(E2, Map, Expr2),
  number(Expr1),
  number(Expr2),
  Expr is Expr1 /\ Expr2.

expr_constr(or_op(E1, E2, _, _, _), Map, Expr) :-
  expr_constr(E1, Map, Expr1),
  expr_constr(E2, Map, Expr2),
  number(Expr1),
  number(Expr2),
  Expr is Expr1 \/ Expr2.

expr_constr(Min2Func, Map, Expr) :-
  isMin2Func(Min2Func, E1, E2),
  expr_constr(E1, Map, Expr1),
  expr_constr(E2, Map, Expr2),
  Expr #= min(Expr1, Expr2).

expr_constr(IntVal, _Map, Expr) :-
  is_const_val(IntVal, Val),
  Expr #= Val.

expr_constr(Var, Map, Expr) :-
  lookup(Var, Map, Expr1),
  Expr #= Expr1.

expr_constr(Term, AR-_Map, Expr) :-
  term_interval(AR, Term, (Min..Max)),
  Expr #>= Min,
  Max < 2**24,
  Expr #=< Max.

expr_constr(Term, AR-Map, Expr) :- 
  simplification(Term, SimpleTerm),
  expr_constr(SimpleTerm, AR-Map, Expr).

%expr_constr(_Term, _AR-_Map, _Expr) :- trace.

step_constr(comma_op_exp(Step1, Step2, _, _, _), Map, Dir) :-
  step_constr(Step1, Map, Dir),
  step_constr(Step2, Map, Dir).
  
step_constr(ForStep, Map, Dir) :-
  isStepsize(ForStep, InductionVar, Step),
  (  Step > 0
  -> Dir = up
  ;  Dir = down),
  lookup(InductionVar, Map, IV),
  IV mod abs(Step) #= 0.

%step_constr(_Term, _AR-_Map, _Dir) :- trace.

init_constr(ForInit, Map, Dir) :-
  isSimpleForInit(ForInit, InductionVar, InitVal),
  lookup(InductionVar, Map, IV),
  (  Dir = up
  -> IV #>= Expr
  ;  IV #=< Expr),
  expr_constr(InitVal, Map, Expr).

%init_constr(_Term, _AR-_Map, _Dir) :- trace.

init_constr2(Map, AR, Var) :-
  get_assoc(Var, Map, CLP_Var),
  ( term_interval(AR, Var, (Min..Max)),
    CLP_Var #>= Min,
    CLP_Var #=< Max
  ; true).
  %fd_dom(CLP_Var, Dom), unparse(Var), write(' -> '), writeln(Dom).

test_constr(expr_statement(E1, _, _, _), Map) :-
  test_constr(E1, Map).

test_constr(and_op(E1, E2, _, _, _), Map) :-
  test_constr(E1, Map),
  test_constr(E2, Map).
  
test_constr(ForTest, Map) :-
  isForTestLE(ForTest, less_or_equal_op(InductionVar, Than, _, _, _)),
  lookup(InductionVar, Map, IV),
  IV #=< Expr,
  expr_constr(Than, Map, Expr).

test_constr(ForTest, Map) :-
  isForTestGE(ForTest, greater_or_equal_op(InductionVar, Than, _, _, _)),
  lookup(InductionVar, Map, IV),
  IV #>= Expr,
  expr_constr(Than, Map, Expr).

%test_constr(_Term, _AR-_Map, _Dir) :- trace.

lookup(Var, _AR-Map, Constraint) :-
  var_stripped(Var, V),
  get_assoc(V, Map, Constraint).


is_for_loop(Fs, InductionVars, ForInit, ForTest, ForStep, Body) :-
  is_fortran_multicond_for_loop(Fs, _, ForInit, ForTest, ForStep, Body),
  iv(ForTest, Body, InductionVars).

iv(expr_statement(E1, _, _, _), Body, IVs) :-
  iv(E1, Body, IVs).

iv(and_op(E1, E2, _, _, _), Body, IVs) :-
  iv(E1, Body, IVs1),
  iv(E2, Body, IVs2),
  append(IVs1, IVs2, IVs).

iv(Op, Body, [IV]) :-
  ( isForTestLE(Op, less_or_equal_op(ItV, _, _, _, _))
  ; isForTestGE(Op, greater_or_equal_op(ItV, _, _, _, _))),
  var_stripped(ItV, IV),
  guarantee(Body, is_transp(IV, local)).

gen_varmap([], Map, Map).
gen_varmap([InductionVar|IVs], Map, Map2) :-
  put_assoc(InductionVar, Map, _Var, Map1),
  gen_varmap(IVs, Map1, Map2).

%% loop_constraints(+Fs, -Fs_Annot, +RootMarker, +Map).
loop_constraints(Fs, Fs_Annot, RootMarker, Map) :-
  Fs = for_statement(_, _, _, _, _An, Aipre, _Fi),

  is_for_loop(Fs, InductionVars, ForInit, ForTest, ForStep,
	      basic_block(Stmts, _An1, Aibody0, _Fi1)),
  (Stmts = [Stmt|_]
  -> analysis_info(Stmt, Aibody)
  ;  Aibody = Aibody0),

  %replace_loopbody(Fs, [], FsPrint),
  %unparse(FsPrint), writeln('...'), 

  !,
  %gtrace,
  gen_varmap(InductionVars, Map, Map1),
  %trace,
  step_constr(ForStep, Aibody/*FIXME!!! body*/-Map1, Dir),
  init_constr(ForInit, Aipre-Map1, Dir),
  test_constr(ForTest, Aibody/*FIXME!!! body*/-Map1),

  % additional init constraints
  maplist(init_constr2(Map1, Aipre), InductionVars),

  assoc_to_values(Map1, Vars),
  %Vars=[I,J,K], findall((I,J,K), (indomain(I),indomain(J)), Is),writeln(Is),
  %length(Ns, IterationCount),
  findall(C, labeling([upto_in(C)], Vars), Cs),
  sum(Cs, #=, IterationCount),
  %writeln(Ns),
  
  get_bb_marker(Stmts, ThisMarker),
  
  % Add constraint
  pragma_text(Annot, wcet_constraint(ThisMarker=<RootMarker*IterationCount)),
  append(Stmts,[Annot],Stmts1), !,

  % foster Children
  iter_children(Stmts1, Stmts2, RootMarker, Map1),
  replace_loopbody(Fs, Stmts2, Fs_Annot).


iter_children([Stmt|Stmts], [Stmt1|Stmts1], RM, Map) :-
  ( loop_constraints(Stmt, Stmt1, RM, Map)
  ; Stmt = Stmt1),
  iter_children(Stmts, Stmts1, RM, Map).
iter_children([], [], _, _).

%-----------------------------------------------------------------------
% Annotate Constraints
%% constraints(?I, ?I, ?I, +Fs, -Fs_Annot).
constraints(I, I, I, Fs, /*Scope,*/ Fs_Annot) :-
  % We need to guarantee that Fs is an induction-variable-
  % based loop, and that the induction variable I is transp().
  %term_stripped(Fsc, Fs),
  is_fortran_multicond_for_loop(Fs, _, _, _, _, _), 
  Fs = for_statement(_,_,_, basic_block(Stmts, _, _, _), _, _, _),

  % We employ our own "interpretation traversal", so don't traverse this
  % subtree if we already visited it from a parent scope
  \+ get_annot_term(Stmts, wcet_constraint(_), _),

  %write('constraints('), unparse(Fs), writeln(')'), gtrace,
  empty_assoc(Map),
  I = parentMarker:RootMarker,
  loop_constraints(Fs, Fs_Annot, RootMarker, Map),

  % Generate a sope for the markers that are used by the constraints
  % FIXME: collect all markers used inside.
  %term_to_string(wcet_scope(ThisMarker), Text),
  %pragma_text(Scope, Text),
  !.
  %ground(Fs_Annot).

% save the parent marker
constraints(I, parentMarker:Marker, I, Term, Term) :-
  Term = basic_block(Stmts, _, _, _),
  get_bb_marker(Stmts, Marker).

constraints(I, I, I, Fs, Fs).


% translate Term -> String in the pragmas
pragma_fixup(I, I, I, Pragma, Pragma) :-
  pragma_text(Pragma, Atom),
  atom(Atom).

pragma_fixup(I, I, I, Pragma, FixedPragma) :-
  pragma_text(Pragma, Term),
  term_to_atom(Term, Atom),
  pragma_text(FixedPragma, Atom).

pragma_fixup(I, I, I, T, T).


% Insert Markers for the ICFG Labels
marker_fixup(Info, InfoInner, InfoPost,
	 basic_block(Stmts, An, Ai, Fi),
	 basic_block([MarkerPragma|Stmts], An, Ai, Fi)) :-
  member(Stmt, Stmts),
  \+ pragma_text(Stmt, wcet_loopbound(_)),
  analysis_info(Stmt, analysis_info(AI)),
  member(entry_exit_labels(Id-_), AI),
  atom_concat(label, Id, Marker),
  term_to_atom(wcet_marker(Marker), MarkerA),
  pragma_text(MarkerPragma, MarkerA),
  update_marker_info(Info, InfoInner, InfoPost,
		     basic_block(Stmts, An, Ai, Fi), _Marker),
  !.

marker_fixup(Info, InfoInner, InfoPost, Node, Node) :- 
  update_marker_info(Info, InfoInner, InfoPost, Node, _Marker).

%-----------------------------------------------------------------------
% MAIN
%-----------------------------------------------------------------------

annot(Input, Output) :-
  X = Input,

  % GOTO -> WHILE conversion
  writeln('% goto -> while() conversion...'),
  transformed_with(X, come_from, [], _, X1), !,

  % WHILE -> FOR conversion
  writeln('% while() to for() conversion...'),
  transformed_with(X1, while_to_for, [], _, X2), !,
  
  % Loop Bounds
  writeln('% Loop Bounds...'),
  empty_assoc(Info),
  transformed_with(X2, loop_bounds, Info, _, X3), !,

  % Constraints
  writeln('% Constraints...'), 
  transformed_with(X3, constraints, [], _, X4), !,

  % Pragma Terms->Atoms
  transformed_with(X4, pragma_fixup, _, _, X5), !,

  % Insert Markers for the ICFG Labels
  transformed_with(X5, marker_fixup, info(marker_stem('m'), marker_count(1)), _, X6), !,

  X6 = Output.

main :-
  catch((
    prompt(_,''),
    % Read input
    read_term(Input, []),
    compound(Input),

    %profile(annot(Input, Output)),gtrace,
    annot(Input, Output),

    % Write output
    write_term(Output, [quoted(true)]),
    writeln('.'),
    statistics

  ), E, (print_message(error, E), fail)),

  halt.

main :-
  format(user_error, '% Usage: loopbounds <Input >Output', []),
  halt(1).

% Finish error handling (see top of source file) by halting with an error
% condition of Prolog generated any warnings or errors.
%:- (prolog_reported_problems -> halt(1) ; true).
