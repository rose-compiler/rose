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


% remember the current file name
strip_headers(_, FileName, FileName, File, File) :-
  File = source_file(_,_,_,file_info(FileName, _, _)),
  !.

% strip all lines from any other files
strip_headers(FileName, FileName, FileName, Node, Node1) :-
  (   (	  file_info(Node, file_info(FileName, _, _))
      ;	  file_info(Node, file_info(compilerGenerated, _, _))
      ;	  file_info(Node, file_info(transformation, _, _))
      )
  ->  Node1 = Node
  ;   Node1 = null(null,null,null,file_info(null,0,0))
      % We don't confuse the traversal that way
  ).

main :-
  prompt(_,''),
  format(user_error, '/* Unparsing from stdin */~n', []),
  read_term(P, []),
  compound(P),

  transformed_with(P, strip_headers, preorder, _, _, P1),

  catch(%profile(
	unparse(P1), E, (print_message(error, E), fail)),
  halt.

main :-
  writeln('Usage: unparse.pl < [termrep].term'),
  halt(1).