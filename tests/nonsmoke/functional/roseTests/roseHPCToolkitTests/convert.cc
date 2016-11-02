// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file examples/convert.cc
 *  \brief Demonstrate XML-to-ProfIR conversion.
 *  \ingroup ROSEHPCT_EXAMPLES
 *
 *  $Id: convert.cc,v 1.1 2008/01/08 02:57:07 dquinlan Exp $
 */

#include <iostream>
#include <string>

#include <rosehpct/util/general.hh>
#include <rosehpct/xml/xml.hh>
#include <rosehpct/xml2profir/xml2profir.hh>

using namespace std;
using namespace GenUtil;
using namespace XMLUtil;
using namespace RoseHPCT;

/*! \brief Driver for XML-to-ProfIR conversion example. */
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
	       << "Converting to RoseHPCTIR..." << endl
	       << "-----------------------------------" << endl;

	  IRTree_t* prog = convertXMLToProfIR (p.getDoc ());
	  if (prog != NULL)
	    delete prog;

	  cout << "Done!" << endl;
	}
      else
	cout << "(Not OK.)" << endl;
    }

  return 0;
}

/* eof */
