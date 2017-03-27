#include "sage3basic.h"
#include "stringify.h"
#include "RoseBin_DotGraph.h"
#include "RoseBin_GmlGraph.h"
#include "RoseBin_CallGraphAnalysis.h"
#include <cctype>

using namespace std;
using namespace rose;
using namespace VirtualBinCFG;

#include "../RoseBin_FlowAnalysis.h"

void
RoseBin_DotGraph::printProlog(  std::ofstream& myfile, string& fileType) ROSE_DEPRECATED("no longer supported") {
}

void
RoseBin_DotGraph::printEpilog(  std::ofstream& myfile) ROSE_DEPRECATED("no longer supported") {
}

void
RoseBin_DotGraph::printNodesCallGraph(std::ofstream& myfile) ROSE_DEPRECATED("no longer supported") {
}

void
RoseBin_DotGraph::printNodes(    bool dfg, RoseBin_FlowAnalysis* flow, bool forward_analysis,
                                 std::ofstream& myfile, string& recursiveFunctionName) ROSE_DEPRECATED("no longer supported") {
}

void
RoseBin_DotGraph::printInternalNodes(    bool dfg, bool forward_analysis,
                                         std::ofstream& myfile, string& recursiveFunctionName,
                                         SgAsmFunction* p_binFunc) ROSE_DEPRECATED("no longer supported") {
}
void RoseBin_DotGraph::printEdges( VirtualBinCFG::AuxiliaryInformation* info,
                bool forward_analysis, std::ofstream& myfile, bool mergedEdges) ROSE_DEPRECATED("no longer supported") {
}
void RoseBin_DotGraph::printEdges_single( VirtualBinCFG::AuxiliaryInformation* info,
                bool forward_analysis, std::ofstream& myfile) ROSE_DEPRECATED("no longer supported") {
}
void RoseBin_DotGraph::printEdges_multiple( VirtualBinCFG::AuxiliaryInformation* info,
                bool forward_analysis, std::ofstream& myfile) ROSE_DEPRECATED("no longer supported") {
}
void RoseBin_DotGraph::printEdges( VirtualBinCFG::AuxiliaryInformation* info,
                bool forward_analysis, std::ofstream& myfile, SgDirectedGraphEdge* edge ) ROSE_DEPRECATED("no longer supported") {
}
void
RoseBin_GMLGraph::printProlog(  std::ofstream& myfile, string& fileType) ROSE_DEPRECATED("no longer supported") {
}

void
RoseBin_GMLGraph::printEpilog(  std::ofstream& myfile) ROSE_DEPRECATED("no longer supported") {
}

void
RoseBin_GMLGraph::printNodes(    bool dfg, RoseBin_FlowAnalysis* flow,bool forward_analysis,
                                 std::ofstream& myfile, string& recursiveFunctionName) ROSE_DEPRECATED("no longer supported") {
}
void RoseBin_GMLGraph::printEdges( VirtualBinCFG::AuxiliaryInformation* info,
                        bool forward_analysis, std::ofstream& myfile, bool mergedEdges) ROSE_DEPRECATED("no longer supported") {
}
void RoseBin_GMLGraph::printEdges_single( VirtualBinCFG::AuxiliaryInformation* info,
                        bool forward_analysis, std::ofstream& myfile) ROSE_DEPRECATED("no longer supported") {
}
void RoseBin_GMLGraph::printEdges_multiple( VirtualBinCFG::AuxiliaryInformation* info,
                        bool forward_analysis, std::ofstream& myfile) ROSE_DEPRECATED("no longer supported") {
}
void RoseBin_GMLGraph::printEdges( VirtualBinCFG::AuxiliaryInformation* info, bool forward_analysis, std::ofstream& myfile, SgDirectedGraphEdge* edge) ROSE_DEPRECATED("no longer supported") {
}
