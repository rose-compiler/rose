// Author: Markus Schordan, 2013.

#include "rose.h"

#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "RDLattice.h"
#include "DFAnalyzer.h"
#include "WorkList.h"
#include "RDAnalyzer.h"
#include "AttributeAnnotator.h"
#include "DataDependenceVisualizer.h"

using namespace std;
using namespace CodeThorn;

int main(int argc, char* argv[]) {
  cout << "INIT: Parsing and creating AST."<<endl;
  boolOptions.registerOption("semantic-fold",false); // temporary
  boolOptions.registerOption("post-semantic-fold",false); // temporary
  SgProject* root = frontend(argc,argv);
  RDAnalyzer* rdAnalyzer=new RDAnalyzer();
  rdAnalyzer->initialize(root);

  std::string funtofind="main";
  RoseAst completeast(root);
  SgFunctionDefinition* startFunRoot=completeast.findFunctionByName(funtofind);
  rdAnalyzer->determineExtremalLabels(startFunRoot);
  rdAnalyzer->run();
  cout << "INFO: attaching results to AST."<<endl;
  rdAnalyzer->attachResultsToAst();
  cout << "INFO: generating visualization data."<<endl;
  DataDependenceVisualizer ddvis(rdAnalyzer->getLabeler(),
                                 rdAnalyzer->getVariableIdMapping());
  ddvis.generateDot(root,"data_dependence_graph.dot");

  cout << "INFO: annotating analysis results as comments."<<endl;
  AnalysisResultAnnotator ara;
  ara.annotateAnalysisResultAttributesAsComments(root, "rd-analysis");
  cout << "INFO: generating annotated source code."<<endl;
  backend(root);

  return 0;
}
