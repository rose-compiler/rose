
/****************************************************
 * Graph Interface for Source Code and Binary
 * Author : tps
 * Date : 27Apr 09
 ****************************************************/

#ifndef __SB_Graph__
#define __SB_Graph__

namespace SB_Edgetype {
 enum Edgetype {
    none,
    cfg,
    usage
 };

 extern Edgetype GraphEdgetype;
}

namespace SB_Graph_Def {

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




class SB_Graph {


};

class SB_DirectedGraph : public SB_Graph {
public:
	// DQ (4/23/2009): Modify the hash_map template type to explicitly include default parameters.
	// typedef rose_hash::hash_map <std::string, SgDirectedGraphNode*> nodeType;
    typedef rose_hash::hash_map <std::string, SgDirectedGraphNode*,rose_hash::hash_string,rose_hash::eqstr_string> nodeType;

    nodeType nodes;
//	SgIncidenceGraphNode* nodes;

	typedef rose_hash::hash_multimap < SgDirectedGraphNode*, SgDirectedGraphEdge*> edgeType;
	edgeType edges;
	edgeType edgesR;

	typedef rose_hash::hash_multimap < SgDirectedGraphNode*, SgDirectedGraphEdge*> edgeTypeUnique;
	edgeTypeUnique unique_edges;

	  VirtualBinCFG::AuxiliaryInformation* info;

public:
	SB_DirectedGraph() { }
	SB_DirectedGraph(VirtualBinCFG::AuxiliaryInformation* info): info(info) {}
	virtual ~SB_DirectedGraph() {}
	VirtualBinCFG::AuxiliaryInformation* get_bininfo() {return info;}

	void getSuccessors(SgDirectedGraphNode* node, std::vector <SgDirectedGraphNode*>& vec );
	void getPredecessors(SgDirectedGraphNode* node, std::vector <SgDirectedGraphNode*>& vec );

	//nodeType get_nodes() {return nodes;}
	//edgeType get_edges() {return edges;}

  std::string getProperty(SB_Graph_Def::GraphProperties property, SgDirectedGraphNode* node);
  std::string getProperty(SB_Graph_Def::GraphProperties property, SgDirectedGraphEdge* edge);
  void setProperty(SB_Graph_Def::GraphProperties property, SgDirectedGraphNode* node,
		   std::string value);
  void setProperty(SB_Graph_Def::GraphProperties property, SgDirectedGraphEdge* edge,
		   std::string value);

  bool checkIfGraphEdgeExists(SgDirectedGraphNode* src);
  bool checkIfGraphEdgeExists(SgDirectedGraphNode* src, SgDirectedGraphNode* trg);

  std::set<SgDirectedGraphEdge*> getEdge(SgDirectedGraphNode* src);
  std::set<SgDirectedGraphEdge*> getEdge(SgDirectedGraphNode* src, SgDirectedGraphNode* trg);

  SgDirectedGraphEdge* createSBEdge(std::string& type, int graph_id,
  				  SgDirectedGraphNode* from,
  				  SgDirectedGraphNode* to);
  SgDirectedGraphNode* createSBNode(std::string& name, std::string& type, int graph_id,
  					 SgNode* int_node);
  SgDirectedGraphNode* getDefinitionForUsage(SgDirectedGraphNode* def);

  void createUniqueEdges();
  SgDirectedGraphNode* checkIfGraphNodeExists(std::string& trg_mnemonic);


  void getDirectCFGSuccessors(SgDirectedGraphNode* node, std::vector <SgDirectedGraphNode*>& vec );
   void getDirectCFGPredecessors(SgDirectedGraphNode* node, std::vector <SgDirectedGraphNode*>& vec );
   bool isValidCFGEdge(SgDirectedGraphNode* sgNode, SgDirectedGraphNode* sgNodeBefore);
   bool isDirectCFGEdge(SgDirectedGraphNode* sgNode,
 		     SgDirectedGraphNode* sgNodeBefore);



   virtual SgDirectedGraphNode* createNode(std::string& name, std::string& type, int address, int graph_id,
 					  bool isFunction, SgNode* int_node) ;
   SgDirectedGraphEdge* createEdge(std::string& type, int graph_id,
 		  SgDirectedGraphNode* from,  int from_addr, SgDirectedGraphNode* to, int to_addr);

};




#endif


