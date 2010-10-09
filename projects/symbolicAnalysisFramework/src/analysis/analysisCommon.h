#ifndef ANALYSIS_COMMON
#define ANALYSIS_COMMON

#include "common.h"
#include "cfgUtils.h"
#include "CallGraphTraverse.h"

// initializes the compiler analysis framework
void initAnalysis(SgProject* p);

// returns the SgProject object that represents our current project
//SgProject* getProject();

// returns the call graph of the current project
SgIncidenceDirectedGraph* getCallGraph();

#endif
