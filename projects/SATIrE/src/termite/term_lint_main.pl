% This is term_lint.pl, a small tool for checking terms against a tree
% grammar (abstract syntax).
% Author: Gergo Barany <gergo@complang.tuwien.ac.at>

% Before doing anything else, set up handling to halt if warnings or errors
% are encountered.
:- dynamic prolog_reported_problems/0.
% If the Prolog compiler encounters warnings or errors, record this fact in
% a global flag. We let message_hook fail because that signals Prolog to
% format output as usual (otherwise, we would have to worry about formatting
% error messages).
user:message_hook(_Term, warning, _Lines) :-
    assert(prolog_reported_problems), !, fail.
user:message_hook(_Term, error, _Lines) :-
    assert(prolog_reported_problems), !, fail.

:- getenv('PWD', CurDir),
   asserta(library_directory(CurDir)),
   prolog_load_context(directory, SrcDir),
   asserta(library_directory(SrcDir)),
   (getenv('TERMITE_LIB', TermitePath)
   ; (print_message(error, 'Please set the environment variable TERMITE_LIB'),
      halt(1))
   ),
   asserta(library_directory(TermitePath)).

:- use_module(library(termlint)).



termite_term(Filename, T) :-
    catch(open(Filename, read, Stream, []),
          Error,
          (print_message(error, Error), !, fail)),
    catch(read_term(Stream, T, [double_quotes(string)]),
          Error,
          (print_message(error, Error), !, fail)),
    close(Stream).

main :-
    format('Termite Lint~n'),
    current_prolog_flag(argv, Argv),
    ( append(_SystemArgs, [_ProgramName, '--', Filename], Argv)
    ; format('* usage: need exactly one term file name argument~n'),
      !,
      fail ),
    !,
    format('checking file "~w"~n', [Filename]),
    termite_term(Filename, T),
    start_symbol(StartSymbol),
    catch(term_match(T, StartSymbol), _, ( !, fail ) ).


% Finish error handling (see top of source file) by halting with an error
% condition of Prolog generated any warnings or errors.
:- (prolog_reported_problems -> halt(1) ; true).
