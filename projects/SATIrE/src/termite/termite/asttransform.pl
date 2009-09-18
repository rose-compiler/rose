#!/usr/bin/pl -t testrun -f
% -*- mode: prolog; truncate-lines: true; -*-

:- module(asttransform,
	  [default_values/4,
	   simple_form_of/2,
           ast_node/6,
	   transformed_with/5,
	   transformed_with/6,
	   unparse_to/2,
	   unparse/1,
	   unparse/2,
	   unparse_storage_modifier/1,
	   unparse_ppi/2,
	   indent/1,
	   needs_semicolon/1,
	   needs_comma/1,
	   replace_types/3
	   ]).
:- use_module(library(astproperties)),
   use_module(library(utils)).

%-----------------------------------------------------------------------
/** <module> Properties of abstract syntax trees

This module defines commonly-used transformation utilities for
C/C++/Objective C ASTs given in the TERMITE term representation
as exported by SATIrE.

@version   @PACKAGE_VERSION@
@copyright Copyright (C) 2007-2009 Adrian Prantl
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

%% simple_form_of(-PreprocessingInfo, -DefaultAnnotation, -AnalysisInfo, -FileInfo) is det.
% Generate default annotations for generated AST nodes
default_values(PPI, DA, AI, FI) :-
  PPI = preprocessing_info([]),
  DA = default_annotation(null, PPI),
  AI = analysis_info([]),
  FI = file_info('compilerGenerated', 0, 0).


% Conversion utilities
%-----------------------------------------------------------------------

%% simple_form_of(?Term, ?SimpleTerm).
% This function is obsoleted, since SATIrE>0.7 defaults to the
% compact representation.
%
% simple_form_of/2 is used to convert the verbose *nary_node() terms
% to a more compact representation
%
% Example:
%
% ==
%  unary_node(int_val, foo ...) <-> int_val(foo, ...)
% ==

simple_form_of(null, null).
simple_form_of([], []).
simple_form_of([X|XS], [Y|YS]) :-
  simple_form_of(X, Y),
  simple_form_of(XS, YS),
  !.	      % [red cut] important for the backwards translation step

simple_form_of(unary_node(Type, N, An, Ai, Fi), Term) :-
  functor(Term, Type, 4),
  arg(1, Term, S),
  arg(2, Term, An),
  arg(3, Term, Ai),
  arg(4, Term, Fi),
  simple_form_of(N, S),
  \+ is_list(N),
  \+ is_list(S), 
  !.
simple_form_of(list_node(Type, List, An, Ai, Fi), Term) :-
  functor(Term, Type, 4),
  arg(1, Term, SimpleList),
  arg(2, Term, An),
  arg(3, Term, Ai),
  arg(4, Term, Fi),
  simple_form_of(List, SimpleList),
  !.
simple_form_of(binary_node(Type, N1, N2, An, Ai, Fi), Term) :-
  functor(Term, Type, 5),
  arg(1, Term, S1),
  arg(2, Term, S2),
  arg(3, Term, An),
  arg(4, Term, Ai),
  arg(5, Term, Fi),
  simple_form_of(N1, S1),
  simple_form_of(N2, S2), 
  !.
simple_form_of(ternary_node(Type, N1, N2, N3, An, Ai, Fi), Term) :-
  functor(Term, Type, 6),
  arg(1, Term, S1),
  arg(2, Term, S2),
  arg(3, Term, S3),
  arg(4, Term, An),
  arg(5, Term, Ai),
  arg(6, Term, Fi),
  simple_form_of(N1, S1),
  simple_form_of(N2, S2),
  simple_form_of(N3, S3),
  !.
simple_form_of(quaternary_node(Type, N1, N2, N3, N4, An, Ai, Fi), Term) :-
  functor(Term, Type, 7),
  arg(1, Term, S1),
  arg(2, Term, S2),
  arg(3, Term, S3),
  arg(4, Term, S4),
  arg(5, Term, An),
  arg(6, Term, Ai),
  arg(7, Term, Fi),
  simple_form_of(N1, S1),
  simple_form_of(N2, S2),
  simple_form_of(N3, S3),
  simple_form_of(N4, S4),
  !.
simple_form_of(leaf_node(Type, An, Ai, Fi), Term) :-
  functor(Term, Type, 3),
  arg(1, Term, An),
  arg(2, Term, Ai),
  arg(3, Term, Fi),
  !.
%-----------------------------------------------------------------------

%% ast_node(?Node, ?Type, ?Children, ?Annot, ?Ai, ?Fi) is nondet.
% ast_node/6 (de)construct an AST node
%
% Since all AST nodes follow the same structure, this predicate can be
% used to quickly compose or decompse a node.

ast_node(Node, Type, Children, Annot, Ai, Fi) :-
  var(Node), !, % avoid unneccesary backtracking in this direction
  append(Children, [Annot, Ai, Fi], Xs),
  Node =.. [Type|Xs].

ast_node(Node, Type, Children, Annot, Ai, Fi) :-
  Node =.. [Type|Xs],
  append(Children, [Annot, Ai, Fi], Xs).

%-----------------------------------------------------------------------

%% transformed_with(+Node, +Transformation, +Info, -Info1, -NodeT).
% Backwards compatibile version of transformed_with/5:
% * if it is used with arity 4, default to preorder
transformed_with(Node, Transformation, Info, Info1, NodeT) :-
  transformed_with(Node, Transformation, preorder, Info, Info1, NodeT).

%% transformed_with(+Node, +Transformation, +Order, +Info, ?InfoInner, -InfoPost, -NodeT).
% Traversal support:
%
% call Transformation(Info, InfoInner, InfoPost, OriginalNode, TransformedNode)
%
% ==
%   transformation(Info, InfoInner, InfoPost, Node, NodeTransformed) :-
%       ...
%
%   ...
%   transformed_with(OriginalNode, transformation,
%                    Info, InfoInner, InfoPost, TransformedNode).
% ==
%
% Warning: InfoPost works only in a sequential fashion - no merging et al.
%
% Todo: replace this with an attribute grammer system
%

transformed_with(Atom, _, _, Info, Info, Atom) :-
  atom(Atom), !.

% preorder ---------------------------------------
transformed_with(ListNode, Transformation, preorder, Info, InfoPost, 
	         ListNode2) :-
  arg(1, ListNode, [_|_]), !, % InfoInner is treated differently for ListNodes
  % This node
  apply_transformation(ListNode, Transformation, Info, InfoInner, InfoPost,
  		       ListNode1),
  % Children
  ListNode1 =.. [Type, List, Annot, Ai, Fi],
  transformed_with(List, Transformation, preorder, InfoInner, _, ListT),
  ast_node(ListNode2, Type, [ListT], Annot, Ai, Fi).

transformed_with([], _, _, Info, Info, []).
transformed_with([A|As], Transformation, preorder, Info, InfoPost, X) :- !,
  % First
  transformed_with(A, Transformation, preorder, Info, InfoPost1, B), 
  % Rest of list
  transformed_with(As, Transformation, preorder, InfoPost1, InfoPost, Bs),
  % Both can be lists
  (is_list(B)
  -> append(B, Bs, X)
  ;  X = [B|Bs]).

% postorder --------------------------------------

transformed_with([A|As], Transformation, postorder, Info, InfoPost, X) :- !,
  transformed_with(A, Transformation, postorder, Info, InfoInner, B),
  transformed_with(As, Transformation, postorder, InfoInner, InfoPost, Bs),
  (is_list(B)
  -> append(B, Bs, X)
  ;  X = [B|Bs]).

% ---------------------------------------------------------------------

% Generic version for 0-4ary nodes
transformed_with(Node, Transformation, preorder, Info, InfoT, NodeT) :-
  % The Inner Info only makes sense for ListNodes.
  apply_transformation(Node, Transformation, Info, _, Info0, Node0),
  transformed_children(Node0, Transformation, preorder, Info0, InfoT, NodeT).

transformed_with(Node, Transformation, postorder, Info, InfoT, NodeT) :-
  transformed_children(Node, Transformation, postorder, Info, Info0, Node0),
  apply_transformation(Node0, Transformation, Info0, _, InfoT, NodeT).

% helper for transformed_with/4
apply_transformation(null, _, I, I, I, null) :- !. % debugging speedup
apply_transformation(Node, Transformation, Info, InfoInner, InfoPost, 
                     NodeTrans) :-
  call(Transformation, Info, InfoInner, InfoPost, Node, NodeTrans).

% Transform subterms, left-to-right
transformed_children([], _, _, I, I, []).
transformed_children([N|Ns], Transformation, Order, Info0, InfoT, [NT|NsT]) :-
  transformed_children(N, Transformation, Order, Info0, InfoT1, NT),
  transformed_children(Ns, Transformation, Order, InfoT1, InfoT, NsT).

transformed_children(Node, Transformation, Order, Info0, InfoT, NodeT) :-
  ast_node(Node, Type, Children, Annot, Ai, Fi),
  transform_children(Children, Transformation, Order, Info0, InfoT, ChildrenT),
  ast_node(NodeT, Type, ChildrenT, Annot, Ai, Fi).

transform_children([], _, _, Info, Info, []).
transform_children([C|Cs], Transformation, Order, Info0, InfoT, [CT|CTs]) :-
  transformed_with(C, Transformation, Order, Info0, Info1, CT),
  transform_children(Cs, Transformation, Order, Info1, InfoT, CTs).

%-----------------------------------------------------------------------

%% unparse_to_atom(?Output, +Term) is det.
% Unparse the program Term using unparse/1 and sent the output to Output
% @see with_output_to/2
unparse_to(Output, Term) :- with_output_to(Output, unparse(Term)).

%% unparse(+Term) is det.
% This predicate prints the original textual (source code)
% representation of the program encoded in Term.  Output is written on
% stdout.
%
% This predicate is especially useful for debugging purposes.
unparse(Node) :- unparse(fi(0, 0, []), Node).

% FIXME implement replacement write/1 that uses col/line

unparse(fi(Line, Col, ParentPPIs), Node) :- 
  % extract mine
  get_preprocessing_infos(Node, PPIs),
  % unparse the node
  unparse_ppi(before, PPIs), !, % I/O means we can't backtrack anyway
  unparse1(fi(Line, Col, PPIs), Node), !,
  % process the parent preprocessing info
  unparse_ppi(inside, ParentPPIs), !,
  unparse_ppi(after, PPIs).


% OPERATORS
unparse1(UI, assign_op(E1, E2, _, _, _))       :- !, unparse_par(UI, E1), write(' = '), unparse_par(UI, E2).
unparse1(UI, plus_assign_op(E1, E2, _, _, _))  :- !, unparse_par(UI, E1), write(' += '), unparse_par(UI, E2).
unparse1(UI, minus_assign_op(E1, E2, _, _, _)) :- !, unparse_par(UI, E1), write(' -= '), unparse_par(UI, E2).
unparse1(UI, mult_assign_op(E1, E2, _, _, _))  :- !, unparse_par(UI, E1), write(' *= '), unparse_par(UI, E2).
unparse1(UI, div_assign_op(E1, E2, _, _, _))   :- !, unparse_par(UI, E1), write(' /= '), unparse_par(UI, E2).
unparse1(UI, lshift_assign_op(E1, E2, _, _, _)):- !, unparse_par(UI, E1), write(' <<= '), unparse_par(UI, E2).
unparse1(UI, rshift_assign_op(E1, E2, _, _, _)):- !, unparse_par(UI, E1), write(' >>= '), unparse_par(UI, E2).
unparse1(UI, and_assign_op(E1, E2, _, _, _))   :- !, unparse_par(UI, E1), write(' &= '), unparse_par(UI, E2).
unparse1(UI, or_assign_op(E1, E2, _, _, _))    :- !, unparse_par(UI, E1), write(' |= '), unparse_par(UI, E2).
unparse1(UI, ior_assign_op(E1, E2, _, _, _))   :- !, unparse_par(UI, E1), write(' |= '), unparse_par(UI, E2).
unparse1(UI, xor_assign_op(E1, E2, _, _, _))   :- !, unparse_par(UI, E1), write(' ^= '), unparse_par(UI, E2).

unparse1(UI, add_op(E1, E2, _, _, _))      :- !, unparse_par(UI, E1), write('+'), unparse_par(UI, E2).
unparse1(UI, bit_and_op(E1, E2, _, _, _))  :- !, unparse_par(UI, E1), write('&'), unparse_par(UI, E2).
unparse1(UI, and_op(E1, E2, _, _, _))      :- !, unparse_par(UI, E1), write('&&'), unparse_par(UI, E2).
unparse1(UI, bit_or_op(E1, E2, _, _, _))   :- !, unparse_par(UI, E1), write('|'), unparse_par(UI, E2).
unparse1(UI, or_op(E1, E2, _, _, _))       :- !, unparse_par(UI, E1), write('||'), unparse_par(UI, E2).
unparse1(UI, bit_xor_op(E1, E2, _, _, _))  :- !, unparse_par(UI, E1), write('^'), unparse_par(UI, E2).
unparse1(UI, mod_op(E1, E2, _, _, _))      :- !, unparse_par(UI, E1), write('%'), unparse_par(UI, E2).
unparse1(UI, divide_op(E1, E2, _, _, _))   :- !, unparse_par(UI, E1), write('/'), unparse_par(UI, E2).
unparse1(UI, lshift_op(E1, E2, _, _, _))   :- !, unparse_par(UI, E1), write('<<'), unparse_par(UI, E2).
unparse1(UI, rshift_op(E1, E2, _, _, _))   :- !, unparse_par(UI, E1), write('>>'), unparse_par(UI, E2).
unparse1(UI, plus_plus_op(E1, _, _, _))    :- !, unparse_par(UI, E1), write('++').
unparse1(UI, minus_minus_op(E1, _, _, _))  :- !, unparse_par(UI, E1), write('--').
unparse1(UI, subtract_op(E1, E2, _, _, _)) :- !, unparse_par(UI, E1), write('-'), unparse_par(UI, E2).
unparse1(UI, multiply_op(E1, E2, _, _, _)) :- !, unparse_par(UI, E1), write('*'), unparse_par(UI, E2).

unparse1(UI, address_of_op(E, _, _, _)) :- !, write('&'), unparse_par(UI, E).
unparse1(UI, minus_op(E, _, _, _)) :- !, write('-'), unparse_par(UI, E).
unparse1(UI, not_op(E, _, _, _)) :- !, write('!'), unparse_par(UI, E).
unparse1(UI, bit_complement_op(E, _, _, _)) :- !, write('~'), unparse_par(UI, E).

unparse1(UI, greater_or_equal_op(E1, E2, _, _, _)) :- !, unparse_par(UI, E1), write(' >= '), unparse_par(UI, E2).
unparse1(UI, greater_than_op(E1, E2, _, _, _)) :- !, unparse_par(UI, E1), write(' > '), unparse_par(UI, E2).
unparse1(UI, less_or_equal_op(E1, E2, _, _, _)) :- !, unparse_par(UI, E1), write(' <= '), unparse_par(UI, E2).
unparse1(UI, less_than_op(E1, E2, _, _, _)) :- !, unparse_par(UI, E1), write(' < '), unparse_par(UI, E2).
unparse1(UI, equality_op(E1, E2, _, _, _)) :- !, unparse_par(UI, E1), write(' == '), unparse_par(UI, E2).
unparse1(UI, not_equal_op(E1, E2, _, _, _)) :- !, unparse_par(UI, E1), write(' != '), unparse_par(UI, E2).

unparse1(UI, size_of_op(null, size_of_op_annotation(ClassType,_Type, _), _)) :- !,
  write('sizeof('), unparse(UI, ClassType), write(')').

% VALUES
unparse1(_UI,          char_val(_,value_annotation(Value,_),_)) :- string(Value), !, string_to_list(Value, [N]), write(N).
unparse1(_UI, unsigned_char_val(_,value_annotation(Value,_),_)) :- string(Value), !, string_to_list(Value, [N]), write(N).

unparse1(_UI, char_val(_,value_annotation(Value,_),_,_)) :- !, write(Value).
unparse1(_UI, int_val(_,value_annotation(Value,_),_,_)) :- !, write(Value).
unparse1(_UI, short_val(_,value_annotation(Value,_),_,_)) :- !, write(Value).
unparse1(_UI, short_int_val(_,value_annotation(Value,_),_,_)) :- !, write(Value).
unparse1(_UI, long_int_val(_,value_annotation(Value,_),_,_)) :- !, write(Value).
unparse1(_UI, long_long_int_val(_,value_annotation(Value,_),_,_)) :- !, write(Value).
unparse1(_UI, unsigned_char_val(_,value_annotation(Value,_),_,_)) :- !, write(Value).
unparse1(_UI, unsigned_int_val(_,value_annotation(Value,_),_,_)) :- !, write(Value).
unparse1(_UI, unsigned_short_int_val(_,value_annotation(Value,_),_,_)) :- !, write(Value).
unparse1(_UI, unsigned_short_val(_,value_annotation(Value,_),_,_)) :- !, write(Value).
unparse1(_UI, unsigned_long_val(_,value_annotation(Value,_),_,_)) :- !, write(Value).
unparse1(_UI, unsigned_long_long_val(_,value_annotation(Value,_),_,_)) :- !, write(Value).
unparse1(_UI, unsigned_long_long_int_val(_,value_annotation(Value,_),_,_)) :- !, write(Value).
unparse1(_UI, float_val(_,value_annotation(Value,_),_,_)) :- !, write(Value).
unparse1(_UI, double_val(_,value_annotation(Value,_),_,_)) :- !, write(Value).
unparse1(_UI, string_val(_,value_annotation(Value,_),_,_)) :- !, write('"'), write(Value), write('"').
unparse1(_UI, bool_val_exp(_, value_annotation(0,_),_,_)) :- !, write('false').
unparse1(_UI, bool_val_exp(_, value_annotation(1,_),_,_)) :- !, write('true').
unparse1(_UI, bool_val_exp(_, value_annotation(Value,_),_, _)) :- !, write(Value).
unparse1(_UI, enum_val(_,value_annotation(_,Value,_,_),_,_)) :- !, write(Value).

% EXPRESSIONS
unparse1(UI, function_call_exp(function_ref_exp(function_ref_exp_annotation(Name, _Type, _), _, _), ExprList, _, _, _)) :-
  !, write(Name), write('('), unparse(UI, ExprList), write(')').

unparse1(_UI, function_ref_exp(function_ref_exp_annotation(Name, _Type, _), _, _)) :- !, write(Name).

unparse1(_UI, expr_list_exp(_, _, _)) :- !.
unparse1(_UI, expr_list_exp([], _, _, _)) :- !.
unparse1(UI, expr_list_exp([E|[]], _, _, _)) :- !, unparse(UI, E).
unparse1(UI, expr_list_exp([E|Es], _, _, _)) :- !,
  unparse(UI, E), write(', '), unparse(UI, expr_list_exp(Es, _, _, _)).

% bb is the running basic block
unparse1(UI, bb([E|Es], A, Ai, Fi)) :- % no semicolon
  (E = pragma_declaration(_, _, _, _) ;
   E = default_option_stmt(_, _, _, _) ;
   E = case_option_stmt(_, _, _, _, _, _)), !,
  unparse(UI, E), nl,
  % in this case we did the PPI stuff already
  unparse1(UI, bb(Es, A, Ai, Fi)).
unparse1(UI, bb([E|Es], A, Ai, Fi)) :- % no semicolon
  scope_statement(E), !, 
  file_info(E, F),
  indent(F), unparse(UI, E), nl,
  unparse1(UI, bb(Es, A, Ai, Fi)).
unparse1(UI, bb([E|Es], A, Ai, Fi)) :- !,
  file_info(E, F),
  indent(F), unparse(UI, E), writeln(';'), 
  unparse1(UI, bb(Es, A, Ai, Fi)).
unparse1(_UI, bb([], _, _, _)) :- !.

unparse1(UI, [E|Es]) :- !,
  unparse(UI, E),
  (needs_semicolon(E) -> writeln(';') ; true),
  ((needs_comma(E), Es \== []) -> write(',') ; true),
  !,
  unparse(UI, Es).

unparse1(_UI, []) :- !.

unparse1(_UI, var_ref_exp(var_ref_exp_annotation(_Type, Name, _, _, _), _, _)) :- !,
  write(Name).

unparse1(UI, pntr_arr_ref_exp(Base, Offset, _, _, _)) :- !, 
  unparse_par(UI, Base), write('['), unparse_par(UI, Offset), write(']').

unparse1(UI, dot_exp(pointer_deref_exp(E1, _, _, _), E2, _, _, _)) :- !,
  unparse(UI, E1), write('->'), unparse_par(UI, E2).

%unparse1(UI, pointer_deref_exp(var_ref_exp(var_ref_exp_annotation(pointer_type(array_type(), _), _) _), _, _)) :- !, gtrace,
%  write('*'), unparse_par(UI, E1).

unparse1(UI, pointer_deref_exp(E1, _, _, _)) :- !, 
  write('*'), unparse_par(UI, E1).
unparse1(UI, cast_exp(E, _, unary_op_annotation(_, Type, _, _, _), _, _)) :- !,
  (  ( Type \= array_type(_, _),
       Type \= pointer_type(array_type(_, _)))
  -> (write('('), unparse(UI, Type), write(')'))
  ;  true),
  unparse_par(UI, E).
unparse1(UI, conditional_exp(E1, E2, E3, _, _, _)) :- !,
  unparse_par(UI, E1), write(' ? '), unparse_par(UI, E2), write(':'), unparse_par(UI, E3).
unparse1(UI, comma_op_exp(E1, E2, _, _, _)) :- !,
  unparse_par(UI, E1), write(', '), unparse_par(UI, E2).
unparse1(UI, dot_exp(E1, E2, _, _, _)) :- !,
  unparse_par(UI, E1), write('.'), unparse_par(UI, E2).
unparse1(UI, arrow_exp(E1, E2, _, _, _)) :- !,
  unparse_par(UI, E1), write('->'), unparse_par(UI, E2).
unparse1(_UI, null_expression(_, _, _)) :- !, write('/*NULL*/').

% STATEMENTS       
unparse1(UI, expression_root(E, _, _, _)) :- !, unparse(UI, E).
unparse1(UI, expr_statement(E, _, _, _)) :- !, unparse(UI, E).
unparse1(UI, for_init_statement(E, _, _, _)) :- !, unparse(UI, E).
unparse1(_UI, for_init_statement(_, _, _)) :- !.

unparse1(UI, for_statement(E1, E2, E3, E4, _, _, _)) :- !,
  write('for ('),
  unparse(UI, E1), write('; '),
  unparse(UI, E2), write('; '),
  unparse(UI, E3), write(') '),
  unparse_sem(UI, E4).

unparse1(UI, while_stmt(E1, E2, _, _, _)) :- !, 
  write('while ('), unparse(UI, E1), write(') '),
  unparse_sem(UI, E2).

unparse1(UI, do_while_stmt(E1, E2, _, _, _)) :- !, 
  write('do '),
  unparse(UI, E1), 
  write('while ('), unparse(UI, E2), writeln(');').

unparse1(UI, if_stmt(E1, E2, E3, _, _, _)) :- !, 
  write('if ('), unparse(UI, E1), write(') '),
  unparse_sem(UI, E2),
  (E3 = null/*basic_block(_,_)*/
  -> true
  ; (write('else '), unparse_sem(UI, E3))
  ).

unparse1(UI, switch_statement(E1, E2, _, _, _)) :- !, write('switch ('), unparse(UI, E1), write(') '), unparse_sem(UI, E2).
unparse1(UI, case_option_stmt(E1, E2, null, _, _, _)) :- !, write('case '), unparse(UI, E1), write(': '), unparse_sem(UI, E2).
unparse1(UI, default_option_stmt(E1, _, _, _)) :- !, writeln('default: '), unparse_sem(UI, E1).

unparse1(UI, return_stmt(E1, _, _, _)) :- !, write('return '), unparse(UI, E1).
unparse1(_UI, break_stmt(_, _, _)) :- !, write('break ').
unparse1(_UI, continue_stmt(_, _, _)) :- !, write('continue ').
unparse1(_UI, goto_statement(label_annotation(Label, _), _, _)) :- !, write('goto '), write(Label).
unparse1(_UI, label_statement(label_annotation(Label, _), _, _)) :- !, write(Label), write(': ').
unparse1(UI, basic_block(E1, An, Ai, Fi)) :- !, 
  writeln(' {'),
  % We did the PPI stuff already
  unparse1(UI, bb(E1, An, Ai, Fi)),
  indent(Fi), writeln('}').
unparse1(_UI, basic_block(_, _, _)) :- !, write(' {} ').
unparse1(UI, global(E1, _, _, _)) :- !, unparse(UI, E1).
unparse1(UI, source_file(E1, _An, _Ai, file_info(Name, _, _))) :- !,
  write('/* '), write(Name), writeln(': */'), unparse(UI, E1).
unparse1(UI, project(E1, _, _, _)) :- !, unparse(UI, E1).

unparse1(UI, function_declaration(Params, Definition,
             function_declaration_annotation(Type, Name, Mod, _), _, _)) :- !,
  unparse_modifier(Mod),
  unparse(UI, Type), write(' '), write(Name),
  write('('),
  (Params = function_parameter_list(Ps, _, _, _) ->
   (replace_types(Ps, Type, Ps1),
    unparse(UI, Ps1))
  ;
   true),
  write(')'),
  (Definition = null)
    -> writeln(';') ;
    (
     Definition = function_definition(Bb, _, _, _),
     unparse(UI, Bb)
    ).


unparse1(UI, function_type(Type, _NumParams, _ParamTypes)) :- !, unparse(UI, Type).
unparse1(_UI, function_parameter_list(_, _, _)) :- !.
unparse1(UI, function_parameter_list(List, _, _, _)) :- !, unparse(UI, List).

% TYPES
unparse1(UI, T) :-
  (T = pointer_type(T1) ; T = T1),
  T1 = modifier_type(Type,
		     type_modifier([0|[1|[Restrict|[0|_]]]],
				   1, ConstVolatile, 1)), !,
  gtrace,(ConstVolatile = 2 -> write('const ') ; true),
  (ConstVolatile = 3 -> write('volatile ') ; true),
  unparse(UI, Type),
  (T = pointer_type(_) -> write('* ') ; true),
  (Restrict = 0 -> write(' /*__restrict__*/ ') ; true).
unparse1(UI, modifier_type(Type, type_modifier(_A, UPC, ConstVolatile, Elaborate))) :- !,
  (UPC = default -> true ; write(UPC), write(' ')),
  (ConstVolatile = default -> true ; write(ConstVolatile), write(' ')),
  (Elaborate = default -> true ; write(Elaborate), write(' ')),
  unparse(UI, Type).
unparse1(_UI, type_void) :- !, write('void').
unparse1(_UI, type_int) :- !, write('int').
unparse1(_UI, type_char) :- !, write('char').
unparse1(_UI, type_signed_char) :- !, write('char').
unparse1(_UI, type_short) :- !, write('short').
unparse1(_UI, type_long) :- !, write('long').
unparse1(_UI, type_long_long) :- !, write('long long').
unparse1(_UI, type_unsigned_char) :- !, write('unsigned char').
unparse1(_UI, type_unsigned_int) :- !, write('unsigned int').
unparse1(_UI, type_unsigned_short) :- !, write('unsigned short').
unparse1(_UI, type_unsigned_long) :- !, write('unsigned long').
unparse1(_UI, type_unsigned_long_long) :- !, write('unsigned long long').
unparse1(_UI, type_bool) :- !, write('bool').
unparse1(_UI, type_float) :- !, write('float').
unparse1(_UI, type_double) :- !, write('double').
unparse1(_UI, type_long_double) :- !, write('long double').
unparse1(_UI, type_ellipse) :- !, write('...').

unparse1(UI, array_type(Type, Val)) :- !,
  unparse_type(UI, '', array_type(Type, Val)).
  %unparse(UI, Type), write('['), unparse(UI, Val), write(']').
unparse1(_UI, typedef_type(Type, _)) :- !, write(Type).
unparse1(_UI, class_type(Name,ClassType,_Scope)) :- !,
  write(ClassType), write(' '), write(Name).
unparse1(UI, enum_type(Type)) :- !, unparse(UI, Type).

%unparse1(UI, pointer_type(array_type(T, V))) :- !, 
%  write('('), unparse(UI, array_type(T, V)), write(')*').
unparse1(UI, pointer_type(T)) :- !, unparse(UI, T), write('*').
unparse1(UI, function_type(T)) :- !, unparse(UI, T), write('()').

% DEFINITIONS/DECLARATIONS

%unparse1(UI, typedef_declaration(_, typedef_annotation(
%          Name,
%          pointer_type(Type),
%          class_declaration(class_definition(Definitions, _, _), _, _), _),
%	  _Ai, _Fi)) :- !, 
%  write('typedef '), 
%  with_output_to(atom(X),
%		 (write('{'), unparse(UI, Definitions), write('} *'), write(Name))),
%  unparse_type(UI, X, Type).

unparse1(UI, typedef_declaration(null, typedef_annotation(Name,Type,_,_), _Ai, _Fi)) :- !,
  write('typedef '), unparse_type(UI, Name, Type).

unparse1(UI, typedef_declaration(Definition, typedef_annotation(
          Name,
          _Type, _Decl/*, _PPI*/), _Ai, _Fi)) :- !,
  write('typedef '), 
  unparse(UI, Definition), write(' '), write(Name).

%unparse1(UI, typedef_declaration(_, typedef_annotation(
%          Name,
%          Type,
%          class_declaration(class_definition(Definitions, _, _), _, _), _),
%	  _Ai, _Fi)) :- !, 
%  write('typedef '), 
%  with_output_to(atom(X),
%		 (write('{'), unparse(UI, Definitions), write('} '), write(Name))),
%  unparse_type(UI, X, Type).

unparse1(UI, function_definition(BB, _, _, _)) :- !, unparse(UI, BB).


% VARIABLES
% typedef structs
unparse1(UI, class_declaration(class_definition(Definitions, _, _, _),
			  class_declaration_annotation(Name, _Struct,
               class_type(_InitializedName,ClassType, _Scope), _), _, _)) :- !,
	write(ClassType), write(' '), write(Name),
	write(' {'),  unparse(UI, Definitions), write('} ').
unparse1(_UI, class_declaration(null,
			  class_declaration_annotation(Name, _Struct,
	       class_type(_InitializedName,ClassType, _Scope), _), _, _)) :- !,
	write(ClassType), write(' '), write(Name).

unparse1(UI, enum_declaration(InitializedNames,
	enum_declaration_annotation(Name, _Attributes, _, _), _, _)) :- !,
	write('enum '), write(Name),
	write(' {'),  unparse_enum(UI, InitializedNames), write(' } ').


% on-the-fly structs
unparse1(UI, variable_declaration([ClassDeclaration|[InitializedName|INs]],
			     Spec, Ai, Fi)) :- 
  ( Spec = variable_declaration_specific(Mod, _)
  ; Spec = variable_declaration_specific(Mod, _, _)),
  unparse_modifier(Mod),
  ClassDeclaration = class_declaration(_, _, _, _), !,
  InitializedName = initialized_name(_,
	initialized_name_annotation(_, Name, _, _), _, _),
  unparse(UI, ClassDeclaration),
  write(Name), % eat the modifier
  unparse(UI, variable_declaration(INs, variable_declaration_specific(null), Ai, Fi)).

unparse1(UI, variable_declaration([InitializedName|INs], Spec, Ai, Fi)) :- !,
  ( Spec = variable_declaration_specific(Mod, _)
  ; Spec = variable_declaration_specific(Mod, _, _)),
  unparse_modifier(Mod),
  unparse(UI, InitializedName), !,
  unparse(UI, variable_declaration(INs, variable_declaration_specific(null), Ai, Fi)).

unparse1(UI, initialized_name(Initializer,
			 initialized_name_annotation(Type, Name, _,_),_, _)) :-
  !,
  unparse_type(UI, Name, Type),
  (Initializer = null -> true ; write(' = ')),
  unparse(UI, Initializer).
unparse1(_UI, variable_declaration([], _, _, _)) :- !.

unparse1(UI, aggregate_initializer(Es, _, _, _)) :- !,
  write('{'), unparse(UI, Es), write(' }').
unparse1(UI, assign_initializer(E, _, _, _)) :- !,
  write(''), unparse(UI, E).

unparse1(_UI, pragma_declaration(Pragma, _, _, _)) :-
  Pragma = pragma(pragma_annotation(Text), _, _), !,
  write('#pragma '), write(Text).
unparse1(_UI, null) :- !.

% ARTIFICIAL NODES

% This one is artificial and only used for the CFG
unparse1(_UI, function_end(_, function_declaration_annotation(_, Name, _, _), _)) :- !, 
  write('END '), write(Name), write(';').

% artificial and only used by simplify
unparse1(_UI, interval(Min, Max)) :- !, write(interval(Min, Max)).

% ERRORS
unparse1(_UI, X) :- var(X), !, writeln('UNBOUND!'), gtrace.
unparse1(UI, X) :- write(X), gtrace, unparse(UI, X).

unparse_sem(UI, X) :-
  functor(X, basic_block, _), !,
  unparse(UI, X).
unparse_sem(UI, X) :-
  unparse(UI, X), writeln(';').


% Helper functions for types (esp. arrays)
unparse_type(UI, Name, Type) :-
  ( Type = array_type(_,_)
  ;(Type = pointer_type(array_type(Type,_)),
    Type \= array_type(_,_))
  ), !,
  inner_type(InnerType, Type),
  unparse(UI, InnerType), write(' '), write(Name),
  unparse_array_type(UI, Type).

% FIXME
unparse_type(UI, Name, pointer_type(Type)) :-
  Type = array_type(array_type(_,_),_), !,
  inner_type(InnerType, Type),
  unparse(UI, InnerType), write(' (*'), write(Name), write(')'),
  unparse_array_type(UI, Type).

unparse_type(UI, Name, Type) :- !,
  unparse(UI, Type), write(' '), write(Name).

inner_type(DataType, array_type(Type, _)) :- !, inner_type(DataType, Type).
inner_type(DataType, pointer_type(array_type(Type,_))) :- !,
  inner_type(DataType, Type).
inner_type(DataType, DataType).

unparse_array_type(UI, array_type(Type, Val1)) :- !, 
  write('['), unparse(UI, Val1), write(']'),
  unparse_array_type(UI, Type).
unparse_array_type(UI, pointer_type(array_type(Type,Val))) :-
  write('[]'),
  unparse_array_type(UI, array_type(Type,Val)).
unparse_array_type(_UI, _).

unparse_enum(_UI, []) :- !.
unparse_enum(UI, [N|Ns]) :- !,
  N = initialized_name(Initializer,
		       initialized_name_annotation(_Type, Name, _, _), _, _),
  write(Name),
  (Initializer = null
  -> true
  ;  (write(' = '), unparse(UI, Initializer))),
  (Ns = []
  -> true
  ; writeln(',')),
  unparse_enum(UI, Ns).

% Put parentheses around E if necessary
unparse_par(UI, E) :- unparse_par1(UI, E), !.
unparse_par1(UI, E) :-
  functor(E, function_call_exp, _),
  unparse(UI, E).
unparse_par1(UI, E) :-
  functor(E, pointer_deref_exp, _),
  write('('), unparse(UI, E), write(')').
unparse_par1(UI, E) :-
  functor(E, _, N), N < 4, !,
  unparse(UI, E).
unparse_par1(UI, E) :-
  write('('), unparse(UI, E), write(')').

unparse_modifier(Mod) :-
  Mod = declaration_modifier(_,_TypeModifier,_,StorageModifier)
  -> unparse_storage_modifier(StorageModifier)
  ;  true.

unparse_storage_modifier(2) :- !, write('extern ').
unparse_storage_modifier(3) :- !, write('static ').
unparse_storage_modifier(4) :- !, write('auto ').
unparse_storage_modifier(6) :- !, write('register ').
unparse_storage_modifier(7) :- !, write('mutable ').
unparse_storage_modifier(8) :- !, write('typedef ').
unparse_storage_modifier(9) :- !, write('asm ').
unparse_storage_modifier(_) :- !.

%% needs_semicolon(+Node) is det.
% Succeeds if Node needs a semicolon ';' after itself during unparsing.
needs_semicolon(typedef_declaration(_,_,_,_)).
needs_semicolon(variable_declaration(_,_,_,_)).
needs_semicolon(class_declaration(_,_,_,_)).
needs_semicolon(enum_declaration(_,_,_,_)).

%% needs_comma(+Node) is det.
% Succeeds if Node needs a comma ',' after itself during unparsing.
needs_comma(initialized_name(_,_,_,_)).

%% replace_types(+InitializedNames, +FuncDecl, -InitializedNames1) is det.
% Replace the instatiated types with the original types from the
% function declaration.
%
% Needed during unparsing.
replace_types([], function_type(_, _, []), []) :- !.
replace_types([I|Is],
	      function_type(ReturnT, NumParams, [Type|Ts]),
	      [J|Js]) :- !,
  I = initialized_name(A, initialized_name_annotation(_, B, C, D), Ai, Fi),
  J = initialized_name(A, initialized_name_annotation(Type, B, C, D), Ai, Fi),
  replace_types(Is, function_type(ReturnT, NumParams, Ts), Js).
replace_types(_,_,_) :- trace.

%% indent(+FileInfo) is det.
% Output the indentation that is encoded in FileInfo.
indent(file_info(_,0,0)) :- !, write('    '). % dummy
indent(file_info(_Filename,_Line,Col)) :- tab(Col).

%% unparse_ppi(+Location, +PPIs) is det.
% Print all preprocessing information(s) PPIs at Location.
%
% Location must be one of [before, after, inside].
unparse_ppi(_, []).
unparse_ppi(Location, [PPI|PPIs]) :-
  PPI =.. [Type, Text, Location, Fi], !,
  (Fi = file_info(_, _, 1) -> nl; true),
  write(Text),
  % FIXME use file info instead!
  (Type = c_StyleComment -> true; nl),
  unparse_ppi(Location, PPIs).

unparse_ppi(Location, [_|PPIs]) :- !, unparse_ppi(Location, PPIs).
