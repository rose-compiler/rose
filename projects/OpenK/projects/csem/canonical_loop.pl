:- module(canonical_loop, [canonicalLoop/1]).

:- use_module(basic).
:- use_module(variable).
:- use_module(loop).
:- use_module(type).

%%	The top level specification
	
canonicalLoop(LoopURL) :-
	forLoop(LoopURL),
	hasForInit(LoopURL, Init),
	isCanonicalInit(Init, LoopVar),

	hasForTest(LoopURL, Test),
	isCanonicalTest(Test, LoopVar),

	hasForIncr(LoopURL, Incr),
	isCanonicalIncr(Incr, LoopVar).

%%	isCanonicalInit

isCanonicalInit(InitURL, LoopVar) :-
	%% forLoop(LoopURL), hasForInit(LoopURL, InitURL),
	roseCanonicalInit(InitURL, LoopVar, _),
	hasType(LoopVar, Type), 
	(intType(Type); Type == pointer_type).

%% normal style
roseCanonicalInit(InitURL, LoopVar, LB) :-
	hasChild(InitURL, AssignOpURL),
	c_is_a(AssignOpURL, 'AssignOp'), !,
	leftOperand(AssignOpURL, VarRefURL),
	get_varDecl(VarRefURL, LoopVar),
	rightOperand(AssignOpURL, LB).

%% C99 style
%% In this case, rose AST nodes have the same location
roseCanonicalInit(InitURL, LoopVar, LB) :-
	%% hasChild(InitURL, VarURL),
	c_is_a(InitURL, 'VariableDecl'),
	c_is_a(InitURL, 'Variable'), !,
	\+ (hasChild(InitURL, VarDecl), c_is_a(VarDecl, 'VariableDecl')),
	roseHasInitValue(InitURL, LB),
	LoopVar = InitURL.

%%	isCanonicalTest
	
isCanonicalTest(TestURL, LoopVar) :- 
	%% forLoop(LoopURL), hasForTest(LoopURL, TestURL),
	roseCanonicalTest(TestURL, LoopVar).

roseCanonicalTest(TestURL, LoopVar) :-
	c_is_one_of(TestURL, ['LeqOp', 'LessOp', 'GeqOp', 'GreaterOp']), !,
	leftOperand(TestURL, Left),
	rightOperand(TestURL, Right),
	(
		get_varDecl(Left, LoopVar);
		get_varDecl(Right, LoopVar)
	).

%%	isCanonicalIncr

isCanonicalIncr(IncrURL, LoopVar) :- 
	%% forLoop(LoopURL), hasForIncr(LoopURL, IncrURL),
	roseCanonicalIncr(IncrURL, LoopVar, _).

%% ++, --
roseCanonicalIncr(IncrURL, LoopVar, C) :-
	c_is_one_of(IncrURL, ['PostfixIncrementOp', 'PostfixDecrementOp']), !,
	hasOperand(IncrURL, VarRefURL),
	get_varDecl(VarRefURL, LoopVar),
	C = 1.

%% var += C, var -= C 
roseCanonicalIncr(IncrURL, LoopVar, C) :-
	c_is_one_of(IncrURL, ['AddAssignOp', 'SubAssignOp']), !,
	leftOperand(IncrURL, VarRefURL),
	get_varDecl(VarRefURL, LoopVar),
	rightOperand(IncrURL, C).

%% var = var + C, var = C + var, var = var - C 
roseCanonicalIncr(IncrURL, LoopVar, C) :-
	c_is_a(IncrURL, 'AssignOp'), !,
	leftOperand(IncrURL, VarRefURL),
	get_varDecl(VarRefURL, LoopVar),
	rightOperand(IncrURL, IncrExpr),
	roseIsValidIncrExpr(IncrExpr, LoopVar, C).

roseIsValidIncrExpr(IncrExpr, LoopVar, C) :-
	c_is_one_of(IncrExpr, ['AddOp', 'SubOp']),
	leftOperand(IncrExpr, Left),
	rightOperand(IncrExpr, Right),
	(
		get_varDecl(Left, LoopVar) -> C = Right;
		get_varDecl(Right, LoopVar) -> C = Left
	).





