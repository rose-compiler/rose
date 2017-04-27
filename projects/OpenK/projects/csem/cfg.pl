:- module(cfg, [
	cfg/1,
	cfg/3,
	getEnclosingLoop/2,
	getEnclosingLoopOrSwitch/2,
	getEnclosingFunction/2
]).

:- use_module(basic).
:- use_module(statement).
:- use_module(function).


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%		
%
%	per statement cfg
%	every 
%	+Stmt to statement to analyze
%	-Entry return single entry statement
%	-Exit return single exit (create one if needed)
%
%   @tbd Goto, Switch
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%		
%
%% !!BUG
%% Rose: code location for switch
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%% cfg for function definition
%	see document.txt for function structure in ROSE   
%	@tbd	  
cfg(Function) :-
	functionBodyOf(Function, FunctionBody), !,
	cfg(FunctionBody, _, _). % call the cfg(Block, _, _)

%%% variable declaration
%	comment   
%	@tbd	  
cfg(VarDecl, Entry, Exit) :-
	isVariableDecl(VarDecl), !,
	Entry = VarDecl,
	Exit = VarDecl.

%%%  The basic statement (Expression statement)
%	 comment
%	 [simple tested] 
cfg(Stmt, Entry, Exit) :-
	isBasicStatement(Stmt), !,
	Entry = Stmt,
	Exit = Stmt.

%%% The if-then-else statement
%	create a exit(stmt_id) exit node   
%	[simple tested]  
cfg(Stmt, Entry, Exit) :-
	isIfElseStatement(Stmt), !,
	c_has(hasCondition, Stmt, ConditionStmt),
	Entry = ConditionStmt,
	Exit = exit(Stmt), % create single blank exit node 
	(
		c_has(hasTrueBody, Stmt, TrueBody) ->
		cfg(TrueBody, TrueEntry, TrueExit),
		setTrueNext(Entry, TrueEntry),
		setNext(TrueExit, Exit) ;
		setTrueNext(Entry, Exit) % if true body is empty
	),
	(
		c_has(hasFalseBody, Stmt, FalseBody) ->
		cfg(FalseBody, FalseEntry, FalseExit),
		setFalseNext(Entry, FalseEntry),
		setNext(FalseExit, Exit);
		setFalseNext(Entry, Exit)
	).

%%% The Ternary operator
%	comment   
%	@tbd	

%%% The ForStatement
%	comment   
%	@tbd what if ForInit, ForIncr empty?
cfg(ForStatement, Entry, Exit) :-
	isForStatement(ForStatement), !,
	c_has(hasForInit, ForStatement, Entry), % Entry
	c_has(hasForTest, ForStatement, ConditionStmt),
	c_has(hasForIncr, ForStatement, ForIncr),
	c_has(hasBody, ForStatement, ForBody),
	Exit = exit(ForStatement), % Exit, blank node 
	cfg(ForBody, BodyEntry, BodyExit),
	setNext(Entry, ConditionStmt),
	setTrueNext(ConditionStmt, BodyEntry),
	setFalseNext(ConditionStmt, Exit),
	setNext(BodyExit, ForIncr),
	setNext(ForIncr, ConditionStmt).

	
%%% The while statement
%	in Rose, while(...); hasBody ExpressionStatement:NullExpr
%	@tbd	
cfg(Stmt, Entry, Exit) :-
	isWhileStatement(Stmt), !,
	c_has(hasCondition, Stmt, ConditionStmt),
	c_has(hasBody, Stmt, BodyStmt),
	Entry = ConditionStmt, % continue Stmt may point to 
	Exit = exit(Stmt),% in cfg(BreakStmt), break use the same name to refer to this exit
	cfg(BodyStmt, BodyEntry, BodyExit),
	setTrueNext(ConditionStmt, BodyEntry),
	setFalseNext(ConditionStmt, Exit),
	setNext(BodyExit, ConditionStmt).


%%% The DoWhile ...
%	comment   
%	@tbd	  
cfg(DoWhileStmt, Entry, Exit) :-
	isDoWhileStatement(DoWhileStmt), !,
	c_has(hasBody, DoWhileStmt, BodyStmt),
	c_has(hasCondition, DoWhileStmt, ConditionStmt),
	cfg(BodyStmt, BodyEntry, BodyExit),
	Entry = BodyEntry,
	Exit = exit(DoWhileStmt),
	setNext(BodyExit, ConditionStmt),
	setTrueNext(ConditionStmt, BodyEntry),
	setFalseNext(ConditionStmt, Exit).


%%% The Switch case statement 
%	to be completed, just a dummy rule noe  
%	rose gets bug here! 
%	@tbd	
cfg(SwitchStmt, Entry, Exit) :-
	isSwitchStatement(SwitchStmt), !,
	Entry = SwitchStmt,
	Exit = SwitchStmt,
	writeln('SwitchStmt not supported yet').


%%% The Break 
%	comment   
%	@tbd
cfg(BreakStmt, Entry, Exit) :-
	isBreakStatement(BreakStmt), !,
	Entry = BreakStmt,
	getEnclosingLoopOrSwitch(BreakStmt, Parent),
	ParentExit = exit(Parent), % exit(Parent) is a blank exit node, consistent with parent
	setNext(BreakStmt, ParentExit),
	Exit = none. % mark the divergent, will be checked in cfg(Block, ...)
	

%%% The continue 
%	comment   
%	@tbd
cfg(ContinueStatement, Entry, Exit) :-
	isContinueStatement(ContinueStatement), !,
	Entry = ContinueStatement,
	getEnclosingLoop(ContinueStatement, Loop),
	(c_has(hasCondition, Loop, NextIterStmt); % while 
		c_has(hasForIncr, Loop, NextIterStmt)), % for loop 
	setNext(ContinueStatement, NextIterStmt),
	Exit = none. % mark the divergent


%%% Goto and labels
%	!just a tmporary holder, not functionaly properly   
%	@tbd test the goto case  
cfg(GotoStatement, Entry, Exit) :-
	isGotoStatement(GotoStatement), !,
	Entry = GotoStatement, % !
	Exit = GotoStatement, % !
	writeln('GotoStatement not supported yet').

%%% Return
%	comment   
%	@tbd should jump to the exit of function 
cfg(ReturnStmt, Entry, Exit) :-
	isReturnStatement(ReturnStmt), !,
	Entry = ReturnStmt,
	getEnclosingFunction(ReturnStmt, FunctionDef),
	Exit = exit(FunctionDef).
	  	      

%%% Block contains a sequence of statements 
%	assume the children are in its code order   
%	@tbd check ChildStmts is in order [checked, yes]
cfg(Block, Entry, Exit) :-
	isBlock(Block), !,
	findall(ChildStmt, hasChild(Block, ChildStmt), ChildStmts), 
	(
		[] = ChildStmts -> Entry = Block, Exit = Block; % empty body
		[First|Rest] = ChildStmts, % otherwise
		cfg(First, FirstEntry, FirstExit),
		Entry = FirstEntry, 
		cfg_sq(Rest, FirstExit, Exit)
	).


%% The final guard catching the Unknown situation
%% must put at last
cfg(Stmt, Entry, Exit) :-
	c_is_a(Stmt, Class), !,
	write('Unknown statement: '), write(Stmt), tab(4), writeln(Class),
	Entry = Stmt, Exit = Stmt.


%% helper, recursively process statement sequence
cfg_sq([], Exit, Exit). % last one exit is the exit 
cfg_sq([Stmt|RestStmts], PredExit, Exit) :-
	cfg(Stmt, CurEntry, CurExit),
	(
		PredExit == none -> true; % if previous is break, continue, return, which has exit none, skip
	 	setNext(PredExit, CurEntry)
	),
	cfg_sq(RestStmts, CurExit, Exit).


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%		
%
%	Assistant rules
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%% predicate 
%	comment   
%	@tbd	  
setNext(Pred, Next) :-
	assert(next(Pred, Next)),
	dbg_out(Pred, Next, _).

setTrueNext(Pred, Next) :-
	assert(next(Pred, Next, true)),
	dbg_out(Pred, Next, 'True').

setFalseNext(Pred, Next) :-
	assert(next(Pred, Next, false)),
	dbg_out(Pred, Next, 'False').


%%% get parent structure for break, continue, return
%	comment   
%	@tbd test hasAncestor search behavior

%% get the innermost loop surrounding the break/continue [tested]
getEnclosingLoop(BreakContinueStmt, Loop) :-
	(isBreakStatement(BreakContinueStmt); isContinueStatement(BreakContinueStmt)), !,
	hasAncestor(BreakContinueStmt, Loop), 
	isLoopStatement(Loop), !.

%% get the switch statement of the break [tested]
getEnclosingLoopOrSwitch(BreakStmt, LoopOrSwitchStmt) :-
	isBreakStatement(BreakStmt), !,
	hasAncestor(BreakStmt, LoopOrSwitchStmt),
	(isSwitchStatement(LoopOrSwitchStmt); isLoopStatement(LoopOrSwitchStmt)), !. 

%% get the enclosing function, return function definition
getEnclosingFunction(ReturnStmt, FunctionDef) :-
	isReturnStatement(ReturnStmt), !,
	hasAncestor(ReturnStmt, FunctionDef),
	isFunctionDef(FunctionDef), !.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%		
%
%	%% helper for debug
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   
%% node has two type: URL, exit(URL)
strip_exit(ExitNodeInURL, ExitNodeInID) :-
	(
		atom(ExitNodeInURL) -> get_id(ExitNodeInURL, ExitNodeInID_);
		exit(StmtURL) = ExitNodeInURL,
		get_id(StmtURL, Stmt),
		ExitNodeInID_ = exit(Stmt)
	),
	term_to_atom(ExitNodeInID_, ExitNodeAtom),
	atomic_list_concat(['"', ExitNodeAtom, '"'], ExitNodeInID).

% turn off dbg out
dbg_out(_, _, _) :- !. 

dbg_out(Pred, Next, Label) :-
	strip_exit(Pred, P), !,
	strip_exit(Next, N), !,
	write(P), write(' -> '), write(N), 
	(
		nonvar(Label) -> write('[label="'), write(Label), write('"];');
		write(';')
	), nl.
%% end helper for debug


