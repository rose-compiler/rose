% -*- prolog -*-
:- module(callgraph,
	  [callgraph/2,
	   collect_calls/5,
	   collect_funcs/5]).

:- use_module(library(ugraphs)),
   use_module(library(astproperties)),
   use_module(library(asttransform)).

%-----------------------------------------------------------------------
/** <module> Create a call graph from an AST

@author

Copyright 2008 Adrian Prantl <adrian@complang.tuwien.ac.at>

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

%% collect_calls(Sig-Edges, [], _-Edges, FunctionDecl, FunctionDecl) is det.
% postorder traversal, unify Sig with the signature of the current function
%
% meant for internal use only

collect_calls(Sig-Edges, [], _-Edges, FunctionDecl, FunctionDecl) :-
  function_signature(FunctionDecl, Type, Name, _Mod), !,
  % Create an empty signature
  Sig = Name/Type.

collect_calls(Sig-Edges, [], Sig-[Sig-Callee|Edges], CallExp, CallExp) :-
  is_function_call_exp(CallExp, Name, Type), !,
  Callee = Name/Type.
collect_calls(I,I,I, N, N).

%% collect_funcs(+Funcs, [], -Funcs1, FunctionDecl, FunctionDecl) is det.
% postorder traversal, collect all leaf functions
%
% FIXME: unify with above

collect_funcs(Sigs, [], [Sig|Sigs], FunctionDecl, FunctionDecl) :-
  function_body(FunctionDecl, _), % not interested in declarations
  function_signature(FunctionDecl, Type, Name, _),
  !, 
  Sig = Name/Type.

collect_funcs(I,I,I, N, N).


%% callgraph(+P, -Graph) is det.
% Construct a call graph from an AST. Graph is in library(ugraphs) form.
% The nodes in the graph have the form Name/Type
%
% @tbd
% NO function pointers or virtual methods yet!
% NO leaves!!!
callgraph(P, Graph) :-
  transformed_with(P, collect_calls, postorder, _-[], _-Edges, _), !,
  transformed_with(P, collect_funcs, postorder, [], Vertices, _), !,
  add_vertices([], Vertices, G1),
  add_edges(G1, Edges, Graph).
