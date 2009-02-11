// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file examples/attach_metrics.cc
 *  \brief Demonstrates the high-level ROSE-HPCT interface
 *  for attaching HPCToolkit profile files to the Sage III IR.
 *  \ingroup ROSEHPCT_EXAMPLES
 *
 *  This program takes a list of profile data files on the
 *  command-line (specified using the "-rose:hpct" flag),
 *  loads these files, and attaches metric data to the appropriate
 *  places in the Sage III IR as AstAttribute objects. On
 *  completion, this program generates a PDF output of the
 *  attributed Sage IR.
 *
 *  To see this program in action, run the accompanying test scripts,
 *  examples/test_attach*.sh.
 *
 *  $Id: attach_metrics.cc,v 1.1 2008/01/08 02:57:07 dquinlan Exp $
 */

#include "rose.h"

#include <iostream>
#include <string>
#include <vector>
#include <rosehpct/xml2profir/xml2profir.hh>
#include <rosehpct/profir2sage/profir2sage.hh>
#include <rosehpct/rosehpct.hh>


using namespace std;

/*!
 *  Driver for examples/\ref attach_metrics.cc.
 */
int
main (int argc, char* argv[])
{
 vector<string> argvList(argv, argv+argc);

  cerr << "[Loading HPCToolkit profiles]...]" << endl;
  RoseHPCT::FilenameList_t filenames =  RoseHPCT::getProfileOptions (argvList);
  RoseHPCT::ProgramTreeList_t profiles = RoseHPCT::loadXMLTrees (filenames);
  RoseHPCT::EquivPathMap_t eqpaths = RoseHPCT::getEquivPaths (argvList);

  cerr << "[Creating a Sage IR...]" << endl;
  SgProject* proj = frontend (argvList);

  cerr << "[Attaching HPCToolkit profiles to Sage IR tree...]" << endl;
  for (RoseHPCT::ProgramTreeList_t::const_iterator hpctree = profiles.begin ();
       hpctree != profiles.end ();
       ++hpctree)
    {
      RoseHPCT::translateFilePaths (*hpctree, eqpaths);
      RoseHPCT::attachMetrics (*hpctree, proj, true);
    }

  cerr << "[Dumping to PDF...]" << endl;
  AstPDFGeneration pdf;
  pdf.generateInputFiles (proj);

  return 0;
}

