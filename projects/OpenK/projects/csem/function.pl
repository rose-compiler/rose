:- module(function, [
	isFunctionDecl/1,
	isFunctionDef/1,
	functionBodyOf/2,

	getFunctionDeclByName/2,
	getFunctionDefByName/2
]).


:- use_module(basic).

%% only consider function with body
%% pure declaration has bug in Rose
isFunctionDecl(Resource) :-
	c_is_a(Resource, 'FunctionDecl'),
	c_has(hasDefinition, Resource, _).

isFunctionDef(Resource) :-
	c_is_a(Resource, 'FunctionDefinition').

%% get function body either by given the function declaration or function definition
%% see document.txt for the function structure
functionBodyOf(FunctionURL, BodyURL) :-
	(
		isFunctionDecl(FunctionURL) -> c_has(hasDefinition, FunctionURL, FunctionDefURL);
		isFunctionDef(FunctionURL) -> FunctionDefURL = FunctionURL;
		writeln('functionBodyOf: not function'), fail
	), c_has(hasBody, FunctionDefURL, BodyURL).
	

isFunctionCallSite(Resource) :-
	c_is_a(Resource, 'FunctionCall').

isFunctionCallee(Resource) :-
	c_is_a(Resource, 'FunctionRef').

hasParameter(Function, Nth, OutPara) :-
	isFunctionDecl(Function),
	c_has(hasParameter, Function, OutPara),
	c_has('NthPara', OutPara, literal(Nth)).

hasArgument(FunctionCall, Nth, OutArg) :-
	isFunctionCallSite(FunctionCall),
	c_has(hasArgumentExpr, FunctionCall, OutArg),
	c_has('NthArg', OutArg, literal(Nth)).


%% 
%% get the entity by its name 
getFunctionDeclByName(Name, Resource) :-
	get_by_name(Name, Resource),
	isFunctionDecl(Resource).

getFunctionDefByName(Name, FunctionDefURL) :-
	getFunctionDeclByName(Name, FunctionDeclURL),
	c_has(hasDefinition, FunctionDeclURL, FunctionDefURL).

getFunctionBodyByName(Name, FunctionBodyURL) :-
	getFunctionDefByName(Name, FunctionDefURL),
	c_has(hasBody, FunctionDefURL, FunctionBodyURL).

