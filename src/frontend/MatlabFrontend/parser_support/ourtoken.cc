/*

Copyright (C) 1993-2011 John W. Eaton

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, see
<http://www.gnu.org/licenses/>.

*/


#include <cassert>
#include "ourtoken.h"
#include <iostream>

token::token (int l, int c)
{
  line_num = l;
  column_num = c;
  type_tag = generic_token;
}

token::token (const std::string& s, int l, int c)
{
  line_num = l;
  column_num = c;
  type_tag = string_token;
  str = new std::string (s);
}

token::token (double d, const std::string& s, int l, int c)
{
  line_num = l;
  column_num = c;
  type_tag = double_token;
  num = d;
  orig_text = s;
}

token::token (end_tok_type t, int l, int c)
{
   line_num = l;
  column_num = c;
  type_tag = ettype_token;
  et = t;
}
/*
token::token (symbol_table::symbol_record *s, int l, int c)
{
  line_num = l;
  column_num = c;
  type_tag = sym_rec_token;
  sr = s;
}

token::token (symbol_table::symbol_record *cls,
              symbol_table::symbol_record *pkg, int l, int c)
{
  line_num = l;
  column_num = c;
  type_tag = meta_rec_token;
  mc.cr = cls;
  mc.pr = pkg;
}

token::token (symbol_table::symbol_record *mth,
              symbol_table::symbol_record *cls,
              symbol_table::symbol_record *pkg, int l, int c)
{
  line_num = l;
  column_num = c;
  type_tag = scls_rec_token;
  sc.mr = mth;
  sc.cr = cls;
  sc.pr = pkg;
}
*/
token::~token (void)
{
  if (type_tag == string_token)
    delete str;
}


std::string
token::text (void)
{
  assert (type_tag == string_token);
  return *str;
}

double
token::number (void)
{
  assert (type_tag == double_token);
  return num;
}

token::end_tok_type
token::ettype (void)
{
  assert (type_tag == ettype_token);
  return et;
}

/*
symbol_table::symbol_record *
token::sym_rec (void)
{
  assert (type_tag == sym_rec_token);
  return sr;
}

symbol_table::symbol_record *
token::method_rec (void)
{
  assert (type_tag == scls_rec_token);
  return sc.mr;
}

symbol_table::symbol_record *
token::class_rec (void)
{
  assert (type_tag == scls_rec_token);
  return sc.cr;
}

symbol_table::symbol_record *
token::package_rec (void)
{
  assert (type_tag == scls_rec_token);
  return sc.pr;
}

symbol_table::symbol_record *
token::meta_class_rec (void)
{
  assert (type_tag == meta_rec_token);
  return mc.cr;
}

symbol_table::symbol_record *
token::meta_package_rec (void)
{
  assert (type_tag == meta_rec_token);
  return mc.pr;
}
*/
std::string
token::text_rep (void)
{
  return orig_text;
}
