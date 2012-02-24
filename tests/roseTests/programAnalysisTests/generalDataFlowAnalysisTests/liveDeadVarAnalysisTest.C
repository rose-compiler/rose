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

bool gfilter (CFGNode cfgn) 
{
  SgNode *node = cfgn.getNode();

  switch (node->variantT())
  {
    //For function calls, we only keep the last node. The function is actually called after all its parameters
    //are evaluated.
//    case V_SgFunctionCallExp:
//      return (cfgn == node->cfgForEnd());
//
   //For basic blocks and other "container" nodes, keep the node that appears before the contents are executed
    case V_SgBasicBlock:
    case V_SgExprStatement:
    case V_SgCommaOpExp:
      return (cfgn == node->cfgForBeginning());

      //Keep the last index for initialized names. This way the def of the variable doesn't propagate to its assign
      //initializer.
    case V_SgInitializedName:
      return (cfgn == node->cfgForEnd());
//
//    case V_SgTryStmt:
//      return (cfgn == node->cfgForBeginning());
//
//      //We only want the middle appearance of the teritatry operator - after its conditional expression
//      //and before the true and false bodies. This makes it behave as an if statement for data flow
//      //purposes
//    case V_SgConditionalExp:
//      return (cfgn.getIndex() == 1);
//
//      //Make these binary operators appear after their operands, because they are evaluated after their operands
//    case V_SgAndOp:
//    case V_SgOrOp:
//      return (cfgn == node->cfgForEnd());

    default:
      return cfgn.isInteresting();
  }
}



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
     ldva.filter = gfilter;
     UnstructuredPassInterDataflow ciipd_ldva(&ldva);
     assert (ciipd_ldva.filter == gfilter);
     ciipd_ldva.runAnalysis();
   // Output the dot graph
    Dbg::dotGraphGenerator (&ldva);
      return 0;
   }



