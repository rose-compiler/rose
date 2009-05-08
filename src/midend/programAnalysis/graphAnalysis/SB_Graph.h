
/****************************************************
 * Graph Interface for Source Code and Binary
 * Author : tps
 * Date : 27Apr 09
 ****************************************************/

#ifndef __SB_Graph__
#define __SB_Graph__




#if 0
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

#endif


#if 0
class SgGraphNode;
namespace __gnu_cxx {
  template <> struct hash <SgGraphNode*> {
    size_t operator()(SgGraphNode* const & n) const {
      return (size_t) n;
    }
  };
}
#endif
#if 0
class GraphNodeList {
 public:
   	typedef rose_graph_hash_multimap nodeType;
   //typedef rose_hash::hash_map <std::string, SgGraphNode*,rose_hash::hash_string,rose_hash::eqstr_string> nodeType;

 nodeType nodes;
 nodeType get_nodes() {return nodes;}

};
class GraphEdgeList {
 public:
	typedef rose_graph_node_edge_hash_multimap edgeType;
	rose_graph_node_edge_hash_multimap edges;
	rose_graph_node_edge_hash_multimap redges;

	//typedef rose_hash::hash_multimap < SgGraphNode*, SgDirectedGraphEdge*> edgeType;
        //typedef rose_hash::hash_multimap <SgGraphNode*, SgGraphEdge*, rose_hash::hash_graph_node,
	//	rose_hash::eqstr_graph_node> edgeType;


	edgeType get_edges() {return edges;}
	edgeType get_reverse_edges() { return redges;}

};
#endif
class SB_DirectedGraph : public SgIncidenceDirectedGraph {
public:


#if 0
	typedef rose_hash::hash_map <std::string, SgDirectedGraphNode*,rose_hash::hash_string,rose_hash::eqstr_string> nodeType;
        nodeType nodes;

	typedef rose_hash::hash_multimap < SgGraphNode*, SgDirectedGraphEdge*> edgeType;
	edgeType edges;
	edgeType redges;

	typedef rose_hash::hash_multimap < SgGraphNode*, SgDirectedGraphEdge*> edgeTypeUnique;
	edgeTypeUnique unique_edges;

	//	typedef rose_graph_node_edge_hash_multimap edgeTypeUnique;
	//rose_graph_node_edge_hash_multimap unique_edges;

	GraphNodeList* nodes;
	GraphEdgeList* edges;
	GraphEdgeList* redges;
//	  VirtualBinCFG::AuxiliaryInformation* info;
#endif
public:
#if 0
	GraphNodeList* get_nodes() {return nodes;}
	GraphEdgeList* get_edges() {return edges;}
	GraphEdgeList* get_reverse_edges() { return redges;}
	SB_DirectedGraph() {
	  nodes = new GraphNodeList();
	  edges = new GraphEdgeList();
	  redges = new GraphEdgeList();
	  ROSE_ASSERT(nodes);
	  ROSE_ASSERT(edges);
	  ROSE_ASSERT(redges);
	  ROSE_ASSERT(get_edges());
	}
#endif
//	SB_DirectedGraph(VirtualBinCFG::AuxiliaryInformation* info): info(info) {}
	virtual ~SB_DirectedGraph() {}
//	VirtualBinCFG::AuxiliaryInformation* get_bininfo() {return info;}


  // add to SgGraph
#if 0
	  std::string getProperty(SgGraph::GraphProperties property, SgGraphNode* node);
	  std::string getProperty(SgGraph::GraphProperties property, SgGraphEdge* edge);
	  void setProperty(SgGraph::GraphProperties property, SgGraphNode* node,
			   std::string value);
	  void setProperty(SgGraph::GraphProperties property, SgGraphEdge* edge,
			   std::string value);
  std::set<SgDirectedGraphEdge*> getEdge(SgGraphNode* src);
  SgGraphNode* checkIfGraphNodeExists(std::string& trg_mnemonic);
  bool checkIfGraphEdgeExists(SgGraphNode* src);
  // add to SgIncidenceDirectedGraph
  std::set<SgDirectedGraphEdge*> getDirectedEdge(SgGraphNode* src, SgGraphNode* trg);
  bool checkIfDirectedGraphEdgeExists(SgGraphNode* src, SgGraphNode* trg);
  // void createUniqueEdges();
  void getSuccessors(SgGraphNode* node, std::vector <SgGraphNode*>& vec );
  void getPredecessors(SgGraphNode* node, std::vector <SgGraphNode*>& vec );

  SgDirectedGraphEdge* addDirectedEdge(SgGraphNode* from, SgGraphNode* to, std::string& type) {
	   SgDirectedGraphEdge* edge = new SgDirectedGraphEdge(from,to,type);
	   edges->get_edges().insert(std::pair<SgGraphNode*,SgDirectedGraphEdge*>( from, edge)) ;
	   redges->get_edges().insert(std::pair<SgGraphNode*,SgDirectedGraphEdge*>( to, edge)) ;
	   return edge;
  }
  //   SgDirectedGraphEdge* createSBEdge(std::string& type, int graph_id,
  //				  SgGraphNode* from,
  //   				  SgGraphNode* to);
     SgGraphNode* addNode( const std::string & name = "", SgNode* sg_node = NULL);
#endif



#if 0
  SgGraphNode* getDefinitionForUsage(SgGraphNode* def);

  void getDirectCFGSuccessors(SgGraphNode* node, std::vector <SgGraphNode*>& vec );
   void getDirectCFGPredecessors(SgGraphNode* node, std::vector <SgGraphNode*>& vec );
   bool isValidCFGEdge(SgGraphNode* sgNode, SgGraphNode* sgNodeBefore);
   bool isDirectCFGEdge(SgGraphNode* sgNode,
 		     SgGraphNode* sgNodeBefore);
#endif



};




#endif


