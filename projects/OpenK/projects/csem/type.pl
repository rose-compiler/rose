:- module(type, [ 
	hasType/2,
	hasBaseType/2,
	hasTypeIn/2,
	intType/1 
]).

:- use_module(basic).


hasType(Resource, Type) :-
	c_has(hasType, Resource, TypeURL),
	get_id(TypeURL, Type).

%% array
hasBaseType(VarURL, Type) :-
	c_has(hasArrayBaseType, VarURL, TypeURL),
	get_id(TypeURL, Type), !.

%% pointer
hasBaseType(PtrVar, Type) :-
	c_has(hasBaseType, PtrVar, TypeURL),
	get_id(TypeURL, Type), !.

%% check if +Resource has one of the type in +TypeList
hasTypeIn(Resource, TypeList) :-
	hasType(Resource, Type),
	memberchk(Type, TypeList).

intType(Type) :-
	memberchk(Type, 
		[int, long, long_double, long_long, short,
        signed_char, signed_int, signed_long, signed_long_long, signed_short,
        unsigned_char, unsigned_int, unsigned_long, unsigned_long_long, unsigned_short]).