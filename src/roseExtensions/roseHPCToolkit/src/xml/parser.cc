// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file rosehpct/xml/parser.cc
 *  \brief XML parser.
 *  \ingroup ROSEHPCT_LIBXML2
 *
 *  $Id: parser.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
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

bool XMLUtil::XMLParser::did_init_ = false;

XMLUtil::XMLParser::XMLParser (void)
  : did_parse_{false}, doc_{nullptr}, handler_{nullptr}
{
  init ();
}

XMLUtil::XMLParser::XMLParser (const char* infile)
  : did_parse_{false}, doc_{nullptr}, handler_{nullptr}
{
  init ();
  parse (infile);
}

XMLUtil::XMLParser::XMLParser (const string& infile)
  : did_parse_{false}, doc_{nullptr}, handler_{nullptr}
{
  init ();
  parse (infile);
}

XMLUtil::XMLParser::~XMLParser (void)
{
  if (doc_ != nullptr)
    xmlFreeDoc (doc_);
  if (handler_ != nullptr)
    delete handler_;
}

void
XMLUtil::XMLParser::init (void)
{
  did_init_ = true;
}

bool
XMLUtil::XMLParser::didParse (void) const
{
  return did_parse_;
}

bool
XMLUtil::XMLParser::parse (const string& infile)
{
  return parse (infile.c_str ());
}

extern "C"
{
//! SAX callback at start of document
static
void
startDoc_(void*)
{
}

//! SAX callback at end of document
static
void
endDoc_(void*)
{
}

//! SAX callback at start of an XML tag
static
void
startElem_ (void* user_data, const xmlChar* name, const xmlChar** attrs)
{
  XMLHandler* handler = (XMLHandler *)user_data;
  handler->startElement (name, attrs);
}

//! SAX callback at end of an XML tag
static
void
endElem_ (void* user_data, const xmlChar* name)
{
  XMLHandler* handler = (XMLHandler *)user_data;
  handler->endElement (name);
}

} // extern "C"

//! List of SAX callbacks
static xmlSAXHandler sax_handlers_ = {
    .internalSubset = nullptr,
    .isStandalone = nullptr,
    .hasInternalSubset = nullptr,
    .hasExternalSubset = nullptr,
    .resolveEntity = nullptr,
    .getEntity = nullptr,
    .entityDecl = nullptr,
    .notationDecl = nullptr,
    .attributeDecl = nullptr,
    .elementDecl = nullptr,
    .unparsedEntityDecl = nullptr,
    .setDocumentLocator = nullptr,
    .startDocument = startDoc_,
    .endDocument = endDoc_,
    .startElement = startElem_,
    .endElement = endElem_,
    .reference = nullptr,
    .characters = nullptr,
    .ignorableWhitespace = nullptr,
    .processingInstruction = nullptr,
    .comment = nullptr,
    .warning = nullptr,
    .error = nullptr,
    .fatalError = nullptr,
    .getParameterEntity = nullptr
};

bool
XMLUtil::XMLParser::parse (const char* infile)
{
  did_parse_ = false;
  if (infile != nullptr)
    {
      handler_ = new XMLHandler;
      if (xmlSAXUserParseFile (&sax_handlers_, (void *)handler_, infile) >= 0)
        did_parse_ = true;
    }
  return did_parse_;
}

const XMLUtil::XMLHandler::node_t *
XMLUtil::XMLParser::getDoc (void) const
{
  return (didParse() && handler_ != nullptr) ? handler_->getRoot () : nullptr;
}

/* eof */
