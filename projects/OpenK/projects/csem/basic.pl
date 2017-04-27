:- module(basic, [
	c_is_a/2,
	c_is_one_of/2,
	c_get_instance/2,
	c_has/3,

	hasChild/2,
	hasParent/2,
	leftOperand/2,
	rightOperand/2,
	hasOperand/2,
	hasAncestor/2,

	get_by_name/2,
	get_by_name/3,
	get_name/2,

	c_name2url/2, 
	show_id/2, 
	show_id/1, 
	get_id/2

]).

:- use_module(library(semweb/rdf_db)).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%		
%
%	The low level APIs
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% (+Concept, ?IndURL)
c_get_instance(Concept, IndURL) :-
	atom(Concept), !,
	c_name2url(Concept, ConceptURL), 
	rdf(IndURL, rdf:type, ConceptURL).

%% (?IndURL, ?Concept)
c_is_a(IndURL, Concept) :-
	rdf(IndURL, rdf:type, ConceptURL),
	c_name2url(Concept, ConceptURL).

%% (?IndURL, +ConceptList)
c_is_one_of(IndURL, ConceptList) :-
	c_is_a(IndURL, Concept),
	memberchk(Concept, ConceptList), !.


%% (+Property, ?SubURL, ?ObjURL)
c_has(Property, SubURL, ObjURL) :-
	atom(Property), !,
	c_name2url(Property, PropertyURL),
	rdf(SubURL, PropertyURL, ObjURL).


hasChild(ParentURL, ChildURL) :-
	c_has(hasParent, ChildURL, ParentURL), ChildURL \== ParentURL.

hasParent(ChildURL, ParentURL) :-
	c_has(hasParent, ChildURL, ParentURL), ChildURL \== ParentURL.

leftOperand(BinaryOpURL, LeftOperandURL) :-
	c_has(hasLeftOperand, BinaryOpURL, LeftOperandURL).

rightOperand(BinaryOpURL, RightOperandURL) :-
	c_has(hasRightOperand, BinaryOpURL, RightOperandURL).

hasOperand(UnaryOpURL, OperandURL) :-
	c_has(hasOperand, UnaryOpURL, OperandURL).

%% recursively find the ancestor
hasAncestor(ThisURL, AncestorURL) :-
	hasParent(ThisURL, AncestorURL).

hasAncestor(ThisURL, AncestorURL) :-
	hasParent(ThisURL, ParentURL),
	hasAncestor(ParentURL, AncestorURL).


%% get an entity by its name 
%% (?Name, ?Resource)
get_by_name(Name, Resource) :-
	c_name2url(hasName, HasNameProperty),
	rdf(Resource, HasNameProperty, literal(Name)).

%% get an entity of Class by the name 
%% (?Name, +Class, ?Resource)
get_by_name(Name, Class, Resource) :-
	get_by_name(Name, Resource),
	c_name2url(Class, ClassURL),
	c_is_a(Resource, ClassURL).

%% get the entity's name (hasName property)
get_name(NamedURL, Name) :-
	get_by_name(Name, NamedURL).


%%	Convert a C concept name to its full url 
%
%	e.g., ID 'Variable'
%   one of ?ID, ?URL must be instantiated
%
%	@tbd	Better error handling 
%   rdf_current_prefix (?Alias, ?URI): query predefined prefixes	
c_name2url(ID, URL) :-
	(
		rdf_current_prefix(c, Prefix); 
		writeln('namespace c is NOT registered!')
	), !, atom_concat(Prefix, ID, URL).

show_id(IndURL, ID) :-
	get_id(IndURL, ID),
	writeln(ID).

show_id(IndURL) :-
	show_id(IndURL, _).

%% shorten an url with the format `http://...#id` to id
%% (+URL -ID)
get_id(URL, ID) :-
	(
		atom(URL) -> 
		atomic_list_concat([_, LocalName], '#', URL),
		ID = LocalName;
		writeln('get_id::URL is not atom'),
		fail
	).


%% strip_spaces(S, NoSpaces) :-
%%     atomic_list_concat(L, ' ', S),
%%     atomic_list_concat(L, NoSpaces).
