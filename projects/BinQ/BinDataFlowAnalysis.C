#include "BinQGui.h"

#include <iostream>


#include "BinQSupport.h"
#include "slide.h"

#include <qtabwidget.h>
#include "BinDataFlowAnalysis.h"

using namespace qrs;
using namespace std;
using namespace __gnu_cxx;

std::string BinDataFlowAnalysis::name() {
  return "Data Flow Graph";
}

std::string BinDataFlowAnalysis::getDescription() {
  return "Creates a dataflow graph. Outputs dfg.dot";
}


void
BinDataFlowAnalysis::run() {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();

  RoseBin_Graph* graph=NULL;
  ROSE_ASSERT(isSgProject(instance->fileA));
  SgBinaryFile* binaryFile = isSgBinaryFile(isSgProject(instance->fileA)->get_fileList()[0]);
  SgAsmFile* file = binaryFile != NULL ? binaryFile->get_binaryFile() : NULL;
  ROSE_ASSERT(file);

  VirtualBinCFG::AuxiliaryInformation* info = new VirtualBinCFG::AuxiliaryInformation(file);

  // call graph analysis  *******************************************************
  ROSE_ASSERT(instance);
  ROSE_ASSERT(instance->analysisTab);
  instance->analysisTab->setCurrentIndex(1);
  QString res = QString("Creating dataflow graph ");
  instance->analysisResult->append(res);  
  
  bool dot=true;
  bool forward=true;
  bool edges=true;
  bool mergedEdges=true;
  bool interprocedural=false;
  string dfgFileName = "dfg.dot";
  graph= new RoseBin_DotGraph(info);
  if (dot==false) {
    dfgFileName = "dfg.gml";
    graph= new RoseBin_GMLGraph(info);
  }

  SgAsmInterpretation* interp = SageInterface::getMainInterpretation(file);
  RoseBin_DataFlowAnalysis* dfanalysis = 
    new RoseBin_DataFlowAnalysis(interp->get_global_block(), forward, new RoseObj(), info);
  ROSE_ASSERT(dfanalysis);
  dfanalysis->init(interprocedural, edges);
  dfanalysis->run(graph, dfgFileName, mergedEdges);

  res = QString("nr of nodes visited %1. nr of edges visited %2. ")
    .arg(dfanalysis->nodesVisited())
    .arg(dfanalysis->edgesVisited());
    
  instance->analysisResult->append(res);  

  
}

void
BinDataFlowAnalysis::test(SgNode* fileA, SgNode* fileB) {


  RoseBin_Graph* graph=NULL;
  ROSE_ASSERT(isSgProject(fileA));
  SgBinaryFile* binaryFile = isSgBinaryFile(isSgProject(fileA)->get_fileList()[0]);
  SgAsmFile* file = binaryFile != NULL ? binaryFile->get_binaryFile() : NULL;
  ROSE_ASSERT(file);

  VirtualBinCFG::AuxiliaryInformation* info = new VirtualBinCFG::AuxiliaryInformation(file);

  // call graph analysis  *******************************************************
  
  bool dot=true;
  bool forward=true;
  bool edges=true;
  bool mergedEdges=true;
  bool interprocedural=false;
  string dfgFileName = "dfg.dot";
  graph= new RoseBin_DotGraph(info);
  if (dot==false) {
    dfgFileName = "dfg.gml";
    graph= new RoseBin_GMLGraph(info);
  }

  SgAsmInterpretation* interp = SageInterface::getMainInterpretation(file);
  RoseBin_DataFlowAnalysis* dfanalysis = 
    new RoseBin_DataFlowAnalysis(interp->get_global_block(), forward, new RoseObj(), info);
  ROSE_ASSERT(dfanalysis);
  dfanalysis->init(interprocedural, edges);
  dfanalysis->run(graph, dfgFileName, mergedEdges);


  
}
