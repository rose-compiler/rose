#!/usr/bin/pl -q  -O  -L64M -G64M -t main -s 
% -*- prolog -*-

%-----------------------------------------------------------------------
% validator.pl
%
% This program performs a source-to-source transformation to convert
% analysis results into assertions thus allowing for a dynamic
% validation of the analysis.
%
% Authors
% -------
%
% Copyright (C) 2009, Adrian Prantl <adrian@complang.tuwien.ac.at>
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
   use_module(library(callgraph)),
   use_module(library(utils)).

%-----------------------------------------------------------------------

% generate an expression assert(v > Lower && v < Upper)
counter_assert(C, N, FI, Assertion) :-
  default_values(PPI, DA, AI, _FI), 
  VarType = type_unsigned_int,
  Assertion =  expr_statement(AssertionExpr, DA, AI, FI),
  AssertionExpr =
    function_call_exp(FRefExp, expr_list_exp([LEop], DA, AI, FI),
		      function_call_exp_annotation(type_void,PPI), AI, FI),
  FRefExp = function_ref_exp(function_ref_exp_annotation(assert, Ftp, PPI),
			     AI, FI),
  Ftp = function_type(type_void,ellipses,[type_int]),

  MaxVal = unsigned_int_val(null, value_annotation(N, PPI), AI, FI),
  LEop = less_or_equal_op(VarRef, MaxVal, TypeAn, AI, FI),

  VarRef = var_ref_exp(var_ref_exp_annotation(VarType, C, default, PPI),AI, FI),
  TypeAn = binary_op_annotation(VarType, PPI).


counter_decl(C, FI, variable_declaration([
    initialized_name(assign_initializer(
        unsigned_int_val(null, value_annotation(0, PPI), AI, FI),
      assign_initializer_annotation(type_unsigned_int, PPI), AI, FI),
      initialized_name_annotation(type_unsigned_int, C, default, null), AI, FI)
				     ],
      variable_declaration_specific(
          declaration_modifier([], type_modifier([default], default, default,
						 default), public, auto),
				    null, PPI),
				     AI, FI)) :-
  default_values(PPI, _DA, AI, _FI).

counter_inc(C, FI, expr_statement(plus_plus_op(
    var_ref_exp(var_ref_exp_annotation(type_unsigned_int, C, default,
				       null, PPI), AI, FI),
			    unary_op_annotation(prefix, type_unsigned_int,
						null, null, PPI), AI, FI),
				  DA, AI, FI)) :-
    default_values(PPI, DA, AI, _FI).

assertions(y, y, y, Statement, AssertedStatement) :-
  Statement = for_statement(Init, Test, Step,
			    basic_block(Stmts, DA1, AI1, FI1), DA, AI, FI),
  get_annot(Stmts, wcet_trusted_loopbound(N), _),

  C = xxyyzzyy,
  counter_decl(C, FI1, CounterDecl),
  counter_inc(C, FI1, Count),
  counter_assert(C, N, FI1, CounterAssert),
  
  AssertedStatement = basic_block([CounterDecl,
		       for_statement(Init, Test, Step,
				     basic_block([Count|Stmts], DA1, AI1, FI1),
				     DA, AI, FI),
		       CounterAssert], DA, AI, FI).

assertions(y, y, y, Statement, AssertedStatement) :-
  Statement = while_stmt(Test, basic_block(Stmts, DA1, AI1, FI1), DA, AI, FI),
  get_annot(Stmts, wcet_trusted_loopbound(N), _),

  C = xxyyzzyy,
  counter_decl(C, FI1, CounterDecl),
  counter_inc(C, FI1, Count),
  counter_assert(C, N, FI1, CounterAssert),
  
  AssertedStatement = basic_block([CounterDecl,
		       while_stmt(Test,
				  basic_block([Count|Stmts], DA1, AI1, FI1),
				  DA, AI, FI),
				   CounterAssert], DA, AI, FI).


assertions(y, y, y, global(Decls, An, Ai, Fi),
	  global([Assert|Decls1], An, Ai, Fi)) :-
  default_values(PPI, DA, AI, _),
  Assert = 
  function_declaration(
   function_parameter_list(
    [initialized_name(null,
		      initialized_name_annotation(type_int, '', default, null),
		      analysis_info([]), AFI)], DA, AI, AFI),
		       null,
		       function_declaration_annotation(
		         function_type(type_void,
				       ellipses, [type_int]),
                      assert,
		declaration_modifier([default],
				     type_modifier([default],
						   default, default, default),
				     public, extern),
	       PPI), analysis_info([]), AFI),

  AFI = file_info('/usr/include/assert.h', 1, 1),
  Include = cpreprocessorIncludeDeclaration('#include <assert.h>\n', before,Fi),
  Decls = [D|Ds],
  D =.. D_u,     append(Dhead, [Dan, Dai, Dfi], D_u),
  Dan =.. Dan_u, append(Dan_head, [preprocessing_info(PPIs)], Dan_u),

  append(Dan_head, [preprocessing_info([Include|PPIs])], Dan_u1),
  Dan1 =.. Dan_u1,
  append(Dhead, [Dan1, Dai, Dfi], D_u1),
  D1 =.. D_u1, 
  Decls1 = [D1|Ds].

assertions(I, I, I, Term, Term).

%% get_annot(+Stmts, -Annotterm, -Pragma) is nondet.
get_annot(Stmts, AnnotTerm, Pragma) :-
  member(Pragma, Stmts),
  pragma_text(Pragma, Text),
  (atom(Text)
  -> atom_to_term(Text, AnnotTerm, _)
  ;  AnnotTerm = Text).


%-----------------------------------------------------------------------  

main :-
  catch((
	 prompt(_,''),
         % Read input
	 read_term(Input, []),
	 compound(Input),

         % Generate the assertions...
	 transformed_with(Input, assertions, postorder, y, _, Output),
   
         % Write output
	 write_term(Output, [quoted(true)]),
	 writeln('.')
	), E, (print_message(error, E), fail))
.