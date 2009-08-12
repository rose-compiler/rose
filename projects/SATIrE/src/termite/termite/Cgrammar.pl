#!/usr/bin/pl -t testrun -f
% -*- mode: prolog; truncate-lines: true; -*-

/*:- module(ast_transform,
	  [simple_form_of/2,
           ast_node/6,
	   transformed_with/5,
	   transformed_with/6,
	   unparse_to_atom/2,
	   unparse/1,
	   unparse_storage_modifier/1,
	   indent/1,
	   needs_semicolon/1,
	   needs_comma/1,
	   replace_types/3
	   ]).
:- use_module(library(ast_properties)),
   use_module(library(utils)).
*/
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

unp(var(X)) --> [X].
unp(int(X)) --> [X].
unp(assign_op(E1, E2)) --> unp(E1), ['='], unp(E2).
%:- unparse(assign_op(var(x), int(42)), S, []).

%assign_expr(UI, C0, C, E) --> cond_expr(UI, C0, C, E).
%assign_expr(UI, C0, C, E) --> cond_expr(UI, C0, C, E).

% OPERATORS
op(UI, [_|C0], C, assign_op(E1, E2, _, _, _))       --> unary_exp(UI, C0, C1, E1), ['='],   rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, plus_assign_op(E1, E2, _, _, _))  --> unary_exp(UI, C0, C1, E1), ['+='],  rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, minus_assign_op(E1, E2, _, _, _)) --> unary_exp(UI, C0, C1, E1), ['-='],  rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, mult_assign_op(E1, E2, _, _, _))  --> unary_exp(UI, C0, C1, E1), ['*='],  rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, div_assign_op(E1, E2, _, _, _))   --> unary_exp(UI, C0, C1, E1), ['/='],  rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, lshift_assign_op(E1, E2, _, _, _))--> unary_exp(UI, C0, C1, E1), ['<<='], rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, rshift_assign_op(E1, E2, _, _, _))--> unary_exp(UI, C0, C1, E1), ['>>='], rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, and_assign_op(E1, E2, _, _, _))   --> unary_exp(UI, C0, C1, E1), ['&='],  rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, or_assign_op(E1, E2, _, _, _))    --> unary_exp(UI, C0, C1, E1), ['|='],  rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, ior_assign_op(E1, E2, _, _, _))   --> unary_exp(UI, C0, C1, E1), ['|='],  rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, xor_assign_op(E1, E2, _, _, _))   --> unary_exp(UI, C0, C1, E1), ['^='],  rexpr(UI, C1, C, E2).

op(UI, [_|C0], C, add_op(E1, E2, _, _, _))      --> unary_exp(UI, C0, C1, E1), ['+'],  rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, bit_and_op(E1, E2, _, _, _))  --> unary_exp(UI, C0, C1, E1), ['&'],  rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, and_op(E1, E2, _, _, _))      --> unary_exp(UI, C0, C1, E1), ['&&'], rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, bit_or_op(E1, E2, _, _, _))   --> unary_exp(UI, C0, C1, E1), ['|'],  rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, or_op(E1, E2, _, _, _))       --> unary_exp(UI, C0, C1, E1), ['||'], rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, bit_xor_op(E1, E2, _, _, _))  --> unary_exp(UI, C0, C1, E1), ['^'],  rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, mod_op(E1, E2, _, _, _))      --> unary_exp(UI, C0, C1, E1), ['%'],  rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, divide_op(E1, E2, _, _, _))   --> unary_exp(UI, C0, C1, E1), ['/'],  rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, lshift_op(E1, E2, _, _, _))   --> unary_exp(UI, C0, C1, E1), ['<<'], rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, rshift_op(E1, E2, _, _, _))   --> unary_exp(UI, C0, C1, E1), ['>>'], rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, subtract_op(E1, E2, _, _, _)) --> unary_exp(UI, C0, C1, E1), ['-'],  rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, multiply_op(E1, E2, _, _, _)) --> unary_exp(UI, C0, C1, E1), ['*'],  rexpr(UI, C1, C, E2).

op(UI, [_|C0], C, greater_or_equal_op(E1, E2, _, _, _)) --> unary_exp(UI, C0, C1, E1), ['>='], rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, greater_than_op(E1, E2, _, _, _))     --> unary_exp(UI, C0, C1, E1), ['>'],  rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, less_or_equal_op(E1, E2, _, _, _))    --> unary_exp(UI, C0, C1, E1), ['<='], rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, less_than_op(E1, E2, _, _, _))        --> unary_exp(UI, C0, C1, E1), ['<'],  rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, equality_op(E1, E2, _, _, _))         --> unary_exp(UI, C0, C1, E1), ['=='], rexpr(UI, C1, C, E2).
op(UI, [_|C0], C, not_equal_op(E1, E2, _, _, _))        --> unary_exp(UI, C0, C1, E1), ['!='], rexpr(UI, C1, C, E2).

op(UI, [_|C0], C, plus_plus_op(E1, _, _, _))     --> unary_exp(UI, C0, C, E1), ['++'].
op(UI, [_|C0], C, minus_minus_op(E1, _, _, _))   --> unary_exp(UI, C0, C, E1), ['--'].
op(UI, [_|C0], C, address_of_op(E, _, _, _))     --> ['&'], rexpr(UI, C0, C, E).
op(UI, [_|C0], C, minus_op(E, _, _, _))          --> ['-'], rexpr(UI, C0, C, E).
op(UI, [_|C0], C, not_op(E, _, _, _))            --> ['!'], rexpr(UI, C0, C, E).
op(UI, [_|C0], C, bit_complement_op(E, _, _, _)) --> ['~'], rexpr(UI, C0, C, E).

% VALUES
%rexpr(_UI, [_|C], C,          char_val(_,value_annotation(Value,_),_)) --> string_to_list(Value, [N]), [N], { string(Value) }.
%rexpr(_UI, [_|C], C, unsigned_char_val(_,value_annotation(Value,_),_)) --> string_to_list(Value, [N]), [N], { string(Value) }.

rexpr(_UI, [_|C], C, enum_val(_,value_annotation(_,Value,_),_,_)) --> [Value], { number(Value) }.
rexpr(_UI, [_|C], C, char_val(_,value_annotation(Value,_),_,_)) --> [Value], { number(Value) }.
rexpr(_UI, [_|C], C, int_val(_,value_annotation(Value,_),_,_)) --> [Value], { number(Value) }.
rexpr(_UI, [_|C], C, short_val(_,value_annotation(Value,_),_,_)) --> [Value], { number(Value) }.
rexpr(_UI, [_|C], C, short_int_val(_,value_annotation(Value,_),_,_)) --> [Value], { number(Value) }.
rexpr(_UI, [_|C], C, long_int_val(_,value_annotation(Value,_),_,_)) --> [Value], { number(Value) }.
rexpr(_UI, [_|C], C, long_long_int_val(_,value_annotation(Value,_),_,_)) --> [Value], { number(Value) }.
rexpr(_UI, [_|C], C, unsigned_char_val(_,value_annotation(Value,_),_,_)) --> [Value], { number(Value) }.
rexpr(_UI, [_|C], C, unsigned_int_val(_,value_annotation(Value,_),_,_)) --> [Value], { number(Value) }.
rexpr(_UI, [_|C], C, unsigned_short_int_val(_,value_annotation(Value,_),_,_)) --> [Value], { number(Value) }.
rexpr(_UI, [_|C], C, unsigned_short_val(_,value_annotation(Value,_),_,_)) --> [Value], { number(Value) }.
rexpr(_UI, [_|C], C, unsigned_long_val(_,value_annotation(Value,_),_,_)) --> [Value], { number(Value) }.
rexpr(_UI, [_|C], C, unsigned_long_long_val(_,value_annotation(Value,_),_,_)) --> [Value], { number(Value) }.
rexpr(_UI, [_|C], C, unsigned_long_long_int_val(_,value_annotation(Value,_),_,_)) --> [Value], { number(Value) }.
rexpr(_UI, [_|C], C, float_val(_,value_annotation(Value,_),_,_)) --> [Value], { number(Value) }.
rexpr(_UI, [_|C], C, double_val(_,value_annotation(Value,_),_,_)) --> [Value], { number(Value) }.
rexpr(_UI, [_,_,_|C], C, string_val(_,value_annotation(Value,_),_,_)) --> ['"', Value, '"'], { string(Value) }.
rexpr(_UI, [_|C], C, bool_val_exp(_, value_annotation(0,_),_,_)) --> ['false'].
rexpr(_UI, [_|C], C, bool_val_exp(_, value_annotation(1,_),_,_)) --> ['true'].
rexpr(_UI, [_|C], C, bool_val_exp(_, value_annotation(Value,_),_, _)) --> [Value], { number(Value) }.
rexpr(UI, C0, C, E) --> expr(UI, C0, C, E).

% VARIABLES
expr(_UI, [_|C], C, var_ref_exp(var_ref_exp_annotation(_Type, Name, _, _, _), _, _)) --> [Name], { atom(Name) }.
expr(UI, [_,_|C0], C, pntr_arr_ref_exp(Base, Offset, _, _, _)) --> unary_exp(UI, C0, C1, Base), ['['], rexpr(UI, C1, C, unparse_par(UI, Offset)), [']'].
expr(UI, C0, C, E) --> op(UI, C0, C, E).

unary_expr(UI, C0, C, E) --> postfix_expr(UI, C0, C, E).
unary_expr(UI, C0, C, E) --> postfix_expr(UI, C0, C, E).
unary_expr(UI, [_|C0], C, plus_plus_op(E1, _, _, _))   --> ['++'], unary_exp(UI, C0, C, E1).
unary_expr(UI, [_|C0], C, minus_minus_op(E1, _, _, _)) --> ['--'], unary_exp(UI, C0, C, E1).
% cast
unary_expr(UI, [_,_,_|C0], C, size_of_op(null, size_of_op_annotation(ClassType,_Type, _), _)) --> ['sizeof','('], rexpr(UI, C0, C, ClassType), [')'].



% STATEMENTS

% STATEMENTS       
statement(UI, C0, C, expr_statement(E, _, _, _)) --> expr(UI, C0, C, E).
statement(UI, C0, C, expression_root(E, _, _, _)) --> expr(UI, C0, C, E).
statement(UI, C0, C, for_init_statement(E, _, _, _)) --> expr(UI, C0, C, E).
statement(_UI, [_|C], C, for_init_statement(_, _, _)) --> [].

%:- length(Xs, N), Xs = [x, '=', 0], phrase(statement(UI, Stmt, Xs, []), Xs).
%?- ['Cgrammar'], Xs = [x, '=', 0], gtrace, phrase(statement(UI,  Xs, [], Stmt), Xs).