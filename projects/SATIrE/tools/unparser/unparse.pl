#!/usr/local/mstools/bin/pl -q -t main -f
% -*- prolog -*-

:- getenv('PWD', CurDir),
   asserta(library_directory(CurDir)),
   prolog_load_context(directory, SrcDir),
   asserta(library_directory(SrcDir)),
   (getenv('TERMITE_LIB', TermitePath)
   ; (print_message(error, 'Please set the environment variable TERMITE_LIB'),
      halt(1))
   ),
   asserta(library_directory(TermitePath)).

:- use_module([library(asttransform), library(astproperties)]).


strip_headers(_, _, _, Node, null) :-
  file_info(Node, file_info(File, _, _)),
  concat_atom([_, '.h'], File).
strip_headers(_, _, _, Node, Node).

main :-
  prompt(_,''),
  format(user_error, '/* Unparsing from stdin */~n', []),
  read_term(P, []),
  compound(P),

  transformed_with(P, strip_headers, postorder, [], _, P1),

  catch(%profile(
	unparse(P1), E, (print_message(error, E), fail)),
  halt.

main :-
  writeln('Usage: unparse.pl < [termrep].term'),
  halt(1).