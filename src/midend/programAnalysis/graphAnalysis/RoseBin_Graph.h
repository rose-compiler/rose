/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : Sep26 07
 * Decription : Visualization in DOT
 ****************************************************/

#ifndef __RoseBin_Graph__
#define __RoseBin_Graph__

//#include <mysql.h>
#include <stdio.h>
#include <iostream>

#include "SB_Graph.h"
//#include "RoseBin_support.h"
#include "MyAstAttribute.h"
//#include "RoseBin_unparse_visitor.h"

//#include "RoseBin.h"

class RoseBin_FlowAnalysis;





//class RoseBin_FlowAnalysis;

class RoseBin_Graph : public SB_DirectedGraph {
 protected:
  bool grouping;
  // specifies that this node has no destination address
  bool nodest_jmp ;
  // specifies that there is a node that has a call error (calling itself)
  bool error ;
  // specifies a call to a unknown location
  bool nodest_call;
  // specifies where its an int instruction
  bool interrupt;
  // specifies wheather a node has been visited by the dfa successfully
  bool checked;

  bool dfa_standard ;
  bool dfa_resolved_func;
  bool dfa_unresolved_func;
  bool dfa_conditional;

  //rose_graph_node_edge_hash_multimap unique_edges;
  rose_graph_integer_edge_hash_multimap unique_edges;
 public:


  //RoseBin_Graph(VirtualBinCFG::AuxiliaryInformation* info):SB_DirectedGraph(info) {grouping = true;}
   RoseBin_Graph() {grouping = true;}
   virtual ~RoseBin_Graph() {}
  //RoseBin_unparse_visitor* unparser;
  SgIncidenceDirectedGraph* graph;
  void setGrouping(bool val) {grouping = val;}

  virtual void printEdges( VirtualBinCFG::AuxiliaryInformation* info,
                          bool forward_analysis, std::ofstream& myfile, bool mergedEdges) =0;
  virtual void printNodes( bool dfg, RoseBin_FlowAnalysis* flow, bool forward_analysis,
                           std::ofstream &myfile, std::string& recursiveFunctionName) =0;

  virtual void printProlog(  std::ofstream& myfile, std::string& fileType) =0;
  virtual void printEpilog(  std::ofstream& myfile) =0;

  void createUniqueEdges();

};

#endif
