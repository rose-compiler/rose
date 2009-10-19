#!/usr/local/mstools/bin/pl -q -t main -f
% -*- prolog -*-
%-----------------------------------------------------------------------
/** <module> Perform loop unrolling on a program

@author

Copyright 2009 Adrian Prantl <adrian@complang.tuwien.ac.at>

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

:- prolog_load_context(directory, CurDir),
   asserta(library_directory(CurDir)),
   (getenv('TERMITE_LIB', TermitePath)
   ; (print_message(error, 'Please set the environment variable TERMITE_LIB'),
      halt(1))
   ),
   asserta(library_directory(TermitePath)).

:- use_module([library(clpfd),
	       library(astproperties),
	       library(asttransform),
	       library(loops),
	       library(utils)]).

%-----------------------------------------------------------------------
% Loop unrolling on for-loops
%
% 
%-----------------------------------------------------------------------

replace_ivar(I+0,I+0,I+0, X, X) :- !.
replace_ivar(I+Inc, I+Inc, I+Inc, VarRefExp, add_op(I, IncVal, An0, Ai0, Fi0)):-
  var_stripped(VarRefExp, I), 
  Inc #> 0, !,
  isIntVal(IncVal, Inc),
  default_values(_PPI, An0, Ai0, Fi0).
replace_ivar(I+Inc, I+Inc, I+Inc, VarRefExp,subtract_op(I,IncVal,An0,Ai0,Fi0)):-
  var_stripped(VarRefExp, I), 
  Inc #< 0, !,
  IncNeg #= - Inc,
  isIntVal(IncVal, IncNeg),
  default_values(_PPI, An0, Ai0, Fi0).
replace_ivar(I,I,I, X, X).

dup_bodies(Stmts, I, Stride, Nn, Body) :-
  Nn1 #= Nn*Stride,
  dup_bodies(Stmts, I, Stride, 0, Nn1, Body).

dup_bodies(_, _, _, N, N, []).
dup_bodies(Stmts, I, Stride, N, Nn, [Body|Bodies]) :-
  Inc #= N,
  transformed_with(Stmts, replace_ivar, postorder, I+Inc, _, Body),
  N1 #= N + Stride,
  dup_bodies(Stmts, I, Stride, N1, Nn, Bodies).

unrolled(_, _, _, Fs, Fs1) :- 
  once(is_fortran_for_loop(Fs, I, Init, Test, Step, Body)),
  %unparse(Fs),nl, gtrace,
  
  % Wrap a single statement inside of { }
  (   Body = basic_block(Stmts, An, Ai, Fi)
  ->  Bb = Body
  ;   Bb = basic_block(Stmts, An, Ai, Fi),
      default_values(_PPI, An, Ai, Fi),
      Stmts = [Body]
  ),
  get_annot(Stmts, wcet_loopbound(N..N), _), % only unroll constant for loops
  isStepsize(Step, _, Stride),

  % Duplicate the loop body
  dup_bodies(Bb, I, Stride, N, Bodies),
  N1 #= N*abs(Stride),
  isIntVal(Nval, N1),
  (   Stride #> 0
  ->  StepN = expr_statement(plus_assign_op(I, Nval,An0,Ai0,Fi0),An0,Ai0,Fi0)
  ;   StepN = expr_statement(minus_assign_op(I, Nval,An0,Ai0,Fi0),An0,Ai0,Fi0)
  ),
  Fs1 = for_statement(Init, Test, StepN,
		      basic_block(Bodies, An, Ai, Fi),
		      An0, Ai0, Fi0),
%  unparse(Fs1),nl, 
  !.

unrolled(_, _, _, Fs, Fs).

%-----------------------------------------------------------------------

main :-
  prompt(_,''),
  % Read input
  read_term(P, []),
  compound(P),

  writeln('% unrolling...'),
  transformed_with(P, unrolled, postorder, [], _, P2), !,

  write_term(P2, [quoted(true)]),
  writeln('.').
