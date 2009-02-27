#include "rose.h"
#include "BinQGui.h"

#include <iostream>


#include "BinQSupport.h"
#include "slide.h"

#include <qtabwidget.h>
#include "BinCallGraph.h"

using namespace qrs;
using namespace std;
using namespace __gnu_cxx;

std::string BinCallGraph::name() {
  return "Binary Call Graph";
}

std::string BinCallGraph::getDescription() {
  return "Creates a call graph for the current Binary. Output is callgraph.dot";
}


void
BinCallGraph::run(SgNode* fileA, SgNode* fileB) {
  instance=NULL;
  if (!testFlag)
    instance = QROSE::cbData<BinQGUI *>();
  if (isSgProject(fileA)==NULL) {
    cerr << "This is not a valid file for this analysis!" << endl;
    if (!testFlag) {
      QString res = QString("This is not a valid file for this analysis");
      instance->analysisResult->append(res);  
    }
    return;
  }

  RoseBin_Graph* graph=NULL;
  ROSE_ASSERT(isSgProject(fileA));
  SgBinaryFile* binaryFile = isSgBinaryFile(isSgProject(fileA)->get_fileList()[0]);
  SgAsmFile* file = binaryFile != NULL ? binaryFile->get_binaryFile() : NULL;
  ROSE_ASSERT(file);

  VirtualBinCFG::AuxiliaryInformation* info = new VirtualBinCFG::AuxiliaryInformation(file);

  // call graph analysis  *******************************************************
  cerr << " creating call graph ... " << endl;
 if (!testFlag) {
  ROSE_ASSERT(instance);
  ROSE_ASSERT(instance->analysisTab);
  QString res = QString("Creating call graph ");
  instance->analysisTab->setCurrentIndex(1);
   instance->analysisResult->append(res);  
  }
  
  graph= new RoseBin_DotGraph(info);
  ROSE_ASSERT(graph);
  string callFileName = "callgraph.dot";
  bool dot=true;
  bool mergedEdges=true;
  if (dot==false) {
    callFileName = "callgraph.gml";
    graph= new RoseBin_GMLGraph(info);
  }

  SgAsmInterpretation* interp = SageInterface::getMainInterpretation(file);
  RoseBin_CallGraphAnalysis* callanalysis = 
    //    new RoseBin_CallGraphAnalysis(isSgAsmNode(instance->fileA), new RoseObj(), info);
   new RoseBin_CallGraphAnalysis(interp->get_global_block(), new RoseObj(), info);

  ROSE_ASSERT(callanalysis);
  callanalysis->run(graph, callFileName, mergedEdges);

  if (!testFlag) {
  QString res = QString("nr of nodes visited %1. nr of edges visited %2. ")
    .arg(callanalysis->nodesVisited())
    .arg(callanalysis->edgesVisited());

    instance->analysisResult->append(res);  
  }

  cerr << " nr of nodes visited in callanalysis : " << callanalysis->nodesVisited() << endl;
  cerr << " nr of edges visited in callanalysis : " << callanalysis->edgesVisited() << endl;
  
}

void
BinCallGraph::test(SgNode* fileA, SgNode* fileB) {
  testFlag=true;
  run(fileA,fileB);
  testFlag=false;

  
}
