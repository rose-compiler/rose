// Liao, 12/6/2011
#include "rose.h"

#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

// TODO group them into one header
#include "genericDataflowCommon.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "printAnalysisStates.h"
#include "liveDeadVarAnalysis.h"

int numFails = 0, numPass = 0;

//-----------------------------------------------------------
int
main( int argc, char * argv[] ) 
   {
     printf("========== S T A R T ==========\n");
     SgProject* project = frontend(argc,argv);
	
     initAnalysis(project);
     Dbg::init("Live dead variable analysis Test", ".", "index.html");

  /* analysisDebugLevel = 0;

     SaveDotAnalysis sda;
     UnstructuredPassInterAnalysis upia_sda(sda);
     upia_sda.runAnalysis();
   */
	
     liveDeadAnalysisDebugLevel = 1;
     analysisDebugLevel = 1;
     if (liveDeadAnalysisDebugLevel)
        {
          printf("*********************************************************************\n");
          printf("**********   Live Dead Variable Analysis (Liveness Analysis)   ******\n");
          printf("*********************************************************************\n");
        }

     LiveDeadVarsAnalysis ldva(project);
     UnstructuredPassInterDataflow ciipd_ldva(&ldva);
     ciipd_ldva.runAnalysis();
   // Output the dot graph
    Dbg::dotGraphGenerator (&ldva);
      return 0;
   }



