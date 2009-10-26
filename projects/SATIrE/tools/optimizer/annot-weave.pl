#!/usr/bin/pl -q -t main -f
% -*- prolog-mode -*-

%-----------------------------------------------------------------------
% transform.pl
%
% About
% -----
%
% Annotate a C++ program in [input.pl] with annotations in 
% [input-annotations.pl] and write the result to [output.pl]
%
% Authors
% -------
%
% Copyright (C) 2007,2009 Adrian Prantl
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

% weaved/5:
% Info = info(marker_stem(M3_1_12), marker_count(1), ...)

% TODO: Throw this and the code from unweave into a library

weaved(Info, InfoInner, Info, FunDecl, FunDecl) :- 
  FunDecl = function_declaration(_Prm, _Def, function_declaration_annotation(_Typ, Name, _, _), _),
  Info = info(marker_stem(Stem), marker_count(Count), A, R),
  foldl1([Stem, '_', Name], string_concat, NewStem), 
  InfoInner = info(marker_stem(NewStem), marker_count(Count), A, R).

weaved(Info, InfoInner, InfoPost, Node, NodeTrans) :-
  % We only have annotations per basic block
  Node = basic_block(_,_,_), 
  !,
  % Count the Markers
  Info = info(marker_stem(Stem), marker_count(Count), Annots, RefMarks),
  foldl1([Stem, '_', Count], string_concat, Marker),

  % Weave!
  weave(Node, Marker, RefMarks, Annots, Annots1, NodeTrans),

  % Prepare the Marker for INNER
  StemInner = Marker,
  InfoInner = info(marker_stem(StemInner), marker_count(1), Annots1, RefMarks),

  % Prepare the Marker for POST
  Count1 is Count + 1,
  InfoPost = info(marker_stem(Stem), marker_count(Count1), Annots1, RefMarks).

weaved(Info, Info, Info, Node, Node) :- !.


% Annotations
weave(basic_block(Stmts, An, Fi), Marker, RefMarks, [A|As], As1, BBweaved) :-
  string_to_term(Marker, MarkerTerm),
  A = annotation(MarkerTerm, Annot),
  term_to_string(Annot, AnnotString),
  !, 
  pragma_text(Pragma, AnnotString),
  weave(basic_block([Pragma|Stmts], An, Fi), 
        Marker, RefMarks, 
        As, As1, 
        BBweaved).


% Markers
weave(basic_block(Stmts, An, Fi), 
      Marker, RefMarks,
      As, As,
      basic_block([Pragma|Stmts], An, Fi)) :-
  % We only include Markers that are actually referenced by the annotations
  string_to_term(Marker, MarkerTerm),
  memberchk(MarkerTerm, RefMarks), !,
  term_to_string(wcet_marker(MarkerTerm), AnnotString),
  pragma_text(Pragma, AnnotString).

weave(basic_block(Stmts, An, Fi), _, _, As, As, basic_block(Stmts, An, Fi)).
  
% atomlist - construct a list of all atoms in term

atomlist(Term, [M1|M2]) :-
  bin_op(Term, _F, A, B), !,
  atomlist(A, M1),
  atomlist(B, M2).

atomlist(Term, M1) :-
  un_op(Term, _F, A), !,
  atomlist(A, M1).

atomlist(Term, [Term]) :-
  atom(Term), !.

atomlist(_, []).

restriction_rhs(annotation(_, wcet_restriction(R)), wcet_restriction(R)) :- !.
restriction_rhs(_, []).

%-----------------------------------------------------------------------
% MAIN
%-----------------------------------------------------------------------

weaveit(Input, InputAnnotations, Output) :-
  open(InputAnnotations, read, _, [alias(rstrm1)]),
  read_term(rstrm1, Annots, [double_quotes(string)]),
  close(rstrm1),
  maplist(restriction_rhs, Annots, Restr),
  maplist(atomlist, Restr, Atoms),
  flatten(Atoms, ReferencedMarkers),

  open(Input, read, _, [alias(rstrm2)]),
  read_term(rstrm2, P, [double_quotes(string)]),
  close(rstrm2),
  
  writeln(Annots),
  transformed_with(P, weaved, info(marker_stem('m'), marker_count(1), Annots, ReferencedMarkers), _,P_Annot),

  open(Output, write, _, [alias(wstrm)]),
  write_term(wstrm, P_Annot, [quoted(true),double_quotes(string)] ),
  write(wstrm, '.\n'),
  close(wstrm).


main :-
  current_prolog_flag(argv, Argv), 
  append(_, [--|Args], Argv),
  Args = [A1, A2, A3],
  catch(weaveit(A1, A2, A3), E, (print_message(error, E), fail)),
  halt.

main :-
  writeln('Usage: weave.pl Input InputAnnotations Output'),
  halt(1).