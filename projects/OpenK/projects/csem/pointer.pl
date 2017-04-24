:- module(pointer, [

]).


:- use_module(basic).
:- use_module(variable).
:- use_module(type).

%% type *ptr
pointerVar(PointerVarURL, Description) :-
	variable(PointerVarURL),
	hasType(PointerVarURL, pointer_type),
	get_name(PointerVarURL, VarName),
	(hasBaseType(PointerVarURL, BaseType); BaseType = ''),
	Description = [name(VarName), baseType(BaseType)].