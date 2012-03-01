#!/usr/bin/pl -t testrun -f
% -*- mode: prolog; truncate-lines: true; -*-

:- module(markers, [update_marker_info/5]).

%-----------------------------------------------------------------------
/** <module> Properties of abstract syntax trees

This module defines commonly-used transformation utilities for
C/C++/Objective C ASTs given in the TERMITE term representation
as exported by SATIrE.

@version   @PACKAGE_VERSION@
@copyright Copyright (C) 2007-2009 Adrian Prantl
@author    Adrian Prantl <adrian@complang.tuwien.ac.at>
@license   See COPYING in the root folder of the SATIrE project

*/
%-----------------------------------------------------------------------


%% update_marker_info
% Encode the FunctionName into the marker
update_marker_info(Info, Info, InfoInner, FunDecl, NewStem) :-
  function_signature(FunDecl, _Type, Name, _Modifier), !,
  %Info = info(marker_stem(Stem), marker_count(Count)),
  %atomic_list_concat([Stem, '_', Name], NewStem),
  % Fixme.. include module too
  Name=NewStem,
  Count=1,
  InfoInner = info(marker_stem(NewStem), marker_count(Count)).

update_marker_info(Info, InfoInner, InfoPost, basic_block(_,_,_,_), Marker) :- 
  !, 
  % Generate the Marker
  %writeln(Info), nl, gtrace,
  Info = info(marker_stem(Stem), marker_count(Count)),
  %writeln(Count),
  atomic_list_concat([Stem, '_', Count], Marker),

  % Prepare the Marker for INNER
  StemInner = Marker,
  InfoInner = info(marker_stem(StemInner), marker_count(1)),

  % Prepare the Marker for POST
  Count1 is Count + 1,
  InfoPost = info(marker_stem(Stem), marker_count(Count1)).

update_marker_info(I, I, I, _, Marker) :-
    I = info(marker_stem(Stem), marker_count(Count)),
    % Fixme: performance.. not needed every time
    atomic_list_concat([Stem, '_', Count], Marker).

