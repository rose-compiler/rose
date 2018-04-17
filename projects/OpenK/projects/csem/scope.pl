%%
:- module(scope, [
	isScope/1,
	inScope/2,
	getDirectScope/2
]).

:- use_module(basic).
:- use_module(variable).
:- use_module(loop).
:- use_module(function).

%%% scope struct
%	incomplete
%	to cover more cases
isScope(Resouce) :- 
	forLoop(Resouce), !;
	forBody(Resouce), !;
	isFunctionDecl(Resouce), !;
	isFunctionDef(Resouce), !. 
	
%% find/check scope of a variable decl
%% +, ?
inScope(Variable, Scope) :-
	variable(Variable), 
	c_has(hasScope, Variable, Scope), 
	isScope(Scope), !.

%% check/find scope of a variable reference
%% +, +
inScope(VariableRef, Scope) :-
	variableRef(VariableRef),
	getDirectScope(VariableRef, Scope), !.

%% check if a scope is in another scope 
%% inScope(Scope, Scope).

inScope(Scope, SuperScope) :-
	isScope(Scope), isScope(SuperScope),
	inScope_(Scope, SuperScope).

%% helper rules
inScope_(Scope, SuperScope) :-
	c_has(hasParent, Scope, SuperScope), !.

inScope_(Scope, SuperScope) :-
	c_has(hasParent, Scope, ParentScope),
	inScope_(ParentScope, SuperScope).

%% +, ?
getDirectScope(Exp, Scope) :-
	c_has(hasParent, Exp, Scope),
	isScope(Scope), !.

getDirectScope(Exp, Scope) :-
	c_has(hasParent, Exp, Inter),
	getDirectScope(Inter, Scope).



%%	basic block
%
%	basic block as in CFG
%
%	@tbd	


