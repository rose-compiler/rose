#!/usr/bin/pl -t testrun -f
% -*- prolog -*-

:- module(astwalk,
	  [walk_to/3,
	   goto_function/3,
	   
	   top/2,
	   up/2,
	   down/3,
	   right/2,
	   
	   zip/2,
	   unzip/3]).

%-----------------------------------------------------------------------
/** <module> Flexible traversals of abstract syntax trees

This module defines commonly-used transformation utilities for the AST
exported by SATIrE. It represents an more flexible alternative to the
transformation interface provided by module ast_transform.

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

% Unit Test
%:- ast_walk(zipper(if_stmt(e,a,[r,if_stmt(e,a,b,x,x),t],x,x), []), P2).

%% zip(+Tree, -Zipper) is det.
% Creates a new Zipper from Tree.
%
% How to move around a tree and replace subtrees=branches?
%
% At each node, we cut out a branch and replace it with a free variable <Gap>.
% The original branch is given as a separate argument,
% allowing us to bind the new branch to <Gap>.
%
% In a way, this works just like Huet's zipper!
zip(X, zipper(X, [])).

%% unzip(?Zipper, ?Tree, ?Context) is det.
% Converts between the Zipper data structure and its contents.
unzip(zipper(X,Ctx), X, Ctx).

% Helper functions to navigate through a zipper

%% walk_to(+Zipper, +Context, -Zipper1) is semidet.
walk_to(Z, Ctx, Z1) :-
  reverse(Ctx, Path),
  top(Z, Top),
  walk_to1(Top, Path, Z1), !.

walk_to1(Z, [], Z).
walk_to1(Z, [Down|Ps], Z2) :-
  ( Down = down(_, _, N)
  ; Down = down_list(_, _, N)),
  down(Z, N, Z1),
  walk_to1(Z1, Ps, Z2).

%% down(+Zipper, +BranchNum, -Zipper1) is semidet.
% Navigate downwards in the tree to child #BranchNum.
%
% * Works also with lists.
down(zipper(List,Ctx), N,
     zipper(Child, [down_list(PredsR, Succs, N)|Ctx])) :- 
  is_list(List), !,
  N1 is N-1,
  length(Preds, N1),
  append(Preds, [Child|Succs], List),
  reverse(Preds, PredsR), !.
  %replace_nth(List, N1, Gap, Child, List1).

down(zipper(X,Ctx), N, zipper(Child,[down(X1,Gap,N)|Ctx])) :-
  X =.. List,
  replace_nth(List, N, Gap, Child, List1),
  X1 =.. List1.

%% up(+Zipper, -Zipper1) is semidet.
% Navigate upwards in the tree.
up(zipper(X,[down(Parent,Gap,_)|Ctx]), zipper(Parent,Ctx)) :- !, X = Gap.
up(zipper(X,[down_list(PredsR, Succs, _)|Ctx]), zipper(Parent,Ctx)) :-
  reverse(PredsR, Preds),
  append(Preds, [X|Succs], Parent),
  !.

%% right(+Zipper, -Zipper1) is semidet.
% Navigate to the next sibling in a tree
right(zipper(X, [down_list(PredsR, [Y|Succs], N)|Ctx]),
      zipper(Y, [down_list([X|PredsR], Succs, N1)|Ctx])) :- !,
  N1 is N+1.

% @tbd Could be done much(!) more efficiently, currently O(n*n)
right(zipper(X, [down(C,Gap,N)|Ctx]), X2) :-
  up(zipper(X, [down(C,Gap,N)|Ctx]), X1),
  N1 is N+1,
  down(X1, N1, X2).

%% top(+Zipper, -Zipper1) is semidet.
% Navigate back to the root of our tree.
%
% @tbd Could be implemented more efficiently, too
top(zipper(X,[]), zipper(X,[])) :- !.
top(X, X2) :- up(X, X1), top(X1, X2), !.

%% goto_function(+Zipper, ?Template, +Zipper1) is nondet.
% find a function like Template in a project or file and return its body
% if there is only a declaration available, the declaration will be returned
goto_function(P, Function, P1) :-
  (  % Prefer to find a real definition first
     find_function(P, Function, P1),
     unzip(P1, Function, _),
     Function = function_declaration(_Def, Body, _A1, _Ai1, _F1),
     Body \= null
  ; 
     find_function(P, Function, P1),
     unzip(P1, Function, _),
     Function = function_declaration(_, null, _, _, _)
  ), !.

find_function(P, FunctionTemplate, P3) :-
  unzip(P, project(_Files, _A, _Ai, _Fi), _), !,
  down(P, 1, P1),
  down(P1, 1, P2),
  find_function1(P2, FunctionTemplate, P3).

find_function(P, FunctionTemplate, P4) :-
  unzip(P, source_file(global(_Funcs, _A1, _Ai, _F1),
		       null, _A2, _Ai2, _F2),
	_), !,
  down(P, 1, P1),
  down(P1, 1, P2),
  down(P2, 1, P3),
  find_function1(P3, FunctionTemplate, P4).

find_function(zipper(FuncTempl, Ctx), FuncTempl, zipper(FuncTempl, Ctx)).

find_function1(zipper([], Ctx), _, zipper([], Ctx)).
find_function1(P, FunctionTemplate, P2) :-
  (  find_function(P, FunctionTemplate, P2)
  ;
     right(P, P1),
     find_function1(P1, FunctionTemplate, P2)
  ).
