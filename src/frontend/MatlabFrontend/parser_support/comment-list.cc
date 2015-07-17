/*

Copyright (C) 2000-2011 John W. Eaton

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


//#include "lo-utils.h"

#include "comment-list.h"
#include "error.h"

octave_comment_buffer *octave_comment_buffer::instance = 0;

octave_comment_list *
octave_comment_list::dup (void) const
{
  octave_comment_list *new_cl = new octave_comment_list ();

  for (const_iterator p = begin (); p != end (); p++)
    {
      const octave_comment_elt elt = *p;

      new_cl->append (elt);
    }

  return new_cl;
}

bool
octave_comment_buffer::instance_ok (void)
{
  bool retval = true;

  if (! instance)
    instance = new octave_comment_buffer ();

  if (! instance)
    {
      ::error ("unable to create comment buffer object");

      retval = false;
    }

  return retval;
}

void
octave_comment_buffer::append (const std::string& s,
                               octave_comment_elt::comment_type t)
{
  if (instance_ok ())
    instance->do_append (s, t);
}

octave_comment_list *
octave_comment_buffer::get_comment (void)
{
  return (instance_ok ()) ? instance->do_get_comment () : 0;
}

void
octave_comment_buffer::do_append (const std::string& s,
                                  octave_comment_elt::comment_type t)
{
  comment_list->append(s, t);
}

octave_comment_list *
octave_comment_buffer::do_get_comment (void)
{
  octave_comment_list *retval = 0;

  if (comment_list && comment_list->length () > 0)
    {
      retval = comment_list;
      comment_list = new octave_comment_list ();
    }

  return retval;
}
