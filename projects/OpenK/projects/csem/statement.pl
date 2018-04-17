:- module(statement, [
	isStatement/1,
	isStatementOf/2,
	isVariableDecl/1,
	isBasicStatement/1,
	isIfElseStatement/1,
	isForStatement/1,
	isWhileStatement/1,
	isDoWhileStatement/1,
	isLoopStatement/1,
	isBlock/1,
	isBreakStatement/1,
	isContinueStatement/1,
	isReturnStatement/1,
	isSwitchStatement/1,
	isCaseStatement/1,
	isDefaultStatement/1,
	isGotoStatement/1,

	getStatementAtLine/2
]).

:- use_module(basic).


statements([
'Statement', 
'ExpressionStatement',
'IfElseStatement', 'SwitchStatement',
'ForStatement', 'WhileStatement', 'DoWhileStatement',
'LabeledStatement',
'CaseOptionStatement',
'DefaultOptionStatement',
'BreakStatement', 'ContinueStatement', 'ReturnStatement', 'GotoStatement',
'NullStatement',
'ForInitStatement', 
'IOStatement'
]).

%%% check or get the statement
%	non-det   
%	?StmtURL
isStatement(StmtURL) :-
	isStatementOf(StmtURL, _).

%% check statement entity, return its concrete class
isStatementOf(StmtURL, Class) :-
	statements(StmtURLs), !,
	c_is_a(StmtURL, Class),
	memberchk(Class, StmtURLs).

isVariableDecl(Stmt) :-
	c_is_a(Stmt, 'VariableDecl').

%% a basic single statement, different than Block
isBasicStatement(StmtURL) :-
	c_is_a(StmtURL, Class),
	memberchk(Class, ['ExpressionStatement', 'NullStatement','IOStatement']).

%% 
isIfElseStatement(StmtURL) :-
	c_is_a(StmtURL, 'IfElseStatement').

isForStatement(StmtURL) :-
	c_is_a(StmtURL, 'ForStatement').

isWhileStatement(StmtURL) :-
	c_is_a(StmtURL, 'WhileStatement').

isDoWhileStatement(StmtURL) :-
	c_is_a(StmtURL, 'DoWhileStatement').

isLoopStatement(StmtURL) :-
	c_is_a(StmtURL, Class),
	memberchk(Class, ['ForStatement', 'WhileStatement', 'DoWhileStatement']).

isBlock(StmtURL) :-
	c_is_a(StmtURL, 'Block').

%% 
isBreakStatement(StmtURL) :-
	c_is_a(StmtURL, 'BreakStatement').

isContinueStatement(StmtURL) :-
	c_is_a(StmtURL, 'ContinueStatement').

isReturnStatement(StmtURL) :-
	c_is_a(StmtURL, 'ReturnStatement').

%% 
isSwitchStatement(StmtURL) :-
	c_is_a(StmtURL, 'SwitchStatement').

isCaseStatement(StmtURL) :-
	c_is_a(StmtURL, 'CaseOptionStatement').

isDefaultStatement(StmtURL) :-
	c_is_a(StmtURL, 'DefaultOptionStatement').

isGotoStatement(StmtURL) :-
	c_is_a(StmtURL, 'GotoStatement').

%% helper function, get the statement at line 
getStatementAtLine(Line, StmtURL) :-
	(atom(Line); writeln('Line need to be atom, use single quote')), !,
	isStatement(StmtURL),
	get_id(StmtURL, StmtID),
	atom_prefix(StmtID, Line).
