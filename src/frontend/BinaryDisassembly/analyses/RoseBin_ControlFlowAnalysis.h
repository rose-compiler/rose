/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : Jul27 07
 * Decription : Control flow Analysis
 ****************************************************/

#ifndef __RoseBin_ControlFlowAnalysis__
#define __RoseBin_ControlFlowAnalysis__

#include "RoseBin_FlowAnalysis.h"

//class RoseBin;
//class RoseFile;

class RoseBin_ControlFlowAnalysis : public RoseBin_FlowAnalysis {

 public:

  RoseBin_ControlFlowAnalysis(SgAsmNode* global, bool forward, RoseBin_abstract* , 
			      bool printedges, VirtualBinCFG::AuxiliaryInformation* info):
      RoseBin_FlowAnalysis(global, info) {
    typeNode="CFG";
    typeEdge="CFG-E";  
    analysisName="cfa";
    printEdges = printedges;
    forward_analysis=forward;
  }
  /*
  RoseBin_ControlFlowAnalysis(SgAsmNode* global, bool forward, RoseFile* , 
			      bool printedges, VirtualBinCFG::AuxiliaryInformation* info):
      RoseBin_FlowAnalysis(global, info) {
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
  //			std::string& nameOfFunction, int func_nr);

  // visit the binary AST
  //void visit(SgNode* node) ;

  // run this analysis
  void run(RoseBin_Graph* vg, std::string fileN, bool multiedge) ;

};

#endif

