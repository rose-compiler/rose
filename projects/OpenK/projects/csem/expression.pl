:- module(expression, [
	inStatement/2
]).

:- use_module(basic).
:- use_module(statement).

constant(Constant) :-
	c_is_a(Constant, 'Value').

inStatement(Expr, Stmt) :-
	isStatement(Expr), Stmt = Expr, !.

inStatement(Expr, Stmt) :-
	hasAncestor(Expr, Stmt),
	isStatement(Stmt), !.



