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
#include "RDAstAttribute.h"
#include "AttributeAnnotator.h"
#include "DataDependenceVisualizer.h"
#include "Miscellaneous.h"

using namespace std;
using namespace CodeThorn;

void createUDAstAttributeFromRDAttribute(Labeler* labeler, string rdAttributeName, string udAttributeName) {
  long labelNum=labeler->numberOfLabels();
  for(long i=0;i<labelNum;++i) {
	Label lab=i;
	SgNode* node=labeler->getNode(lab);
	RDAstAttribute* rdAttr=dynamic_cast<RDAstAttribute*>(node->getAttribute(rdAttributeName));
	if(rdAttr)
	  node->setAttribute(udAttributeName,new UDAstAttribute(rdAttr, node));
  }
}

template<typename T>
void printAttributes(Labeler* labeler, VariableIdMapping* vim, string attributeName) {
  long labelNum=labeler->numberOfLabels();
  for(long i=0;i<labelNum;++i) {
	Label lab=i;
	SgNode* node=labeler->getNode(i);
	cout<<"@Label "<<lab<<":";
	T* node0=dynamic_cast<T*>(node->getAttribute(attributeName));
	if(node0)
	  node0->toStream(cout,vim);
	else
	  cout<<" none.";
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
  cout << "INFO: attaching RD-data to AST."<<endl;
  rdAnalyzer->attachResultsToAst("rd-analysis");
  printAttributes<RDAstAttribute>(rdAnalyzer->getLabeler(),rdAnalyzer->getVariableIdMapping(),"rd-analysis");
  cout << "INFO: generating and attaching UD-data to AST."<<endl;
  createUDAstAttributeFromRDAttribute(rdAnalyzer->getLabeler(),"rd-analysis-pre-info", "ud-analysis");
  cout << "INFO: generating visualization data."<<endl;
  DataDependenceVisualizer ddvis(rdAnalyzer->getLabeler(),
                                 rdAnalyzer->getVariableIdMapping(),
								 "ud-analysis");
  printAttributes<UDAstAttribute>(rdAnalyzer->getLabeler(),rdAnalyzer->getVariableIdMapping(),"ud-analysis");
  //ddvis._showSourceCode=false; // for large programs
  ddvis.generateDefUseDotGraph(root,"datadependencegraph.dot");

  // generate ICFG visualization
  write_file("cfg.dot", rdAnalyzer->getFlow()->toDot(rdAnalyzer->getLabeler()));
  cout << "generated cfg.dot."<<endl;

  // simple test
  RDLattice elem;
  RDAstAttribute* rda=new RDAstAttribute(&elem);
  delete rda;

  cout << "INFO: annotating analysis results as comments."<<endl;
  AnalysisResultAnnotator ara(rdAnalyzer->getLabeler());
  ara.annotateAnalysisResultAttributesAsComments(root, "rd-analysis-post-info");
  cout << "INFO: generating annotated source code."<<endl;
  backend(root);

  return 0;
}
