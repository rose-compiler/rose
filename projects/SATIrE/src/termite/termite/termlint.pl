:- module(termlint,
	  [term_match/2,
	   start_symbol/1,
          % operator declarations for great prettiness
	   op(1200, xfx, ::=),      % grammar rule
	   op(754,   fy, atoms),    % enumeration of terminal atoms
	   op(754,   fy, functors), % enumeration of terminal functors...
	   op(753,  yfy, with),	    % ... with identical argument structure
	   op(752,  xfx, where),    % semantic constraints
	   op(751,  xf,	 ?)	    % optional occurrence of nonterminal
	  ]).

%-----------------------------------------------------------------------
/** <module> Term type checker

  This is term_lint.pl, a small tool for checking terms against a tree
  grammar (abstract syntax).

@version   @PACKAGE_VERSION@
@copyright Copyright (C) 2009 Gergo Barany
@author    Gergo Barany <gergo@complang.tuwien.ac.at>
@license

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 3 of the License.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

*/
%-----------------------------------------------------------------------

% TODO: @ operator for as-patterns to allow more complex semantic
% constraints; example:  foo ::= foo(X@bar) where condition(X).


% This is the grammar specification we use. Its name is hard-coded here,
% which is fine for our single use case.
:- consult('termite_spec.pl').


% The tree grammar is a sequence of rules. This predicate defines what a
% rule is. The predicate itself is not called by term_lint, but it's a nice
% executable specification.
is_rule(Nonterminal ::= Body) :-
    atom(Nonterminal),
    is_body(Body).

is_body(Atom) :-
    atom(Atom).
is_body(A | B) :-
    is_body(A),
    is_body(B).
is_body(Body where _Cond) :-
    is_body(Body).  % and Cond is a prolog goal
is_body({_}).  % unify body with term in braces
is_body(atoms As) :-
    maplist(atom, As).
is_body(functors Fs with ArgTuple) :-
    maplist(atom, Fs),
    atomtuple_list(ArgTuple, Args),
    maplist(is_arg, Args).
is_body(Term) :-
    Term =.. [_Functor | Args],
    maplist(is_arg, Args).

atomtuple_list(A, [A]) :-
    atom(A).
atomtuple_list((A,B), [A|Bs]) :-
    atomtuple_list(B, Bs).

is_arg(Atom) :-
    atom(Atom).
is_arg(Option?) :-
    is_arg(Option).
is_arg({_}).  % unify arg with term in braces
is_arg([ElemKind]) :-
    is_arg(ElemKind).


% Obscure implementation predicates follow. No need to read these, I think.
% (There is a main predicate at the very bottom.)

%% term_match(+Term, +NonterminalSymbol).
%
% OK. Here is the semantics to the structures defined above:
%
% The grammar is a sequence of grammar rules. Each rule is of the form
% Nonterminal ::= Body.
% There should be no two rules for the same nonterminal (use =|||= instead), but
%   this is not checked.
%
% Body has the following meaning:
%
%     *  atom
%        a nonterminal, references another rule to be used
%     *  A | B
%        match A; if that fails, match B
%     *  A where C
%        match A; if that succeeds, call Prolog goal C
%     *  {A}
%        unify with term A; {_} means "any term", {foo} is terminal foo
%     *  atoms As
%        match one of the atoms in As
%     *  functors Fs with (A1,...,An)
%        match term F(A1,...,An) where F is a functor in Fs
%     *  f(A1,...,An)
%        match a term with functor f, where arguments match A1,...,An
%
%
% Argument expressions (in argument tuples) can be:
%
%     *  atom
%        a nonterminal
%     *  A?
%        term is "missing" (see below) or matches argument expression A
%     *  {A}
%        unify with term A
%     *  [A]
%        list of terms matching argument expression A
%
% As a special case, =|[_]|= means "list of any type".
%
% Options (A?) are resolved as follows: If the term under consideration
% is a solution of predicate missing/1 (to be defined along with the grammar
% rules), there is a match; otherwise a term of type A must be matched.
%
% Predicates to be defined along with the grammar rules are:
%
%     *  missing/1
%        defines what A? can match except for A
%     *  start_symbol/1
%        single solution is the start symbol of the grammar
%
%
% Here are a few example grammars to illustrate the explanations above.
%
%
% *|Arithmetic expressions, simple verbose version:|*
%     ==
%     var ::= {VarName} where atom(VarName).
%     num ::= {Number} where number(Number).
%     expr ::=
%       var
%     | num
%     | expr + expr
%     | expr - expr
%     | expr * expr
%     | expr / expr.
%     ==
%
% *|Arithmetic expressions, more condensed version:|*
%     ==
%     expr ::=
%       {Leaf} where (atom(Leaf) ; number(Leaf))
%     | functors [+, -, *, /] with (expr, expr).
%     ==
%
% *Simple type system*:
%     ==
%     type ::=
%       atoms [number, character, string]
%     | function_type([type] /* argument types */, type? /* return type if any */).
%
%     missing(none).    /* what to match if no return type is given */
%     ==
%
% *|Partial specification (some parts are not constrained):|*
%     ==
%     allowed ::=
%       lst([_])        /* argument is list of some unknown things */
%     | opt(allowed?)   /* argument is missing, or of type allowed */
%     | any({_}).       /* argument is any term (anything unifies with _) */
%
%     missing(nothing).
%     ==
%
% In this last example, some allowed terms are:
%     ==
%       lst([]), lst([f(f)]), lst([x,y]), ...
%       opt(nothing), opt(lst([])), opt(opt(nothing)), ...
%       any(foo), any(g(f(x))), ...
%     ==
%
% One more thing, which might not be explicit from the stuff above: The
% anonymous variable _ (free variables in general) may appear in the
% grammar, but *only* inside [] or {}:
%
%   *  [_]
%      (list of any type)   or
%   *  {_}
%      (any term)   or
%   *  {foo(_)}
%      (any term with functor foo)
%
% but:
%
%   *  foo ::= foo(_)
%      is not allowed; use =|foo ::= foo({_})|=
%   *  foo ::= X where cond(X)
%      is not allowed;
%      use =|foo ::= {X} where cond(X)|=
%
% The interpreter does not check these things at the moment. Which means
% that grammars containing variables in weird places will misbehave in weird
% ways.
term_match(Term, NonterminalSymbol) :-
    nonterminal_body(NonterminalSymbol, Body),
    catch(term_nt_bodymatch(Term, NonterminalSymbol, Body),
          ErrorTerm,
          term_match_error(ErrorTerm)).

%% nonterminal_body(+NonterminalSymbol, -Body).
nonterminal_body(NonterminalSymbol, Body) :-
    ( clause(NonterminalSymbol ::= Body, true)
      -> true
      ;  throw(error(no_such_nonterminal, nonterminal(NonterminalSymbol))) ).

% wrapper for term_nt_bodymatch_; generates error if no case matches
term_nt_bodymatch(Term, Nonterminal, Body) :-
    term_nt_bodymatch_(Term, Nonterminal, Body),
    !.
term_nt_bodymatch(Term, Nonterminal, Body) :-
    throw(error(nonterminal_does_not_match,
                nt_body_term(Nonterminal, Body, Term))).

% this does the real work of recursively checking the rule body against the
% term; typically, we want this to fail silently, and the wrapper that calls
% it will generate an error message
term_nt_bodymatch_(Term, _NT, Nonterminal) :-
    atom(Nonterminal),
    nonterminal_body(Nonterminal, Body),
    term_nt_bodymatch_(Term, Nonterminal, Body).
term_nt_bodymatch_(Term, NT, A | _B) :-
    term_nt_bodymatch_(Term, NT, A),
    !.
term_nt_bodymatch_(Term, NT, _A | B) :-
    !,
    term_nt_bodymatch_(Term, NT, B).
term_nt_bodymatch_(Term, NT, T where Cond) :-
    !,
    nonvar(T), % grammar sanity check
    term_nt_bodymatch_(Term, NT, T),
    ( call(Cond)
    ; throw(error(condition_failed, cond_nt_term(Cond, NT, Term))) ).
term_nt_bodymatch_(Term, _NT, {T}) :-
    Term = T,
    !.
term_nt_bodymatch_(Term, _NT, atoms Fs) :-
    atom(Term),
    member(Term, Fs).
term_nt_bodymatch_(Term, _NT, functors Fs with ArgTuple) :-
    functor(Term, F, _Arity),
    member(F, Fs),
    atomtuple_list(ArgTuple, ArgList),
    CheckTerm =.. [F | ArgList],
    term_terminalmatch(Term, CheckTerm).
term_nt_bodymatch_(Term, _NT, T) :-
    \+ atom(T),
    term_terminalmatch(Term, T).

term_terminalmatch(Term, T) :-
    Term =.. [Functor | TermArgs],
    T =.. [Functor | ProductionArgs],
    maplist(nonvar, ProductionArgs), % grammar sanity check
    length(TermArgs, N),
    length(ProductionArgs, N),
    ( maplist(term_argumentmatch, TermArgs, ProductionArgs)
    ; throw(error(argument_match_failure,
                  functor_args_nts(Functor, TermArgs, ProductionArgs))) ).

term_argumentmatch(Term, A) :-
    atom(A),
    nonterminal_body(A, Body),
    term_nt_bodymatch(Term, A, Body).
term_argumentmatch(Term, {A}) :-
    term_nt_bodymatch(Term, '{}', {A}).
term_argumentmatch(Term, _A?) :-
    missing(Term),
    !.
term_argumentmatch(Term, A?) :-
    term_argumentmatch(Term, A).
term_argumentmatch(Term, [A]) :-
    is_list(Term),
    ( var(A) ; forall(member(T, Term), term_argumentmatch(T, A)) ).

body_alternatives(A | B, FA/N | Alts) :-
    !,
    ( A = (A1 where _C)
      -> functor(A1, FA, N)
      ;  functor(A, FA, N) ),
    body_alternatives(B, Alts).
body_alternatives(X, FX/N) :-
    functor(X, FX, N).

term_match_error(error(nonterminal_does_not_match, nt_body_term(N, B, T))) :-
    !,
    body_alternatives(B, Alternatives),
    functor(T, F, Arity),
    die('nonterminal "~w" with alternatives "~w" does not match ~w term',
        [N, Alternatives, F/Arity]).
term_match_error(error(argument_match_failure,functor_args_nts(F,As,NTs))) :-
    !,
    maplist(functor, As, Args, _Arities),
    die('~w term: could not match arguments ~w against ~w~n',
        [F, Args, NTs]).
term_match_error(error(no_such_nonterminal, nonterminal(NT))) :-
    !,
    die('internal error: nonterminal ~w is not defined', [NT]).
term_match_error(error(condition_failed, cond_nt_term(C, NT, T))) :-
    !,
    functor(T, Functor, Arity),
    die('for ~w ~w: condition ~w failed', [NT, Functor/Arity, C]).
term_match_error(ErrorTerm) :-
    !,
    die('error term: ~w', [ErrorTerm]).

die(Format, Args) :-
    format(atom(A), Format, Args),
    format('**ERROR termlint:~n    ~w~n', A),
    !,
    fail.

