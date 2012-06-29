// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file examples/test_highlevel.cc
 *  \brief Demonstrates a high-level interface to ROSE-HPCT.
 *  \ingroup ROSEHPCT_EXAMPLES
 *
 *  This program is functionally equivalent the example program,
 *  examples/\ref propagate_metrics.cc.
 *
 *  $Id: test_highlevel.cc,v 1.1 2008/01/08 02:57:08 dquinlan Exp $
 */

/*
 * This software was produced with support in part from the Defense Advanced
 * Research Projects Agency (DARPA) through AFRL Contract FA8650-09-C-1915.
 * Nothing in this work should be construed as reflecting the official policy
 * or position of the Defense Department, the United States government,
 * or Rice University.
 */

#include "rose.h"

#include <iostream>
#include <rosehpct/rosehpct.hh>
#include <rosehpct/sage/collectors.hh>

using namespace std;

//! Driver for examples/\ref test_highlevel.cc.
int
main (int argc, char* argv[])
{
  vector<string> argvList(argv, argv+argc);
  cerr << "[Loading HPCToolkit or Gprof profiling data...]" << endl;
  RoseHPCT::ProgramTreeList_t profiles
    = RoseHPCT::loadProfilingFiles(argvList);

  cerr << "[Creating a Sage IR...]" << endl;
  SgProject* proj = frontend (argvList);

  cerr << "[Attaching HPCToolkit metrics to Sage IR tree...]" << endl;
  RoseHPCT::attachMetrics (profiles, proj, true);

  // Traverse the project tree and print all the attached MetricAtt.
  RoseHPCT::Vis_PrintMetricInfo vis(cout);
  RoseHPCT::Vis_PreOrder preVis(&vis);
  proj->accept(preVis);

  cerr << "[Dumping to PDF...]" << endl;
  generatePDF (*proj);

  return 0;
}

/* eof */
