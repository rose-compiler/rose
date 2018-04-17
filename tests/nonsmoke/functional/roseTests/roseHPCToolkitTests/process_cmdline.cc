// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file examples/process_cmdline.cc
 *  \brief Simple program to test/experiment with command-line
 *  processing specific to ROSE-HPCT.
 *  \ingroup ROSEHPCT_EXAMPLES
 *
 *  $Id: process_cmdline.cc,v 1.1 2008/01/08 02:57:07 dquinlan Exp $
 */

#include "rose.h"

#include <iostream>
#include <string>
#include <vector>

#include <rosehpct/util/general.hh>
#include <rosehpct/rosehpct.hh>

// #include "rose.h"

using namespace std;
using namespace GenUtil;

/*!
 *  Driver for examples/\ref process_cmdline.cc.
 */
int
main ( int argc, char* argv[] )
{
 vector<string> argvList(argv, argv+argc);

  // option with parameter to option
  vector<string> files = RoseHPCT::getProfileOptions (argvList);
  RoseHPCT::EquivPathMap_t eqpaths = RoseHPCT::getEquivPaths (argvList);

  cerr << "*** Processed options ***" << endl;
  for (vector<string>::const_iterator i = files.begin ();
       i != files.end ();
       ++i)
    {
      cout << *i
	   << " [dir=" << getDirname (*i)
	   << ", file=" << getBaseFilename (*i)
	   << "]" << endl;

      string dirname = getDirname (*i);
      string filename = getBaseFilename (*i);
      RoseHPCT::EquivPathMap_t::const_iterator ep = eqpaths.find (dirname);
      if (ep != eqpaths.end ())
	{
	  cerr << "\t(equivalent to "
	       << ep->second << "/" << filename << ")" << endl;
	}
    }

  cerr << "*** Unprocessed options ***" << endl;
  for (int i = 1; i < argc; i++)
    {
      cerr << "[option " << i << "] '" << argv[i] << "'" << endl;
    }

  return 0;
}

/* eof */
