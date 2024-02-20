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
  : did_parse_ (false), doc_ (NULL), handler_ (NULL)
{
  init ();
}

XMLUtil::XMLParser::XMLParser (const char* infile)
  : did_parse_ (false), doc_ (NULL), handler_ (NULL)
{
  init ();
  parse (infile);
}

XMLUtil::XMLParser::XMLParser (const string& infile)
  : did_parse_ (false), doc_ (NULL), handler_ (NULL)
{
  init ();
  parse (infile);
}

XMLUtil::XMLParser::~XMLParser (void)
{
  if (doc_ != NULL)
    xmlFreeDoc (doc_);
  if (handler_ != NULL)
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
static
xmlSAXHandler sax_handlers_ =
  {
    NULL, //internalSubsetSAXFunc internalSubset;
    NULL, //isStandaloneSAXFunc isStandalone;
    NULL, //hasInternalSubsetSAXFunc hasInternalSubset;
    NULL, //hasExternalSubsetSAXFunc hasExternalSubset;
    NULL, //resolveEntitySAXFunc resolveEntity;
    NULL, //getEntitySAXFunc getEntity;
    NULL, //entityDeclSAXFunc entityDecl;
    NULL, //notationDeclSAXFunc notationDecl;
    NULL, //attributeDeclSAXFunc attributeDecl;
    NULL, //elementDeclSAXFunc elementDecl;
    NULL, //unparsedEntityDeclSAXFunc unparsedEntityDecl;
    NULL, //setDocumentLocatorSAXFunc setDocumentLocator;
    startDoc_, //startDocumentSAXFunc startDocument;
    endDoc_, //endDocumentSAXFunc endDocument;
    startElem_, //startElementSAXFunc startElement;
    endElem_, //endElementSAXFunc endElement;
    NULL, //referenceSAXFunc reference;
    NULL, //charactersSAXFunc characters;
    NULL, //ignorableWhitespaceSAXFunc ignorableWhitespace;
    NULL, //processingInstructionSAXFunc processingInstruction;
    NULL, //commentSAXFunc comment;
    NULL, //warningSAXFunc warning;
    NULL, //errorSAXFunc error;
    NULL //fatalErrorSAXFunc fatalError;
  };

bool
XMLUtil::XMLParser::parse (const char* infile)
{
  did_parse_ = false;
  if (infile != NULL)
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
  return (didParse() && handler_ != NULL) ? handler_->getRoot () : NULL;
}

/* eof */
