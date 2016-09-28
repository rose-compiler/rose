// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file examples/simple_read2.cc
 *  \brief Demonstrate XMLParser functionality.
 *  \ingroup ROSEHPCT_EXAMPLES
 *
 *  $Id: simple_read2.cc,v 1.1 2008/01/08 02:57:07 dquinlan Exp $
 */

/*
 * This software was produced with support in part from the Defense Advanced
 * Research Projects Agency (DARPA) through AFRL Contract FA8650-09-C-1915.
 * Nothing in this work should be construed as reflecting the official policy
 * or position of the Defense Department, the United States government,
 * or Rice University.
 */

#include <iostream>
#include <string>

#include <rosehpct/util/general.hh>
#include <rosehpct/xml/xml.hh>
#include <rosehpct/xml2profir/xml2profir.hh>

using namespace GenUtil;
using namespace XMLUtil;
using namespace std;

/*!
 *  Driver for examples/\ref simple_read2.cc.
 */
int
main (int argc, char* argv[])
{
  if (argc < 2)
    {
      cerr << endl
	   << "usage: " << argv[0]
	   << " <file1.xml> [<file2.xml> ...]" << endl
	   << endl;
      return 1;
    }

  for (int i = 1; i < argc; i++)
    {
      const char* xml_file = argv[i];
      cout << "<< " << xml_file << " >>" << endl;
      XMLParser p (xml_file);
      if (p.didParse ())
	{

	  cout << "-----------------------------------" << endl
	       << "Tree structure" << endl
	       << "-----------------------------------" << endl;
	  cout << p.getDoc () << endl;

	  cout << "-----------------------------------" << endl
	       << "Sample of tree properties" << endl
	       << "-----------------------------------" << endl;
	  cout << "XML Tree Version: "
	       << RoseHPCT::XMLQuery::getVersion (p.getDoc ())
	       << endl;

      cout << "\nMetric Table:" << endl;
	  map<string, string> mettab =
	    RoseHPCT::XMLQuery::getMetricTable (p.getDoc ());
      map<string, string>::iterator it;
	  for (it = mettab.begin(); it != mettab.end(); it++)
	      cout << "Metric[" << (*it).first << "] = " << (*it).second << endl;

      cout << "\nStatements:" << endl;
	  vector<XMLElem> stmts =
	    RoseHPCT::XMLQuery::getStatements (p.getDoc ());
	  for (vector<XMLElem>::const_iterator i = stmts.begin (); i != stmts.end (); ++i)
	      cout << tabs (1) << (*i) << endl;
	}
      else
	cout << "(Not OK.)" << endl;
    }

  return 0;
}

/* eof */
