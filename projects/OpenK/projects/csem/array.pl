%% 
:- module(array, [
	arrayDecl/2
]).

:- use_module(basic).
:- use_module(variable).
:- use_module(type).


%%	array
%
%	The array patterns
%
%	@tbd	

%% the array declaration
%% type array[size]
arrayDecl(ArrayVar, Description) :-
	variable(ArrayVar),
	hasType(ArrayVar, array_type),
	get_name(ArrayVar, ArrayName),
	(hasBaseType(ArrayVar, BaseType); BaseType = ''),
	Description = [name(ArrayName), baseType(BaseType)].

%% ArraySub: 
%% V_SgPntrArrRefExp
%% V_SgSubscriptExpression

%% find array expr and return its array var and index 
%% array[index] ...
%% ?ArraySub, ?ArrayRef, ?ArrayIndex
arrayRef(ArraySub, ArrayVarRef, ArrayIndex) :- 
	c_is_a(ArraySub, 'ArraySub'),
	leftOperand(ArraySub, ArrayVarRef),
	rightOperand(ArraySub, ArrayIndex).



%%	some specific analysis
%	thread T
%	for N
%		for M
%			C[] = A[] + B[]
%
%	@tbd	
%% stmtFromPattern(Stmt, Pattern)
%% inNestedLoops()



