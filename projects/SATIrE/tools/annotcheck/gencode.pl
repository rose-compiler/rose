#!/usr/bin/pl -q -t main -f
% -*- mode: prolog; truncate-lines: true; -*-


%-----------------------------------------------------------------------
% gencode.pl
%
% About
% -----
%
% Generate Error-checking C-code for an annotated C++ file
%
% Authors
% -------
%
% Copyright 2007,2010 Adrian Prantl
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

:- getenv('PWD', CurDir),
   asserta(library_directory(CurDir)),
   prolog_load_context(directory, SrcDir),
   asserta(library_directory(SrcDir)),
   (getenv('TERMITE_LIB', TermitePath)
   ; (print_message(error, 'Please set the environment variable TERMITE_LIB'),
      halt(1))
   ),
   asserta(library_directory(TermitePath)).

:- use_module(library(asttransform)),
   use_module(library(astproperties)),
   use_module(library(clpfd)),
   use_module(library(utils)).

:- op(550, yfx, &&).

%% Convert between #pragma decls an their Terms
annotation_term(PragmaDecl, AnnotTerm) :-
  pragma_text(PragmaDecl, Text),
  (atom(Text)
  -> atom_to_term(Text, AnnotTerm, _)
  ;  AnnotTerm = Text).

% code_visit/5:
% Our all-in-one postorder traversal
%

% Loop bound code generation
code_visit(Info, _, InfoT, Loop, LoopT) :-
  (   Loop = for_statement(_, _, _, Bb, _, _, _)
  ;   Loop = while_stmt(_, Bb, _, _, _)
  ;   Loop = do_while_stmt(Bb, _, _, _, _)
  ), 
  default_values(_PPI, An, Ai, Fi),

  % Initialize new counter variables before entering a loop
  get_loop_marker(Bb, LoopMarker),
  intz_decl(LoopMarker, CounterDecl),

  % Generate an assertion right after the loop
  Bb = basic_block(Stmts, _, _, _),
  get_annot(Stmts, wcet_loopbound(Lo..Hi), _),

  prolog_to_cxx((LoopMarker =< Hi) && (LoopMarker >= Lo), CxxExpr),
  assert_expr(CxxExpr, Assert),

  % Create the replacement loop
  Loop1 = basic_block([CounterDecl,Loop,Assert], An, Ai, Fi),
  
  % Also initialize implicit markers
  code_visit(Info, _, InfoT, Loop1, LoopT).

code_visit(_, _, _, basic_block(Stmts, An, Ai, Fi), basic_block(StmtsT, An, Ai, Fi)) :-
  % Handle Explicit Pragma Annotations
  Context = basic_block(Stmts, An, Ai, Fi),
  maplist(code_for(Context), Stmts, Stmts1),
  flatten(Stmts1, Stmts2),
  % Prepare LoopBounds
  % for each while/do/for && loopbound
  maplist(get_marker, Stmts2, LMs), flatten(LMs, Markers),
  maplist(intz_decl, Markers, MarkerDecls),

  % Also, Create a static counter variable for each implicit marker
  get_implicit_marker(Context, ImplicitMarker),
  static_intz_decl(ImplicitMarker, IMDecl),
  plusplus_expr(ImplicitMarker, IMIncr),
  append([[IMDecl, IMIncr], MarkerDecls, Stmts2], StmtsT).

% Insert missing scopes
code_visit(Info, _InfoInner, _InfoPost,
	   function_definition(basic_block(Stmts, BAn, BAi, BFi), An, Ai, Fi),
	   function_definition(basic_block(NewStmts, BAn, BAi, BFi), An, Ai, Fi)) :-
  %gtrace, % todo: markers-scopes
  Info = info(markers(_Ms), scopes(Ss)),
  maplist(wrap_wcet_scope(Ss), Ss, WSs),
  maplist(code_for_term(_), WSs, Scopes),
  append(Scopes, Stmts, NewStmts).

% Harvest all markers that are used in constraint terms
code_visit(Info, Info, Info1, PragmaDecl, PragmaDecl) :-
  %trace,
  annotation_term(PragmaDecl, wcet_constraint(Term)), 
  harvest_markers(Term, Markers),

  Info = info(markers(Ms), Scopes), 
  append(Ms, Markers, Ms1),
  Info1= info(markers(Ms1), Scopes).

% Collect all scopes that were declared by the user
code_visit(Info, Info, Info1, PragmaDecl, PragmaDecl) :-
  %trace,
  annotation_term(PragmaDecl, wcet_scope(Marker)),
  Info = info(Ms, scopes(Ss)),
  Info1= info(Ms, scopes([Marker|Ss])).

code_visit(Info, Info, Info, Node, Node).

wrap_wcet_scope(X, wcet_scope(X)).

% Collect all Markers(=leaves) from a term
harvest_markers(Marker, [Marker]) :-
  atom(Marker).
harvest_markers(Term, Markers) :-
  un_op(Term, _, T1),
  harvest_markers(T1, Markers).
harvest_markers(Term, Markers) :-
  bin_op(Term, _, T1, T2),
  harvest_markers(T1, Markers1),
  harvest_markers(T2, Markers2),
  append(Markers1, Markers2, Markers).

% helper for code_visit/5
code_for(Context, PragmaDecl, [PragmaDecl|Code]) :-
  annotation_term(PragmaDecl, Term), 
  code_for_term(Context, Term, Code).
code_for(_, Stmt, Stmt).

% Some conventions for code generation:
%
% *) Markers are translated into counter variables.
% *) Markers are initialized at the entry of their respective Scope.
% *) The default Scope for any Marker is the outermost (C-)scope of the
%    current function.
% *) Loopbounds are translates into a Constraint and a Marker with the
%    parent (C-)scope as Scope.
% *) A Constraint is treated as an invariant. Therefore, the assertions
%    are placed exactly where the constraint pragma was located.
% X) X The assertion for a Constraint will be located at the end of
%    X scope S_i, where S_i is the innermost Scope of all Markers that are
%    X referenced by the Constraint.

% SCOPE: int M = 0;
code_for_term(_, wcet_scope(M), [MarkerDecl]) :-
  intz_decl(M, MarkerDecl),
  !.

% MARKER: M++;
code_for_term(_, wcet_marker(M), [MarkerIncr]) :-
  plusplus_expr(M, MarkerIncr),
  !.

% LOOPMARKER++;
code_for_term(Context, wcet_loopbound(_), [MarkerIncr]) :-
  get_loop_marker(Context, M),
  plusplus_expr(M, MarkerIncr),
  !.


% CONSTRAINT: assert(R);
% fehlende scopes generieren
% assert muﬂ an das Ende des scopes geschoben werden
code_for_term(_, wcet_constraint(PrologExpr), [Assert]) :-
  prolog_to_cxx(PrologExpr, CxxExpr),
  assert_expr(CxxExpr, Assert),
  !.

code_for_term(_, []).

% Extract the marker from a list of statements
get_marker(Stmts, Marker) :-
  get_annot(Stmts, wcet_marker(Marker), _), !.
get_marker(_, []).

% Generate a unique name for a loop marker
get_loop_marker(Bb, LoopMarker) :-
  get_implicit_marker(Bb, Marker),
  concat_atom(['loop_', Marker], LoopMarker).


% Induce the name of the loop marker from a loop statement
%get_loop_marker(     for_statement(_, _, _, Bb, _, _, _), LoopMarker) :- get_loop_marker1(Bb, LoopMarker).
%get_loop_marker(   while_statement(_, _, _, Bb, _, _, _), LoopMarker) :- get_loop_marker1(Bb, LoopMarker).
%get_loop_marker(do_while_statement(_, _, _, Bb, _, _, _), LoopMarker) :- get_loop_marker1(Bb, LoopMarker).
%get_loop_marker(_, []).

get_implicit_marker(basic_block(Stmts, _, _, _), LoopMarker) :-
  member(Stmt, Stmts),
  \+ pragma_text(Stmt, wcet_loopbound(_)),
  get_implicit_marker(Stmt, LoopMarker).

get_implicit_marker(Stmt, LoopMarker) :-
  analysis_info(Stmt, analysis_info(Ai)),
  member(entry_exit_labels(Id-_), Ai),
  atom_concat(label, Id, LoopMarker).
%  get_marker(Stmts, Marker),
%  string_concat('loop_', Marker, LoopMarker).

% Some C++ AST templates

% Alternative.. to implement sometime later
%static_intz_decl(VarName, verbatim_text(Text)) :-
%  foldl1(['static int ', VarName, ';\n'] , string_concat).
%plusplus_expr(VarName, verbatim_text(Text)) :-
%  foldl1([VarName, '++;\n'] , string_concat).

% unsigned int VarName = 0;
intz_decl(VarName, variable_declaration([
    initialized_name(assign_initializer(
        unsigned_int_val(value_annotation(0, PPI), AI, FI),
      assign_initializer_annotation(type_unsigned_int, PPI), AI, FI),
      initialized_name_annotation(type_unsigned_int, VarName, default, null), AI, FI)
				     ],
      variable_declaration_specific(
          declaration_modifier([], type_modifier([default], default, default,
						 default), public, default),
				    null, PPI),
				     AI, FI)) :-
  default_values(PPI, _DA, AI, FI).

% static int VarName = 0;
static_intz_decl(VarName, variable_declaration([
    initialized_name(assign_initializer(
        unsigned_int_val(value_annotation(0, PPI), AI, FI),
      assign_initializer_annotation(type_unsigned_int, PPI), AI, FI),
      initialized_name_annotation(type_unsigned_int, VarName, default, null), AI, FI)
				     ],
      variable_declaration_specific(
          declaration_modifier([], type_modifier([public_access], default, default,
						 default), public, static),
				    null, PPI),
				     AI, FI)) :-
  default_values(PPI, _DA, AI, FI).

% VarName++;
plusplus_expr(VarName, expr_statement(plus_plus_op(
    var_ref_exp(var_ref_exp_annotation(type_unsigned_int, VarName, default,
				       null, PPI), AI, FI),
			    unary_op_annotation(prefix, type_unsigned_int,
						null, null, PPI), AI, FI),
				  DA, AI, FI)) :-
    default_values(PPI, DA, AI, FI).

% assert(Expr);
assert_expr(Expr, Assertion) :-
  default_values(PPI, DA, AI, FI), 
  Assertion =  expr_statement(AssertionExpr, DA, AI, FI),
  AssertionExpr =
    function_call_exp(FRefExp, expr_list_exp([Expr], DA, AI, FI),
		      function_call_exp_annotation(type_void,PPI), AI, FI),
  FRefExp = function_ref_exp(function_ref_exp_annotation(assert, Ftp, PPI),
			     AI, FI),
  Ftp = function_type(type_void,ellipses,[type_int]).

%code_for_term(wcet_constraint(), ).

% build_expr(PrologExpr, CxxExpr)
build_expr(IntVal,
	      int_val(value_annotation(IntVal, PPI), AI, FI)) :-
  number(IntVal),
  default_values(PPI, _DA, AI, FI).

var_ref_exp(Name, unsigned_int,
	    var_ref_exp(var_ref_exp_annotation(VarType, Name, default, null, PPI), AI, FI)) :-
  VarType = type_unsigned_int,
  atom(Name),
  default_values(PPI, _DA, AI, FI).

prolog_to_cxx(PExp1 =< PExp2, BinOp) :-
  cxx_binop(PExp1, PExp2, less_or_equal_op, BinOp).


build_expr(PExp1 =< PExp2, BinOp) :-
  build_binop(PExp1, PExp2, less_or_equal_op, BinOp).

build_expr(PExp1 >= PExp2, BinOp) :-
  build_binop(PExp1, PExp2, greater_or_equal_op, BinOp).

build_expr(PExp1 < PExp2, BinOp) :-
  build_binop(PExp1, PExp2, less_than_op, BinOp).

build_expr(PExp1 > PExp2, BinOp) :-
  build_binop(PExp1, PExp2, greater_than_op, BinOp).

build_expr(PExp1 && PExp2, BinOp) :-
  build_binop(PExp1, PExp2, and_op, BinOp).

build_expr(PExp1 + PExp2, BinOp) :-
  build_binop(PExp1, PExp2, add_op, BinOp).

build_expr(PExp1 - PExp2, BinOp) :-
  build_binop(PExp1, PExp2, subtract_op, BinOp).

build_expr(PExp1 * PExp2, BinOp) :-
  build_binop(PExp1, PExp2, multiply_op, BinOp).

build_expr(PExp1 / PExp2, BinOp) :-
  build_binop(PExp1, PExp2, divide_op, BinOp).


build_expr(_, _) :- trace.

build_binop(PExp1, PExp2, Op, BinOp) :-
  VarType = type_unsigned_int,
  TypeAn = binary_op_annotation(VarType, PPI),
  BinOp =.. [Op, CExp1, CExp2, TypeAn, AI, FI],
  default_values(PPI, _DA, AI, FI),
  build_expr(PExp1, CExp1),
  build_expr(PExp2, CExp2).


%-----------------------------------------------------------------------
% MAIN
%-----------------------------------------------------------------------

main :-
  prompt(_,''),
  % Read input
  read_term(P, []),
  compound(P),

  StartInfo = info(markers([]), scopes([])),

  (transformed_with(P, code_visit, postorder, StartInfo, Info, P_withcode) -> true;
   writeln('** ERROR: transformed_with/6 failed'), fail),

  Info = info(markers(Markers), scopes(Scopes)),

  write_term(P_withcode, [quoted(true),double_quotes(string)] ),
  write('.\n'),

  halt.

main :-
  writeln('Usage: gencode.pl Input Output'),
  halt(1).