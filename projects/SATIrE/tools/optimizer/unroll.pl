#!/usr/local/mstools/bin/pl -q -t main -f
% -*- prolog -*-
%-----------------------------------------------------------------------
/** <module> Perform loop unrolling on a program

@author   Copyright 2009 Adrian Prantl <adrian@complang.tuwien.ac.at>
@license  See COPYING in the root folder of the SATIrE project

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
	       library(utils),
	       library(markers)]).

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
% Loop unrolling on for-loops
%
% 
%-----------------------------------------------------------------------

replace_ivar(I+0,I+0,I+0, X, X) :- !.
replace_ivar(I+Inc, I+Inc, I+Inc, VarRefExp, add_op(I, IncVal, An0, Ai0, Fi0)):-
  var_stripped(VarRefExp, I), 
  Inc #> 0, !,
  new_intval(Inc, IncVal),
  default_values(_PPI, An0, Ai0, Fi0).
replace_ivar(I+Inc, I+Inc, I+Inc, VarRefExp,subtract_op(I,IncVal,An0,Ai0,Fi0)):-
  var_stripped(VarRefExp, I), 
  Inc #< 0, !,
  Dec #= - Inc,
  new_intval(Dec, IncVal),
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

strip_labels(DAi,DAi,DAi, Node, N1) :-
  ast_node(Node, Type, Children, Annot, _, Fi),
  ast_node(N1, Type, Children, Annot, DAi, Fi).

% Remove invalidated annotation
remove_loopbounds(_, _, _, Pragma, []) :-
  pragma_text(Pragma, Text),
  atom_to_term(Text, wcet_loopbound(N..N), _), !.
remove_loopbounds(_, _, _, Node, Node).

% Perform loop unrolling for loops with strict bounds
unrolled(Info, InfoInner, InfoPost, Fs, Fs1) :-
  once(is_fortran_for_loop(Fs, I, Init, Test, Step, Body)),
  default_values(_PPI, An0, Ai0, Fi0),
  %unparse(Fs),nl, gtrace,

  % Only unroll innermost loops
  max_nesting_level(Fs, NestingLevel),
  (   NestingLevel #> 1 -> fail; true ),

  % Wrap a single statement inside of { }
  (   Body = basic_block(Stmts, An, Ai, Fi)
  ->  Bb = Body
  ;   Bb = basic_block(Stmts, An0, Ai0, Fi0),
      Stmts = [Body]
  ),
  get_annot(Stmts, wcet_loopbound(N..N), _), % only unroll constant for loops
  isStepsize(Step, _, Stride),
  
  % Duplicate the loop body
  transformed_with(Bb, strip_labels, preorder, Ai0, _, BbStripped),

  % Calculate the unroll factor
  unroll_factor(N, 8, K), 
  
  dup_bodies(BbStripped, I, Stride, K, Bodies),
  N1 #= K*abs(Stride),
  new_intval(N1, Nval),
  (   Stride #> 0
  ->  StepN = expr_statement(plus_assign_op(I, Nval,An0,Ai0,Fi0),An0,Ai0,Fi0)
  ;   StepN = expr_statement(minus_assign_op(I, Nval,An0,Ai0,Fi0),An0,Ai0,Fi0)
  ),
  Fs1 = for_statement(Init, Test, StepN,
		      basic_block(Bodies, An, Ai, Fi),
		      An0, Ai0, Fi0),

  % Ouptut trace
  update_marker_info(Info, InfoInner, InfoPost, Fs, Mbody),
  format(optrace, ' ~W,~n', [unrolled(Mbody, K), [quoted(true)]]),

%  unparse(Fs1),nl, 
  !.

unrolled(Info, InfoInner, InfoPost, Fs, Fs) :- 
  update_marker_info(Info, InfoInner, InfoPost, Fs, _).

unroll_factor(0, _, 0) :- !, fail.
unroll_factor(N, Max, N) :- N #=<Max.
unroll_factor(N, Max, K) :- N #>Max, K in 1..Max, N mod K #= 0,
  labeling([max(K)], [K]).

%-----------------------------------------------------------------------

main :-
  prompt(_,''),
  % Read input
  read_term(P, []),
  compound(P),

  writeln('% unrolling...'),

  current_prolog_flag(argv, Argv), 
  append(_, [--|[OutputTrace]], Argv),
  open(OutputTrace, write, _, [alias(optrace)]),
 
%  guitracer, profile(
  write(optrace, '[\n'),
  transformed_with(P, unrolled, preorder, 
		   info(marker_stem('m'), marker_count(1)), _, P1),
  transformed_with(P1, remove_loopbounds, postorder, [], _, P2)
%  ), !, gtrace
  ,
  
  write(optrace, 'end].\n'),
  close(optrace),


  write_term(P2, [quoted(true)]),
  writeln('.').

main :-
  format(user_error, 'Usage: unroll traceoutput <Input >Output~n',[]),
  halt(1).

% Finish error handling (see top of source file) by halting with an error
% condition of Prolog generated any warnings or errors.
:- (prolog_reported_problems -> halt(1) ; true).
