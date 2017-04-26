#!/usr/bin/env swipl
%% swipl --nosignals --quiet run.pl is.c.ttl is.c.txt

:- set_prolog_flag(verbose, silent).

:- use_module(library(semweb/rdf_db)).
:- use_module(library(semweb/turtle)).

:- use_module('../csem/basic.pl').
:- use_module('../csem/canonical_loop.pl').

:- initialization main.

%% get the root folder of openk project
openk_root(OPENK) :-
	working_directory(W, W), 
	atom_concat(W, '../..', OPENKR),
	absolute_file_name(OPENKR, OPENK).


c_onto(Cowl) :-
	openk_root(R), atom_concat(R, '/projects/csem/c.owl', Cowl).

eval :-
	current_prolog_flag(argv, Argv),
	[Inputfile, Outputfile|_] = Argv,
	c_onto(Conto),
	rdf_load(Conto, [register_namespaces(ture)]),
	%% writeln('c.owl is loaded!'), 
	analysis(Inputfile, Outputfile),
	writeln(done).
%% test comments	
main :- 
	catch(eval, E, (print_message(error, E), fail)), 
	halt(0).
	
main :-
	halt(0).

%% analysis(InTurtle, Outputfile) :-
%% 	write(InTurtle), nl, write(Outputfile), nl, !.

analysis(InTurtle, OutReport) :-
	%% open output report file
	atom(OutReport), open(OutReport, write, Out), !,
	rdf_load(InTurtle, [format(turtle), register_namespaces(true)]),
	%% write(InTurtle), tab(1), writeln('is loaded'),
	statistics(cputime, T1),

	forall( (canonicalLoop(Loop), get_id(Loop, L)), fwriteln(Out, L)),
	
	statistics(cputime, T2),
	Tdiff is (T2 - T1) * 1000000,
	fwriteln(Out, '-----------------'),
	write(Out, '#'), fwriteln(Out, Tdiff), 
	fwriteln(Out, '-----------------'),
	close(Out).


fwriteln(Out, Text) :-
	write(Out, Text), nl(Out).

