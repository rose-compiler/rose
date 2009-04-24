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

//#include "RoseBin_support.h"
#include "MyAstAttribute.h"
//#include "RoseBin_unparse_visitor.h"

//#include "RoseBin.h"

class RoseBin_FlowAnalysis;

namespace RoseBin_Def {

  enum GraphProperties {
    name,  // name of the node
    type,  // name of the node
    nodest_jmp, // this a jump without dest
    itself_call, // this call calls itself
    nodest_call, // call without dest
    interrupt, // interrupt (int)
    eval, // dfa : evaluation
    regs, // dfa : registers
    done, // dfa : indicates that this node has successfully been visited
    dfa_standard, // dfa_standard_node color
    dfa_resolved_func, // a function that has been resolved by dfa
    dfa_unresolved_func, // a function that cant be resolved by dfa
    dfa_variable, // defines the variable found by dfa
    dfa_conditional_def, // specifies the nodes that are defines but also conditional instructions
    edgeLabel,
    visitedCounter, // counts the amount of a node being visited during dfa
    variable, // stores the string of the variable
    dfa_bufferoverflow // indicates buffer overflow
  };

  extern GraphProperties RoseProperties;
}

namespace RoseBin_Edgetype {
 enum Edgetype {
    none,
    cfg,
    usage
 };

 extern Edgetype GraphEdgetype;
}

//class RoseBin_FlowAnalysis;

class RoseBin_Graph {
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



 public:

  VirtualBinCFG::AuxiliaryInformation* info;

// DQ (4/23/2009): Modify the hash_map template type to explicitly include default parameters.
// typedef rose_hash::hash_map <std::string, SgDirectedGraphNode*> nodeType;
  typedef rose_hash::hash_map <std::string, SgDirectedGraphNode*,rose_hash::hash_string,rose_hash::eqstr_string> nodeType;

  nodeType nodes;

  typedef rose_hash::hash_multimap < SgDirectedGraphNode*, SgDirectedGraphEdge*> edgeType;
  edgeType edges;
  edgeType edgesR;

  typedef rose_hash::hash_multimap < SgDirectedGraphNode*, SgDirectedGraphEdge*> edgeTypeUnique;
  edgeTypeUnique unique_edges;

  void createUniqueEdges();

  RoseBin_Graph(VirtualBinCFG::AuxiliaryInformation* info): info(info) {grouping = true;}
  virtual ~RoseBin_Graph() {}
  //RoseBin_unparse_visitor* unparser;
  SgDirectedGraph* graph;

  std::string getProperty(RoseBin_Def::GraphProperties property, SgDirectedGraphNode* node);
  std::string getProperty(RoseBin_Def::GraphProperties property, SgDirectedGraphEdge* edge);
  void setProperty(RoseBin_Def::GraphProperties property, SgDirectedGraphNode* node,
		   std::string value);
  void setProperty(RoseBin_Def::GraphProperties property, SgDirectedGraphEdge* edge,
		   std::string value);

  void setGrouping(bool val) {grouping = val;}

  virtual void printEdges( bool forward_analysis, std::ofstream& myfile, bool mergedEdges) =0;
  virtual void printNodes( bool dfg, RoseBin_FlowAnalysis* flow, bool forward_analysis,  
			   std::ofstream &myfile, std::string& recursiveFunctionName) =0;


  virtual SgDirectedGraphNode* createNode(std::string& name, std::string& type, int address, int graph_id, 
					  bool isFunction, SgNode* int_node) ;
  SgDirectedGraphEdge* createEdge(std::string& type, int graph_id,
		  SgDirectedGraphNode* from,  int from_addr, SgDirectedGraphNode* to, int to_addr);

  SgDirectedGraphNode* getDefinitionForUsage(SgDirectedGraphNode* def);

  SgDirectedGraphNode* checkIfGraphNodeExists(std::string& trg_mnemonic);
  bool checkIfGraphEdgeExists(SgDirectedGraphNode* src);
  bool checkIfGraphEdgeExists(SgDirectedGraphNode* src, SgDirectedGraphNode* trg);

  std::vector <SgDirectedGraphNode*> getSuccessors(SgDirectedGraphNode* node);
  std::vector <SgDirectedGraphNode*> getPredecessors(SgDirectedGraphNode* node);

  void getSuccessors(SgDirectedGraphNode* node, std::vector <SgDirectedGraphNode*>& vec );
  void getPredecessors(SgDirectedGraphNode* node, std::vector <SgDirectedGraphNode*>& vec );
  void getDirectCFGSuccessors(SgDirectedGraphNode* node, std::vector <SgDirectedGraphNode*>& vec );
  void getDirectCFGPredecessors(SgDirectedGraphNode* node, std::vector <SgDirectedGraphNode*>& vec );

  virtual void printProlog(  std::ofstream& myfile, std::string& fileType) =0;
  virtual void printEpilog(  std::ofstream& myfile) =0;

  std::set<SgDirectedGraphEdge*> getEdge(SgDirectedGraphNode* src);
  std::set<SgDirectedGraphEdge*> getEdge(SgDirectedGraphNode* src, SgDirectedGraphNode* trg);

  //std::string getEdgeKeyName(  SgDirectedGraphNode* from, int from_addr,
  //			       SgDirectedGraphNode* to, int to_addr);

  bool isValidCFGEdge(SgDirectedGraphNode* sgNode, SgDirectedGraphNode* sgNodeBefore);
  bool isDirectCFGEdge(SgDirectedGraphNode* sgNode,SgDirectedGraphNode* sgNodeBefore);

};

#endif
