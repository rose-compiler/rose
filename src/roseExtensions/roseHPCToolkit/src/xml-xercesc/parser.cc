// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file src/xml-xercesc/parser.cc
 *  \brief XML parser.
 *  \ingroup ROSEHPCT_XERCESC
 *
 *  $Id: parser.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
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

bool XMLUtil::XMLParser::did_init_ = false;

XMLUtil::XMLParser::XMLParser (void)
{
  init ();
}

XMLUtil::XMLParser::XMLParser (const char* infile)
{
  init ();
  parse (infile);
}

XMLUtil::XMLParser::XMLParser (const string& infile)
{
  init ();
  parse (infile);
}

XMLUtil::XMLParser::~XMLParser (void)
{
  if (parser_ != NULL)
    delete parser_;
  if (handler_ != NULL)
      delete handler_;
  if (errhandler_ != NULL)
    delete errhandler_;
}

void
XMLUtil::XMLParser::init (void)
{
  if (!did_init_)
    {
      try
	{
	  XMLPlatformUtils::Initialize ();
	  did_init_ = true;
	}
      catch (XMLException& e)
	{
	  cerr << "[*** ERROR ***] Can't initialize XML library" << endl
	       << toString (e.getMessage ()) << endl;
	}
    }

  if (did_init_)
    {
      parser_ = new SAXParser ();
      parser_->setDoValidation (true);
      parser_->setDoNamespaces (true);
      
      handler_ = new XMLHandler();
      parser_->setDocumentHandler (handler_);
      
      errhandler_ = new HandlerBase();
      parser_->setErrorHandler (errhandler_);
      
      did_parse_ = false;
    }
  else
    {
      parser_ = NULL;
      handler_ = NULL;
      errhandler_ = NULL;
    }
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

bool
XMLUtil::XMLParser::parse (const char* infile)
{
  did_parse_ = false;
  if (infile != NULL && parser_ != NULL)
    {
      try
	{
	  parser_->parse (infile);
	  did_parse_ = true;
	}
      catch (const XMLException& e)
	{
	  cerr << "[*** ERROR ***] XML exception" << endl
	       << toString (e.getMessage ()) << endl;
	}
      catch (const SAXParseException& e)
	{
	  cerr << "[*** ERROR ***] Occurred while parsing" << endl
	       << toString (e.getMessage ()) << endl;
	}
      catch (...)
	{
	  cerr << "[*** ERROR ***] Unexpected exception" << endl;
	}
    }
  return did_parse_;
}

const XMLUtil::XMLHandler::node_t *
XMLUtil::XMLParser::getDoc (void) const
{
  return (didParse() && handler_ != NULL) ? handler_->getRoot () : NULL;
}

/* eof */
