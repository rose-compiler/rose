// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file rosehpct/xml/handler.cc
 *  \brief Call-back handler for XML parser.
 *  \ingroup ROSEHPCT_LIBXML2
 *
 *  $Id: handler.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>

#include "rosehpct/util/general.hh"
#include "rosehpct/xml/xml.hh"

using namespace std;
using namespace GenUtil;
using namespace XMLUtil;

XMLUtil::XMLHandler::XMLHandler (void)
  : root_ (NULL), level_(0)
{
  level_ = 0;

  XMLElem root ("DOCROOT");
  root_ = new node_t (root);
  elements_.push (root_);
}

XMLUtil::XMLHandler::~XMLHandler (void)
{
  /*! \todo POSSIBLE MEMORY LEAK: root_ */
}

void
XMLUtil::XMLHandler::startElement (const XMLCh* const name,
                                   ConstAttributeList attr)
{
  XMLElem e (toString (name), attr);
  level_++;

  node_t* parent = elements_.top ();
  node_t::Ind_t id_child = parent->getNumChildren () + 1;
  node_t* child = parent->setChildValue (id_child, e);
  elements_.push (child);
}

void
XMLUtil::XMLHandler::endElement (const XMLCh* const)
{
  level_--;
  elements_.pop ();
}

const XMLUtil::XMLHandler::node_t *
XMLUtil::XMLHandler::getRoot (void) const
{
  return root_;
}

/* eof */
