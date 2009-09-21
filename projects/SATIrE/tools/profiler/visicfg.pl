:- module(visicfg,
	  [visicfg/2]).

%-----------------------------------------------------------------------
/** <module> 

@version   @PACKAGE_VERSION@
@copyright Copyright (C) 2009 Adrian Prantl
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

:- use_module(library(clpfd)).

new_graph(Name, G) :- new_graph(Name, 0, G).

new_graph(Name, Label, graph(G,Label1,Start)) :-
  Label1 #= Label + 1,
  Start = node(Label, Name, style='shape=tab, fillcolor=azure, pencolor=azure4'),
  vertices_edges_to_ugraph([Start], [], G).

visicfg(P, Base) :-
  function_signature(BaseFunc, function_type(_, _, _), Base, _),
  zip(P, Pz),
  new_graph('Start', G0),
  ast_walk(Pz-G0, BaseFunc, _-graph(G, _, _)),
  dump_graph(graphviz, 'icfg.dot', G, BaseFunc).

bound(V) :-
  (   nonvar(V) -> true ; trace ).

% ast_walk/6: main traversal.
ast_walk(P-G, Function, P4-G3) :- bound(P), bound(G),
  (   goto_function(P, Function, P1)
  ->  true
  ;   format('**ERROR: Could not locate function ~w.', [Function]),
      halt(1)
  ),
  Function = function_declaration(Params, _Def, DeclAnnot, AI, FI),
  FunctionHd = function_declaration(Params, null, DeclAnnot, AI, FI),
  unparse_to_safe_atom(FunctionHd, Sig),

  % start a new subgraph
  G = graph(G0, Label0, Last0),
  new_graph(Sig, Label0, SubG1),
  SubG1 = graph(_,_,EntryNode),
  
  %  function_signature(Function, Type, Name, _),
  %  writeln(Name),
  %  (Name = 'Read_AD_Channel' -> trace ; true),
  
  down(P1, 2, P2),	    
  ast_walk(P2-SubG1, P3-SubG2),
  top(P3, P4),

  faux_node(SubG2, 'return', SubG3),
  SubG3 = graph(_,Label,ExitNode),
  add_vertices(G0, [SubG3], G1),
  add_edges(G1, [Last0-EntryNode], G2),
  G3 = graph(G2, Label, ExitNode).

% type checking
ast_walk(P-G, _) :- bound(P), bound(G), fail.

% FUNCTION DEF
ast_walk(P-G, P3-G1) :- 
  unzip(P, function_definition(_Bb, _An, _Ai, _Fi), _), !, 
  down(P, 1, P1),

  ast_walk(P1-G, P2-G1),
  up(P2, P3).

% BASIC BLOCK
ast_walk(P-G, P3-G1) :-
  unzip(P, basic_block(_Stmts, _An, _Ai, _Fi), _), !,
  down(P, 1, P1),

  ast_walk(P1-G, P2-G1),
  up(P2, P3).

% EMPTY BASIC BLOCK
ast_walk(P-G, P-G) :-
  unzip(P, basic_block(_An, _Ai, _Fi), _Ctx), !.

% LOOPS

ast_walk(P-G, P3-G3) :-
  unzip(P, for_statement(E1, E2, E3, Bb, An, Ai, Fi), _), !,
  make_node(zipper(for_statement(E1, E2, E3, null, An, Ai, Fi), [])-G, G1),
  
  down(P, 4, P1),
  extract_loopbound(Bb, _LoopBound),
    
  ast_walk(P1-G1, P2-G2),
  make_back_edge(G1, G2, G3),
  up(P2, P3).

ast_walk(P-G, P3-G3) :-
  unzip(P, while_stmt(E, Bb, An, Ai, Fi), _), !,
  make_node(zipper(while_stmt(E, null, An, Ai, Fi), [])-G, G1),
  down(P, 2, P1),

  extract_loopbound(Bb, _LoopBound),
 
  ast_walk(P1-G1, P2-G2),
  make_back_edge(G1, G2, G3),
  up(P2, P3).

ast_walk(P-G, P3-G3) :-
  unzip(P, do_while_stmt(Bb, E, An, Ai, Fi), _), !,
  make_node(zipper(do_while_stmt(null, E, An, Ai, Fi), [])-G, G1),
  down(P, 1, P1),

  extract_loopbound(Bb, _LoopBound),
  
  ast_walk(P1-G1, P2-G2),
  make_back_edge(G1, G2, G3),
  up(P2, P3).

% BRANCHES

ast_walk(P-G, P7-G7) :-
  unzip(P, if_stmt(_E1, _Bb1, _Bb2, _An, _Ai, _Fi), _), !,
  %make_node(zipper(if_stmt(E1, null, null, An, Ai, Fi), [])-G, G1),
  faux_node(G, 'if', G1),
  
  % Cond
  down(P, 1, P1),

  ast_walk(P1-G1, P2-graph(G2,N2,If)),
  
  % Then
  right(P2, P3),
  ast_walk(P3-graph(G2,N2,If), P4-graph(G3,N3,Then)),

  % Else
  right(P4, P5),

  faux_node(graph(G3,N3,If), 'else', G4),
  ast_walk(P5-G4, P6-graph(G5,N5,End)),
  add_edges(G5, [Then-End], G6),
  G7 = graph(G6,N5,End),
  
  up(P6, P7).

ast_walk(P-G, P3-G1) :-
  unzip(P, switch_statement(_E1, _Bb, _An, _Ai, _Fi), _), !,

  down(P, 2, P1),
  ast_walk(P1-G, P2-G1),
  % FIXME! Fallthrough is not handled!!!
  % If-Cascade would be better
  up(P2, P3).

ast_walk(P-G, P3-G1) :-
  ( (unzip(P, case_option_stmt(_E1, Bb, null, _An, _Ai, _Fi), _),
     S = 2)
  ; (unzip(P, default_option_stmt(Bb, _An1, _Ai1, _Fi1), _),
     S = 1)
  ), !,

  down(P, S, P1),
  ast_walk(P1-G, P2-G1),
  up(P2, P3).

% FUNCTION CALL
ast_walk(P-G, P2-G1) :-
  unzip(P, FCall, Ctx),
  is_function_call_exp(FCall, Name, Type), !,
  function_signature(FunctionDecl, Type, Name, _Mod),
  top(P, Top),

  ast_walk(Top-G, FunctionDecl, P1-G1),
  walk_to(P1, Ctx, P2).

% Lists
ast_walk(zipper([], Ctx)-G, zipper([], Ctx)-G) :- bound(G),
  !.

ast_walk(P-G, P3-G1) :-
  unzip(P, List, _), is_list(List), !,
  length(List, N),
  down(P, 1, P1),

  ast_walk1(P1-G, N, P2-G1), bound(G1),
  up(P2, P3).

% % UnOp
ast_walk(P-G, P3-G2) :-
   unzip(P, UnOp, _), 
   functor(UnOp, F, 4), !, %UnOp =.. [_Op, _E1, _, _, _],
   down(P, 1, P1),

   % skip leaves
   (   (   atom_concat(_, 'op',F)
       ;   atom_concat(_, 'val',F)
       )
   ->  G=G1
   ;   make_node(P-G, G1)  ),

   % skip vardecls
   (   atom_concat(_, 'declaration',F) 
   ->  P1-G1 = P2-G2
   ;   ast_walk(P1-G1, P2-G2)  ),
   up(P2, P3).

% BinOp
ast_walk(P-G, P5-G3) :-
  unzip(P, BinOp, _), 
  functor(BinOp, _, 5), !, %BinOp =.. [_Op, _E1, _E2, _, _, _],
  
  G =G1,%make_node(P-G, G1),
  down(P, 1, P1),
  ast_walk(P1-G1, P2-G2),
      
  right(P2, P3),
  ast_walk(P3-G2, P4-G3),
  up(P4, P5).

% For a leaf node, return a zipper that contains the whole node
ast_walk(P-G, P-G).% :- unzip(P, S,_),unparse(S),nl.

% List iteration
ast_walk1(P-G, N, P3-G2) :-
  %unzip(P, X, _), unparse(X), writeln(' <-- now walking'),
  ast_walk(P-G, P1-G1),
  ((N > 1)
  -> (right(P1, P2),
      N1 is N - 1,
      ast_walk1(P2-G1, N1, P3-G2))
  ;  P3 = P1,
     G2 = G1
  ).

%-----------------------------------------------------------------------
% GRAPH Printing
%-----------------------------------------------------------------------

% make_back_edge(+Entry, +Exit, -Next)
% introduce a back edge and rewire Last to point to loop entry
make_back_edge(graph(_,_,Entry),
	       graph(G,Last,Exit),
	       graph(G1,Last,Entry)) :-
  add_edges(G, [Exit-Entry], G1).

make_node(P-G, G) :-
  unzip(P, Pragma, _),
  pragma_text(Pragma, _).
	  
make_node(P-graph(G,Label,Last), graph(G1,Label1,V)) :-
  bound(P), bound(G), bound(Label),

  % The statement
  unzip(P, S, _),
  unparse_to_safe_atom(S, Stmt),

  % Put the surounding context in a tooltip if it's not too large
  distance_from_root(P, D),
  (   D #> 5
  ->  
      up(P, P1), unzip(P1, Ctx, _),
      unparse_to_safe_atom(Ctx, Context)
  ;   Context = Stmt  ),
  format(atom(Style),
	 'shape=note, style=filled, fillcolor=cornsilk, pencolor=cornsilk4, id=<~w>',
	 [Context]),
  V = node(Label, Stmt, style=Style),
  
  Label1 #= Label+1,
  add_edges(G, [Last-V], G1).

faux_node(graph(G,Label,Last), Name, graph(G1,Label1,V)) :-
  V = node(Label, Name, style='shape=note, style=filled, fillcolor=gold, pencolor=gold4'),
  Label1 #= Label+1,
  add_edges(G, [Last-V], G1).

replace_all1(CsI,A1,A2,CsO) :-
  atom_codes(A1, [C1]),
  atom_codes(A2, Cs2),
  replace_all(CsI, C1, Cs2, CsO).

unparse_to_safe_atom(S, Atom) :-
  unparse_to(codes(Cs), S),
  replace_all1(Cs, '\n', '<br align="left"/>', Cs1), % -> html syntax
  replace_all1(Cs1,'"', '&quot;', Cs2),
  replace_all1(Cs2,'\\','&#92;', Cs3),
  replace_all1(Cs3,'&', '&amp;', Cs4),
  replace_all1(Cs4,'>', '&gt;', Cs5),
  replace_all1(Cs5,'<', '&lt;', Cs7),
  atom_codes('<font face="Courier">', F1),
  atom_codes('</font>', F2),
  flatten([F1,Cs7,F2], Cs8),
%  atom_codes(A1, Cs),
%  atom_codes(A2, Cs7), writeln(A1), writeln(A2),trace,
  atom_chars(Atom, Cs8).


display(N) :- write(N).

%% dump_graph(+Method, +Filename, +Graph, +Flags) is det.
% Method must be one of _graphviz_ or _vcg_.
% Flags is a list of terms
% * layout(tree)
dump_graph(Method, Filename, Graph, Flags) :-   
  open(Filename, write, _, [alias(dumpstream)]),
  call(Method, dumpstream, Graph, Flags), !,
  close(dumpstream).

get_label(node(L,_,_), L).
get_label(graph(_,L,_), L).

viz_edge(F, N1-N2) :-
  get_label(N1, L1),
  get_label(N2, L2),
  format(F, '~w -> ~w ;~n', [L1, L2]).
viz_edge(_, Edge) :- write(Edge), trace.

viz_node(F, _, node(Label,Stmt,style=Style)) :- !,
  format(F, '~w [ label=<~w>, ~w ];~n', [Label,Stmt,Style]).

viz_node(F, Color, graph(G, Label, Last)) :- !,
  format(F, 'subgraph cluster~w {~n', [Label,G,Last]),
  format(F, 'node [style=filled];~n', []),
  format(F, 'style=filled; color=gray~w;~n', [Color]),
  Color1 #= Color - 12,
  edges(G, E),     maplist(viz_edge(F), E),
  vertices(G, V),  maplist(viz_node(F,Color1), V),
  format(F, 'label="Function" ;~n', []),
  format(F, '} ;~n', []).

viz_node(_, _, Node) :- write(Node), trace.


%% graphviz(F, G, _).
%  Dump an ugraph in dotty syntax
graphviz(F, G, _Base) :-
  edges(G, E),
  vertices(G, V), 
  %Root = Base/_Type, member(Root, V),
  format(F, 'digraph G {~n', []),
  %format(F, '  root="~w";~n', [Root]),
  format(F, 'splines=true; overlap=false; rankdir=TB;~n', []),
  maplist(viz_edge(F), E),
  maplist(viz_node(F, 100), V),
  write(F, '}\n').
