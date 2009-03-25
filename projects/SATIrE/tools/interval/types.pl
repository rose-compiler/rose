%-----------------------------------------------------------------------
% types.pl
%
% About
% -----
%
% Type Definition Database
%
% Authors
% -------
%
% Copyright 2008, Adrian Prantl
%
% License
% -------
%
% This program is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; version 3 of the License.
% 
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
%-----------------------------------------------------------------------

:- module(types, [type_info/3, type_interval/2]).


% FIXME: parameterize this for the different CPU types

type_info(type_unsigned_long_long_int, unsigned, 32).
type_info(type_unsigned_long_int,      unsigned, 32).
type_info(type_unsigned_int,           unsigned, 32).
type_info(type_unsigned_short,         unsigned, 16).
type_info(type_unsigned_char,          unsigned, 8).

type_info(type_long_long_int, signed, 32).
type_info(type_long_int,      signed, 32).
type_info(type_int,           signed, 32).
type_info(type_short,         signed, 16).
type_info(type_char,          signed, 8).

type_info(typedef_type(_Name, Type), Signed, Size) :-
  type_info(Type, Signed, Size).

type_interval(Type, interval(Min, Max)) :-
  type_info(Type, Signed, Size),
  (Signed = signed
  -> (Min is -(2**Size),
      Max is (2**Size)-1)
  ;  (Min is 0,
      Max is (2**Size)-1)).

type_interval(typedef_type(_Name, Type),  Interval) :-
  type_interval(Type, Interval).