// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file rosehpct/util/general.cc
 *  \brief Implements a variety of miscellaneous utility functions.
 *  \ingroup ROSEHPCT_UTIL
 *
 *  $Id: general.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

#include <sstream>
#include <string>

#include "rosehpct/util/general.hh"

using namespace std;

string
GenUtil::toAddr (const void* p)
{
  ostringstream o;
  o.setf (ios_base::hex, ios_base::basefield);
  o.setf (ios_base::showbase);
  o << (unsigned long)(p);
  o.unsetf (ios_base::hex);
  o.unsetf (ios_base::basefield);
  return o.str ();
}

string
GenUtil::toFileLoc (const string& filename, size_t line_start, size_t line_end)
{
  ostringstream o;
  o << filename;
  if (line_start > 0)
    {
      o << ":" << line_start;
      if (line_end > line_start)
	o << "-" << line_end;
    }
  return o.str ();
}

string
GenUtil::tabs (size_t n, size_t tab_width)
{
  string s = "";
  for (size_t i = 0; i < n * tab_width; i++)
    s += " ";
  return s;
}

string
GenUtil::getBaseFilename (const string& full_pathname)
{
  string::size_type len = full_pathname.size ();
  string::size_type pos = full_pathname.rfind ("/", len-1);
  return (pos == string::npos)
    ? full_pathname
    : full_pathname.substr (pos+1, len-pos);
}

string
GenUtil::getBaseFilename (const char* full_pathname)
{
  return GenUtil::getBaseFilename (string (full_pathname));
}

string
GenUtil::getDirname (const string& full_pathname)
{
  string::size_type len = full_pathname.size ();
  string::size_type pos = full_pathname.rfind ("/", len-1);
  return (pos == string::npos) ? "." : full_pathname.substr (0, pos);
}

string
GenUtil::getDirname (const char* full_pathname)
{
  return GenUtil::getDirname (string (full_pathname));
}

bool
GenUtil::isSamePathname (const string& source, const string& target)
{
  string sd = GenUtil::getDirname (source);
  string sf = GenUtil::getBaseFilename (source);
  string td = GenUtil::getDirname (target);
  string tf = GenUtil::getBaseFilename (target);
  return sd == sf && td == tf;
}

bool
GenUtil::isSamePathname (const char* source, const char* target)
{
  return GenUtil::isSamePathname (string (source), string (target));
}



/* eof */
