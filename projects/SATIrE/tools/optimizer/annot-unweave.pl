#!/usr/local/mstools/bin/pl -q -t main -f
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
	       library(utils),
	       library(markers)]).

% unweaved/5:
% Info = info(marker_stem(M3_1_12), marker_count(1), [Annot1|..])

% Extract the Annotations from the pragma decl
unweaved(Info, Info, Info, Pragma, Replacement) :-
  pragma_text(Pragma, Text), !, %gtrace,
  atom_to_term(Text, T2, _),
  Info = info(marker_stem(Stem), marker_count(_Count)),
  Marker = Stem, 
  (   ground(T2) -> true
  ;   format(user_error, '** WARNING: Ill-formatted annotation "~w"~n', [Text])
  ),
  functor(T2, F, 1),
  (   memberchk(F, [wcet_marker, wcet_loopbound, wcet_constraint]) -> true
  ;   format(user_error, '** WARNING: Unknown annotation "~w"~n', [Text])
  ),
  Annot = annotation(Marker, T2),

  % Keep strict loop bounds for the unroller
  (   T2 = wcet_loopbound(N..N)
  ->  Replacement = Pragma
  ;   Replacement = []
  ),
  
  % This is a bit ugly
  retract(annotation_list(AL)),
  append(AL, [Annot], AL1),
  assert(annotation_list(AL1)).

% % Insert Markers for the ICFG Labels
% unweaved(Info, InfoInner, InfoPost,
% 	 basic_block(Stmts, An, Ai, Fi),
% 	 basic_block([MarkerPragma|Stmts], An, Ai, Fi)) :-
%   member(Stmt, Stmts),
%   \+ pragma_text(Stmt, wcet_loopbound(_)),
%   analysis_info(Stmt, analysis_info(AI)),
%   member(entry_exit_labels(Id-_), AI),
%   atom_concat(label, Id, Marker),
%   term_to_atom(wcet_marker(Marker), MarkerA),
%   pragma_text(MarkerPragma, MarkerA),
%   update_marker_info(Info, InfoInner, InfoPost,
% 		     basic_block(Stmts, An, Ai, Fi), _Marker),
%   !.


unweaved(Info, InfoInner, InfoPost, Node, Node) :- 
  update_marker_info(Info, InfoInner, InfoPost, Node, _Marker).

%-----------------------------------------------------------------------
% MAIN
%-----------------------------------------------------------------------

unweave(OutputAnnotations) :- 
  prompt(_,''),
  read_term(P, []),

  % Clear the global state
  assert(annotation_list([])), retractall(annotation_list(_)),
  assert(annotation_list([])),

  (   transformed_with(P, unweaved, info(marker_stem('m'), marker_count(1)),
		    _, P_stripped)
  ->  true
  ;   format(user_error, '** ERROR: transformed_with/5 failed', []), fail
  ),
  open(OutputAnnotations, write, _, [alias(wstrm1)]),
  annotation_list(AL),
  write_term(wstrm1, AL, [quoted(true),double_quotes(string)] ),
  write(wstrm1, '.\n'),
  close(wstrm1),

  write_term(P_stripped, [quoted(true)]),
  write('.\n').

main :- 
  current_prolog_flag(argv, Argv), 
  append(_, [--|Args], Argv),
  Args = [A1], 
  catch(unweave(A1), E, (print_message(error, E), fail)),
  halt.

main :-
  format(user_error, 'Usage: unweave.pl OutputAnnotations <Input >Output', []),
  halt(1).