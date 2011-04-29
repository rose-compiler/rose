/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : Jul27 07
 * Decription : Control flow Analysis
 ****************************************************/

#ifndef __RoseBin_CallGraphAnalysis__
#define __RoseBin_CallGraphAnalysis__

#include "RoseBin_FlowAnalysis.h"
#include "RoseBin_abstract.h"
//class RoseBin;
//class RoseFile;

class RoseBin_CallGraphAnalysis : public RoseBin_FlowAnalysis {
  void findClusterOfNode(SgGraphNode* next_n,
                         int& currentCluster,
                         std::map<SgAsmFunctionDeclaration*,int>& visited);

 public:

 RoseBin_CallGraphAnalysis(SgAsmNode* global, RoseBin_abstract* ,
                           GraphAlgorithms* algo):RoseBin_FlowAnalysis(global,algo) {
    typeNode="function";
    typeEdge="CG-E";
    analysisName="callgraph";
    forward_analysis=true;
  }

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


  // run this analysis
  void run(RoseBin_Graph* vg, std::string fileN, bool multiedge) ;
  void getConnectedComponents(std::map<int,std::set<SgAsmFunctionDeclaration*> >& ret);

};

#endif

