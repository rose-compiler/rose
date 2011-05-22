#!/usr/bin/pl -q  -O  -L64M -G64M -t main -s 
% -*- prolog -*-
% vim: syntax=prolog

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
   ; (print_message(error, 'Please set the environment variable TERMITE_LIB'),
      halt(1))
   ),
   asserta(library_directory(TermitePath)).

:- use_module([library(asttransform),
	       library(astproperties),
	       library(callgraph),
	       library(utils)]).

%-----------------------------------------------------------------------

% don't compare against top
compval(top, V, V, _, _, _) :- !.
compval('BOTTOM', V, V, _, _, _) :- !.
compval(Val, _, int_val(null, value_annotation(Val, PPI), AI, FI), PPI, AI, FI).

% look up type for some variable
var_withtype(Decls, Var->Intvl, Var:Type->Intvl) :-
  member(Var:Type, Decls),
  !.
var_withtype(_Decls, _V, not_in_scope).

% get Var:Type pair from a variable declaration
vardecl_vartype(variable_declaration([IN],_,_,_), Var:Type) :-
  IN = initialized_name(_, initialized_name_annotation(Type,Var,_,_),_,_).

% check for pointer type
is_pointer_type(pointer_type(_)).
is_pointer_type(modifier_type(T, _)) :-
    is_pointer_type(T).
is_pointer_type(typedef_type(_, T)) :-
    is_pointer_type(T).

% check for array type
is_array_type(array_type(_, _)).
is_array_type(modifier_type(T, _)) :-
    is_array_type(T).
is_array_type(typedef_type(_, T)) :-
    is_array_type(T).

% update declared variables according to a scope's declarations
decls_stmts_newdecls(Decls0, S, Decls) :-
  % accept both single statements and statement lists
  flatten(S, Stmts),
  include(subsumes_chk(variable_declaration(_,_,_,_)), Stmts, VarDecls),
  maplist(vardecl_vartype, VarDecls, VarsTypes),
  append(VarsTypes, Decls0, Decls).

% force the body to be a basic block so we can add assertion statements
body_wrapped(Body0, Body1) :-
  ( Body0 = basic_block(_,_,_,_)
  -> Body1 = Body0
  ; default_values(_, BDA, BAI, _),
    % ugly hack: if the wrapping basic block is marked as compiler
    % generated, the unparser does not emit it! (this is the case for
    % if_stmt bodies, at least) so we steal the original body's file info
    Body0 =.. Body0List,
    last(Body0List, BFI),
    Body1 = basic_block([Body0], BDA, BAI, BFI)
  ).

% surround each statement with a guard functor to keep it from being
% transformed
statements_guarded([], []).
statements_guarded([S|Ss], [guard(S)|Gs]) :-
  statements_guarded(Ss, Gs).

% generate an expression assert(v > Lower && v < Upper)

% factored out general case from assertions/5
stmt_decls_infos_asserted(Statement, Decls, AI, FI, AssertedStatement) :-
  DA = default_annotation(null, preprocessing_info([])),
  AI = analysis_info(AIs),
  member(pre_info(interval, PreInfo), AIs),
  member(post_info(interval, PostInfo), AIs),
  info_decls_assertions(PreInfo, Decls, PreAssertions),
  info_decls_assertions(PostInfo, Decls, PostAssertions),
  chain_up(PreAssertions, DA, AI, FI, PreStmt),
  chain_up(PostAssertions, DA, AI, FI, PostStmt),
  flatten([PreStmt, Statement, PostStmt], AssertedStatement).

% added case to generate negative assertions for unreachable program points
info_decls_assertions(Info, Decls, Assertions) :-
  member(merged:map([top,top], Vars), Info),
  maplist(var_withtype(Decls), Vars, VarsWithTypes),
  exclude(=(not_in_scope), VarsWithTypes, ValidVars),
  maplist(interval_assert, ValidVars, Assertions).
info_decls_assertions(Info, _Decls, [AssertCall]) :-
  member(merged:bot, Info),
  default_values(PPI, _DA, AI, FI),
  Zero = int_val(null, value_annotation(0, PPI), AI, FI),
  ErrorMsg = string_val(null,
                        value_annotation(
                            'error: branch should be unreachable!', PPI),
                        AI, FI),
  ErrorInt = cast_exp(ErrorMsg, null,
                      unary_op_annotation(
                          prefix, type_int, implicit, null, PPI),
                      AI, FI),
  And = and_op(Zero, ErrorInt, binary_op_annotation(type_int, PPI), AI, FI),
  assert_call(And, AssertCall).

% build call expression to assert, given the expression to be asserted
assert_call(AssertExpression, CallExpression) :-
  default_values(PPI, DA, AI, FI),
  CallExpression =
    function_call_exp(FRefExp, expr_list_exp([AssertExpression], DA, AI, FI),
		      function_call_exp_annotation(type_void,PPI), AI, FI),
  FRefExp = function_ref_exp(function_ref_exp_annotation(assert, Ftp, PPI),
			     AI, FI),
  Ftp = function_type(type_void,ellipses,[type_int]).

interval_assert(Var:VarType->[Min,Max], AssertionExpr) :-
  default_values(PPI, _DA, AI, FI),
  % VarType = type_int, %null,
  assert_call(AndOp, AssertionExpr),

  % skip temp vars, pointer and array variables
  ( ( atom_concat('$', _, Var)
      ; is_pointer_type(VarType) ; is_array_type(VarType) )
  -> AndOp = int_val(null, value_annotation(1, PPI), AI, FI)
  ;  AndOp = and_op(GE, LE, binary_op_annotation(type_int, PPI), AI, FI)
  ),

  % ignore comparison with top and bot
  compval(Min, VarRef, MinVal, PPI, AI, FI),
  compval(Max, VarRef, MaxVal, PPI, AI, FI),
  GE = greater_or_equal_op(VarRef, MinVal, TypeAn, AI, FI),
  LE =    less_or_equal_op(VarRef, MaxVal, TypeAn, AI, FI),

  VarRef = var_ref_exp(var_ref_exp_annotation(VarType,Var,default,null,PPI),
		       AI, FI),
  TypeAn = binary_op_annotation(VarType, PPI).

% generate a comma-seperated chain of assertions
comma_chain(Expr1, Expr2, comma_op_exp(Expr1, Expr2, BA, AI, FI)) :-
  BA = binary_op_annotation(type_void, PPI),
  default_values(PPI, _, AI, FI).

% generate a comma chain or leave empty
chain_up(Assertions, DA, AI, FI,  Statement) :-
  flatten(Assertions, As),
  ( As = []
  -> Statement = []
  ; (foldl1(As, comma_chain, PreExp),
     Statement = expr_statement(PreExp, DA, AI, FI))
  ).
  
% do not modify guarded terms
assertions(I, n, I, guard(Term), Term).

% transform basic blocks with their variable declarations in effect
assertions(y-Decls0, y-Decls, y-Decls0, Bb, Bb) :-
  Bb = basic_block(Stmts, _,_,_),
  decls_stmts_newdecls(Decls0, Stmts, Decls).

% don't generate assertions for branch conditions (put guards around these),
% but do generate assertions inside bodies, with any declarations from the
% condition in effect inside the body
assertions(y-Decls0, y-Decls, y-Decls0, For, AssertedFor) :-
  For = for_statement(D, C, I, Body, A, AI, FI),
  D = for_init_statement(Inits, IA, IAI, IFI),
  statements_guarded(Inits, GuardedInits),
  GuardedD = for_init_statement(GuardedInits, IA, IAI, IFI),
  decls_stmts_newdecls(Decls0, Inits, Decls),
  body_wrapped(Body, Body1),
  For1 = for_statement(GuardedD, guard(C), guard(I), Body1, A, AI, FI),
  stmt_decls_infos_asserted(For1, Decls0, AI, FI, AssertedFor).
assertions(y-Decls0, y-Decls, y-Decls0, If, AssertedIf) :-
  If = if_stmt(Cond, Body, Else, A, AI, FI),
  decls_stmts_newdecls(Decls0, Cond, Decls),
  body_wrapped(Body, Body1),
  ( Else = null
  -> Else1 = null
  ;  body_wrapped(Else, Else1)
  ),
  If1 = if_stmt(guard(Cond), Body1, Else1, A, AI, FI),
  stmt_decls_infos_asserted(If1, Decls0, AI, FI, AssertedIf).
assertions(y-Decls0, y-Decls, y-Decls0, While, AssertedWhile) :-
  While = while_stmt(Cond, Body, A, AI, FI),
  decls_stmts_newdecls(Decls0, Cond, Decls),
  body_wrapped(Body, Body1),
  While1 = while_stmt(guard(Cond), Body1, A, AI, FI),
  stmt_decls_infos_asserted(While1, Decls0, AI, FI, AssertedWhile).
assertions(y-Decls0, y-Decls, y-Decls0, DoWhile, AssertedDoWhile) :-
  DoWhile = do_while_stmt(Body, Cond, A, AI, FI),
  % would anyone really declare a variable in a do-while condition? is it
  % even possible?
  decls_stmts_newdecls(Decls0, Cond, Decls),
  body_wrapped(Body, Body1),
  DoWhile1 = do_while_stmt(Body1, guard(Cond), A, AI, FI),
  stmt_decls_infos_asserted(DoWhile1, Decls0, AI, FI, AssertedDoWhile).
assertions(y-Decls0, y-Decls, y-Decls0, Switch, AssertedSwitch) :-
  Switch = switch_statement(Key, Body, A, AI, FI),
  decls_stmts_newdecls(Decls0, Key, Decls),
  body_wrapped(Body, Body1),
  Switch1 = switch_statement(guard(Key), Body1, A, AI, FI),
  stmt_decls_infos_asserted(Switch1, Decls0, AI, FI, AssertedSwitch).

assertions(y-Decls, y-Decls, y-Decls, Statement, AssertedStatement) :-
  functor(Statement, F, Arity),
  ( StmtSuffix = stmt
  ; StmtSuffix = statement
  ),
  atom_concat(_Name, StmtSuffix, F),
  N1 is Arity-1, arg(N1, Statement, AI),
  N0 is Arity-0, arg(N0, Statement, FI),

  stmt_decls_infos_asserted(Statement, Decls, AI, FI, AssertedStatement),
  !.

assertions(y-[], y-[], y-VarsTypes, global(Decls, An, Ai, Fi),
	  global([Assert|Decls1], An, Ai, Fi)) :-
  include(subsumes_chk(variable_declaration(_,_,_,_)), Decls, VarDecls),
  maplist(vardecl_vartype, VarDecls, VarsTypes),

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
  Decls = [D|Ds],
  D =.. D_u,     append(Dhead, [Dan, Dai, Dfi], D_u),
  Dan =.. Dan_u, append(Dan_head, [preprocessing_info(PPIs)], Dan_u),

  % Guard the #include because CBMC doesn't like it.
  Ifdef = cpreprocessorIfdefDeclaration('#ifdef INCLUDE_ASSERT_H', before, Fi),
  Include = cpreprocessorIncludeDeclaration('#include <assert.h>\n', before,Fi),
  Endif = cpreprocessorEndifDeclaration('#endif', before, Fi),
  append(Dan_head, [preprocessing_info([Ifdef,Include,Endif|PPIs])], Dan_u1),
  Dan1 =.. Dan_u1,
  append(Dhead, [Dan1, Dai, Dfi], D_u1),
  D1 =.. D_u1, 
  Decls1 = [D1|Ds].

% leave all other terms alone
assertions(I, I, I, Term, Term).
%-----------------------------------------------------------------------  

prettyprint(Func/Type) :- unparse(Type), format(' ~w(); ', [Func]).

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
	 transformed_with(Input, assertions, preorder, y-[], _, P1),

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
