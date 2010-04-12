#!/usr/local/mstools/bin/pl -q -t main -f
% -*- prolog -*-

%-----------------------------------------------------------------------
% wcetcc.pl
%
% About
% -----
%
% This program is a backend for the wcetC language. It can be used to
% convert the output of the loopbounds analysis into the appropriate
% wcetC constructs.
%
% Authors
% -------
%
% Copyright (C) 2007-2009 Adrian Prantl
%
% License
% -------
%
% This program is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; version 2 of the License.
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
   use_module(library(clpfd)), % for the .. infix operator
   use_module(library(utils)).

% A modifyed pretty-printer for C'
% FOR
my_unparse1(UI, for_statement(E1, E2, E3, E4, _, _, Fi)) :- !, 
  write('for ('), unparse(E1), write('; '), unparse(E2), write('; '), unparse(E3), write(') '),
  write_loopbound(E4),
  writeln('{'),
  unparse_scope(UI, E4),
  indent(Fi), writeln('}').
% WHILE
my_unparse1(UI, while_stmt(E1, E2, _, _, Fi)) :- !, 
  write('while ('), unparse(E1), write(') '), write_loopbound(E2),
  writeln('{'),
  unparse_scope(UI, E2),
  indent(Fi), writeln('}').
% DO
my_unparse1(UI, do_while_stmt(E1, E2, _, _, Fi)) :- !, 
  write('do '), write_loopbound(E1),
  writeln('{'),
  unparse_scope(UI, E1),
  indent(Fi), writeln('}'),
  write('while ('), unparse(E2), write('); ').
% IF
my_unparse1(UI, if_stmt(E1, E2, E3, _, _, Fi)) :- !,
  write('if ('), unparse(E1), write(') '), 
  writeln('{'),
  unparse_scope(UI, E2),
  indent(Fi), writeln('}'),
  (E3 = null/*basic_block(_,_)*/ -> true ;
   (write('else '),
    writeln('{'),
    unparse_scope(UI, E3),
    indent(Fi), writeln('}'))).
% SWITCH
my_unparse1(UI, switch_statement(E1, E2, _, _, Fi)) :- !,
  write('switch ('), unparse(E1), write(') '),
  write('{'), unparse_scope(UI, E2), indent(Fi), write('}').
my_unparse1(UI, case_option_stmt(E1, E2, _, _, _, _Fi)) :- !,
  write('case '), unparse(E1), write(': '),
  unparse_scope(UI, E2).
my_unparse1(UI, default_option_stmt(E1, _, _, _Fi)) :- !,
  write('default '), write(': '), unparse_scope(UI, E1).


my_unparse1(_UI, pragma_declaration(pragma(pragma_annotation(Text), _Ai, _Fi),
			      _An, _Ai1, _Fi1)) :- 
  !, nl, write('#pragma '), write(Text).

my_unparse1(UI, function_declaration(Params, Definition,
             function_declaration_annotation(Type, Name,
		declaration_modifier(_,_TypeModifier,_,StorageModifier),_),
				_Ai, Fi)) :- !,
  unparse_storage_modifier(StorageModifier),
  unparse(Type), write(' '), write(Name),
  write('('),
  (Params = function_parameter_list(Ps, _, _, _) ->
   (replace_types(Ps, Type, Ps1),
    my_unparse(UI, Ps1))
  ;
   true),
  write(')'),
  (Definition = null)
    -> writeln(';') ;
    (
     writeln(' {'),
     Definition = function_definition(Bb, _, _, _),
     unparse_scope(UI, Bb),
     indent(Fi), writeln('}')
    ).
my_unparse1(UI, project([E|Es], _, _, _)) :- !,
  my_unparse(UI, E), my_unparse(UI, project(Es, _, _, _)).

my_unparse1(UI, global(E1, _, _, _)) :- !, my_unparse(UI, E1).
my_unparse1(UI, source_file(E1, _, _, file_info(Name, _, _))) :- !, 
  % Strip directory
  sub_atom(Name, _, _, 0, Filename),
  atom_chars(Filename, Cs),
  \+ member(/, Cs),
  \+ member('\\', Cs),
  atom_concat(Dir, Filename, Name),
  concat_atom([Dir, 'wcetc_', Filename], File),

  format('Unparsing to file "~a"~n', [File]),
  open(File, write, _, [alias(wstrm)]),
  write(wstrm, '/* This file was autmatically generated from '),
  write(wstrm, Name), write(wstrm, ': */\n'),
  write(wstrm, '#include <wcet.h>\n'), 
  with_output_to(wstrm, my_unparse(UI, E1)),
  close(wstrm).

my_unparse1(UI, [E|Es]) :- !,
  my_unparse(UI, E),
  (needs_semicolon(E) -> writeln(';') ; true),
  ((needs_comma(E), Es \== []) -> write(',') ; true), !,
  my_unparse(UI, Es).

my_unparse1(UI, basic_block([variable_declaration(_,_,_,_)|Es],An,Ai,Fi)) :- !,
  % ignore
  my_unparse(UI, basic_block(Es, An, Ai, Fi)).

my_unparse1(UI, basic_block([E|Es], An, Ai, Fi)) :- % no semicolon
  (E = pragma_declaration(_,_,_,_) ;
   E = default_option_stmt(_,_,_,_) ;
   E = case_option_stmt(_,_,_,_,_,_)), !,
  my_unparse(UI, E), nl,
  my_unparse(UI, basic_block(Es, An, Ai, Fi)).
my_unparse1(UI, basic_block([E|Es], An, Ai, Fi)) :- % scope
  E = basic_block(_,_,_,_), !,
  file_info(E, F), indent(F), !,
  
  writeln('{'),
  unparse_scope(UI, E),
  indent(Fi), writeln('}'),

  my_unparse(UI, basic_block(Es, An, Ai, Fi)).
my_unparse1(UI, basic_block([E|Es], An, Ai, Fi)) :- % no semicolon
  scope_statement(E), !, 
  file_info(E, F), indent(F), !,
  my_unparse(UI, E), nl,
  my_unparse(UI, basic_block(Es, An, Ai, Fi)).
my_unparse1(UI, basic_block([E|Es], An, Ai, Fi)) :- !,
  file_info(E, F), indent(F), !,
  my_unparse(UI, E), writeln(';'), 
  my_unparse(UI, basic_block(Es, An, Ai, Fi)).

% WCETC-specific translation rules for annotations

my_unparse1(_UI, Pragma) :-
  pragma_text(Pragma, Text), 
  ignore_pragma(T),
  string_to_term(Text, T), !. % ignore, we have it already

my_unparse1(UI, N) :- unparse(UI, N).

my_unparse(fi(Line, Col, ParentPPIs), Node) :- 
  % process the parent preprocessing info
  unparse_ppi(inside, ParentPPIs),
  % extract mine
  get_preprocessing_infos(Node, PPIs),
  % unparse the node
  unparse_ppi(before, PPIs),
  my_unparse1(fi(Line, Col, []), Node),
  unparse_ppi(after, PPIs).

ignore_pragma(wcet_loopbound(_.._)).
ignore_pragma(wcet_scope(_)).
ignore_pragma(wcet_marker(_)).
ignore_pragma(wcet_constraint(_)).

write_loopbound(basic_block(List, _,_,_)) :- 
  get_annot(List, wcet_loopbound(_..Bound), _), !,
  maplist(write, ['WCET_LOOP_BOUND (', Bound, ')']), nl.
write_loopbound(_) :- gtrace,
  writeln('/* **WARNING: no loop bound */'),
  maplist(write, ['WCET_LOOP_BOUND (', 4294967295, ')']),
  nl.

% output scope-specific things
unparse_scope(UI, Bb) :- 
  Bb = basic_block(List, _,_,_),

  % FIXME: this translation (scopes/restrictions) is actually not 1-1 accurate..
  (   get_annot(List, wcet_scope(Marker), _)
  ->  maplist(write, ['WCET_SCOPE (scope_', Marker, ') {']), nl,

      % Markers must come after vardecls
      maplist(output_vardecl, List),
      maplist(output_marker, List),
    
      my_unparse(UI, Bb),
      write_restrictions(Bb),
      writeln('}')
  ;
      maplist(output_vardecl, List),
      maplist(output_marker, List),
     
      my_unparse(UI, Bb)
  ).

unparse_scope(UI, Other) :-
  file_info(Other, Fi),
  indent(Fi), my_unparse(UI, Other), writeln(';').

unparse_scope(UI, [Stmt]) :-
  unparse_scope(UI, Stmt).

unparse_scope(_, _) :-
  writeln('ERROR!'), trace.


write_restrictions(Bb) :-
  %simple_form_of(Children, Bb),
  transformed_with(Bb, harvest_restrictions, postorder, [], Restrictions, _),
  maplist(format_restriction, Restrictions).

harvest_restrictions(Info, Info, [wcet_restriction(Restriction)|Info], Pragma, Pragma) :-
  %simple_form_of(P, Pragma),
  pragma_text(Pragma, Text),
  atom(Text),
  atom_to_term(Text, wcet_constraint(Restriction), []).
harvest_restrictions(I,I,I, N,N).

format_restriction(A=<B) :- !,
  format_restriction(A),
  write('<='),
  format_restriction(B).

format_restriction(A=B) :- !,
  format_restriction(A),
  write('='),
  format_restriction(B).

format_restriction(A*B) :-
  number(B), !,
  format_restriction(B),
  write('*'),
  format_restriction(A).

format_restriction(A) :-
  number(A), !,
  write('('), write(A), write(')').

format_restriction(wcet_restriction(Term)) :- !,
  write('WCET_RESTRICTION ('),
  format_restriction(Term),
  writeln(');').

format_restriction(A) :- !, write(A).

% format_restriction(R, R3) :-
%   term_to_atom(R, R1), 
%   foldl1(['WCET_RESTRICTION ',R1,');'], atom_concat, R2),
%   replace(R2, '=<', '<=(', R3).

output_marker(Pragma) :-
  pragma_text(Pragma, Text),
  (atom(Text)
  -> atom_to_term(Text, wcet_marker(Marker), [])
  ;  Text = wcet_marker(Marker)),
  maplist(write, ['WCET_MARKER(', Marker, ');\n']).
output_marker(_).

output_vardecl(variable_declaration(InitializedNames, _, _, Fi)) :- !,
  indent(Fi), unparse(InitializedNames), writeln(';').
output_vardecl(_).

% translate Atom -> Term in the pragmas
pragma_convert(I, I, I, Pragma, ConvertedPragma) :-
  pragma_text(Pragma, Atom),
  term_to_atom(Term, Atom),
  pragma_text(ConvertedPragma, Term).

pragma_convert(I, I, I, T, T).


% remove ifdefs as we never could handle them correct here anyway
strip_ifdef(PrepDecl, PrepDecl1) :-
  PrepDecl =.. [Name, S, _, F],
  atom_concat(cpreprocessor,_,Name), !,
  PrepDecl1 =.. [Name, S, never, F].
strip_ifdef(T, T).



%-----------------------------------------------------------------------
% MAIN
%-----------------------------------------------------------------------

convert :-
  prompt(_,''),
  read_term(P, []),
  compound(P),

  transformed_with(P, pragma_convert, [], _, P1),
  %P1 = P2,
  term_mod(P1, strip_ifdef, P2),
  
  my_unparse(fi(1, 1, []), P2).


main :- 
  catch(convert, E, (print_message(error, E), fail)),
  halt.

main :-
  writeln('Usage: wcetcc < input.term'),
  halt(1).
