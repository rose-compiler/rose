:- module(loop, [
	forLoop/1,
	forBody/1,
	hasForInit/2,
	hasForTest/2,
	hasForIncr/2
]).

:- use_module(basic).

forLoop(Resource) :-
	c_get_instance('ForStatement', Resource).

forBody(Resource) :-
	forLoop(Loop), hasBody(Loop, Resource).

hasForInit(LoopURL, InitURL) :-
	c_has(hasForInit, LoopURL, InitURL).

hasForTest(LoopURL, TestURL) :-
	c_has(hasForTest, LoopURL, TestURL).

hasForIncr(LoopURL, IncrURL) :-
	c_has(hasForIncr, LoopURL, IncrURL).

hasBody(LoopURL, Body) :-
	c_has(hasBody, LoopURL, Body).