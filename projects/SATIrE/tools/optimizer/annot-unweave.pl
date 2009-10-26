#!/usr/bin/pl -q -t main -f
% -*- prolog -*-

%-----------------------------------------------------------------------
% unweave.pl
%
% About
% -----
%
% Seperate a C++ program from its embedded annotations
% [input.pl] and write the result to [output-annotations.pl], [output.pl]
%
% Authors
% -------
%
% Copyright (C) 2007, 2009 Adrian Prantl
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

:- prolog_load_context(directory, CurDir),
   asserta(library_directory(CurDir)),
   (getenv('TERMITE_LIB', TermitePath)
   ; (print_message(error, 'Please set the environment variable TERMITE_LIB'),
      halt(1))
   ),
   asserta(library_directory(TermitePath)).

:- use_module([library(clpfd),
	       library(astproperties),
	       library(asttransform),
	       library(utils)]).

% unweaved/5:
% Info = info(marker_stem(M3_1_12), marker_count(1), [Annot1|..])

% Encode the FunctionName into the marker
unweaved(Info, InfoInner, Info, FunDecl, FunDecl) :-
  FunDecl = function_declaration(_Prm, _Def, function_declaration_annotation(_Typ, Name, _, _), _),
  Info = info(marker_stem(Stem), marker_count(Count)),
  foldl1([Stem, '_', Name], string_concat, NewStem), 
  InfoInner = info(marker_stem(NewStem), marker_count(Count)).

unweaved(Info, InfoInner, InfoPost, Node, Node) :-
  Node = basic_block(_,_,_),
  !,
  % Generate the Marker
  %writeln(Info), write(Node),nl,nl, trace,
  Info = info(marker_stem(Stem), marker_count(Count)),
  %writeln(Count),
  foldl1([Stem, '_', Count], string_concat, Marker),

  % Prepare the Marker for INNER
  StemInner = Marker,
  InfoInner = info(marker_stem(StemInner), marker_count(1)),

  % Prepare the Marker for POST
  Count1 is Count + 1,
  InfoPost = info(marker_stem(Stem), marker_count(Count1)).

% Extract the Annotations from the pragma decl
unweaved(Info, Info, Info, 
         pragma_declaration(pragma(pragma_annotation(Text,_),_),_,_), []) :-
  !, 
  Info = info(marker_stem(Stem), marker_count(_Count)),
  Marker = Stem, 
  string_to_term(Marker, T1),
  string_to_term(Text, T2),
  (ground(T2) -> true; 
    maplist(write, ['** WARNING: Ill-formatted annotation "', Text, '"\n'])),
  functor(T2, F, 1),
  (memberchk(F, [wcet_marker, wcet_loopbound, wcet_restriction]) -> true;
    maplist(write, ['** WARNING: Unknown annotation "', Text, '"\n'])),
  Annot = annotation(T1, T2),

  % This is a bit ugly
  retract(annotation_list(AL)),
  append(AL, [Annot], AL1),
  assert(annotation_list(AL1)).

unweaved(Info, Info, Info, Node, Node).

%-----------------------------------------------------------------------
% MAIN
%-----------------------------------------------------------------------

unweave(Input, Output, OutputAnnotations) :-
  open(Input, read, _, [alias(rstrm2)]),
  read_term(rstrm2, P, [double_quotes(string)]),
  close(rstrm2),

  % Clear the global state
  assert(annotation_list([])), retractall(annotation_list(_)),
  assert(annotation_list([])),

  simple_form_of(Pc, P),

  (transformed_with(Pc, unweaved, info(marker_stem('m'), marker_count(1)), _, P_stripped) -> true;
   writeln('** ERROR: transformed_with/5 failed'), fail),

  open(OutputAnnotations, write, _, [alias(wstrm1)]),
  annotation_list(AL),
  write_term(wstrm1, AL, [quoted(true),double_quotes(string)] ),
  write(wstrm1, '.\n'),
  close(wstrm1),

  %simple_form_of(P, Ps), unparse(Ps), nl,nl, simple_form_of(P_stripped, Pss), unparse(Pss), 

  open(Output, write, _, [alias(wstrm2)]),
  write_term(wstrm2, P_stripped, [quoted(true),double_quotes(string)] ),
  write(wstrm2, '.\n'),
  close(wstrm2).

main :-
  current_prolog_flag(argv, Argv), 
  append(_, [--|Args], Argv),
  Args = [A1, A2, A3], 
  catch(unweave(A1, A2, A3), E, (print_message(error, E), fail)),
  halt.

main :-
  writeln('Usage: unweave.pl Input Output OutputAnnotations'),
  halt(1).