/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : Jul27 07
 * Decription : Control flow Analysis
 ****************************************************/

#ifndef __RoseBin_CallGraphAnalysis__
#define __RoseBin_CallGraphAnalysis__

#include "RoseBin_FlowAnalysis.h"

class RoseBin;
class RoseFile;

class RoseBin_CallGraphAnalysis : public RoseBin_FlowAnalysis {

 public:

  RoseBin_CallGraphAnalysis(SgAsmNode* global, RoseBin_abstract* ):RoseBin_FlowAnalysis(global) {
    typeNode="function";
    typeEdge="CG-E";  
    analysisName="callgraph";
    forward_analysis=true;
  }

  /*
  RoseBin_CallGraphAnalysis(SgAsmNode* global, RoseFile* ):RoseBin_FlowAnalysis(global) {
    typeNode="function";
    typeEdge="CG-E";  
    analysisName="callgraph";
    forward_analysis=true;
  }
  */
  ~RoseBin_CallGraphAnalysis() {
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

