#include "rose.h"
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

int main( int argc, char * argv[] )
{
  vector<string> argvList(argv, argv + argc);
  SgProject* project = frontend(argvList);
  if (project->get_fileList().size() ==0)
    return 0;
  // Prepare the Def-Use Analysis
  DFAnalysis* defuse = new DefUseAnalysis(project);
  bool debug = false;
  defuse->run(debug);
  if (debug)
    defuse->dfaToDOT();

  // Prepare liveness analysis
  LivenessAnalysis* liv = new LivenessAnalysis(debug,(DefUseAnalysis*)defuse);
  ROSE_ASSERT (liv != NULL);

  // Find all function definitions
  Rose_STL_Container<SgNode*> nodeList= NodeQuery::querySubTree(project, V_SgFunctionDefinition);
  std::vector <FilteredCFGNode < IsDFAFilter > > dfaFunctions;
  Rose_STL_Container<SgNode*>::const_iterator i = nodeList.begin();
  bool abortme=false;
  for (; i!=nodeList.end();++i)
  {
    SgFunctionDefinition* func = isSgFunctionDefinition(*i);
    // run liveness analysis
    FilteredCFGNode <IsDFAFilter> rem_source = liv->run(func,abortme);
    if (abortme) {
      cerr<<"Error: Liveness analysis is ABORTING ." << endl;
      ROSE_ASSERT(false);
    }
    if (rem_source.getNode()!=NULL)
      dfaFunctions.push_back(rem_source);
  }

  SgFilePtrList file_list = project->get_fileList();
  std::string firstFileName = StringUtility::stripPathFromFileName(file_list[0]->getFileName());
  std::string fileName = firstFileName+"_liveness.dot" ;
  std::ofstream fs(fileName.c_str());
  dfaToDot(fs, string("var"), dfaFunctions, (DefUseAnalysis*)defuse, liv);
  fs.close();
  return 0;
}

