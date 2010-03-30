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
	       library(utils),
	       library(markers)]).

% weaved/5:
% Info = info(marker_stem(M3_1_12), marker_count(1), ...)

weaved(t(Info,     Annots, RefMarks),
       t(InfoInner,Annots1,RefMarks),
       t(InfoPost, Annots1,RefMarks),
       Node, NodeTrans) :-
  % We only have annotations per basic block
  Node = basic_block(_,_,_,_), 
  !,
  
  % Update the Markers
  update_marker_info(Info, InfoInner, InfoPost, Node, Marker),

  % Weave!
  weave(Node, Marker, RefMarks, Annots, Annots1, NodeTrans).

weaved(t(Info,A,R), t(InfoInner,A,R), t(InfoPost,A,R), Node, Node) :- 
  update_marker_info(Info, InfoInner, InfoPost, Node, _Marker).


% Annotations
weave(basic_block(Stmts, An, Ai, Fi), Marker, RefMarks, As, AsN, BBweaved):-
  select(annotation(Marker, Annot), As, As1),
  format(atom(Text), '~w', Annot),
  !,
  pragma_text(Pragma, Text),
  weave(basic_block([Pragma|Stmts], An, Ai, Fi), 
        Marker, RefMarks, 
        As1, AsN, 
        BBweaved).

% Markers
weave(basic_block(Stmts, An, Ai, Fi), 
      Marker, RefMarks,
      As, As,
      basic_block([Pragma|Stmts], An, Ai, Fi)) :-
  % We only include Markers that are actually referenced by the annotations
  memberchk(Marker, RefMarks), !, 
  format(atom(Text), '~w', wcet_marker(Marker)),
  pragma_text(Pragma, Text).

weave(basic_block(Stmts, An, Ai, Fi), _, _, As, As,
      basic_block(Stmts, An, Ai, Fi)).
  
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

constraint_rhs(annotation(_, wcet_constraint(R)), wcet_constraint(R)) :- !.
constraint_rhs(_, []).

%-----------------------------------------------------------------------
% MAIN
%-----------------------------------------------------------------------

weaveit(InputAnnotations) :- 
  prompt(_,''),
  open(InputAnnotations, read, _, [alias(rstrm)]),
  read_term(rstrm, Annots, [double_quotes(string)]),
  close(rstrm),
  maplist(constraint_rhs, Annots, Restr),
  maplist(atomlist, Restr, Atoms),
  flatten(Atoms, ReferencedMarkers),


  read_term(P, []),
  
  %writeln(Annots),
  transformed_with(P, weaved,
		   t(info(marker_stem('m'), marker_count(1)),
		     Annots, ReferencedMarkers),
		   _, P_Annot),

  write_term(P_Annot, [quoted(true)]),
  write('.\n').


main :-
  current_prolog_flag(argv, Argv), 
  append(_, [--|Args], Argv),
  Args = [A1],
  catch(weaveit(A1), E, (print_message(error, E), fail)),
  halt.

main :-
  format(user_error, 'Usage: weave.pl InputAnnotations <Input >Output', []),
  halt(1).