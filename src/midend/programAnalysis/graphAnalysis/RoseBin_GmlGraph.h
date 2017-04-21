/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : Sep26 07
 * Decription : Visualization in GML
 ****************************************************/

#ifndef __RoseBin_GMLGraph__
#define __RoseBin_GMLGraph__

#include "RoseBin_Graph.h"


class ROSE_DLL_API RoseBin_GMLGraph : public RoseBin_Graph {
 private:
  std::string getInternalNodes(SgGraphNode* node,
                               bool forward_analysis, SgAsmNode* internal) ROSE_DEPRECATED("no longer supported");

  std::map < SgAsmStatement*, int > nodesMap;
  std::map < SgAsmFunction*, int > funcMap;

  void printEdges_single( VirtualBinCFG::AuxiliaryInformation* info,
                          bool forward_analysis, std::ofstream& myfile) ROSE_DEPRECATED("no longer supported");
  void printEdges_multiple( VirtualBinCFG::AuxiliaryInformation* info,
                          bool forward_analysis, std::ofstream& myfile) ROSE_DEPRECATED("no longer supported");
  void printEdges( VirtualBinCFG::AuxiliaryInformation* info,
                          bool forward_analysis, std::ofstream& myfile, SgDirectedGraphEdge* edge) ROSE_DEPRECATED("no longer supported");

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
                   std::ofstream &myfile, std::string& recursiveFunctionName) ROSE_DEPRECATED("no longer supported");

  void printEdges( VirtualBinCFG::AuxiliaryInformation* info,
                   bool forward_analysis, std::ofstream& myfile, bool mergedEdges) ROSE_DEPRECATED("no longer supported");

  void printProlog(  std::ofstream& myfile, std::string& fileType) ROSE_DEPRECATED("no longer supported");
  void printEpilog(  std::ofstream& myfile) ROSE_DEPRECATED("no longer supported");

};

#endif
