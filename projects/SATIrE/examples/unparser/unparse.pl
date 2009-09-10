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
  current_prolog_flag(argv, Argv), 
  append(_, [--|Args], Argv),
  Args = [File],

  open(File, read, _, [alias(rstrm)]),
  read_term(rstrm,P,[double_quotes(string)]),
  close(rstrm),

  catch(profile(unparse(P)), E, (print_message(error, E), fail)),
  halt.

main :-
  writeln('Usage: unparse.pl [termrep].term'),
  halt(1).