// Author: Markus Schordan, 2013.

#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "RDLattice.h"
#include "DFAnalyzer.h"
#include "WorkList.h"
#include "RDAnalyzer.h"
#include "AttributeAnnotator.h"

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
  rdAnalyzer->attachResultsToAst();
  AnalysisResultAnnotator ara;
  ara.annotateAnalysisResultAttributesAsComments(root, "rd-analysis");
  backend(root);
#if 0
  // print results
  int loc=0;
  for(RDAnalyzer::iterator i=rdAnalyzer->begin();
	  i!=rdAnalyzer->end();
	  ++i) {
	cout<<"At location "<<loc<<": ";
	//(*i).toStream(cout,rdAnalyzer->getVariableIdMapping());
	(*i).toStream(cout);
	cout<<endl;
	
	loc++;
  }
#endif
  return 0;
}
