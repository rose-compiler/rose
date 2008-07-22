// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file rosehpct/xml/elem.cc
 *  \brief Implement value classes that represent XML tags.
 *  \ingroup ROSEHPCT_LIBXML2
 *
 *  $Id: elem.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

#include "rosehpct/xml/xml.hh"
#include <algorithm>

using namespace std;
using namespace XMLUtil;

XMLUtil::XMLAttr::XMLAttr (const string& n, const string& t,
			   const string& v)
  : name (n), type (t), value (v)
{
}

XMLUtil::XMLAttr::XMLAttr (const XMLAttr& a)
  : name (a.name), type (a.type), value (a.value)
{
}

XMLUtil::XMLAttr::MatchName::MatchName (const std::string& n)
  : name(n)
{
}

bool
XMLUtil::XMLAttr::MatchName::operator() (const XMLAttr& a)
{
  return a.name == name;
}

/* ---------------------------------------------------------------- */

XMLUtil::XMLElem::XMLElem (void)
{
}

XMLUtil::XMLElem::XMLElem (const string& n, ConstAttributeList attr)
  : name (n)
{
  if (attr != NULL)
    {
      size_t i = 0;
      while (attr[i] != NULL)
        {
          string tag ((const char *)attr[i]);
          string value ((const char *)attr[i+1]);
          
          XMLAttr a_i (tag, string ("TEXT"), value);
          attributes.push_back (a_i);
          
          i += 2;
        }
    }
}

XMLUtil::XMLElem::XMLElem (const string& n)
  : name (n)
{
}

XMLUtil::XMLElem::XMLElem (const XMLElem& e)
  : name (e.name), attributes (e.attributes)
{
}

string
XMLUtil::XMLElem::getAttr (const string& n) const
{
  vector<XMLAttr>::const_iterator i =
    find_if (attributes.begin (), attributes.end (), XMLAttr::MatchName (n));
  return (i != attributes.end ()) ? i->value : "";
}

XMLUtil::XMLElem::MatchName::MatchName (const std::string& n)
  : name(n)
{
}

bool
XMLUtil::XMLElem::MatchName::operator() (const XMLElem& e)
{
  return name == e.name;
}

ostream &
XMLUtil::operator<< (ostream& o, const XMLElem& e)
{
  bool is_first = true;
  o << "[element] '" << e.name << "' := (";
  for (vector<XMLAttr>::const_iterator a_i = e.attributes.begin ();
       a_i != e.attributes.end ();
       ++a_i)
    {
      if (!is_first)
	o << ", ";
      else
	is_first = false;

      o << a_i->name
	<< " = '" << a_i->value << "'"
	<< " : <" << a_i->type << ">";
    }
  o << ")";
  return o;
}

/* ---------------------------------------------------------------- */

XMLUtil::XMLElemFinder::XMLElemFinder (void)
{
}

XMLUtil::XMLElemFinder::XMLElemFinder (const string& elem_name,
			      const XMLHandler::node_t* doc)
{
  setElemNamePattern (elem_name);
  traverse (doc);
}

void
XMLUtil::XMLElemFinder::visit (const XMLHandler::node_t* cur_node)
{
  const XMLElem& e = cur_node->value;
  if (pattern_elem_name_.length () == 0
      || pattern_elem_name_ == e.name)
    matches_.push_back (e);
}

size_t
XMLUtil::XMLElemFinder::getNumMatches (void) const
{
  return matches_.size ();
}

void
XMLUtil::XMLElemFinder::setElemNamePattern (const string& elem_name)
{
  pattern_elem_name_ = elem_name;
}

XMLUtil::XMLElemFinder::ElemList_t::const_iterator
XMLUtil::XMLElemFinder::begin (void) const
{
  return matches_.begin ();
}

XMLUtil::XMLElemFinder::ElemList_t::const_iterator
XMLUtil::XMLElemFinder::end (void) const
{
  return matches_.end ();
}

/* eof */
