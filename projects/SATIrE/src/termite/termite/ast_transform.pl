#!/usr/bin/pl -t testrun -f
% -*- prolog -*-

:- module(ast_transform,
	  [simple_form_of/2,
           ast_node/6,
	   transformed_with/5,
	   transformed_with/6,
	   unparse/1,
	   unparse_storage_modifier/1,
	   indent/1,
	   needs_semicolon/1,
	   needs_comma/1,
	   replace_types/3
	   ]).
:- use_module(library(ast_properties)),
   use_module(library(utils)).

%-----------------------------------------------------------------------
/** <module> Properties of abstract syntax trees

This module defines commonly-used transformation utilities
for the AST exported by SATIrE.

@author

Copyright 2007-2009 Adrian Prantl <adrian@complang.tuwien.ac.at>

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

% Conversion utilities
%-----------------------------------------------------------------------

%% simple_form_of(+Term, -SimpleTerm) is det.
%% simple_form_of(-Term, +SimpleTerm) is det.
% simple_form_of/2 is used to convert the verbose *nary_node() terms
% to a more compact representation
%
% Example: unary_node(int_val, foo ...) <-> int_val(foo, ...)

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
% ast_node/6 deconstruct an AST node
%

ast_node(Node, Type, Children, Annot, Ai, Fi) :-
  var(Node), !, % avoid unneccesary backtracking in this direction
  append(Children, [Annot, Ai, Fi], Xs),
  Node =.. [Type|Xs].

ast_node(Node, Type, Children, Annot, Ai, Fi) :-
  Node =.. [Type|Xs],
  append(Children, [Annot, Ai, Fi], Xs).

%-----------------------------------------------------------------------

%% transformed_with(+Node, +Transformation, +Info, -Info1, -NodeT).
%  Traversal support:
% transformed_with(OriginalNode, Transformation, Info, InfoPost TransformedNode)
% -> apply <Transformation, Info> on <OriginalNode> yields <TransformedNode>
%
% Warning: InfoPost works only in a sequential fashion - no merging et al.
% TODO: replace this with an attribute grammer system
%
% Backwards compatibility:
%   if it is used with arity 4, default to preorder
transformed_with(Node, Transformation, Info, Info1, NodeT) :-
  transformed_with(Node, Transformation, preorder, Info, Info1, NodeT).

transformed_with(null, _, _, Info, Info, null) :- !. % [green cut] just a speedup for debugging

transformed_with(Atom, _, _, Info, Info, Atom) :-
  atom(Atom).

%% transformed_with(+Node, +Transformation, +Order, +Info, -Info1, -NodeT).

% preorder ---------------------------------------
transformed_with(ListNode, Transformation, preorder, Info, InfoPost, 
	         ListNode2) :-
  arg(1, ListNode, [_|_]), !, % InfoInner is treated differently for ListNodes
  % This node
  apply_transformation(ListNode, Transformation, Info, InfoInner, InfoPost,
  		       ListNode1),
  % Children
  functor(ListNode1, Type, 4),
  arg(1, ListNode1, List),
  arg(2, ListNode1, Annot),
  arg(3, ListNode1, Ai),
  arg(4, ListNode1, Fi),
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

%% unparse(+Term) is det.
% This is a debugging function that prints a textual representation
% of a program that is given in simple form.

% OPERATORS
unparse(assign_op(E1, E2, _, _, _))       :- !, unparse_par(E1), write(' = '), unparse_par(E2).
unparse(plus_assign_op(E1, E2, _, _, _))  :- !, unparse_par(E1), write(' += '), unparse_par(E2).
unparse(minus_assign_op(E1, E2, _, _, _)) :- !, unparse_par(E1), write(' -= '), unparse_par(E2).
unparse(mult_assign_op(E1, E2, _, _, _))  :- !, unparse_par(E1), write(' *= '), unparse_par(E2).
unparse(div_assign_op(E1, E2, _, _, _))   :- !, unparse_par(E1), write(' /= '), unparse_par(E2).
unparse(lshift_assign_op(E1, E2, _, _, _)):- !, unparse_par(E1), write(' <<= '), unparse_par(E2).
unparse(rshift_assign_op(E1, E2, _, _, _)):- !, unparse_par(E1), write(' >>= '), unparse_par(E2).
unparse(and_assign_op(E1, E2, _, _, _))   :- !, unparse_par(E1), write(' &= '), unparse_par(E2).
unparse(or_assign_op(E1, E2, _, _, _))    :- !, unparse_par(E1), write(' |= '), unparse_par(E2).
unparse(ior_assign_op(E1, E2, _, _, _))    :- !, unparse_par(E1), write(' |= '), unparse_par(E2).
unparse(xor_assign_op(E1, E2, _, _, _))   :- !, unparse_par(E1), write(' ^= '), unparse_par(E2).

unparse(add_op(E1, E2, _, _, _))      :- !, unparse_par(E1), write('+'), unparse_par(E2).
unparse(bit_and_op(E1, E2, _, _, _))  :- !, unparse_par(E1), write('&'), unparse_par(E2).
unparse(and_op(E1, E2, _, _, _))      :- !, unparse_par(E1), write('&&'), unparse_par(E2).
unparse(bit_or_op(E1, E2, _, _, _))   :- !, unparse_par(E1), write('|'), unparse_par(E2).
unparse(or_op(E1, E2, _, _, _))       :- !, unparse_par(E1), write('||'), unparse_par(E2).
unparse(bit_xor_op(E1, E2, _, _, _))  :- !, unparse_par(E1), write('^'), unparse_par(E2).
unparse(mod_op(E1, E2, _, _, _))      :- !, unparse_par(E1), write('%'), unparse_par(E2).
unparse(divide_op(E1, E2, _, _, _))   :- !, unparse_par(E1), write('/'), unparse_par(E2).
unparse(lshift_op(E1, E2, _, _, _))   :- !, unparse_par(E1), write('<<'), unparse_par(E2).
unparse(rshift_op(E1, E2, _, _, _))   :- !, unparse_par(E1), write('>>'), unparse_par(E2).
unparse(plus_plus_op(E1, _, _, _))    :- !, unparse_par(E1), write('++').
unparse(minus_minus_op(E1, _, _, _))  :- !, unparse_par(E1), write('--').
unparse(subtract_op(E1, E2, _, _, _)) :- !, unparse_par(E1), write('-'), unparse_par(E2).
unparse(multiply_op(E1, E2, _, _, _)) :- !, unparse_par(E1), write('*'), unparse_par(E2).

unparse(address_of_op(E, _, _, _)) :- !, write('&'), unparse_par(E).
unparse(minus_op(E, _, _, _)) :- !, write('-'), unparse_par(E).
unparse(not_op(E, _, _, _)) :- !, write('!'), unparse_par(E).
unparse(bit_complement_op(E, _, _, _)) :- !, write('~'), unparse_par(E).

unparse(greater_or_equal_op(E1, E2, _, _, _)) :- !, unparse_par(E1), write(' >= '), unparse_par(E2).
unparse(greater_than_op(E1, E2, _, _, _)) :- !, unparse_par(E1), write(' > '), unparse_par(E2).
unparse(less_or_equal_op(E1, E2, _, _, _)) :- !, unparse_par(E1), write(' <= '), unparse_par(E2).
unparse(less_than_op(E1, E2, _, _, _)) :- !, unparse_par(E1), write(' < '), unparse_par(E2).
unparse(equality_op(E1, E2, _, _, _)) :- !, unparse_par(E1), write(' == '), unparse_par(E2).
unparse(not_equal_op(E1, E2, _, _, _)) :- !, unparse_par(E1), write(' != '), unparse_par(E2).

unparse(size_of_op(null, size_of_op_annotation(ClassType,_Type, _), _)) :- !,
  write('sizeof('), unparse(ClassType), write(')').

% VALUES
unparse(Char) :-
  (Char =          char_val(_,value_annotation(Value,_),_) ;
   Char = unsigned_char_val(_,value_annotation(Value,_),_)),
  string(Value), !,
  string_to_list(Value, [N]), write(N).

unparse(enum_val(_,value_annotation(_,Value,_),_,_)) :- !, write(Value).
unparse(char_val(_,value_annotation(Value),_,_)) :- !, write(Value).
unparse(int_val(_,value_annotation(Value),_,_)) :- !, write(Value).
unparse(short_val(_,value_annotation(Value),_,_)) :- !, write(Value).
unparse(short_int_val(_,value_annotation(Value),_,_)) :- !, write(Value).
unparse(long_int_val(_,value_annotation(Value),_,_)) :- !, write(Value).
unparse(long_long_int_val(_,value_annotation(Value),_,_)) :- !, write(Value).
unparse(unsigned_char_val(_,value_annotation(Value),_,_)) :- !, write(Value).
unparse(unsigned_int_val(_,value_annotation(Value),_,_)) :- !, write(Value).
unparse(unsigned_short_int_val(_,value_annotation(Value),_,_)) :- !, write(Value).
unparse(unsigned_short_val(_,value_annotation(Value),_,_)) :- !, write(Value).
unparse(unsigned_long_val(_,value_annotation(Value),_,_)) :- !, write(Value).
unparse(unsigned_long_long_val(_,value_annotation(Value),_,_)) :- !, write(Value).
unparse(unsigned_long_long_int_val(_,value_annotation(Value),_,_)) :- !, write(Value).
unparse(float_val(_,value_annotation(Value),_,_)) :- !, write(Value).
unparse(double_val(_,value_annotation(Value),_,_)) :- !, write(Value).
unparse(string_val(_,value_annotation(Value),_,_)) :- !, write('"'), write(Value), write('"').
unparse(bool_val_exp(_, value_annotation(0),_,_)) :- !, write('false').
unparse(bool_val_exp(_, value_annotation(1),_,_)) :- !, write('true').
unparse(bool_val_exp(_, value_annotation(Value),_, _)) :- !, write(Value).

% EXPRESSIONS
unparse(function_call_exp(function_ref_exp(function_ref_exp_annotation(Name, _),
					   _, _),
			  ExprList, _, _, _)) :- !,
  write(Name), write('('), unparse(ExprList), write(')').

unparse(function_ref_exp(function_ref_exp_annotation(Name, _), _, _)) :- !,
  write(Name).

unparse(expr_list_exp(_, _, _)) :- !.
unparse(expr_list_exp([], _, _, _)) :- !.
unparse(expr_list_exp([E|[]], _, _, _)) :- !, unparse(E).
unparse(expr_list_exp([E|Es], _, _, _)) :- !,
  unparse(E), write(', '), unparse(expr_list_exp(Es, _, _, _)).

% bb is the running basic block
unparse(bb([E|Es], A, Ai, Fi)) :- % no semicolon
  (E = pragma_declaration(_, _, _, _) ;
   E = default_option_stmt(_, _, _, _) ;
   E = case_option_stmt(_, _, _, _, _, _)), !,
  unparse(E), nl,
  unparse(bb(Es, A, Ai, Fi)).
unparse(bb([E|Es], A, Ai, Fi)) :- % no semicolon
  scope_statement(E), !, 
  file_info(E, F),
  indent(F), unparse(E), nl,
  unparse(bb(Es, A, Ai, Fi)).
unparse(bb([E|Es], A, Ai, Fi)) :- !,
  file_info(E, F),
  indent(F), unparse(E), writeln(';'), 
  unparse(bb(Es, A, Ai, Fi)).
unparse(bb([], _, _, _)) :- !.

unparse([E|Es]) :- !,
  unparse(E),
  (needs_semicolon(E) -> writeln(';') ; true),
  ((needs_comma(E), Es \== []) -> write(',') ; true),
  !,
  unparse(Es).

unparse([]) :- !.

unparse(var_ref_exp(var_ref_exp_annotation(_, Name, _, _), _, _)) :- !,
  write(Name).

unparse(pntr_arr_ref_exp(Base, Offset, _, _, _)) :- !, 
  unparse_par(Base), write('['), unparse_par(Offset), write(']').

unparse(dot_exp(pointer_deref_exp(E1, _, _, _), E2, _, _, _)) :- !,
  unparse(E1), write('->'), unparse_par(E2).

%unparse(pointer_deref_exp(var_ref_exp(var_ref_exp_annotation(pointer_type(array_type(), _), _, ) _), _, _)) :- !, gtrace,
%  write('*'), unparse_par(E1).

unparse(pointer_deref_exp(E1, _, _, _)) :- !, 
  write('*'), unparse_par(E1).
unparse(cast_exp(E, _, unary_op_annotation(_, Type, _, _), _, _)) :- !,
  (  ( Type \= array_type(_, _),
       Type \= pointer_type(array_type(_, _)))
  -> (write('('), unparse(Type), write(')'))
  ;  true),
  unparse_par(E).
unparse(conditional_exp(E1, E2, E3, _, _, _)) :- !,
  unparse_par(E1), write(' ? '), unparse_par(E2), write(':'), unparse_par(E3).
unparse(comma_op_exp(E1, E2, _, _, _)) :- !,
  unparse_par(E1), write(', '), unparse_par(E2).
unparse(dot_exp(E1, E2, _, _, _)) :- !,
  unparse_par(E1), write('.'), unparse_par(E2).
unparse(arrow_exp(E1, E2, _, _, _)) :- !,
  unparse_par(E1), write('->'), unparse_par(E2).
unparse(null_expression(_, _, _)) :- !, write('/*NULL*/').

% STATEMENTS       
unparse(expression_root(E, _, _, _)) :- !, unparse(E).
unparse(expr_statement(E, _, _, _)) :- !, unparse(E).
unparse(for_init_statement(E, _, _, _)) :- !, unparse(E).
unparse(for_init_statement(_, _, _)) :- !.

unparse(for_statement(E1, E2, E3, E4, _, _, _)) :- !,
  write('for ('),
  unparse(E1), write('; '),
  unparse(E2), write('; '),
  unparse(E3), write(') '),
  unparse_sem(E4).

unparse(while_stmt(E1, E2, _, _, _)) :- !, 
  write('while ('), unparse(E1), write(') '),
  unparse_sem(E2).

unparse(do_while_stmt(E1, E2, _, _, _)) :- !, 
  write('do '),
  unparse(E1), 
  write('while ('), unparse(E2), writeln(');').

unparse(if_stmt(E1, E2, E3, _, _, _)) :- !, 
  write('if ('), unparse(E1), write(') '),
  unparse_sem(E2),
  (E3 = null/*basic_block(_,_)*/
  -> true
  ; (write('else '), unparse_sem(E3))
  ).

unparse(switch_statement(E1, E2, _, _, _)) :- !,
  write('switch ('), unparse(E1), write(') '),
  unparse_sem(E2).
unparse(case_option_stmt(E1, E2, null, _, _, _)) :- !,
  write('case '), unparse(E1), write(': '),
  unparse_sem(E2).
unparse(default_option_stmt(E1, _, _, _)) :- !,
  writeln('default: '), unparse_sem(E1).

unparse(return_stmt(E1, _, _, _)) :- !, write('return '), unparse(E1).
unparse(break_stmt(_, _, _)) :- !, write('break ').
unparse(continue_stmt(_, _, _)) :- !, write('continue ').
unparse(goto_statement(label_annotation(Label), _, _)) :- !, write('goto '), write(Label).
unparse(label_statement(label_annotation(Label), _, _)) :- !, write(Label), write(': ').
unparse(basic_block(E1, An, Ai, Fi)) :- !,
  writeln(' {'),
  unparse(bb(E1, An, Ai, Fi)),
  indent(Fi), writeln('}').
unparse(basic_block(_, _, _)) :- !, write(' {} ').
unparse(global(E1, _, _, _)) :- !, unparse(E1).
unparse(file(E1, _, _An, _Ai, file_info(Name, _, _))) :- !,
  write('/* '), write(Name), writeln(': */'), unparse(E1).
unparse(project(E1, _, _, _)) :- !, unparse(E1).

unparse(function_declaration(Params, Definition,
             function_declaration_annotation(Type, Name, Mod), _, _)) :- !,
  unparse_modifier(Mod),
  unparse(Type), write(' '), write(Name),
  write('('),
  (Params = function_parameter_list(Ps, _, _, _) ->
   (replace_types(Ps, Type, Ps1),
    unparse(Ps1))
  ;
   true),
  write(')'),
  (Definition = null)
    -> writeln(';') ;
    (
     Definition = function_definition(Bb, _, _, _),
     unparse(Bb)
    ).


unparse(function_type(Type, _NumParams, _ParamTypes)) :- !, unparse(Type).
unparse(function_parameter_list(_, _, _)) :- !.
unparse(function_parameter_list(List, _, _, _)) :- !, unparse(List).

% TYPES
unparse(T) :-
  (T = pointer_type(T1) ; T = T1),
  T1 = modifier_type(Type,
		     type_modifier([0|[1|[Restrict|[0|_]]]],
				   1, ConstVolatile, 1)), !,
  (ConstVolatile = 2 -> write('const ') ; true),
  (ConstVolatile = 3 -> write('volatile ') ; true),
  unparse(Type),
  (T = pointer_type(_) -> write('* ') ; true),
  (Restrict = 0 -> write(' /*__restrict__*/ ') ; true).
unparse(modifier_type(Type, type_modifier(A, UPC, ConstVolatile, Elaborate))) :- !,
  write('#modifier# '), writeln(A), writeln(UPC), writeln(ConstVolatile), writeln(Elaborate),
  unparse(Type).
unparse(type_void) :- !, write('void').
unparse(type_int) :- !, write('int').
unparse(type_char) :- !, write('char').
unparse(type_signed_char) :- !, write('char').
unparse(type_short) :- !, write('short').
unparse(type_long) :- !, write('long').
unparse(type_long_long) :- !, write('long long').
unparse(type_unsigned_char) :- !, write('unsigned char').
unparse(type_unsigned_int) :- !, write('unsigned int').
unparse(type_unsigned_short) :- !, write('unsigned short').
unparse(type_unsigned_long) :- !, write('unsigned long').
unparse(type_unsigned_long_long) :- !, write('unsigned long long').
unparse(type_bool) :- !, write('bool').
unparse(type_float) :- !, write('float').
unparse(type_double) :- !, write('double').
unparse(type_long_double) :- !, write('long double').
unparse(type_ellipse) :- !, write('...').

unparse(array_type(Type, Val)) :- !,
  unparse_type('', array_type(Type, Val)).
  %unparse(Type), write('['), unparse(Val), write(']').
unparse(typedef_type(Type, _)) :- !, write(Type).
unparse(class_type(Name,ClassType,_Scope)) :- !,
  unparse_class_type(ClassType), write(' '), write(Name).
unparse(enum_type(Type)) :- !, unparse(Type).

%unparse(pointer_type(array_type(T, V))) :- !, 
%  write('('), unparse(array_type(T, V)), write(')*').
unparse(pointer_type(T)) :- !, unparse(T), write('*').
unparse(function_type(T)) :- !, unparse(T), write('()').

% DEFINITIONS/DECLARATIONS

unparse(typedef_declaration(_, typedef_annotation(
          Name,
          pointer_type(Type),
          class_declaration(class_definition(Definitions, _, _), _, _)),
	  _Ai, _Fi)) :- !,
  write('typedef '), 
  with_output_to(atom(X),
		 (write('{'), unparse(Definitions), write('} *'), write(Name))),
  unparse_type(X, Type).

unparse(typedef_declaration(_, typedef_annotation(
          Name,
          Type,
          class_declaration(class_definition(Definitions, _, _), _, _)),
	  _Ai, _Fi)) :- !,
  write('typedef '), 
  with_output_to(atom(X),
		 (write('{'), unparse(Definitions), write('} '), write(Name))),
  unparse_type(X, Type).

unparse(typedef_declaration(_, typedef_annotation(Name,Type,_), _Ai, _Fi)) :- !,
  write('typedef '), unparse_type(Name, Type).

unparse(function_definition(BB, _, _, _)) :- !, unparse(BB).


% VARIABLES
% typedef structs
unparse(class_declaration(class_definition(Definitions, _, _, _),
			  class_declaration_annotation(Name, _,
               class_type(_InitializedName,ClassType, _Scope)), _, _)) :- !,
	unparse_class_type(ClassType), write(' '), write(Name),
	write(' {'),  unparse(Definitions), write('} ').
unparse(class_declaration(null,
			  class_declaration_annotation(Name, _,
	       class_type(_InitializedName,ClassType, _Scope)), _, _)) :- !,
	unparse_class_type(ClassType), write(' '), write(Name).

unparse(enum_declaration(InitializedNames,
	enum_declaration_annotation(Name, _, _), _, _)) :- !,
	write('enum '), write(Name),
	write(' {'),  unparse_enum(InitializedNames), write(' } ').


% on-the-fly structs
unparse(variable_declaration([ClassDeclaration|[InitializedName|INs]],
			     Spec, Ai, Fi)) :-
  ( Spec = variable_declaration_specific(Mod)
  ; Spec = variable_declaration_specific(Mod, _)),
  unparse_modifier(Mod),
  ClassDeclaration = class_declaration(_, _, _, _), !,
  InitializedName = initialized_name(_,
	initialized_name_annotation(_, Name, _, _), _, _),
  unparse(ClassDeclaration),
  write(Name), % eat the modifier
  unparse(variable_declaration(INs, variable_declaration_specific(null), Ai, Fi)).

unparse(variable_declaration([InitializedName|INs], Spec, Ai, Fi)) :- !,
  ( Spec = variable_declaration_specific(Mod)
  ; Spec = variable_declaration_specific(Mod, _)),
  unparse_modifier(Mod),
  unparse(InitializedName), !,
  unparse(variable_declaration(INs, variable_declaration_specific(null), Ai, Fi)).

unparse(initialized_name(Initializer,
			 initialized_name_annotation(Type, Name, _,_),_, _)) :-
  !,
  unparse_type(Name, Type),
  (Initializer = null -> true ; write(' = ')),
  unparse(Initializer).
unparse(variable_declaration([], _, _, _)) :- !.

unparse(aggregate_initializer(Es, _, _, _)) :- !,
  write('{'), unparse(Es), write(' }').
unparse(assign_initializer(E, _, _, _)) :- !,
  write(''), unparse(E).

unparse(pragma_declaration(pragma(pragma_annotation(Text),_, _), _, _, _)) :- !,
  write('#pragma '), write(Text).
unparse(null) :- !.

% ARTIFICIAL NODES

% This one is artificial and only used for the CFG
unparse(function_end(_, function_declaration_annotation(_, Name, _), _)) :- !, 
  write('END '), write(Name), write(';').

% artificial and only used by simplify
unparse(interval(Min, Max)) :- !, write(interval(Min, Max)).

% ERRORS

unparse(X) :- var(X), !, writeln('UNBOUND!'), trace.
unparse(X) :- write(X), trace, unparse(X).

unparse_sem(X) :-
  functor(X, basic_block, _), !,
  unparse(X).
unparse_sem(X) :-
  unparse(X), writeln(';').


% Helper functions for types (esp. arrays)
unparse_type(Name, Type) :-
  ( Type = array_type(_,_)
  ;(Type = pointer_type(array_type(Type,_)),
    Type \= array_type(_,_))
  ), !,
  inner_type(InnerType, Type),
  unparse(InnerType), write(' '), write(Name),
  unparse_array_type(Type).

% FIXME
unparse_type(Name, pointer_type(Type)) :-
  Type = array_type(array_type(_,_),_), !,
  inner_type(InnerType, Type),
  unparse(InnerType), write(' (*'), write(Name), write(')'),
  unparse_array_type(Type).

unparse_type(Name, Type) :- !,
  unparse(Type), write(' '), write(Name).

inner_type(DataType, array_type(Type, _)) :- !, inner_type(DataType, Type).
inner_type(DataType, pointer_type(array_type(Type,_))) :- !,
  inner_type(DataType, Type).
inner_type(DataType, DataType).

unparse_array_type(array_type(Type, Val1)) :- !,
  write('['), unparse(Val1), write(']'),
  unparse_array_type(Type).
unparse_array_type(pointer_type(array_type(Type,Val))) :-
  write('[]'),
  unparse_array_type(array_type(Type,Val)).
unparse_array_type(_).

unparse_class_type(0) :- write('class').
unparse_class_type(1) :- write('struct').
unparse_class_type(2) :- write('union').
unparse_class_type(3) :- write('<template parameter>'), trace.

unparse_enum([]) :- !.
unparse_enum([N|Ns]) :- !,
  N = initialized_name(Initializer,
		       initialized_name_annotation(_Type, Name, _, _), _, _),
  write(Name),
  (Initializer = null
  -> true
  ;  (write(' = '), unparse(Initializer))),
  (Ns = []
  -> true
  ; writeln(',')),
  unparse_enum(Ns).

% Put parentheses around E if necessary
unparse_par(E) :- unparse_par1(E), !.
unparse_par1(E) :-
  functor(E, function_call_exp, _),
  unparse(E).
unparse_par1(E) :-
  functor(E, pointer_deref_exp, _),
  write('('), unparse(E), write(')').
unparse_par1(E) :-
  functor(E, _, N), N < 4, !,
  unparse(E).
unparse_par1(E) :-
  write('('), unparse(E), write(')').

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
needs_semicolon(typedef_declaration(_,_,_,_)).
needs_semicolon(variable_declaration(_,_,_,_)).
needs_semicolon(class_declaration(_,_,_,_)).
needs_semicolon(enum_declaration(_,_,_,_)).

%% needs_comma(+Node) is det.
needs_comma(initialized_name(_,_,_,_)).

%% replace_types(+InitializedNames, +FuncDecl, -InitializedNames1) is det.
% replace the instatiated types with the original types from the function decl
replace_types([], function_type(_, _, []), []) :- !.
replace_types([I|Is],
	      function_type(ReturnT, NumParams, [Type|Ts]),
	      [J|Js]) :- !,
  I = initialized_name(A, initialized_name_annotation(_, B, C, D), Ai, Fi),
  J = initialized_name(A, initialized_name_annotation(Type, B, C, D), Ai, Fi),
  replace_types(Is, function_type(ReturnT, NumParams, Ts), Js).
replace_types(_,_,_) :- trace.

%% indent(+FileInfo) is det.
% Output indentation
indent(file_info(_,0,0)) :- !, write('    '). % dummy
indent(file_info(_Filename,_Line,Col)) :- tab(Col).
