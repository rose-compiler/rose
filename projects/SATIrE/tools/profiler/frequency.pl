#!/usr/local/mstools/bin/pl -O -q -L0 -G0 -T0 -t main -f
% -*- prolog -*-

%-----------------------------------------------------------------------
% frequncy.pl
%
% About
% -----
%
% Create an frequency-colored ICFG
%
% Authors
% -------
%
% Copyright (C) 2008, 2009 Adrian Prantl
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

:- getenv('PWD', CurDir),
   asserta(library_directory(CurDir)),
   prolog_load_context(directory, SrcDir),
   asserta(library_directory(SrcDir)),
   (getenv('TERMITE_LIB', TermitePath)
   ; (print_message(error, 'Please set the environment variable TERMITE_LIB'),
      halt(1))
   ),
   asserta(library_directory(TermitePath)).

:- use_module(library(astwalk)),
   use_module(library(astproperties)),
   use_module(library(asttransform)),
   use_module(library(callgraph)),
   use_module(library(termlint)),   
   use_module(library(utils)).
:- use_module(library(clpfd)).
%:- use_module(library(clpq)), use_module(sum).

:- guitracer.

% FUNCTION SEARCH

% ast_walk/6: main traversal.
ast_walk(P, Function, Marker, CallGraph-Target, PM, P4) :-
%  catch(term_match(Function, function_declaration), _, ( !, fail ) ),
  ( goto_function(P, Function, P1)
  ; format('**ERROR: Could not locate function ~w.', [Function]),
    halt(1)
  ),
  function_signature(Function, Type, Name, _), 
  %writeln(Name),   writeln(Marker), 
  %  (Name = 'Read_AD_Channel' -> trace ; true),
  marker_into(Marker, Name, Marker1),
  marker_freq(Marker, Freq),
  marker_freq(Marker1, Freq),

  reachable(Name/Type, CallGraph, Vs),    
  (  (member(Target/_, Vs)
   ; Name = Target
   ; Target = 'SUM')
  -> down(P1, 2, P2),	    
     ast_walk(P2, Marker1, CallGraph-Target, PM, P3)
  ; P1 = P3),
% 
%   % NOT TRUE:
%   % Since we necessarily have already visited this function and all its
%   % siblings, we can safely label the constraints at this point.
%   % @tbd recursive functions won't work with the current walk
%   unzip(P3, FuncDef, Ctx),
%   transformed_with(FuncDef, gather_constraints, postorder, [], Constraints, _),
%   transformed_with(FuncDef, gather_constraints1, postorder,
% 		   triple([],   Target, _),
% 		   triple(Sums, Target, MaxTerm), FuncDef1), 
%   append([ff,down,bisect], [max(MaxTerm)], Behaviour),
%   append(Sums, Constraints, Cs), !,
%   (Cs \= [] -> once(labeling(Behaviour, Cs)); true),

%   unzip(P4, FuncDef1, Ctx),
  top(P3, P4).

% FUNCTION DEF
ast_walk(P, Marker, CG-Tgt, PM, P3) :-
  unzip(P, function_definition(_Bb, _An, _Ai, _Fi), _), !, 
  down(P, 1, P1),
  marker_down(Marker, 1, Marker1),

  marker_freq(Marker, Freq),
  marker_freq(Marker1, Freq), 

  ast_walk(P1, Marker1, CG-Tgt, PM, P2),
  up(P2, P3).

% BASIC BLOCK
ast_walk(P, Marker, CG-Tgt, PM, P3) :-
  unzip(P, basic_block(_Stmts, _An, _Ai, _Fi), _), !,
  down(P, 1, zipper(Stmts, Ctx)),
  marker_down(Marker, 1, Marker1),

  marker_freq(Marker, Freq),
  marker_freq(Marker1, Freq), 
  %Freq #>= 0, % { Freq >= 0 },
  %print_dom(Freq),

  % Insert the marker
  marker_atom(Marker, A),
  pragma_text(Pragma, freq(A, Freq)),
  P1 = zipper([Pragma|Stmts], Ctx),

  ast_walk(P1, Marker1, CG-Tgt, PM, P2),
  up(P2, P3).

% EMPTY BASIC BLOCK
ast_walk(P, Marker, _, _, P1) :-
  unzip(P, basic_block(An, Ai, Fi), Ctx), !,

  % Insert the marker
  %marker_down(Marker, 1, Marker1),
  marker_freq(Marker, Freq),
  Freq #>= 0, % { Freq >= 0 },
  marker_atom(Marker, A),
  pragma_text(Pragma, freq(A, Freq)),
  unzip(P1, basic_block([Pragma], An, Ai, Fi), Ctx).


% LOOPS

ast_walk(P, Marker, CG-Tgt, PM, P3) :-
  unzip(P, for_statement(_E1, _E2, _E3, Bb, _An, _Ai, _Fi), _), !,
  down(P, 4, P1),
  marker_down(Marker, 4, Marker1),

  extract_loopbound(Bb, LoopBound),
  marker_freq(Marker, Fouter),
  marker_freq(Marker1, Finner),
  Finner #>= 0,
  Finner #=< Fouter * LoopBound,
%  { Finner >= 0 },
%  { Finner =< Fouter * LoopBound },
  
  ast_walk(P1, Marker1, CG-Tgt, PM, P2),
  up(P2, P3).

ast_walk(P, Marker, CG-Tgt, PM, P3) :-
  unzip(P, while_stmt(_E, Bb, _An, _Ai, _Fi), _), !,
  down(P, 2, P1),
  marker_down(Marker, 2, Marker1),

  extract_loopbound(Bb, LoopBound),
  marker_freq(Marker, Fouter),
  marker_freq(Marker1, Finner),
  Finner #>= 0,
  Finner #=< Fouter * LoopBound,
%  { Finner >= 0 },
%  { Finner =< Fouter * LoopBound },
 
  ast_walk(P1, Marker1, CG-Tgt, PM, P2),
  up(P2, P3).

ast_walk(P, Marker, CG-Tgt, PM, P3) :-
  unzip(P, do_while_stmt(Bb, _E, _An, _Ai, _Fi), _), !,
  down(P, 1, P1),
  marker_down(Marker, 1, Marker1),

  extract_loopbound(Bb, LoopBound),
  marker_freq(Marker, Fouter),
  marker_freq(Marker1, Finner),
  Finner #>= 0,
  Finner #=< Fouter * LoopBound,
%  { Finner >= 0 },
%  { Finner =< Fouter * LoopBound },
  
  ast_walk(P1, Marker1, CG-Tgt, PM, P2),
  up(P2, P3).

% BRANCHES

ast_walk(P, Marker, CG-Tgt, PM, P7) :-
  unzip(P, if_stmt(_E1, _Bb1, _Bb2, _An, _Ai, _Fi), _), !,

  % Cond
  down(P, 1, P1),
  marker_down(Marker, 1, Marker1),
  marker_freq(Marker, Fouter),
  marker_freq(Marker1, Fouter), 

  ast_walk(P1, Marker1, CG-Tgt, PM, P2),
  
  % Then
  right(P2, P3),
  marker_down(Marker, 2, Marker2),
  marker_freq(Marker, Fouter),
  marker_freq(Marker2, Fthen), 
  Fthen #>= 0,
  Fthen #=< Fouter,
%  { Fthen >= 0 },
%  { Fthen =< Fouter },
  ast_walk(P3, Marker2, CG-Tgt, PM, P4),

  % Else
  right(P4, P5),
  marker_down(Marker, 3, Marker3),
  marker_freq(Marker3, Felse),
  Felse #>= 0,
  Felse #=< Fouter,
%  { Felse >= 0 },
%  { Felse =< Fouter },

  ast_walk(P5, Marker3, CG-Tgt, PM, P6),

  Fouter #= Fthen + Felse,
%  { Fouter = Fthen + Felse },
  
  up(P6, P7).

ast_walk(P, Marker, CG-Tgt, PM, P3) :-
  unzip(P, switch_statement(_E1, _Bb, _An, _Ai, _Fi), _), !,

  down(P, 2, P1),
  marker_down(Marker, 2, Marker2),
  marker_freq(Marker, Fparent),
  marker_freq(Marker2, Fswitch), 
  Fswitch #= Fparent,%  { Fswitch = Fparent },
  ast_walk(P1, Marker2, CG-Tgt, PM, P2),
  % FIXME! Fallthrough is not handled!!!
  % If-Cascade would be better
  case_freqs(P2, CaseFreqs),
  sum(CaseFreqs, #=, Fswitch),%  sum(SwitchFreqs, Fswitch),
  %maplist(print_dom, CaseFreqs), 
  up(P2, P3).

ast_walk(P, Marker, CG-Tgt, PM, P3) :-
  ( (unzip(P, case_option_stmt(_E1, Bb, null, _An, _Ai, _Fi), _),
     S = 2)
  ; (unzip(P, default_option_stmt(Bb, _An1, _Ai1, _Fi1), _),
     S = 1)
  ), !,

  down(P, S, P1),
  marker_down(Marker, S, Marker2),
  marker_freq(Marker, Fswitch),
  marker_freq(Marker2, Fcase),
  Fcase #>= 0,
  Fcase #=< Fswitch, %print_dom(Fcase),
%  { Fcase >= 0 },
%  { Fcase =< Fswitch },
  ast_walk(P1, Marker2, CG-Tgt, PM, P2),
  up(P2, P3).

% FUNCTION CALL
ast_walk(P, Marker, CG-Tgt, PM, P2) :-
  unzip(P, FCall, Ctx),
  is_function_call_exp(FCall, Name, Type), !,
  function_signature(FunctionDecl, Type, Name, _Mod),
  top(P, Top),

  ast_walk(Top, FunctionDecl, Marker, CG-Tgt, PM, P1),
  walk_to(P1, Ctx, P2).

% Lists
ast_walk(zipper([], Ctx), _, _, _, zipper([], Ctx)) :- !.
ast_walk(P, Marker, CG-Tgt, PM, P3) :-
  unzip(P, List, _), is_list(List), !,
  length(List, N),
  down(P, 1, P1),
  marker_down(Marker, 1, Marker1),
  marker_freq(Marker, Freq),
  marker_freq(Marker1, Freq),

  ast_walk1(P1, Marker1, CG-Tgt, PM, N, P2),
  up(P2, P3).

% % UnOp
ast_walk(P, Marker, CG-Tgt, PM, P3) :-
   unzip(P, UnOp, _), 
   functor(UnOp, _, 4), !, %UnOp =.. [_Op, _E1, _, _, _],
   down(P, 1, P1),
   marker_down(Marker, 1, Marker1),
   marker_freq(Marker, Fouter),
   marker_freq(Marker1, Fouter),
   
   ast_walk(P1, Marker1, CG-Tgt, PM, P2),
   up(P2, P3).

% BinOp
ast_walk(P, Marker, CG-Tgt, PM, P5) :-
  unzip(P, BinOp, _), 
  functor(BinOp, _, 5), !, %BinOp =.. [_Op, _E1, _E2, _, _, _],
  down(P, 1, P1),
  marker_down(Marker, 1, Marker1),
  marker_freq(Marker, Fouter),
  marker_freq(Marker1, Fouter), 
  ast_walk(P1, Marker1, CG-Tgt, PM, P2),
  
  marker_down(Marker, 2, Marker2),
  marker_freq(Marker, Fouter),
  marker_freq(Marker2, Fouter), 
  right(P2, P3),
  ast_walk(P3, Marker2, CG-Tgt, PM, P4),
  up(P4, P5).

% For a leaf node, return a zipper that contains the whole node
ast_walk(P, _, _, _, P).% :- unzip(P, S,_),unparse(S),nl.

% List iteration
ast_walk1(P, Marker, CG-Tgt, PragmaMap, N, P3) :-
  %unzip(P, X, _), unparse(X), writeln(' <-- now walking'),
  % Catch the effect of pragmas
  parse_pragma(P, Marker, PragmaMap, PragmaMap1),
  ast_walk(P, Marker, CG-Tgt, PragmaMap, P1),
  ((N > 1)
  -> (right(P1, P2),
      N1 is N - 1,
      marker_right(Marker, Marker1),
      ast_walk1(P2, Marker1, CG-Tgt, PragmaMap1, N1, P3))
  ;  P1 = P3
  ).


extract_loopbound(Bb, LoopBound) :-
  (get_loopbound(Bb, LoopBound)
  ; (writeln('** ERROR missing loop bound for the following loop body:'),
     unparse(Bb),
     LoopBound is 2**31-1  %,halt
    )
  ), !.

% Marker creation support functions
marker_down(freq(marker(Ms, M), _), N, freq(marker([M|Ms], N), _)).

marker_right(freq(marker(Ms, N), Freq),
	     freq(marker(Ms, N1), Freq)) :-
  N1 is N + 1.

marker_into(freq(marker(Ms, M), _), Func, freq(marker([M|Ms], Func), _)).

marker_freq(freq(_, Frequency), Frequency).

marker_atom(freq(marker(Ms, M), _), Atom) :-
  reverse([M|Ms], Xs),
  concat_atom(Xs, '_', Atom), !.


% Get the loop bound annotation of a basic block
get_loopbound(basic_block(Stmts, _An, _Ai, _Fi), LoopBound) :-
  get_annot(Stmts, wcet_loopbound(LoopBound), _).

% Insert a pragma if not there already
update_pragma(zipper(Stmts, Ctx), Annot, zipper(Stmts, Ctx)) :-
  get_annot(Stmts, Annot, _), !. % return pragma

update_pragma(zipper(Stmts, Ctx), Annot, zipper([Pragma|Stmts], Ctx)) :-
  pragma_text(Pragma, Annot), !. % Insert the pragma

% SWITCH/CASE helpers
case_freqs(P, CaseFreqs) :-
  unzip(P, basic_block(Stmts, _An, _Ai, _Fi), _),
  maplist(case_freq, Stmts, CaseFreqs), !.

case_freq(OptionStmt, CaseFreq) :-
  ( OptionStmt = case_option_stmt(_E1, Bb, null, _An, _Ai, _Fi)
  ; OptionStmt = default_option_stmt(Bb, _An1, _Ai1, _Fi1)),
  % Careful, we assume that our freq was added last and is thus the first in the bb
  Bb = basic_block([Pragma|_], _An2, _Ai2, _Fi2),
  pragma_text(Pragma, freq(_, CaseFreq)).

case_freq(_, 0).
  

% PRAGMAs
parse_pragma(P, Marker, PragmaMap, PragmaMap1) :-
  (unzip(P, Pragma, _), 
   pragma_text(Pragma, Text), 
   atom(Text),			% our freq pragmas are terms
   atom_to_term(Text, AnnotTerm, _), 
   % We carry a table with Marker<->CLP_Var
   (  AnnotTerm = wcet_marker(M),
      put_assoc(M, PragmaMap, Var, PragmaMap1),
      marker_freq(Marker, Freq),
      Var #= Freq
   ;  AnnotTerm = wcet_constraint(Term), 
      translate(Term, PragmaMap, Constraint),
      Constraint,
      PragmaMap = PragmaMap1
   ;  AnnotTerm = wcet_loopbound(Term),	% loop bounds are handled extra
      PragmaMap = PragmaMap1
   ;  %trace,
      write('//** WARNING: could not understand #pragma '), writeln(Text),
      fail
   )
  ; PragmaMap = PragmaMap1
  ), !.

% Interpret a constraint
translate(A=B, Map, A1#=B1) :- translate(A, Map, A1), translate(B, Map, B1).
translate(A>B, Map, A1#>B1) :- translate(A, Map, A1), translate(B, Map, B1).
translate(A<B, Map, A1#<B1) :- translate(A, Map, A1), translate(B, Map, B1).
translate(A>=B, Map, A1#>=B1) :- translate(A, Map, A1), translate(B, Map, B1).
translate(A=<B, Map, A1#=<B1) :- translate(A, Map, A1), translate(B, Map, B1).
translate(A+B, Map, A1+B1) :- translate(A, Map, A1), translate(B, Map, B1).
translate(A-B, Map, A1-B1) :- translate(A, Map, A1), translate(B, Map, B1).
translate(A*B, Map, A1*B1) :- translate(A, Map, A1), translate(B, Map, B1).
translate(A, Map, A1) :-
  atom(A),
  get_assoc(A, Map, A1).
translate(A, _, A) :- number(A).
translate(A, _, _) :- write('//** WARNING: could translate '), writeln(A), fail.

%-------------------------------------------------------------

% gather_constraints/5 

get_freqs([Pragma|Stmts], Fs) :-
  ((pragma_text(Pragma, freq(_,Freq)),
    %unparse(Pragma), print_dom(Freq), nl,
    Fs = [Freq|Freqs]
   )
  ; Fs = Freqs
  ),
  get_freqs(Stmts, Freqs).
get_freqs([], []).

% Extract the name of the current function
gather_constraints1(triple([Sum|Sums], Target, Mt),
		    triple([Sum|Sums], Target, Maxterm),
		    triple([Sum|Sums], Target, Maxterm),
		    
		    FuncDecl, FuncDecl) :-
  function_signature(FuncDecl, _, FuncName, _),
  (FuncName = Target  
  -> (Maxterm = Sum)
   ; Maxterm = Mt). 

gather_constraints1(triple(Sums, Target, Maxterm),
		    triple([Sum|Sums], Target, Maxterm),
		    triple([Sum|Sums], Target, Maxterm),
		    
		    basic_block(Stmts, An, Ai, Fi),
		    basic_block([Pragma|Stmts], An, Ai, Fi)) :-
  get_freqs(Stmts, Freqs),
  Freqs \= [],
  %writeln(Freqs),
  pragma_text(Pragma, sum_freq(Sum)),
  sum(Freqs, #=, Sum).
%  sum(Freqs, Sum).
gather_constraints1(I, I, I, T, T).


gather_constraints(I, [Var|I], [Var|I], Pragma, Pragma) :-
  pragma_text(Pragma, freq(_,Var)),
  var(Var).
gather_constraints(I, I, I, T, T).
  
print_dom(Var) :-
  fd_dom(Var, Dom),
  write('Dom('), write(Var), write(') = '), writeln(Dom).

plusify([], []).
plusify([X], X).
plusify([X|Xs], X+Sum) :-
  plusify(Xs, Sum).

main1(Filename, Target, Base) :-
  %catch(
  (
   open(Filename, read, _, [alias(rstrm)]),
   read_term(rstrm, P, []),
   close(rstrm),

   % Calculate the Call Graph
   callgraph(P, CallGraph),
   %dump_graph(vcg, 'call.vcg', CallGraph),

   % start with BaseFunc(...)
   function_signature(BaseFunc, function_type(_, _, _), Base, _),
   Marker = freq(marker([], 'm'), 1),

   empty_assoc(PragmaMap), 
   ast_walk(zipper(P, []),
	    BaseFunc, Marker, CallGraph-Target, PragmaMap,
	    zipper(P1, [])),

   transformed_with(P1, gather_constraints, postorder, [], Constraints, _),

   transformed_with(P1, gather_constraints1, postorder,
   		    triple([],   Target, _),
   		    triple(Sums, Target, MaxT), P2),

   (  Target = 'SUM'
   -> plusify(Sums, MaxTerm)
   ;  MaxTerm = MaxT ),
   
   append([ff,down,step], [max(MaxTerm)], Behaviour),
   append(Sums, Constraints, Cs), !,
   maplist(print_dom, Cs), writeln(MaxTerm), nl, 
   (Cs \= [] -> once(labeling(Behaviour, Cs)); true),
   %maximize(MaxTerm),

%    Dump the constraints for later analysis by Markus (Triska)
%    copy_term(Constraints, _, Cs1),
%    copy_term(Sums, _, Cs2),
%    open('constraint-dump.pl', write, _, [alias(wstrm)]),
%    write(wstrm, '% Constraints:\n'),
%    write(wstrm, ':- use_module(library(clpfd)).\n'),
%    append(Cs1, Cs2, Css),
%    write(wstrm, pair(Css, Constraints)),
%    write(wstrm, '.\n'),
%    close(wstrm)
   unparse(P2)
  ).%, E, (print_message(error, E), fail)).

main :-
  current_prolog_flag(argv, Argv),
  append(_, [--|Args], Argv),
  Args = [Arg1, Arg2, Arg3],
  main1(Arg1, Arg2, Arg3).
  %profile(main1), trace.

main :-
  writeln('The static profiler. (C) 2008,2009 Adrian Prantl'),
  writeln('Usage: frequency [annotated program].pl [max_Target]|[SUM] [Base]'),
  writeln('Example: frequency input.pl my_func main'),
  writeln('Use SUM to maximize the sum over all constraints (expensive!)'),
  halt(1).
