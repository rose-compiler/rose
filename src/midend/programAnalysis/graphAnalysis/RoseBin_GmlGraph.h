/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : Sep26 07
 * Decription : Visualization in GML
 ****************************************************/

#ifndef __RoseBin_GMLGraph__
#define __RoseBin_GMLGraph__

#include "RoseBin_Graph.h"


class RoseBin_GMLGraph : public RoseBin_Graph {
 private:
  std::string getInternalNodes(SgGraphNode* node,
                               bool forward_analysis, SgAsmNode* internal);

  std::map < SgAsmStatement*, int > nodesMap;
  std::map < SgAsmFunctionDeclaration*, int > funcMap;

  void printEdges_single( VirtualBinCFG::AuxiliaryInformation* info,
                          bool forward_analysis, std::ofstream& myfile);
  void printEdges_multiple( VirtualBinCFG::AuxiliaryInformation* info,
                          bool forward_analysis, std::ofstream& myfile);
  void printEdges( VirtualBinCFG::AuxiliaryInformation* info,
                          bool forward_analysis, std::ofstream& myfile, SgDirectedGraphEdge* edge);

  bool skipFunctions;
  bool skipInternalEdges;
  bool onlyControlStructure;

 public:
  RoseBin_GMLGraph() {

    //added this for vizz3d visualization
    skipFunctions=false;
    skipInternalEdges=true;
    onlyControlStructure=true;
  }
  virtual ~RoseBin_GMLGraph() {}

  void printNodes( bool dfg, RoseBin_FlowAnalysis* flow,  bool forward_analysis,
                   std::ofstream &myfile, std::string& recursiveFunctionName);

  void printEdges( VirtualBinCFG::AuxiliaryInformation* info,
                   bool forward_analysis, std::ofstream& myfile, bool mergedEdges);

  void printProlog(  std::ofstream& myfile, std::string& fileType);
  void printEpilog(  std::ofstream& myfile);

};

#endif
