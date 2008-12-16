#include "rose.h"
#include "DefUseAnalysis.h"
#include <string>
#include <iostream>
using namespace std;

void runCurrentFile(vector<string> argvList) {
  SgProject* project = frontend(argvList);
  std::cout << ">>>> generate PDF " << endl;
  generatePDF ( *project );
  std::cout << ">>>> start def-use analysis ... " << endl;

  // Call the Def-Use Analysis
  DFAnalysis* defuse = new DefUseAnalysis(project);
  bool debug = true;
  defuse->run(debug);
  defuse->dfaToDOT();
  
  NodeQuerySynthesizedAttributeType vars = NodeQuery::querySubTree(project, V_SgInitializedName); 
  NodeQuerySynthesizedAttributeType::const_iterator i = vars.begin();
  for (; i!=vars.end();++i) {
    SgInitializedName* initName = isSgInitializedName(*i);
    std::string name = initName->get_qualified_name().str();
    vector<SgNode* > vec = defuse->getDefFor(initName, initName);
    if (vec.size()>0)
      std::cout << "  DEF>> Vector entries for " << name <<  " ( " << 
	initName << " ) : " << vec.size() << std::endl;
  }
}

int main( int argc, char * argv[] ) {
  vector<string> argvList(argv, argv + argc);
  runCurrentFile(argvList);
  return 0;
}
