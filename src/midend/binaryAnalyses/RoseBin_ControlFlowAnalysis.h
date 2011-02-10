/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : Jul27 07
 * Decription : Control flow Analysis
 ****************************************************/

#ifndef __RoseBin_ControlFlowAnalysis__
#define __RoseBin_ControlFlowAnalysis__

#include "RoseBin_FlowAnalysis.h"
#include "RoseBin_abstract.h"
//class RoseBin;
//class RoseFile;

class RoseBin_ControlFlowAnalysis : public RoseBin_FlowAnalysis {
  void getCFGNodesForFunction(std::set<SgGraphNode*>& visited_f,
                              std::set<std::string>& visited_names,
                              SgGraphNode* next_n, std::string nodeName);

 public:

  RoseBin_ControlFlowAnalysis(SgAsmNode* global, bool forward, RoseBin_abstract* ,
                              bool printedges,GraphAlgorithms* algo):
  RoseBin_FlowAnalysis(global,algo) {
    typeNode="CFG";
    typeEdge="CFG-E";
    analysisName="cfa";
    printEdges = printedges;
    forward_analysis=forward;
  }
  /*
  RoseBin_ControlFlowAnalysis(SgAsmNode* global, bool forward, RoseFile* ,
                              bool printedges, GraphAlgorithms* algo):
      RoseBin_FlowAnalysis(global, algo) {
    typeNode="CFG";
    typeEdge="CFG-E";
    analysisName="cfa";
    printEdges = printedges;
    forward_analysis=forward;
  }
  */

  ~RoseBin_ControlFlowAnalysis() {
    delete globalBin;
    //delete roseBin;
    delete vizzGraph;

    std::map <std::string, SgAsmFunctionDeclaration* >::iterator it;
    for (it = bin_funcs.begin();
         it!= bin_funcs.end(); it++) {
      delete it->second;
    }
  }

  //void checkControlFlow(SgAsmInstruction* binInst, int functionSize, int countDown,
  //                    std::string& nameOfFunction, int func_nr);

  // visit the binary AST
  //void visit(SgNode* node) ;

  // run this analysis
  void run(RoseBin_Graph* vg, std::string fileN, bool multiedge) ;
  void printGraph(std::string fileName, std::set<std::string>& filter);

};

#endif

