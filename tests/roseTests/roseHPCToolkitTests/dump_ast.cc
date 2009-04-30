// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file examples/dump_ast.cc
 *  \brief Demonstrates how to dump and read annotated AST with performance metrics
 *  based on Rich's propagate_metrics.cc 
 *  \ingroup ROSEHPCT_EXAMPLES
 *
 *  $Id: dump_ast.cc,v 1.1 2008/01/08 02:57:07 dquinlan Exp $
 */

#include "rose.h"

#include <iostream>
#include <string>
#include <map>
#include <rosehpct/util/general.hh>
#include <rosehpct/xml2profir/xml2profir.hh>
#include <rosehpct/sage/sage.hh>
#include <rosehpct/profir2sage/profir2sage.hh>
#include <rosehpct/rosehpct.hh>

using namespace std;
using namespace GenUtil;
using namespace RoseHPCT;

/*!
 *  Driver for examples/\ref dump_ast.cc.
 */
int
main ( int argc, char* argv[] )
{
  vector<string> argvList(argv, argv+argc);
  cerr << "[Loading HPCToolkit profiles...]" << endl;
  RoseHPCT::EquivPathMap_t eqpaths = RoseHPCT::getEquivPaths (argvList);
  RoseHPCT::FilenameList_t filenames = RoseHPCT::getProfileOptions (argvList);
  RoseHPCT::ProgramTreeList_t profiles = RoseHPCT::loadXMLTrees (filenames);

  cerr << "[Creating a Sage IR...]" << endl;
  SgProject* proj = frontend (argvList);

  cerr << "[Attaching HPCToolkit profiles to Sage IR tree...]" << endl;
  map<string, size_t> metrics;
  for (RoseHPCT::ProgramTreeList_t::const_iterator hpctree = profiles.begin ();
       hpctree != profiles.end ();
       ++hpctree)
    {
      RoseHPCT::postProcessingProfIR(*hpctree, eqpaths);
      RoseHPCT::attachMetrics (*hpctree, proj, true);

      RoseHPCT::collectMetricNames (*hpctree, metrics);
    }

 RoseHPCT::propagateMetrics (proj, getKeys (metrics));
#if 0  // get the original pdf dump
  cerr << "[Dumping to PDF...]" << endl;
  AstPDFGeneration pdf;
  pdf.generateInputFiles (proj);
#else // get the pdf dump generated from a binary file
  // dump 
// Liao, 2/19/2008. Cannot dump a pdf file from IR if the IR has been attempted to write to a file!
  REGISTER_ATTRIBUTE_FOR_FILE_IO(MetricAttr);
  AST_FILE_IO::startUp (proj);
  AST_FILE_IO::writeASTToFile ("input.perf.bin");

 // read it back
  REGISTER_ATTRIBUTE_FOR_FILE_IO(MetricAttr);
  AST_FILE_IO::clearAllMemoryPools();
  SgProject * proj2 = (SgProject*) (AST_FILE_IO::readASTFromFile("input.perf.bin"));
  // verify result
  cerr << "[Dumping to PDF...]" << endl;
  AstPDFGeneration pdf;
  pdf.generateInputFiles (proj2);
#endif

  return 0;
}

/* eof */
