// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file src/xml-xercesc/util.cc
 *  \brief Miscellaneous helper routines.
 *  \ingroup ROSEHPCT_XERCESC
 *
 *  $Id: util.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>

#include "rosehpct/util/general.hh"
#include "rosehpct/xml-xercesc/xml.hh"

using namespace std;
using namespace GenUtil;
using namespace XMLUtil;

ostream &
XMLUtil::operator<< (ostream& o, const XMLCh* const s)
{
  return o << toString (s);
}

/* ---------------------------------------------------------------- */

/*!
 *  \brief Print an XML tree (for debugging purposes only).
 */
class XMLTreePrint : public Trees::PreTraversal<XMLHandler::node_t>
{
public:
  /*! \brief Initialize to print to the specified output stream. */
  XMLTreePrint (ostream& o);

  /*! \brief Called on each node of the tree. */
  virtual void visit (const XMLHandler::node_t* cur_node);

protected:
  /*! \brief Returns the current depth. */
  size_t getDepth (void) const;
  /*! \brief Increment the current depth. */
  virtual void descending (void);
  /*! \brief Decrement the current depth. */
  virtual void ascending (void);

private:
  ostream& out_; //!< Output stream.
  size_t level_; //!< Current depth in the traversal, used for indenting.
};

XMLTreePrint::XMLTreePrint (ostream& o)
  : out_(o), level_(0)
{
}

void
XMLTreePrint::descending (void)
{
  level_++;
}

void
XMLTreePrint::ascending (void)
{
  level_--;
}

size_t
XMLTreePrint::getDepth (void) const
{
  return level_;
}

void
XMLTreePrint::visit (const XMLHandler::node_t* cur_node)
{
  out_ << tabs (getDepth ()) << cur_node->value << endl;
}

ostream &
XMLUtil::operator << (ostream& o, const XMLHandler::node_t* root)
{
  XMLTreePrint printer (o);
  printer.traverse (root);
  return o;
}

/* ---------------------------------------------------------------- */

string
XMLUtil::toString (const XMLCh* const name)
{
  char* name_x = XMLString::transcode (name);
  string s = name_x;
  XMLString::release (&name_x);
  return s;
}

/* eof */
