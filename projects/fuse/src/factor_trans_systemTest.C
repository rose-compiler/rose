#include "rose.h"
#include "compose.h"
#include "factor_trans_system.h"
#include "const_prop_analysis.h"
#include "call_context_sensitivity_analysis.h"
#include "dead_path_elim_analysis.h"
#include "analysis_tester.h"
#include <vector>
#include <ctype.h>
#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>

using namespace std;
using namespace fuse;

int main(int argc, char** argv)
{
  printf("========== S T A R T ==========\n");
  
  Rose_STL_Container<string> args = CommandlineProcessing::generateArgListFromArgcArgv(argc, argv);
  // Strip the dataflow analysis options
  
  // Run the front end
  frontend(argc, argv);
  //generatePDF(*project);
  printf("Frontend done\n");fflush(stdout);
  
  string fuseCmd = "";
  Rose_STL_Container<string> dataflowoptions = CommandlineProcessing::generateOptionList(args, "-fuse:");
  //std::vector<std::string>  dataflowoptions = project->get_originalCommandLineArgumentList();
  if(dataflowoptions.size()>1) { cout << "ERROR: too many -dataflow options!"; exit(-1); }
  // If the caller provided a description of the composition structure on the command line, set fuseCmd to it
  else if(dataflowoptions.size()==1) {
    fuseCmd = dataflowoptions[0];
  }
  
  {
    ComposedAnalysisPtr analysisToTest = boost::make_shared<ConstantPropagationAnalysis>();
    set<ComposedAnalysisPtr> precisionAnalyses;
    precisionAnalyses.insert(boost::make_shared<CallContextSensitivityAnalysis>(2, CallContextSensitivityAnalysis::callSite));
    precisionAnalyses.insert(boost::make_shared<ConstantPropagationAnalysis>());
    precisionAnalyses.insert(boost::make_shared<DeadPathElimAnalysis>());
    ComposedAnalysisPtr stxAnalysis;
    
    ComposedAnalysisSelfTester test(analysisToTest, precisionAnalyses, stxAnalysis);
    test.testAnalysis();
    return 0;
  }

  // If this is a command for the compositional framework
  //if(fuseCmd.size()>0) {
    std::set<int> transitionFactors;
    transitionFactors.insert(3);
    transitionFactors.insert(5);
    transitionFactors.insert(7);
    
    std::set<int> subGraphTransFactors;
    subGraphTransFactors.insert(11);
    subGraphTransFactors.insert(13);
    
    list<ComposedAnalysis*> subAnalyses;
    
    ComposedAnalysisPtr stxAnalysis = boost::make_shared<FactorTransSystemAnalysis>(true, // firstAnalysis
                                                          transitionFactors,
                                                          //2, // myCommonFactor
                                                          5, // maxSteps,
                                                          4, // numRollbackSteps
                                                          3, // maxNumStepsInDerivedGraph
                                                          subGraphTransFactors
                                    );
    
    set<ComposedAnalysisPtr> precisionAnalyses;
    precisionAnalyses.insert(boost::make_shared<FactorTransSystemAnalysis>(false, // firstAnalysis
                                                          transitionFactors,
                                                          //2, // myCommonFactor
                                                          5, // maxSteps,
                                                          4, // numRollbackSteps
                                                          3, // maxNumStepsInDerivedGraph
                                                          subGraphTransFactors
                            ));
    ComposedAnalysisPtr analysisToTest = boost::make_shared<FactorTransSystemAnalysis>(false, // firstAnalysis
                                                          transitionFactors,
                                                          //2, // myCommonFactor
                                                          5, // maxSteps,
                                                          4, // numRollbackSteps
                                                          3, // maxNumStepsInDerivedGraph
                                                          subGraphTransFactors
                            );
    
    /*ChainComposer* cc = new ChainComposer(subAnalyses, NULL, true, stxAnalysis);
    cc->runAnalysis();*/
    
    ComposedAnalysisSelfTester test(analysisToTest, precisionAnalyses, stxAnalysis);
    test.testAnalysis();
  //}
  
  printf("==========  E  N  D  ==========\n");
  
  return 0;
}

