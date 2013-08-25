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
#include "AstAnnotator.h"
#include "DataDependenceVisualizer.h"
#include "Miscellaneous.h"
#include "ProgramStats.h"

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
	//	cout<<"@Label "<<lab<<":";
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
  rdAnalyzer->initializeGlobalVariables(root);

  std::string funtofind="main";
  RoseAst completeast(root);
  SgFunctionDefinition* startFunRoot=completeast.findFunctionByName(funtofind);
  rdAnalyzer->determineExtremalLabels(startFunRoot);
  rdAnalyzer->run();
  cout << "INFO: attaching RD-data to AST."<<endl;
  rdAnalyzer->attachInInfoToAst("rd-analysis-in");
  rdAnalyzer->attachOutInfoToAst("rd-analysis-out");
  //printAttributes<RDAstAttribute>(rdAnalyzer->getLabeler(),rdAnalyzer->getVariableIdMapping(),"rd-analysis-in");
  cout << "INFO: generating and attaching UD-data to AST."<<endl;
  createUDAstAttributeFromRDAttribute(rdAnalyzer->getLabeler(),"rd-analysis-in", "ud-analysis");

#if 0
  cout << "INFO: computing program statistics."<<endl;
  computeStatistics(rdAnalyzer->getVariableIdMapping(),
					rdAnalyzer->getLabeler(), 
					rdAnalyzer->getFlow(),
					"ud-analysis");
#endif
  cout << "INFO: generating visualization data."<<endl;
  // generate ICFG visualization
  cout << "generating icfg.dot."<<endl;
  Flow* flow=rdAnalyzer->getFlow();
  write_file("icfg.dot", flow->toDot(rdAnalyzer->getLabeler()));
  cout << "generating datadependencegraph.dot."<<endl;
  DataDependenceVisualizer ddvis0(rdAnalyzer->getLabeler(),
                                 rdAnalyzer->getVariableIdMapping(),
								 "ud-analysis");
  //printAttributes<UDAstAttribute>(rdAnalyzer->getLabeler(),rdAnalyzer->getVariableIdMapping(),"ud-analysis");
  //ddvis._showSourceCode=false; // for large programs
  ddvis0.generateDefUseDotGraph(root,"datadependencegraph.dot");
  flow->resetDotOptions();

  cout << "generating icfgdatadependencegraph.dot."<<endl;
  DataDependenceVisualizer ddvis1(rdAnalyzer->getLabeler(),
                                 rdAnalyzer->getVariableIdMapping(),
								 "ud-analysis");
  ddvis1.includeFlowGraphEdges(flow);
  ddvis1.generateDefUseDotGraph(root,"icfgdatadependencegraph.dot");
  flow->resetDotOptions();

  cout << "generating icfgdatadependencegraph_clustered.dot."<<endl;
  DataDependenceVisualizer ddvis2(rdAnalyzer->getLabeler(),
                                 rdAnalyzer->getVariableIdMapping(),
								 "ud-analysis");
  ddvis2.generateDotFunctionClusters(root,rdAnalyzer->getCFAnalyzer(),"icfgdatadependencegraph_clustered.dot",true);

  cout << "generating icfg_clustered.dot."<<endl;
  DataDependenceVisualizer ddvis3(rdAnalyzer->getLabeler(),
                                 rdAnalyzer->getVariableIdMapping(),
								 "ud-analysis");
  ddvis3.generateDotFunctionClusters(root,rdAnalyzer->getCFAnalyzer(),"icfg_clustered.dot",false);

  cout << "INFO: annotating analysis results as comments."<<endl;
  AstAnnotator ara(rdAnalyzer->getLabeler());
  ara.annotateAstAttributesAsCommentsBeforeStatements(root, "rd-analysis-in");
  ara.annotateAstAttributesAsCommentsAfterStatements(root, "rd-analysis-out");
  cout << "INFO: generating annotated source code."<<endl;
  root->unparse(0,0);
  return 0;
}
