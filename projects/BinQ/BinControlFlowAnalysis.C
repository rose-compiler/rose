#include "rose.h"
#include "BinQGui.h"

#include <iostream>


#include "BinQSupport.h"
#include "slide.h"

#include <qtabwidget.h>
#include "BinControlFlowAnalysis.h"

using namespace qrs;
using namespace std;
using namespace __gnu_cxx;

std::string BinControlFlowAnalysis::name() {
  return "Control Flow Graph";
}

std::string BinControlFlowAnalysis::getDescription() {
  return "Creates a control flow graph. Outputs cfg.dot";
}


void
BinControlFlowAnalysis::run(SgNode* fileA, SgNode* fileB) {
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


  // control flow analysis  *******************************************************
  if (!testFlag) {
  ROSE_ASSERT(instance);
  ROSE_ASSERT(instance->analysisTab);
  instance->analysisTab->setCurrentIndex(1);
  QString res = QString("Creating control flow graph ");
  instance->analysisResult->append(res);  
  }
  string cfgFileName = "cfg.dot";
  graph= new RoseBin_DotGraph(info);
  bool dot=true;
  bool forward=true;
  bool edges=true;
  bool mergedEdges=true;
  if (dot==false) {
    cfgFileName = "cfg.gml";
    graph= new RoseBin_GMLGraph(info);
  }


  SgAsmInterpretation* interp = SageInterface::getMainInterpretation(file);
  RoseBin_ControlFlowAnalysis* cfganalysis = 
    new RoseBin_ControlFlowAnalysis(interp->get_global_block(), forward, new RoseObj(), edges, info);
  ROSE_ASSERT(cfganalysis);
  cfganalysis->run(graph, cfgFileName, mergedEdges);
  if (!testFlag) {
  QString res = QString("nr of nodes visited %1. nr of edges visited %2. ")
    .arg(cfganalysis->nodesVisited())
    .arg(cfganalysis->edgesVisited());
    
  instance->analysisResult->append(res);  
  }  
}

void
BinControlFlowAnalysis::test(SgNode* fileA, SgNode* fileB) {
  testFlag=true;
  run(fileA,fileB);
  testFlag=false;
  
}
