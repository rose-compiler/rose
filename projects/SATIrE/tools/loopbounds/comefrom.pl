#!/usr/bin/pl -t main -s 
% -*- prolog -*-

%-----------------------------------------------------------------------
% comefrom.pl
%
% About
% -----
%
% This source-to-source transformer performs a de-goto-fication
% wherever possible, by recognizing several patterns that can be
% transformed into loops
%
% Authors
% -------
%
% Copyright (C) 2009, Adrian Prantl
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

:- module(comefrom, [come_from/5]).

:- use_module(library(ast_transform)),
   use_module(library(ast_properties)).

come_from(I, I, I, basic_block(Stmts, Annot, Ai, Fi), Bb) :-
  % decompose the old basic block
  append([Prefix,
	  [label_statement(label_annotation(Label, L1), L2, L3)],
	  LoopBody,
	  [if_stmt(Condition,
		   goto_statement(label_annotation(Label, _), _, _),
		   null,
		   IfAn, _IfAi, IfFi)],
	  Suffix], Stmts),

  % Transform to a DO-WHILE-Statement
  DA = default_annotation(null, preprocessing_info([])),
  DAi = analysis_info([]),
  DoWhileStmt = do_while_stmt(basic_block(LoopBody, DA, DAi, IfFi),
			      Condition, 
			      IfAn, DAi, IfFi),

  % construct the new basic block
  append([Prefix,
	  [label_statement(label_annotation(Label, L1), L2, L3),
	  DoWhileStmt],
	  Suffix], Stmts1),  
  Bb = basic_block(Stmts1, Annot, Ai, Fi).


come_from(I, I, I, S, S).

%-----------------------------------------------------------------------
% MAIN
%-----------------------------------------------------------------------

main :-
  open('input.pl',read,_,[alias(rstrm)]),
  read_term(rstrm,X,[double_quotes(string)]),
  close(rstrm),
  
  transformed_with(X, come_from, [], _, Y),
  
  open('output.pl',write,_,[alias(wstrm)]),
  write_term(wstrm,Y,[quoted(true),double_quotes(string)]),
  close(wstrm).
