:- module(visicfg,
	  [visicfg/4]).

%-----------------------------------------------------------------------
/** <module>

@version   @PACKAGE_VERSION@
@copyright Copyright (C) 2009 Adrian Prantl
@author    Adrian Prantl <adrian@complang.tuwien.ac.at>
@license   See COPYING in the root folder of the SATIrE project

*/
%-----------------------------------------------------------------------

:- use_module(library(clpfd)).

% Graph Data structure:
% * G        ugraph
% * Mode     one of [compact,explode]
% * Count    current node ID
% * Last     last node
% * Fundecls [EntryNode]

% Node Data structure:
% * Label    Integer
% * Name     
% * Style    
% * SameRank [Labels]

% Accessors:
graph_count(graph(_G,_Mode,Count,_Last,_FunDecls), Count).
graph_last( graph(_G,_Mode,_Count,Last,_FunDecls), Last).
graph_mode( graph(_G,Mode,_Count,_Last,_FunDecls), Mode).

% graph_connect(+G1, +G2, -G3).
% make the last node in G1 the last node in G2 yielding G3
graph_connect(graph(_, _, _, Last, _),
	      graph(UG, Mode, Count, _, FunDecls),
	      graph(UG, Mode, Count, Last, FunDecls)).

new_graph(Name, Mode, G) :- new_graph(Name, Mode, 0, [], G).

new_graph(Name, Mode, Count, FunDecls,
	  graph(G,Mode,Count1,Start,[Start|FunDecls])) :-
  Count1 #= Count + 1,
  Start = node(Count, Name,
	       style='shape=tab, fillcolor=azure,pencolor=azure4', []),
  vertices_edges_to_ugraph([Start], [], G).

visicfg(P, Mode, Base, Filename) :-
  function_signature(BaseFunc, function_type(_, _, _), Base, _),
  zip(P, Pz),
  new_graph('Start', Mode, G0),
  ast_walk(Pz-G0, BaseFunc, _-graph(G, _, _, _, _)),

  dump_graph(graphviz, Filename, G, Mode).

% basic type checking
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

  G = graph(G0, Mode, Count0, Last0, FunDecls0),
  % start a new subgraph
  new_graph(Sig, Mode, Count0, FunDecls0, SubG1),
  SubG1 = graph(_,Mode,_,EntryNode,_),

  down(P1, 2, P2),
  ast_walk(P2-SubG1, P3-SubG2),
  top(P3, P4),

% FIXME return edges
  faux_node(SubG2, 'return', SubG3),
  SubG3 = graph(_,Mode,Count,ExitNode,FunDecls1),
  add_vertices(G0, [SubG3], G1),
  add_edges(G1, [Last0-EntryNode], G2),
  G3 = graph(G2, Mode, Count, ExitNode, FunDecls1).

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

ast_walk(P-G, P7-Gn) :-
  unzip(P, if_stmt(_E1, _Bb1, _Bb2, _An, _Ai, _Fi), _), !,
  %make_node(zipper(if_stmt(E1, null, null, An, Ai, Fi), [])-G, G1),

  % make if appear next to then
  samerank_node(G, 'if', IfLabel, ThenLabel, G1),
  ThenLabel #= IfLabel + 1,
    
  %faux_node(G, 'if', G1),
  
  % Cond
  down(P, 1, P1),

  ast_walk(P1-G1, P2-graph(G2,M,N2,If,FDs2)),

  % Then
  right(P2, P3),
  ast_walk(P3-graph(G2,M,N2,If,FDs2), P4-graph(G3,M,N3,Then,FDs3)),

  % Else
  right(P4, P5),

  % make else appear next to if
  samerank_node(graph(G3,M,N3,If,FDs3), 'else', _, IfLabel, G4),
  ast_walk(P5-G4, P6-graph(G5,M,N5,Else,FDs5)),
  faux_node(graph(G5,M,N5,Then,FDs5), 'endif', graph(G6,M,N6,End,FDs6)),
  add_edges(G6, [Else-End], G7),
  Gn = graph(G7,M,N6,End,FDs6),

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
ast_walk(P-G, P2-Gn) :-
  unzip(P, FCall, Ctx),
  is_function_call_exp(FCall, Name, Type), !,
  function_signature(FunctionDecl, Type, Name, _Mod),

  top(P, Top),
  goto_function(Top, FunctionDecl,_), % fixme... slow! used just for lookup
  FunctionDecl = function_declaration(Params, _Def, DeclAnnot, AI, FI),
  FunctionHd = function_declaration(Params, null, DeclAnnot, AI, FI),
  unparse_to_safe_atom(FunctionHd, Sig),
  % Fixme add function exit too
  faux_node(G, 'return from call', G1),
  
  graph_connect(G, G1, G2),
  graph_last(G1, RetTo),
  G2 = graph(UG2, Mode, Count, Call, FunDecls),
  % Only add edge if we already visited that function
  (   Mode=compact, member(node(Label, Sig, _, _), FunDecls)
  ->  % connect to an existing function
      add_edges(UG2, [Call-Label], UG3),
      Exit = Label, % FIXME: should be function exit, not entry!
      G3 = graph(UG3, Mode, Count, Exit, FunDecls),
      P2 = P
  ;   % dive into the function
      ast_walk(Top-G2, FunctionDecl, P1-G3),
      walk_to(P1, Ctx, P2)
  ),

  % connect to the 'return from' node
  G3 = graph(UG3, Mode3, Count3, Last3, FunDecls3),
  add_edges(UG3, [Last3-RetTo], UG4),
  Gn = graph(UG4, Mode3, Count3, Last3, FunDecls3).

  %),
  %faux_node(G1, 'Function return', Gn).

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
make_back_edge(graph(_,_,_,Entry,_),
	       %graph(G,Mode,Last,Exit,FDs),
	       G_Final,
	       graph(G1,Mode,Last,Entry,FDs)) :-
  faux_node(G_Final, 'Loop end', graph(G,_,_,_,_)),
  G_Final=graph(_,Mode,Last,Exit,FDs),
  add_edges(G, [Exit-Entry], G1).

% Edge labels are modelled via the following trick
% N1 ->(Label) N2 is represented as N1 -> (Label, N2), N1 -> N2

make_node(P-G, G) :-
  unzip(P, Pragma, _),
  pragma_text(Pragma, _).

make_node(P-graph(G,Mode,Count,Last,FDs), graph(G1,Mode,Count1,Node,FDs)) :-
  unzip(P, Pragma, _),
  pragma_text(Pragma, _),
  unparse_to_safe_atom(Pragma, Text),
  Node = node(Count, Text, style='', []),
  Count1 #= Count+1,
  add_edges(G, [Last-edge(Text,Node)], G1).

make_node(P-graph(G,Mode,Count,Last,FDs), graph(G1,Mode,Count1,Node,FDs)) :-
  bound(P), bound(G), bound(Count),

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
    'shape=note,style=filled,fillcolor=cornsilk,pencolor=cornsilk4,id=<~w>',
	 [Context]),
  Node = node(Count, Stmt, style=Style, []),

  Count1 #= Count+1,
  add_edges(G, [Last-Node], G1).

faux_node(graph(G,Mode,Count,Last,FDs), Name, graph(G1,Mode,Count1,Node,FDs)) :-
  Node = node(Count, Name,
	      style='shape=note, style=filled, fillcolor=gold, pencolor=gold4',
	      []),
  Count1 #= Count+1,
  add_edges(G, [Last-Node], G1).

% same as faux but with SameRank of OtherLabel
samerank_node(graph(G,Mode,Count,Last,FDs), Name, Count, OtherLabel,
              graph(G1,Mode,Count1,Node,FDs)) :-
  Node = node(Count, Name,
	      style='shape=note, style=filled, fillcolor=gold, pencolor=gold4',
	      [OtherLabel]),
  Count1 #= Count+1,
  add_edges(G, [Last-Node], G1).


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

get_label(node(L,_,_,_), L, '').
get_label(G, L, '') :- graph_count(G, L).
get_label(L, L, '[constraint=false]') :- number(L). % back edge
%  format(atom(A), '~w ', L).

viz_edge(F, N1-N2) :-
  get_label(N1, L1, Constraint),
  get_label(N2, L2, _),
  format(F, '~w -> ~w ~w;~n', [L1, L2, Constraint]).
viz_edge(F, N1-edge(Attribute, N2)) :-
  get_label(N1, L1),
  get_label(N2, L2),
  format(F, '~w -> ~w [label=<~w>];~n', [L1, L2, Attribute]).

viz_edge(_, Edge) :- writeln(Edge), trace.

viz_node(F, _, node(Label,Stmt,style=Style,SameRank)) :- !,
  format(F, '~w [ label=<~w>, ~w ];~n', [Label,Stmt,Style]),
  viz_rank(F, Label, SameRank).

viz_node(F, _-Color, graph(G, explode, Count, Last, _FDs)) :- !,
  viz_exploded_subgraph(F, Color, graph(G, Count, Last)).

viz_node(F, Free-_, graph(G, compact, Count, Last, _FDs)) :- !,
  % hack ahead: delay execution until Free is bound
  freeze(Free, viz_compact_subgraph(F, Free, graph(G, Count, Last))).

% already handled above
viz_node(_, _, N) :- number(N).
viz_node(_, _, edge(_, _)).

% Error
viz_node(_, _, Node) :- writeln(Node), trace.

% subgraphs
viz_compact_subgraph(F, _Free, graph(G, Count, Last)) :-
  format(F, 'subgraph cluster~w {~n', [Count,G,Last]),
  format(F, 'node [style=filled];~n', []),
  format(F, 'style=filled; color=azure;~n', []),
  edges(G, E),     maplist(viz_edge(F), E),
  vertices(G, V),  maplist(viz_node(F, Free1-_), V),
  format(F, 'label="Function" ;~n', []),
  format(F, '} ;~n', []),
  Free1 = springtime.

viz_exploded_subgraph(F, Color, graph(G, Count, Last)) :-
  format(F, 'subgraph cluster~w {~n', [Count,G,Last]),
  format(F, 'node [style=filled];~n', []),
  format(F, 'style=filled; color=gray~w;~n', [Color]),
  Color1 #= Color - 12,
  edges(G, E),     maplist(viz_edge(F), E),
  vertices(G, V),  maplist(viz_node(F,_-Color1), V),
  format(F, 'label="Function" ;~n', []),
  format(F, '} ;~n', []).

viz_rank(_, _, []).
viz_rank(F, Label, SameRank) :-
  format(F, '{ rank = same; ', []),
  maplist(print_w_semi(F),[Label|SameRank]),
  format(F, '}~n', []).
print_w_semi(F, N) :- format(F, '~w; ', [N]).

%% graphviz(F, G, _).
%  Dump an ugraph in dotty syntax
graphviz(F, G, _Mode) :-
  edges(G, E),
  vertices(G, V),
  format(F, 'digraph G {~n', []),
  %Root = Base/_Type, member(Root, V),
  %format(F, '  root="~w";~n', [Root]),
  format(F, 'splines=true; overlap=true; rankdir=TB;~n', []),
  maplist(viz_edge(F), E),
  Free = _Unbound, Color #= 100,
  maplist(viz_node(F, Free-Color), V),
  Free = springtime,
  write(F, '}\n').
