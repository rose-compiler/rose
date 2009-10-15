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

:- use_module(library(asttransform)).

main :-
  format(user_error, '/* Unparsing from stdin */~n', []),
  read_term(P, []),
  compound(P),

  catch(%profile(
	unparse(P), E, (print_message(error, E), fail)),
  halt.

main :-
  writeln('Usage: unparse.pl < [termrep].term'),
  halt(1).