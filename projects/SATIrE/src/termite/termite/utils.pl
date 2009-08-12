#!/usr/bin/pl -t testrun -f
% -*- prolog -*-

:- module(utils,
	  [dup/3,
	   foldl/4,
	   foldl1/3,
           string_to_term/2,
           term_to_string/2,
	   atom_to_string/2,
	   list_from_to/3,
	   repeat_string/3,

	   replace_in_atom/4,
	   replace_nth/5,
	   
	   un_op/3,
	   bin_op/4,
	   tern_op/5,
	   quad_op/6,

	   term_mod/3]).

%-----------------------------------------------------------------------
/** <module> A collection of useful general-purpose predicates

@author

Copyright (C) 2007-2009 Adrian Prantl <adrian@complang.tuwien.ac.at>

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

%% dup(+A, +Num, -As) is det.
%% dup(?A, ?Num, ?As) is nondet.
% something like repeat in Haskell:
%   dup(A, Num, As) :- length(As, Num), maplist(=(A),As).
dup(A, Num, As) :-
  length(As, Num),
  maplist(=(A),As).
% dup(_, 0, []).
% dup(A, N, B) :-
%   append(A, Bs, B),
%   N1 is N-1,
%   dup(A, N1, Bs).

foldl1([F|Fs], Pred, V) :-
  foldl(Fs, Pred, F, V).

%% foldl(+Fs, +Pred, +Start, -V) is det.
% foldl/4 - from the SWI-Mailing List
% Fold a list using [pred], just as you would do in Haskell ;-)
foldl([], _, V, V). 
foldl(Fs, Pred, F, V) :- 
  fold_lag(Fs, F, Pred, V). 

fold_lag([], V, _, V). 
fold_lag([F|Fs], E, Pred, V) :- 
  call(Pred, E, F, G), 
  fold_lag(Fs, G, Pred, V). 

%% string_to_term(+Text, -Term) is det.
% Convert a String to a Term, stripping whitespaces
string_to_term(Text, Term) :-
  string(Text), 
  string_to_list(Text, X1),
  sublist(\=(32), X1, X2), % remove whitespaces
  string_to_atom(X2, X3), 
  atom_to_term(X3, Term, _).

%% atom_to_string(+Atom, -String) is det.
% Convert an Atom to a String
atom_to_string(A, S) :-
  atom_codes(A, Cs),
  string_to_list(S, Cs).

%% term_to_string(+Term, -String) is det.
term_to_string(Term, String) :-
  term_to_atom(Term, X),
  string_to_atom(String, X).

%% list_from_to(+Start, +End, -List) is det.
% Create a list of integers [Start..End]
list_from_to(E, E, [E]) :- number(E).

list_from_to(S, E, [X|Xs]) :-
  number(S), number(E),
  S < E,
  X = S,
  S1 is S + 1,
  list_from_to(S1, E, Xs).

%% repeat_string(+S, +N, -Res) is det.
repeat_string(_, 0, "").
repeat_string(S, 1, S).
repeat_string(S, N, Res) :-
  N_1 is N-1,
  repeat_string(S, N_1, S_1),
  string_concat(S, S_1, Res).

% replace_in_atom(+Atom, +What, +With, -NewAtom) is det.
% replace 1 instance of What with With in Atom yielding NewAtom
replace_in_atom(Atom, What, With, NewAtom) :-
  sub_atom(Atom, Be, Len, _, What),
  sub_atom(Atom, 0, Be, _, A1),
  Re is Be+Len,
  sub_atom(Atom, Re, _, 0, A2),
  concat_atom([A1, With, A2], NewAtom).

%% replace_nth(+Xs, +N, +E, +R, -Ys) is det.
% replace the nth element of a list with R and return it in E
replace_nth(_, [], _, []).
replace_nth([R|Xs], 0, E, R, [E|Xs]) :- !.
replace_nth([X|Xs], N, E, R, [X|Ys]) :-
  N1 is N - 1,
  replace_nth(Xs, N1, E, R, Ys).


%% helper functions for basic term replacing
% FIXME: ..=
un_op(Term, F, A) :-
  functor(Term, F, 1),
  arg(1, Term, A).
bin_op(Term, F, A, B) :-
  functor(Term, F, 2),
  arg(1, Term, A),
  arg(2, Term, B).
tern_op(Term, F, A, B, C) :-
  functor(Term, F, 3),
  arg(1, Term, A),
  arg(2, Term, B),
  arg(3, Term, C).
quad_op(Term, F, A, B, C, D) :-
  functor(Term, F, 4),
  arg(1, Term, A),
  arg(2, Term, B),
  arg(3, Term, C),
  arg(4, Term, D).
quint_op(Term, F, A, B, C, D, E) :-
  functor(Term, F, 5),
  arg(1, Term, A),
  arg(2, Term, B),
  arg(3, Term, C),
  arg(4, Term, D),
  arg(5, Term, E).
seni_op(Term, F, A, B, C, D, E, Ef) :-
  functor(Term, F, 6),
  arg(1, Term, A),
  arg(2, Term, B),
  arg(3, Term, C),
  arg(4, Term, D),
  arg(5, Term, E),
  arg(6, Term, Ef).

%% term_mod(+Term, +M, -ModTerm) is nondet.
% Try to apply M on Term recursively
term_mod(Term, _, Term) :- var(Term), !.

term_mod([], _, []) :- !.

term_mod([T|Ts], M, [TM|TMs]) :- !,
  term_mod(T, M, TM),
  term_mod(Ts, M, TMs).

term_mod(Term, M, Mod) :- 
  M =.. L1,
  append(L1, [Term, Term1], L2),
  Pred =.. L2,
  Pred,

  (var(Term1)
  -> Mod = Term1
  ; (Term1 =.. [F|Ts],
     term_mod(Ts, M, Tms),
     Mod =.. [F|Tms])
  ).

