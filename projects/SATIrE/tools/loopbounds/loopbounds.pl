#!/usr/bin/pl -q  -O  -L64M -G64M -t main -f 
% -*- prolog -*-

%-----------------------------------------------------------------------
% loopbounds.pl
%
% This program analyzes loop bounds of C (and some C++) programs.  The
% code was extraced from the TuBound WCET analysis tool and is now
% distributed with SATIrE to serve as an example of what you can do
% with TERMITE.
%
% A detailed description of the algorithm can be found in the appendix
% of [1].
%
%
% References
%
% [1] A. Prantl, J. Knoop, M. Schordan and M. Triska.
%     Constraint solving for high-level WCET analysis.
%     The 18th Workshop on Logic-based methods in Programming
%     Environments (WLPE 2008). Udine, Italy, December 12, 2008.
%
%
% Authors
% -------
%
% Copyright (C) 2007-2009, Adrian Prantl
%
% License
% -------
%
% This program is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; version 3 of the License.
% 
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
%-----------------------------------------------------------------------
:- prolog_load_context(directory, CurDir),
   asserta(library_directory(CurDir)),
   getenv('TERMITE_LIB', TermitePath),
   ( atom(TermitePath)
   ; writeln('**ERROR: Please set then environment variable $TERMITE_LIB')
   ),
   asserta(library_directory(TermitePath)).

:- use_module(library(ast_transform)),
   use_module(library(ast_properties)),
   use_module(library(loops)),
   use_module(library(types)),
   use_module(library(utils)),
   use_module(while2for),
   use_module(library(assoc)),
   use_module(library(apply_macros)),
   use_module(library(swi/pce_profile)).

%-----------------------------------------------------------------------
% merge_info/3
% replaces has() and equiv() info with new data
%
% FIXME: Rewrite this more efficiently using assoc
%

merge_info(equiv(_,Var), _, _) :- var(Var), gtrace, fail.

merge_info(Info, [X|Xs], Merged) :-
  merge_info(Info, X, M1),
  merge_info(M1, Xs, Merged).

merge_info([], Info, [Info]) :- !.
merge_info(Info, [], Info) :- !.
merge_info([has(Var,_)|Xs], has(Var,Interval), Merged) :-
  append([has(Var,Interval)], Xs, Merged), !.
merge_info([equiv(Var,_)|Xs], equiv(Var,Term), Merged) :-
  append([equiv(Var,Term)], Xs, Merged), !.
merge_info([X|Xs], Info, [X|Ms]) :-
  merge_info(Xs, Info, Ms), !.
merge_info(A, B, C) :- write('ERROR: merge_info '), writeln(A), writeln(B),writeln(C),nl,gtrace.

interval_of(AI-_, Var, Interval) :-
  term_interval(AI, Var, Interval).

interval_of(_-Info, Var, Interval) :-
  (   memberchk(has(Var,Interval), Info)
  ;
      %write('INFO: Could not analyze interval_of('), unparse(Var), writeln(')'),
      fail
  ), !.

equiv_with(Info, Var, Val) :-
  (   memberchk(equiv(Var,Val), Info)
  ;
      %write('INFO: Could not eval equiv_with('), unparse(Var), writeln(')'),
      fail
  ), !.

%-----------------------------------------------------------------------
% reduce/3:
% try to reduce an AST expression into an interval(Min, Max)
% analysis information Info

reduce(_, interval(Min, Max), interval(Min, Max)).

reduce(_, Int, interval(I, I)) :-
  isIntVal(Int, I).

% If evaluation doesn't yield a result, try to rewrite the expression
reduce(Info, Expr, X) :-
  simplify(Expr, ExprT),
  Expr \= ExprT,
  % This line is most powerful to debug the simplification rules:
  %write('simplifyed '), unparse(Expr), write(' INTO '), unparse(ExprT), nl,
  reduce(Info, ExprT, X).

reduce(Info, add_op(Expr1, Expr2, _, _, _), I) :-
  reduce(Info, Expr1, interval(I1lo, I1hi)),
  reduce(Info, Expr2, interval(I2lo, I2hi)),
  Ilo is I1lo + I2lo,
  Ihi is I1hi + I2hi,
  I = interval(Ilo, Ihi).

% The subtraction of two intervals is (xmin-ymax, xmax-ymin)
reduce(Info, subtract_op(Expr1, Expr2, _, _, _), I) :-
  reduce(Info, Expr1, interval(I1lo, I1hi)),
  reduce(Info, Expr2, interval(I2lo, I2hi)),
  Ilo is I1lo - I2hi,
  Ihi is I1hi - I2lo,
  I = interval(Ilo, Ihi).

% Interval Arithmetic
reduce(_, add_op(interval(Min1, Max1), interval(Min2, Max2), _, _, _),
	       interval(Min3, Max3)) :-
  Min3 is Min1 + Min2,
  Max3 is Max1 + Max2.

reduce(_, subtract_op(interval(Min1, Max1), interval(Min2, Max2), _, _, _),
	       interval(Min3, Max3)) :-
  Min3 is Min1 - Max2,
  Max3 is Max1 - Min2.

reduce(_, minus_op(interval(Min1, Max1), _, _, _), interval(Min2, Max2)) :-
  Min2 is -Max1,
  Max2 is -Min1.

reduce(_, and_op(interval(Min1, Max1), E1, _, _, _), interval(Min2, Max2)) :-
  isIntVal(E1, Mask),
  Min2 is Min1 /\ Mask,
  Max2 is Max1 /\ Mask.

reduce(_, or_op(interval(Min1, Max1), E1, _, _, _), interval(Min2, Max2)) :-
  isIntVal(E1, Mask),
  Min2 is Min1 \/ Mask,
  Max2 is Max1 \/ Mask.

% Minimum function
reduce(Info, Min2Func, I) :-
  isMin2Func(Min2Func, Expr1, Expr2),
  reduce(Info, Expr1, I1),
  reduce(Info, Expr2, I2),
  wc_min(I1,I2,I).
  %write('min('), write(I1), write(', '), write(I2), write(') = '), writeln(I).

reduce(Info, Var, Interval) :-
   % Use this rule only if we cannot determine a result otherwise
  Var = var_ref_exp(_, _, _),
  interval_of(Info, Var, Interval).

%-----------------------------------------------------------------------
% simplification_fixpoint/2:
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
% simplify/2
% recurse simplification
simplify(E, Et) :-
  simplification_fixpoint(E, Ewk),
  isMin2Func(Ewk, E1, E2),
  simplify(E1, E1t),
  simplify(E2, E2t),
  isMin2Func(Et, E1t, E2t).
  %write('#simplifyed '), unparse(E), write(' INTO '), unparse(Ewk2), nl,
  %simplify(Ewk2, Et),
  %(E \= Et -> write('>simplifyed1 '), unparse(E), write(' INTO '), unparse(Et), nl).
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
% simplification/2
% In here, we define a set of simplification rules that allow us to reason
% about loop bound information.
% All simplification rules have to satisfy the following conditions:
%   (*) Correctness (duh)
%   (*) Confluence - No cycles!
%         Ideally, simplifications should shrink the expression

% Strip Vars
simplification(var_ref_exp(var_ref_exp_annotation(Type1,Name,T,S), Ai, Fi),
	       var_ref_exp(var_ref_exp_annotation(Type1,Name,T,S), Ai, _)) :-
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

simplification(arrow_exp(_,interval(_,_),binary_op_annotation(Type), _, _), I):-
  type_interval(Type, I). % Fallback, we don't know anything

% Type Casting
simplification(assign_op(E1,
         cast_exp(E2, _, unary_op_annotation(_, Type, _, _), _, _),
			 binary_op_annotation(Type), _, _),
	       assign_op(E1, E2, _, _, _)).

simplification(cast_exp(var_ref_exp(var_ref_exp_annotation(Type1,Name,T,S),A,_),
		        _, unary_op_annotation(_, Type2, _, _), _, _),
	       var_ref_exp(var_ref_exp_annotation(Type1,Name,T,S),A,_)) :-
  type_info(Type1, Signed1, Size1),
  type_info(Type2, Signed2, Size2),
  Size1 =< Size2,
  ( Signed1 = Signed2
  ; (Signed1 = unsigned,
     Signed2 = signed)).
  
simplification(CastExp, and_op(E1, Mask, _, _, _)) :- 
  CastExp = cast_exp(E1, _, unary_op_annotation(_, Type, _, _), _, _),
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

% bring the min2 function to the top-most level
simplification(subtract_op(MinFunc, Var, _, _, _), MinFuncT) :-
  isVar(Var, _), % Hint  
  isMin2Func(MinFunc, Expr1, Expr2), 
  isMin2Func(MinFuncT, subtract_op(Expr1, Var, _, _, _), subtract_op(Expr2, Var, _, _, _)).  
simplification(subtract_op(MinFunc, Int, _, _, _), MinFuncT) :-
  isIntVal(Int, _), % Hint  
  isMin2Func(MinFunc, Expr1, Expr2), 
  isMin2Func(MinFuncT, subtract_op(Expr1, Int, _, _, _), subtract_op(Expr2, Int, _, _, _)).  
simplification(add_op(Int, MinFunc, _, _, _), MinFuncT) :-
  isIntVal(Int, _), % Hint  
  isMin2Func(MinFunc, Expr1, Expr2), 
  isMin2Func(MinFuncT, add_op(Expr1, Int, _, _, _), subtract_op(Expr2, Int, _, _, _)).

simplification(assign_op(V1, subtract_op(V1, E1, _, _, _), _, _, _),
	       minus_assign_op(V1, E1, _, _, _)).

simplification(assign_op(V1, add_op(V1, E1, _, _, _), _, _, _),
	       plus_assign_op(V1, E1, _, _, _)).
%simplification(E, E). - not necessary any more; see simplification_fixpoint

%-----------------------------------------------------------------------
% wc_min/3:
% Get the worst-case of the min function
wc_min(interval(Xmin, Xmax), interval(Ymin, Ymax), interval(Min, Max)) :-
  Zmin is min(Xmin, Ymin),
  Zmax is min(Xmax, Ymax),
  Min is min(Zmin, Zmax),
  Max is max(Zmin, Zmax).

% Worstcase minimal stepsize for unsigned variable increments
isUnsignedVarStep(Info, plus_assign_op(InductionVar, Var, _, _, _),
		  InductionVar, 1) :-
  var_interval(Info, Var, interval(Min, _)),
  Min >= 0.

%-----------------------------------------------------------------------
% is_real_for_loop/7
% Determine wether a given for loop is a "simple", counter-based loop and
% extract its parameters
is_real_for_loop(for_statement(ForInit,
			       ForTest,
			       ForStep,
			       Body,
			       _An, Ai, _Fi),
		 Info, Ai, Body, IterationVar,
		 interval(Min, Max),
		 Step) :-
  %unparse(for_statement(ForInit,ForTest,ForStep,[], _, _, _)),nl,gtrace,
  (isSimpleForInit(ForInit, IterationVar, B1v)
  -> (
      term_interval(Ai, B1v, B1),

      isForTestOp(ForTest, TestOp), 
      isBinOpRhs(TestOp, B2v),
      term_interval(Ai, B2v, B2))
  ; ( % ForInit is empty
     isEmptyForInit(ForInit), 
     isForTestOp(ForTest, TestOp),
     isBinOpLhs(TestOp, I1),
     var_stripped(I1, IterationVar),
     ( interval_of(Info, IterationVar, B1)
     ; term_interval(Ai, IterationVar, B1)),
     isBinOpRhs(TestOp, B2v),
     term_interval(Ai, B2v, B2)
  )), 
    %unparse(B1v), write('->'), unparse(B1), nl,
    %writeln(B2v),
    %unparse(B2v), write('->'), unparse(B2), nl,
%gtrace,
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
      reduce(Info, B1, interval(B1l, B1h)),
      reduce(Info, B2, interval(B2l, B2h)),
      isIntVal(Min, MinX), 
      isIntVal(Max, MaxX))
  ), 

  guarantee(Body, is_transp(IterationVar, local)),
  write('**WARNING: Assuming that '), unparse(IterationVar),
  writeln(' is local').
  %write('is transp: '), writeln(I),


find_iv_interval(Info, InfoInner, PostCondition, I, Base, End) :- 
  % Find out the interval of IV
  reduce(Info, Base, StartR),
  reduce(Info, End, StopR),
  
  %unparse(for_statement(ForInit,ForTest,ForStep, [], _, _, _)), nl,
  StartR = interval(StartMin,_),          % write('StartR='), writeln(StartR),
  StopR = interval(StopMin,StopMax),      %  write('StopR='), writeln(StopR),
  IVinterval2 = interval(StartMin, StopMax), write('IVinterval='), writeln(IVinterval2),
  merge_info(Info,has(I, IVinterval2), InfoInner), %write('inner '), writeln(InfoInner),

  PCmin is StopMin /*+Increment*/,
  PCmax is StopMax /*+Increment*/,
  merge_info(Info,has(I, interval(PCmin, PCmax)), PostCond1),
  %write('post '), writeln(PostCondition)
  merge_info(PostCond1, equiv(I, End),
    %add_op(End, int_val(_, value_annotation(Increment), _, _), _)),
	     PostCondition).%,   write('post '), writeln(PostCondition).

%-----------------------------------------------------------------------
% get_loopbound/6:
% This is the heart of the loop_bounds/4 predicate.
% Algorithm:
%   1. transform(loop_end - loop_start)
%   2. divided by step
%   resulting in: either an Integer
%                 or at least a Interval(min, max)
%

get_loopbound(Fs, Bound, I, Info, InfoInner, PostCondition) :- 
  is_real_for_loop(Fs, Info, AnInfo, _, I,
		   interval(Base, End),
		   Increment),

  replace_loopbody(Fs, [], FsPrint),
  unparse(FsPrint), write(' --> '),

  % Find out the iteration space
  %unparse(subtract_op(EndExpr, BaseExpr, _, _, _)), nl,
  term_stripped(subtract_op(End, Base, null, null, null), Term),
  reduce(AnInfo-Info, Term, LoopBoundR), %write('LoopBoundR= '),writeln(LoopBoundR),
  LoopBoundR = interval(_, IVinterval1),

				% Find the loop bound
  Bound is ceil((IVinterval1 + 1 /*LE*/) / abs(Increment)),
  write('Bound= '), writeln(Bound),

  % Try to find the Induction Variable interval
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
  Var = var_ref_exp(var_ref_exp_annotation(Type, _, _, _), _, _),
  type_info(Type, _, Size),
  Bound is ceil(Size/AmountVal).


%-----------------------------------------------------------------------
% Insert an annotation as the first Statement of a for loop's inner basic block
insert_annot(for_statement(Init,Test,Incr,basic_block(XS, A1, Fi1), A2, Fi2),
	     Annot,
	     for_statement(Init,Test,Incr,basic_block(XS1, A1, Fi1),
			   A2, Fi2)) :-
  !, append(XS, [Annot], XS1).
insert_annot(for_statement(Init,Test,Incr,Stmt, A, Ai, Fi),
	     Annot,
	     for_statement(Init,Test,Incr,basic_block([Stmt|[Annot]], A, Ai, Fi),
			   A, Ai, Fi)).
insert_annot(while_stmt(Expr,basic_block(A1, Fi1), A2, Fi2),
	     Annot,
	     while_stmt(Expr,basic_block([Annot], A1, Fi1), A2, Fi2)).

insert_annot(while_stmt(Expr,basic_block(XS, A1, Fi1), A2, Fi2),
	     Annot,
	     while_stmt(Expr,basic_block(XS1, A1, Fi1), A2, Fi2)) :-
  append(XS, [Annot], XS1).


%-----------------------------------------------------------------------
% Annotate the easy loop bounds

loop_bounds(Info, InfoInner, [], Fs, Fs_Annot) :- 
  is_list(Info),
  get_loopbound(Fs, Bound, /*InductionVar*/_, Info, InfoIn, _InfoPo), !,
%  term_to_string(wcet_loopbound(Bound), A),
  A = wcet_loopbound(Bound),
  pragma_text(Annot, A), writeln(A),

  ((Fs = for_statement(_,_,_,basic_block(Stmts, _, _, _), _, _, _),
   get_annot(Stmts, wcet_loopbound(_), _),
   Fs = Fs_Annot)
  ; % only insert new loop bounds - don't overwrite manual annotations
  insert_annot(Fs, Annot, Fs_Annot)),
  
  merge_info(Info, InfoIn, InfoInner).
%  merge_info(Info, InfoPo, InfoPost).
  %writeln(InfoIn),
  %writeln(InfoInner),
  %writeln(InfoPost),nl.
loop_bounds(I, I, I, Term, Term).

%-----------------------------------------------------------------------
% MAIN
%-----------------------------------------------------------------------

annot(Input, Output) :-
  X = Input,

  % WHILE -> FOR conversion
  writeln('while() to for() conversion...'),
  transformed_with(X, while_to_for, [], _, X1), !,

  % Loop Bounds
  writeln('Loop Bounds...'),
  time(transformed_with(X1, loop_bounds, [], _, X2)), !,

  X2 = Output.

main :-
  current_prolog_flag(argv, Argv), 
  append(_, [--|Args], Argv),
  Args = [A1, A2], 

  catch((

    % Read input file
    open(A1, read, _, [alias(rstrm)]),
    read_term(rstrm, Input, []),
    close(rstrm), 
    compound(Input),

    %profile(annot(Input, Output)),gtrace,
    annot(Input, Output),
   
    % Write output file
    open(A2, write, _, [alias(wstrm)]),
    write_term(wstrm, Output, [quoted(true)]),
    write(wstrm, '.\n'),
    close(wstrm)

  ), E, (print_message(error, E), fail)),

  halt.

main :-
  writeln('Usage: loopbounds.pl Input Output'),
  halt(1).
