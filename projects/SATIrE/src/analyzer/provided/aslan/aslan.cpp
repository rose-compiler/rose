/**
 *******************************************
 * @file aslan.cpp
 * @author Dietmar Schreiner
 * @version 1.0
 * @date 2009-11-02
 *******************************************
 */ 

#ifdef HAVE_CONFIG_H
#include <rose_config.h>
#endif


#include <satire.h>
#include "aslanalysis.h"
#include "locationmapper.h"
#include <pointsto.h>
#include<map>

using namespace SATIrE;
using namespace SATIrE::Analyses;

/**
 * Example for the ASL analysis usage.
 */
int main ( int argc, char** argv) {
  AnalyzerOptions* ap=extractOptions(argc,argv);
  ap->runPointsToAnalysisOn();
  SgProject* root=createRoseAst(ap);

   ASLAnalysis analyzer;
   analyzer.run(root);
   
   std::cout<<"Analysis found "<<analyzer.getCallSiteCounter()<<" call sites containing "<< analyzer.getFunctionPointerCallCounter() <<" calls through function pointers"<<std::endl;
   
  return 0;
}   

