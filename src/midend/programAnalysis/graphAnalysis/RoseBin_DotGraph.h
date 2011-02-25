/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : Sep26 07
 * Decription : Visualization in DOT
 ****************************************************/

#ifndef __RoseBin_DotGraph__
#define __RoseBin_DotGraph__

#include "RoseBin_Graph.h"
#include "virtualBinCFG.h"

class RoseBin_DotGraph : public RoseBin_Graph {
 private:
  void printInternalNodes( bool dfg,  bool forward_analysis,
                           std::ofstream &myfile, std::string& recursiveFunctionName,
                           SgAsmFunctionDeclaration* p_binFunc);

  typedef std::multimap < SgAsmFunctionDeclaration*,
    std::pair <int, SgGraphNode*> > inverseNodeType;
  inverseNodeType inverse_nodesMap;

  void printEdges_single( VirtualBinCFG::AuxiliaryInformation* info,
                          bool forward_analysis, std::ofstream& myfile);
  void printEdges_multiple( VirtualBinCFG::AuxiliaryInformation* info,
                          bool forward_analysis, std::ofstream& myfile);
  void printEdges( VirtualBinCFG::AuxiliaryInformation* info,
                          bool forward_analysis, std::ofstream& myfile, SgDirectedGraphEdge* edge);

 public:
  RoseBin_DotGraph(): RoseBin_Graph() {}
  ~RoseBin_DotGraph() {}

  void printNodes( bool dfg,  RoseBin_FlowAnalysis* flow, bool forward_analysis,
                   std::ofstream &myfile, std::string& recursiveFunctionName);
  void printNodesCallGraph(std::ofstream& myfile);

  void printEdges( VirtualBinCFG::AuxiliaryInformation* info,
                   bool forward_analysis, std::ofstream& myfile, bool mergedEdges);

  void printProlog(  std::ofstream& myfile, std::string& fileType);
  void printEpilog(  std::ofstream& myfile);

};

#endif
