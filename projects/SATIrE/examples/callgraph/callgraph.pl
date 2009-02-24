#!/usr/local/mstools/bin/pl -q -t main -f
% -*- prolog -*-
:- module(callgraph,
	  [callgraph/2,
	   collect_calls/5,
	   dump_graph/3]).

:- use_module(library(ugraphs)),
   use_module(ast_properties),
   use_module(ast_transform),
   use_module(utils).

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

:- guitracer.

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

%% callgraph(+P, -Graph) is det.
% Construct a call graph from an AST. Graph is in library(ugraphs) form.
% The nodes in the graph have the form Name/Type
%
% @tbd
% NO function pointers or virtual methods yet!
callgraph(P, Graph) :-
  transformed_with(P, collect_calls, postorder, _-[], _-Edges, _), !,
  add_edges([], Edges, Graph).


%-----------------------------------------------------------------------
% GRAPH Printing
%-----------------------------------------------------------------------

% Fake ^Nodes are converted into labeled edges.

display(N) :- write(N).

%% dump_graph(+Method, +Filename, +Graph) is det.
% Method must be one of _graphviz_ or _vcg_.
dump_graph(Method, Filename, Graph) :-
  dump_graph(Method, Filename, Graph, []).

%% dump_graph(+Method, +Filename, +Graph, +Flags) is det.
% Method must be one of _graphviz_ or _vcg_.
% Flags is a list of terms
% * layout(tree)
dump_graph(Method, Filename, Graph, Flags) :-   
  open(Filename, write, _, [alias(dumpstream)]),
  call(Method, dumpstream, Graph, Flags), !,
  close(dumpstream).

%% grahviz(F, Edge).
%  Dump an ugraph in dotty syntax
viz_edge(F, Edge) :-
  Edge = N1-N2,
  write(F, '"'), with_output_to(F, display(N1)), write(F, '"'),
  write(F, ' -> '),
  write(F, '"'), with_output_to(F, display(N2)), write(F, '"'), 
  write(F, ';\n').

graphviz(F, G, _) :-
  edges(G, E),
  write(F, 'digraph G {\n'),
  maplist(viz_edge(F), E),
  write(F, '}\n').

% VCG
vcg_node(_, label(_,_)) :- !. 

vcg_node(F, Node) :-
  write(F, '  node: { '),
  write(F, 'title: '),
  write(F, '"'), with_output_to(F, display(Node)), write(F, '"'),
  write(F, ' }\n').

vcg_edge(F, G, N1-label(X,N3)) :- !,
  (neighbours(label(X,N3), G, [N2]) ; trace, fail),
  write(F, '  edge: {\n'),
  write(F, '    sourcename: '),
  write(F, '"'), with_output_to(F, display(N1)), write(F,'"'), write(F,'\n'),
  write(F, '    targetname: '),
  write(F, '"'), with_output_to(F, display(N2)), write(F,'"'), write(F,'\n'),
  write(F, '    label: '),
  write(F, '"'), with_output_to(F, display(N3)), write(F,'"'), write(F,'\n'),
  write(F, '  }\n').

vcg_edge(_F, _G, label(_,_N1)-_N2) :- !. % been there, done that

vcg_edge(F, _G, N1-N2) :-
  write(F, '  bentnearedge: {\n'),
  write(F, '    sourcename: '),
  write(F, '"'), with_output_to(F, display(N1)), write(F,'"'), write(F,'\n'),
  write(F, '    targetname: '),
  write(F, '"'), with_output_to(F, display(N2)), write(F,'"'), write(F,'\n'),
  write(F, '  }\n').

%% vcg(F,node).:
%  Dump an ugraph in VCG syntax
vcg(F, G, Flags) :-
  write(F, 'graph: {\n'),
  (member(layout(tree), Flags) -> write(F, 'layoutalgorithm: tree\n') ; true),
  write(F, 'display_edge_labels: yes\n'),
  write(F, 'splines: yes\n'),
  vertices(G, V1),
  reverse(V1, V),
  maplist(vcg_node(F), V),
  edges(G, E1),
  reverse(E1, E),
  maplist(vcg_edge(F, G), E),
  write(F, '}\n').

%-----------------------------------------------------------------------

main :-
  current_prolog_flag(argv, Argv), 
  append(_, [--|[File]], Argv),

  open(File, read, _, [alias(rstrm)]),
  read_term(rstrm, P, []),
  close(rstrm),

  callgraph(P, CG),
  dump_graph(vcg, 'call.vcg', CG),
  dump_graph(graphviz, 'call.dot', CG),
  dump_graph(gdl, 'call.gdl', CG).
