#!/usr/bin/env swipl
%% swipl --nosignals --quiet run.pl is.c.ttl

:- set_prolog_flag(verbose, silent).

:- use_module(library(semweb/rdf_db)).
:- use_module(library(semweb/turtle)).

:- ensure_loaded(load).

:- initialization eval.

:- rdf_register_prefix(c, 'http://www.semanticweb.org/yzhao30/ontologies/2015/7/c#').

eval :-
	current_prolog_flag(argv, Argv),
	[Inputfile|_] = Argv,
	%% c_onto(Conto),
	%% rdf_load(Conto, [register_namespaces(ture)]),
	%% writeln('c.owl is loaded!'), 
	analysis(Inputfile),
	writeln('Input knowledge base is loaded.').
	
main :- 
	catch(eval, E, (print_message(error, E), fail)), 
	halt(0).
	
main :-
	halt(0).


analysis(InTurtle) :-
	%% load knowledge base of input program
	rdf_load(InTurtle, [format(turtle), register_namespaces(true)]).



