:- module(variable, [
	variable/1,
	variableRef/1,
	get_varDecl/2,
	roseHasInitValue/2
]).

:- use_module(basic).

% find all variables: initialized name or declared variable
variable(Resource) :-
	c_is_a(Resource, 'Variable').

% find all variable references
variableRef(Resource) :-
	c_is_a(Resource, 'VariableRef').

get_varDecl(VarRefURL, VarDefURL) :-
	c_has(referTo, VarRefURL, VarDefURL).

%% 
%% get the entity by its name
getVariable(Name, Resource) :-
	get_by_name(Name, Resource),
	c_is_a(Resource, 'Variable').

getVariableRef(Name, Resource) :-
	get_by_name(Name, Resource),
	c_is_a(Resource, 'VariableRef').

getVariableDef(Name, Resource) :-
	get_by_name(Name, Resource),
	c_is_a(Resource, isAssignedTo).


%%	Rose compiler specific
%
%	Based on current rose AST structure
%
%	@tbd: more general rules

roseHasInitValue(Variable, ValueURL) :-
	hasChild(Variable, AssignInitializer),
	hasChild(AssignInitializer, ValueURL).
