// Example ROSE Translator: used for testing ROSE infrastructure

#include "rose.h"
#include "cfgToDot.h"
using namespace std;

int main(int argc, char *argv[]) {
  // Build the AST used by ROSE
  SgProject* sageProject = frontend(argc,argv);

  // Run internal consistency tests on AST
  AstTests::runAllTests(sageProject);

  // Print the CFG view of each procedure in the AST in Dot format to cout
  NodeQuerySynthesizedAttributeType functions = NodeQuery::querySubTree(sageProject, V_SgFunctionDefinition);
  for (NodeQuerySynthesizedAttributeType::const_iterator i = functions.begin(); i != functions.end(); ++i) {
    SgFunctionDefinition* proc = isSgFunctionDefinition(*i);
    if (proc) {
      // Print out the full CFG, including bookkeeping nodes
      VirtualCFG::cfgToDotForDebugging(cout, proc->get_declaration()->get_name(), proc->cfgForBeginning());
      // Print out only those nodes which are "interesting" for analyses
      // VirtualCFG::cfgToDot(junk, proc->get_declaration()->get_name(), VirtualCFG::makeInterestingCfg(proc));
    }
  }

  return 0;
}

