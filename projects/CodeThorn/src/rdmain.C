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
#include "RDAnalysisAstAttribute.h"
#include "AttributeAnnotator.h"
#include "DataDependenceVisualizer.h"

using namespace std;
using namespace CodeThorn;

void createDefUseAttributeFromRDAttribute(Labeler* labeler, string rdAttributeName, string udAttributeName) {
  long labelNum=labeler->numberOfLabels();
  for(long i=0;i<labelNum;++i) {
	Label lab=i;
	SgNode* node=labeler->getNode(i);
	RDAnalysisAstAttribute* rdAttr=dynamic_cast<RDAnalysisAstAttribute*>(node->getAttribute(rdAttributeName));
	node->setAttribute(udAttributeName,new UseDefInfoAttribute(rdAttr, node));
  }
}

template<typename T>
void printAttributes(Labeler* labeler, VariableIdMapping* vim, string attributeName) {
  long labelNum=labeler->numberOfLabels();
  for(long i=0;i<labelNum;++i) {
	Label lab=i;
	SgNode* node=labeler->getNode(i);
	cout<<"@Label "<<lab<<":";
	dynamic_cast<T*>(node->getAttribute(attributeName))->toStream(cout,vim);
	cout<<endl;
  }
}
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
  rdAnalyzer->attachResultsToAst("rd-analysis");
  cout << "INFO: generating visualization data."<<endl;
  printAttributes<RDAnalysisAstAttribute>(rdAnalyzer->getLabeler(),rdAnalyzer->getVariableIdMapping(),"rd-analysis");
  createDefUseAttributeFromRDAttribute(rdAnalyzer->getLabeler(),"rd-analysis", "ud-analysis");
  DataDependenceVisualizer ddvis(rdAnalyzer->getLabeler(),
                                 rdAnalyzer->getVariableIdMapping(),
								 "ud-analysis");
  printAttributes<UseDefInfoAttribute>(rdAnalyzer->getLabeler(),rdAnalyzer->getVariableIdMapping(),"ud-analysis");
  //ddvis._showSourceCode=false; // for large programs
  ddvis.generateDot(root,"datadependencegraph.dot");

  // simple test
  RDLattice elem;
  RDAnalysisAstAttribute* rda=new RDAnalysisAstAttribute(&elem);
  delete rda;

  cout << "INFO: annotating analysis results as comments."<<endl;
  AnalysisResultAnnotator ara;
  ara.annotateAnalysisResultAttributesAsComments(root, "rd-analysis");
  cout << "INFO: generating annotated source code."<<endl;
  backend(root);

  return 0;
}
