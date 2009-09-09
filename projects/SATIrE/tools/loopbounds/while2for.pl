#!/usr/bin/pl -t annot -f
% -*- prolog -*-

%-----------------------------------------------------------------------
% while2for.pl
%
% About
% -----
%
% Convert while()-loops into for()-loops, so they are recognized
% by the ROSE LoopProcessor.
%
% Authors
% -------
%
% Copyright (C) 2007-2008 Adrian Prantl
%
% License
% -------
%
% This program is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; version 3 of the License.
% 
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
%-----------------------------------------------------------------------

:- module(while2for, [while_to_for/5]).

:- use_module(library(asttransform)),
   use_module(library(astproperties)).

is_simple_update(expr_statement(Op, _, _, _), Var) :-
  ( Op = plus_plus_op(V, _, _, _)
  ; Op = assign_op(V, _, _, _, _)
  ; Op = plus_assign_op(V, _, _, _, _)
  ; Op = minus_assign_op(V, _, _, _, _)
  ; Op = rshift_assign_op(V, _, _, _, _)
  ),
  var_stripped(V, Var).

contains_safe_increment(basic_block(Statements, Annot, Ai, Fi),
			basic_block(ForBody, Annot, Ai, Fi),
			Var, Increment) :-
  select(Increment, Statements, ForBody),
  is_simple_update(Increment, Var),
  guarantee(Statements, is_transp(Var, local)).

while_to_for(_Info, _InfoInner, _InfoPost, WhileStmt, ForStmt) :-
  % Transform the WHILE-Statement
  isWhileStatement(WhileStmt, Condition, V, Body, Annot, Ai, Fi),
  var_stripped(V, Var),
  %unparse(WhileStmt),
  %nl, nl,
  %write('Condition: '), unparse(Condition), nl,
  %write('Var: '), unparse(Var), nl,
  
  % Find and remove Increment from Body
  contains_safe_increment(Body, NewBody, Var, Increment),
  %isSimpleForInit(ForInit, AssignOp),
  %writeln('is simple!'), nl,nl,

  % Construct FOR-Statement
  ForInit = for_init_statement(Annot, Ai, Fi),
  ForTest = Condition,
  expr_statement(ForStep, _, _, _) = Increment,
  ForStmt = for_statement(ForInit, ForTest, ForStep, NewBody, Annot, Ai, Fi).
  %unparse(ForStmt).

while_to_for(_Info, _InfoInner, _InfoPost, DoWhileStmt, UnrolledFor) :-
  % Transform the WHILE-Statement
  isDoWhileStatement(DoWhileStmt, Condition, V, Body, Annot, Ai, Fi),
  var_stripped(V, Var),
  % Find and remove Increment from Body
  contains_safe_increment(Body, NewBody, Var, Increment),
  %isSimpleForInit(ForInit, AssignOp),
  %writeln('is simple!'), nl,nl,

  % Construct FOR-Statement
  ForInit = for_init_statement(Annot,Ai,Fi),
  ForTest = Condition,
  expr_statement(ForStep, _, _, _) = Increment,
  ForStmt = for_statement(ForInit, ForTest, ForStep, NewBody, Annot, Ai, Fi),

  % Unroll the first iteration
  append(Body, [ForStmt], UnrolledFor).


while_to_for(I, I, I, S, S).

%-----------------------------------------------------------------------
% MAIN
%-----------------------------------------------------------------------

main :-
  open('input.pl',read,_,[alias(rstrm)]),
  read_term(rstrm,X,[double_quotes(string)]),
  close(rstrm),
  
  transformed_with(X, while_to_for, [], _, Y),
  
  open('output.pl',write,_,[alias(wstrm)]),
  write_term(wstrm,Y,[quoted(true),double_quotes(string)]),
  close(wstrm).
