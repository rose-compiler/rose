#!/usr/bin/pl -q  -O  -L64M -G64M -t main -f 
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

:- prolog_load_context(directory, CurDir),
   asserta(library_directory(CurDir)),
   (getenv('TERMITE_LIB', TermitePath)
   ; (print_message(error, 'Please set then environment variable $TERMITE_LIB'),
      halt(1))
   ),
   asserta(library_directory(TermitePath)).

:- use_module(library(ast_transform)),
   use_module(library(ast_properties)),
   use_module(library(callgraph)),
   use_module(library(utils)).

%-----------------------------------------------------------------------

default_values(PPI, DA, AI, FI) :-
  PPI = preprocessing_info(null),
  DA = default_annotation(null, PPI),
  AI = analysis_info([]),
  FI = file_info('compilerGenerated', 0, 0).

% don't compare against top
compval(top, V, V, _, _, _) :- !.
compval(Val, _, int_val(null, value_annotation(Val, PPI), AI, FI), PPI, AI, FI).

% generate an expression assert(v > Lower && v < Upper)

interval_assert(Stmt, Var->Intvl, AssertionExpr) :-
  % handle the case where the variable is declared inside a scope stmt
  (member(Op, [for_statement
	      %, while_stmt, do_while_stmt, if_stmt, switch_statement
	      ]),
   Stmt =.. [Op|[VarDecl|_]],
   VarDecl = for_init_statement(Decls, _, _, _),
   member(variable_declaration(INs, _, _, _), Decls),
   member(initialized_name(_, initialized_name_annotation(_,Var,_,_),_,_), INs))
  -> AssertionExpr = []
  ; interval_assert1(Var->Intvl, AssertionExpr).


interval_assert1(Var->[Min,Max], AssertionExpr) :-
  default_values(PPI, DA, AI, FI),
  VarType = type_int, %null,
  AssertionExpr =
    function_call_exp(FRefExp, expr_list_exp([AndOp], DA, AI, FI),
		      function_call_exp_annotation(type_void,PPI), AI, FI),
  FRefExp = function_ref_exp(function_ref_exp_annotation(assert, Ftp, PPI),
			     AI, FI),
  Ftp = function_type(type_void,ellipses,[VarType]),

  % skip temp vars
  ( atom_concat('$', _, Var)
  -> AndOp = int_val(null, value_annotation(1, PPI), AI, FI)
  ;  AndOp = and_op(GE, LE, binary_op_annotation(type_int, PPI), AI, FI)
  ),

  % ignore comparison with top
  compval(Min, VarRef, MinVal, PPI, AI, FI),
  compval(Max, VarRef, MaxVal, PPI, AI, FI),
  MaxVal = int_val(null, value_annotation(Max, PPI), AI, FI),
  GE = greater_or_equal_op(VarRef, MinVal, TypeAn, AI, FI),
  LE =    less_or_equal_op(VarRef, MaxVal, TypeAn, AI, FI),

  VarRef = var_ref_exp(var_ref_exp_annotation(VarType, Var, default, PPI),
		       AI, FI),
  TypeAn = binary_op_annotation(VarType, PPI).

% generate a comma-seperated chain of assertions
comma_chain(Expr1, Expr2, comma_op_exp(Expr1, Expr2, BA, AI, FI)) :-
  BA = binary_op_annotation(null, PPI),
  default_values(PPI, _, AI, FI).

% generate a comma chain or leave empty
chain_up(Assertions, DA, AI, FI,  Statement) :-
  flatten(Assertions, As),
  ( As = []
  -> Statement = []
  ; (foldl1(As, comma_chain, PreExp),
     Statement = expr_statement(PreExp, DA, AI, FI))
  ).
  
% don't generate assertions for branch statements
assertions(_, y, y, Bb, Bb) :- functor(Bb, basic_block, _).
assertions(_, n, n, Branch, Branch) :-
  functor(Branch, if_stmt, _)
%  ; functor(Branch, for_statement, _)
  ; functor(Branch, while_stmt, _)
  ; functor(Branch, do_while_stmt, _)
  ; functor(Branch, switch_statement, _).

assertions(y, y, y, Statement, AssertedStatement) :-
  functor(Statement, F, Arity),
  ( StmtSuffix = stmt
  ; StmtSuffix = statement
  ),
  atom_concat(_Name, StmtSuffix, F),
  N1 is Arity-1, arg(N1, Statement, AI),
  N0 is Arity-0, arg(N0, Statement, FI),

  DA = default_annotation(null, preprocessing_info(null)),
  AI = analysis_info(AIs),
  member(pre_info(PreInfo), AIs),
  member(post_info(PostInfo), AIs),
  member(merged:map([top,top],PreVars), PreInfo),
  member(merged:map([top,top],PostVars), PostInfo),

  maplist(interval_assert(Statement), PreVars, PreAssertions),
  maplist(interval_assert(Statement), PostVars, PostAssertions),

  chain_up(PreAssertions, DA, AI, FI, PreStmt),
  chain_up(PostAssertions, DA, AI, FI, PostStmt),
  
  flatten([PreStmt, Statement, PostStmt], AssertedStatement),
  !.

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
%-----------------------------------------------------------------------  

prettyprint(Func/Type) :- unparse(Type), format(' ~w; ', [Func]).

maingen(t(FuncName/FuncType,Closure,Unreachable), [], [],
	global(Decls, An, Ai, FI),
	global(Decls1, An, Ai, FI)) :-

  default_values(PPI, DA, AI, _),
  with_output_to(atom(Covd), maplist(prettyprint, [FuncName/FuncType|Closure])),
  with_output_to(atom(NotCovd), maplist(prettyprint, Unreachable)),
  atom_concat('// Test coverage: ', Covd, Comment1),
  atom_concat('// NOT covered: ', NotCovd, Comment2),

  Main = function_declaration(
     function_parameter_list([], DA, AI, FI),
     function_definition(basic_block(BB, DA, AI, FI),
			 DA, AI, FI),
     function_declaration_annotation(function_type(type_int, ellipses, []),
				     main,
	declaration_modifier([default], type_modifier([default],
			      default, default, default), public, unspecified),
	preprocessing_info([cplusplusStyleComment(Comment1, before, FI),
			    cplusplusStyleComment(Comment2, before, FI)])),
     AI, FI),

  % no functions to call?
  (FuncName = 0
  -> BB = [Return] ;
  BB = [expr_statement(
            function_call_exp(function_ref_exp(FREA, AI, FI),
			      expr_list_exp([], DA, AI, FI),
			      function_call_exp_annotation(type_int, PPI),
			      AI, FI), DA, AI, FI), Return]
  ),
  Return = return_stmt(int_val(null, value_annotation(0, PPI),AI,FI),DA,AI,FI),
  FREA = function_ref_exp_annotation(FuncName, FuncType, PPI),
  append(Decls, [Main], Decls1).

maingen(I, I, I, Term, Term).
%-----------------------------------------------------------------------  

largest_coverage(Closure, Largest) :-
  largest_coverage(Closure, t(-1,error,[]), Largest).
largest_coverage([], Max, Max).
largest_coverage([N-Ns|R], t(CurMax, CurM, Reachable), Max) :-
  length(Ns, L),
  (L > CurMax
  -> largest_coverage(R, t(L, N, Ns), Max)
  ;  largest_coverage(R, t(CurMax, CurM, Reachable), Max)).


main :-
  catch((
	 prompt(_,''),
         % Read input
	 read_term(Input, []),
	 compound(Input),

         % Generate the assertions...
	 transformed_with(Input, assertions, preorder, y, _, P1),

	 % Generate a main() if necessary
	 callgraph(P1, CG),
	 transitive_closure(CG, Closure),
	 (member(main/_-_, Closure)
	 -> Output = P1
	 ; (largest_coverage(Closure, t(_, F, Reachable)),
	    vertices(CG, Fs),
	    subtract(Fs, [F|Reachable], UnReachable),
	    transformed_with(P1, maingen, postorder,
			     t(F, Reachable, UnReachable), _, Output))
	 ),
   
         % Write output
	 write_term(Output, [quoted(true)]),
	 writeln('.')
	), E, (print_message(error, E), fail))
.