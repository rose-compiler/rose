// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file src/xml2profir/query.cc
 *  \brief Module to query a parsed XML HPCToolkit profile tree.
 *  \ingroup ROSEHPCT_XML2PROFIR
 *
 *  $Id: query.cc,v 1.1 2008/01/08 02:56:44 dquinlan Exp $
 */

#include <iostream>
#include <string>
#include <vector>

#include "rosehpct/xml2profir/xml2profir.hh"

using namespace RoseHPCT;
using namespace XMLUtil;

using namespace std;

string
RoseHPCT::XMLQuery::getVersion (const XMLNode_t* doc)
{
  XMLElemFinder finder ("HPCToolkitExperiment", doc);
  if (finder.getNumMatches () != 1)
    return "";

  XMLElemFinder::ElemList_t::const_iterator i = finder.begin ();
  return (*i).getAttr ("version");
}

vector<XMLElem>
RoseHPCT::XMLQuery::getStatements (const XMLNode_t* doc)
{
  vector<XMLElem> statements;
  XMLElemFinder finder ("S", doc);
  for (XMLElemFinder::ElemList_t::const_iterator i = finder.begin ();
       i != finder.end ();
       ++i)
    {
      statements.push_back (*i);
    }
  return statements;
}

map<string, string>
RoseHPCT::XMLQuery::getMetricTable (const XMLNode_t* doc)
{
  map<string, string> metrics;

  XMLElemFinder finder ("Metric", doc);
  for (XMLElemFinder::ElemList_t::const_iterator i = finder.begin ();
       i != finder.end ();
       ++i)
    {
      string key = (*i).getAttr ("i");
      string value = (*i).getAttr ("n");
      if (key.length())
	metrics[key] = value;
    }

  return metrics;
}

/* eof */
