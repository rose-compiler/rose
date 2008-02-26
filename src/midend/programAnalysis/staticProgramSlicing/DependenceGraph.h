#ifndef _DEPENDENCEGRAPH_H_
#define _DEPENDENCEGRAPH_H_

#include "AstInterface.h"
#include "StmtInfoCollect.h"
#include "ReachingDefinition.h"
#include "DefUseChain.h"

#include <ostream>
#include <string>
#include <map>
#include <utility>
#include <set>
#include "DominanceFrontier.h"
#include "SimpleDirectedGraph.h"

// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
// using namespace std;

/*! \class DependenceNode

  This class represents a node in a dependence graph. By inheriting
  from SimpleDirectedGraphNode, it keeps track of both its successors and
  its predecessors. The node can be one of several different types,
  indicating its role in a dependence graph. The node may also contain
  a pointer to an SgNode.

  The use of this class requires that the file SimpleDirectedGraph.h exist.

*/

class DependenceNode : public SimpleDirectedGraphNode {

public:  

  //! This enum notes what type of node this is
  enum NodeType {
    CONTROL,    /*!< Used to indicate a dummy node for control dependence*/
    SGNODE,     /*!< Used to indicate a node with an SgNode in it*/
    CALLSITE,   /*!< Used to indicate a call-site node for interprocedural slicing*/
    ACTUALIN,   /*!< Used to indicate the arguments at a call site*/
    ACTUALOUT,  /*!< Used to indicate returned values at a call site*/
    FORMALIN,   /*!< Used to indicate the arguments into a function*/
    FORMALOUT,  /*!< Used to indicate the return values from a function*/
    ENTRY,      /*!< Used to indicate the entry point of a function*/
    NUM_NODE_TYPES /*!< Must be last enum in list to establish number of types*/
  };

  /*! \brief Holds C-strings associated with the node types

  The array is initialized in DependenceGraph.C
  */
  static char* typeNames[NUM_NODE_TYPES];

  /*!

  \brief Constructor for DependenceNode, defines type, SgNode it contains, and name

  Every DependenceNode has a type. Some types have an associated SgNode with them:
     - SGNODE: the SgNode that the DependenceNode represents
     - CALLSITE: the SgFunctionCallExp associated with the function call
     - ENTRY: the SgFunctionDeclaration for the function
     - ACTUALIN/OUT: the SgExpressions that are the arguments to the function call

  The FORMALIN/OUT nodes have a name associated with them (the name of
  the variable in the parameter list).

  CONTROL nodes have neither associated SgNodes nor names.

  If we build a node using this constructor, the _copiedfrom member is NULL.
  */
  DependenceNode(NodeType type, SgNode * node = NULL,std::string name = "") : _type(type),
									  _node(node),
									  _name(name),
									  _copiedfrom(NULL)
  {}

  //! If only a name is provided, we assume that there is no associated SgNode
  DependenceNode(NodeType type, std::string name) : _type(type),
					       _name(name),
					       _node(NULL),
					       _copiedfrom(NULL)
  {}

  /*! \brief Copy constructor

  If we use the copy constructor, we save a link to the DependenceNode
  we copied this from (useful for determining whether two separate
  dependence nodes were copied from the same source).
  */
  DependenceNode(DependenceNode * other) : _type(other->_type),
					   _name(other->_name),
					   _node(other->_node),
					   _copiedfrom(other)
  {}
  
  //! returns the associated SgNode
  SgNode * getNode() {return _node;}

  //! returns the type
  NodeType getType() {return _type;}

  //! returns the associated name
  std::string getName() {return _name;}

  //! returns the DependenceNode this one was copied from
  DependenceNode * getCopiedFrom() {return _copiedfrom;}

  /*! \brief returns whether a node has an interprocedural type

  Certain node types are only used during interprocedural slicing
  (i.e. when we use a SystemDependenceGraph. These are:
     - CALLSITE
     - ACTUALIN
     - ACTUALOUT
     - FORMALIN
     - FORMALOUT
     - ENTRY
  */
  bool isInterproc() {
    switch(_type) {
    case CALLSITE:
    case ACTUALIN:
    case ACTUALOUT:
    case FORMALIN:
    case FORMALOUT:
    case ENTRY:
      return true;
    default:
      return false;
    }
  }

  //! returns whether a node is a formal argument
  bool isFormal() {
    switch(_type) {
    case FORMALIN:
    case FORMALOUT:
      return true;
    default:
      return false;
    }
  }

  //! returns whether a node is an actual argument
  bool isActual() {
    switch(_type) {
    case ACTUALIN:
    case ACTUALOUT:
      return true;
    default:
      return false;
    }
  }

  //! Prints the contents of the node to os.
  virtual void writeOut(std::ostream &os) {
    char buf[sizeof(DependenceNode *)*2 + 3];
    sprintf(buf, "%p", this);
    os << buf << ": ";
    os << typeNames[_type] << " ";
    if (_name != "") os << "[" << _name << "] ";
    if (_node != NULL) {
      char buf[sizeof(SgNode *)*2 + 3];
      sprintf(buf, "%p", _node);
      os << buf << ":" << _node->sage_class_name() << " ";
      if (isSgExpressionRoot(_node)) {
	os << "[" << isSgExpressionRoot(_node)->get_operand()->unparseToString() << "]";
      } else if (isSgFunctionDeclaration(_node)) {
	os << "[" << isSgFunctionDeclaration(_node)->get_name().str() << "]";
      } else {
	os << "[" << _node->unparseToString() << "]";
      }
    }
  }
  
private:
  
  SgNode * _node;
  NodeType _type;
  std::string _name;
  DependenceNode * _copiedfrom;

};

class InterproceduralInfo;

/*! \class DependenceGraph

  This class is a SimpleDirectedGraph which uses DependenceNodes. However,
  unlike a standard SimpleDirectedGraph, the SimpleDependenceGraph may contain
  several different types of edges, as indicated by EdgeType.

  The use of this class requires that the file SimpleDirectedGraph.h exist

*/

class DependenceGraph : public SimpleDirectedGraph {

public:

  /*! \brief This enum marks what type of edge connects two DependenceNodes
    
  This enum is used in conjunction with bit vector representations, so
  some of the values are powers of two
  */
  enum EdgeType {
    CONTROL = 0x1,         /*!< A control dependence edge*/
    DATA = 0x2,            /*!< A data dependence edge*/
    SUMMARY = 0x4,         /*!< A summary edge between actual-in and actual-out nodes (used for interprocedural*/
    CALL = 0x8,            /*!< An edge between a call site and a function entry, or from actual-in to formal-in nodes*/
    RETURN = 0x10,         /*!< An edge from formal-out nodes to actual-in nodes*/
    NUM_EDGE_TYPES = 0x11  /*!< Set to be 1 more than the last entry, to fix size of the name array */
  };

  /*! \brief create a new DependenceNode that is a copy of node

  Params:
    - DependenceNode * node: the node we want to copy
  
  Return: Either a newly created copy, or, if this has been copied
    before, the previous copy

  Side effects: If we created a new copy, we insert a mapping from
    node to the newly created copy in _depnode_map.
  */
  DependenceNode * createNode(DependenceNode * node);

  /*! \brief retrieve the DependenceNode associated with node

  Params:
    - DependenceNode * node: the node we want to find a copy for

  Return: If node exists in _depnode_map, we return the associated
    copy, otherwise we return NULL.
  */
  DependenceNode * getNode(DependenceNode * node);

  /*! \brief create a new DependenceNode which contains node

  Params:
    - SgNode * node: the SgNode we want to wrap in a DependenceNode

  Return: Either a new DependenceNode or, if we've already wrapped
    this SgNode, the existing DependenceNode containing node

  Side effects: If we created a new DependenceNode, we insert a
    mapping from node to the newly created DependenceNode in
    _sgnode_map.
  */
  DependenceNode * createNode(SgNode * node);

  /*! \brief retrieve the DependenceNode that wraps node

  Params:
    - SgNode * node: the SgNode for which we want to find the associated DependenceNode

  Return: If there is a wrapper DependenceNode in _sgnode_map, we
    return it. Otherwise, we return NULL.
  */
  DependenceNode * getNode(SgNode * node);

  //! return the InterproceduralInfo object associated with the DependenceGraph
  InterproceduralInfo * getInterprocedural() {return _interprocedural;}

  /*! \brief create an edge of type e between from and to

  Params:
    - DependenceNode * from: the source of the edge
    - DependenceNode * to: the sink of the edge
    - EdgeType e: the type of the edge

  Side effects: Inserts the Edge (from, to) into the set associated
    with e by _edgetype_map. Inserts e into the set associated with
    Edge(from, to) by _edge_map.

  */
  virtual void establishEdge(DependenceNode * from, DependenceNode * to, EdgeType e);

  /*! \brief determine if there is an edge of type e between from and to

  Params:
    - DependenceNode * from: the source of the edge
    - DependenceNode * to: the sink of the edge
    - EdgeType e: the type of the edge

  Return: true if e is in the set associated with Edge(from, to) by _edge_map.
  */
  bool edgeExists(DependenceNode * from, DependenceNode * to, EdgeType e);

  /*! \brief returns all edges between from and to

  Params:
    - DependenceNode * from: the source of the edge
    - DependenceNode * to: the sink of the edge

  Return: the set of EdgeTypes associated with Edge(from, to) by _edge_map.

  */
  std::set<EdgeType> edgeType(DependenceNode * from, DependenceNode * to);

  //! writes a dot file representing this dependence graph to filename
  virtual void writeDot(char * filename);

protected:
  //! Maps a DependenceNode to a copy unique to this DependenceGraph
  std::map<DependenceNode *, DependenceNode *> _depnode_map;
  //! Maps an SgNode to a DependenceNode unique to this DependenceGraph
  std::map<SgNode *, DependenceNode *> _sgnode_map;

  //! The InterproceduralInfo associated with this DependenceGraph
  InterproceduralInfo * _interprocedural;

  typedef pair<DependenceNode *, DependenceNode *> Edge;

  //! a map from EdgeType to all the edges of that type
  std::map<EdgeType, std::set<Edge> > _edgetype_map;
  //! a map from an edge to all the variants of that edge in the graph
  std::map<Edge, std::set<EdgeType> > _edge_map;

  /*! \brief an array of C-strings for each EdgeType

  This array is initialized in DependenceGraph.C
  */
  static char * edgeNames[NUM_EDGE_TYPES];

};

/*! \class ControlDependenceGraph

  This class is a DependenceGraph which expresses control dependences
  for a given procedure. Control dependences essentially link decision
  points in CFGs with the nodes affected by those decisions. The
  implementation of this class depends on the calculation of Dominator
  Trees and Dominance Frontiers.

  If interprocedural analysis is specified (by passing in a non-null
  InterproceduralInfo object), building this graph will also
  initialize the InterproceduralInfo object by creating the required
  formal-in, formal-out, actual-in, actual-out and entry nodes (as
  well as any required call nodes) for the procedure, and creating the
  appropriate control dependences:
     - all actual-in and actual-out nodes are control dependent on the call site
     - all formal-in and formal-out nodes are control dependent on the entry node
     - the call site node is control dependent on the DependenceNode representing the function call.


  The use of this class requires that DominanceFrontier.h exist. Also,
  the use of this class requires linking against libDominance.

*/

class ControlDependenceGraph : public DependenceGraph {

public:

  /*! \brief Contstructor for ControlDependenceGraph
    
  Params:
    - SgNode * head: The root of the AST that you want to build the CDG for
    - InterproceduralInfo * ii: the InterproceduralInfo object for storing interprocedural information

  Side effects:
    - initializes _interprocedural

  If ii is NULL, we assume that we are not doing interprocedural
  analysis. Otherwise, we assume that ii is a newly allocated (but not
  yet initialized) object.
  */
  ControlDependenceGraph(SgNode * head, InterproceduralInfo * ii = NULL);

  /*! \brief create a DependenceNode wrapping cnode
   
  Params:
    - ControlNode * cnode: The ControlNode we want to wrap in a DependenceNode

  Return: If we have already wrapped this node, we return the existing
    DependenceNode. Otherwise we create a new DependenceNode (of type
    CONTROL) and return it.

  Side effects: If a new DependenceNode is created, cnode is mapped to
    it by _cnode_map.

  */
  DependenceNode * createNodeC(DominatorTreesAndDominanceFrontiers::ControlNode * cnode);

  //! calls establishEdge with EdgeType defaulted to CONTROL
  virtual void establishEdge(DependenceNode * from, DependenceNode * to) {
    DependenceGraph::establishEdge(from, to, CONTROL);
  }

private:

  //! builds the CDG
  void _buildCDG();

  //! if we're performing interprocedural analysis, initializes the InterproceduralInfo object
  void _buildInterprocedural();

  //! DEPRECATED. Not used anymore.
  void _addDependence(SgNode * from, SgNode * to);

  //! The root of the AST that the ControlDependenceGraph represents
  SgNode * _head;
  //! The control flow graph for the AST
  DominatorTreesAndDominanceFrontiers::ControlFlowGraph * _cfg;
  //! The dominator tree for the CFG
  DominatorTreesAndDominanceFrontiers::DominatorTree * _dt;
  //! The dominance frontier for nodes in the CFG
  DominatorTreesAndDominanceFrontiers::DominanceFrontier * _df;
  
  //! Maps ControlNodes (nodes in the CFG) to DependenceNodes unique to this CDG
  std::map<DominatorTreesAndDominanceFrontiers::ControlNode *, DependenceNode *> _cnode_map;
};

/*! \class DataDependenceGraph

  This class is a DependenceGraph which expresses data dependences for
  a given procedure. Data dependences link statement that define
  variables with the statements that may use those definitions. The
  implementation of this class depends on the calculation of
  DefUseChains, and so must link against librose.

  If interprocedural analysis is specified (by passing in a non-null
  InterproceduralInfo object which has been initialized by
  ControlDependenceGraph), building this graph will create the
  appropriate data dependences:
     - if a statement uses a variable defined in the parameter list for the function, it is data dependent on the formal-in node
     - if a statement defs a variable used as a function parameter in a call, the actual-in node for the parameter is data dependent on the statement
     - if a statement uses a variable returned from a function, it is data dependent on the actual-out node
     - if a statement is a return statement, the formal-out return is data dependent on it.

  @todo Currently, there is no way to link defs that reach the end of
  the function (other than return statements) to the appropriate
  actual-out nodes (in the case of pass-by-reference arguments).
*/

class DataDependenceGraph : public DependenceGraph {

public:

  /*! \brief Contstructor for DataDependenceGraph
    
  Params:
    - SgNode * head: The root of the AST that you want to build the DDG for
    - InterproceduralInfo * ii: the InterproceduralInfo object for storing interprocedural information

  Side effects:
    - adds data dependence edges to nodes from _interprocedural

  If ii is NULL, we assume that we are not doing interprocedural
  analysis. Otherwise, we assume that ii is an InterproceduralInfo
  object that has been initialized by the CDG for the same procedure
  */
  DataDependenceGraph(SgNode * head, InterproceduralInfo * ii = NULL);

  //! calls establishEdge with EdgeType defaulted to DATA
  virtual void establishEdge(DependenceNode * from, DependenceNode * to) {
    DependenceGraph::establishEdge(from, to, DATA);
  }

private:

  //! builds the def-use chains for the procedure
  void _buildDefUseChains(SgFunctionDefinition * fD);
  //! uses def-use chains to build the data dependence graph
  void _buildDDG();

  //! if doing interprocedural, adds the data dependences from return statements to formal-out statements
  void _processReturns();
  /*! \brief Finds the function argument that use is in

  Params:
    - SgNode * &funcArg: Placeholder for function argument that use is in
    - SgNode * use: The variable use we are trying to find the argument for

  Return: If use was in a function argument, returns the function call
    expression the argument was in. Otherwise returns NULL

  Side effects: if returning non-NULL, funcArg is the function
    argument that use was in.
  */
  SgFunctionCallExp * _findArgExprFromRef(SgNode * &funcArg, SgNode * use);

  //! The function that we are building the DDG for
  SgFunctionDefinition * _head;

  //! The def use chains for the function
  DefaultDUchain _defuse;

};

/*! \class MergedDependenceGraph

  This class provides a mechanism for merging together multiple
  DependenceGraphs. It also provides a mechanism for getting backwards
  reachable nodes given a specific start node and using specified edge
  types. This can be called by the abstract function getSlice to
  create specific types of slices (i.e. intraprocedural or
  interprocedural).

*/
class MergedDependenceGraph : public DependenceGraph {

public:

  /*! \brief creates a new dependence node that reflects the argument (not a direct copy)

  Params:
    - DependenceNode * node: The node we want to make a "copy" of

  Return: If we've already "copied" the node, return the existing
    DependenceNode. Otherwise create a new one.

  Side effects: calls createNode appropriately to perform "copies," so
    _sgnode_map or _depend_map may be updated.

  If the node we are adding is an interprocedural node, we want to
  copy the _interproc pointer, not node itself. If it's an SgNode, we
  want to build the DependenceNode around that, as opposed to node. If
  it's neither, we just copy the argument.
  */
  DependenceNode * _importNode(DependenceNode * node);

  /*! \brief creates a backward slice starting from node

  Params:
    - SgNode * node: the slicing criterion

  Return: returns a set of SgNodes which belong in the slice with
    slicing criterion node.

  This function calls getSlice, and prunes the returned values to find
  just the SgNodes.
  */
  std::set<SgNode *> slice(SgNode * node);

  /*! \brief creates a backward slice starting from node

  Params:
    - DependenceNode * node: the slicing criterion

  Return: returns a set of DependenceNodes which belong in the slice
    with slicing criterion node.

  This is a more general version of slice, which operates on any
  DependenceNode.
  */
  virtual std::set<DependenceNode *> getSlice(DependenceNode * node) = 0;

protected:

  /*! \brief performs a backwards reachability analysis starting from nodes in start
    
  Params: 
    - set<DependenceNode *> start: the starting nodes for the backwards reachability analysis
    - int edgeTypesToFollow: a bit-vector whose set bits indicate the types of edges to consider
                             for reachability.
  */
  std::set<DependenceNode *> _getReachable(std::set<DependenceNode *> start, int edgeTypesToFollow = 0);

  /*! \brief merges graph into the current MergedDependenceGraph

  Params:
    - DependenceGraph * graph: the DependenceGraph we want to merge into the current graph

  Side effects: Any new nodes and edges from graph are added to the MergedDependenceGraph
  */
  void _mergeGraph(DependenceGraph * graph);

};

/*! \class ProgramDependenceGraph

  This is a MergedDependenceGraph which merges a
  ControlDependenceGraph and a DataDependenceGraph for a specific
  procedure, thus representing a complete DependenceGraph for one
  procedure. If we are planning on doing interprocedural analysis
  (i.e. we've passed in a non-null InterproceduralInfo object), then
  we also create summary edges between the actual-in nodes for a call
  and the appropriate actual-out nodes.

  @todo Right now, we assume that the function will have no
  side-effects, and simply create a summary between all the actual-in
  nodes and the actual-out node representing the return. This should
  be extended to create real summary edges as defined in the paper by
  Horwitz et al. However, because we can't correctly handle defs that
  reach the end of functions, we can't properly summarize, and so we
  skip this for now.

*/

class ProgramDependenceGraph : public MergedDependenceGraph {

public:
  /*! \brief Constructor for ProgramDependenceGraph, initialized with
    the CDG and DDG for the function.

  Params:
    - ControlDependenceGraph * cdg: a previously built CDG for the function
    - DataDependenceGraph * ddg: a previously build DDG for the function
    - InterproceduralInfo * ii: If NULL, we aren't doing interprocedural. Otherwise, the
                                fully initialized InterproceduralInfo object for the function.

  */
  ProgramDependenceGraph(ControlDependenceGraph * cdg, DataDependenceGraph * ddg,
			 InterproceduralInfo * ii = NULL);

  /*! \brief gets a slice with slicing criterion node
    
  This simply does a backwards reachability across all edges to produce the slice.
  */
  virtual std::set<DependenceNode *> getSlice(DependenceNode * node);

private:
  
  //! DEPRECATED not used anymore
  void _addCDG();
  //! DEPRECATED not used anymore
  void _mergeDDG();

  /*! \brief creates summary edges for interprocedural analysis

  Currently, this simply points all the actual-in edges of a call to
  the actual-out return statement. This means we assume two things:
    # All functions are pass-by-value (so actual-out nodes for parameters are irrelevant)
    # Every parameter in the function is relevant (there are no unused arguments).

  @todo Should replace with attribute-grammar method for creating
  summary edges (see Horwitz et al). There is a preliminary start at
  writing the classes required to do this in the subdirectory
  summary/. However it is not complete and possibly has some
  structural issues which would require the entire thing to be
  rewritten.
  */
  void _summarize();

  //! The ControlDependenceGraph for the function
  ControlDependenceGraph * _cdg;
  //! The DataDependenceGraph for the function
  DataDependenceGraph * _ddg;

};

/*! \class SystemDependenceGraph

  This class is a MergedGraph which merges together
  ProgramDependenceGraphs representing all procedures in the
  program. It assumes that the merged ProgramDependenceGraphs have
  defined InterproceduralInfo objects. A slice performed on this graph
  is an interprocedural slice as defined in the paper by Horwitz et al.

  @todo _getPossibleFuncs simply assumes that the AST correctly
  resolves every SgFunctionCallExp to a specific SgFunctionDeclaration
  with an existing SgFunctionDefinition. This should be redone to take
  advantage of call graph analysis, which will let us slice codes
  which have more ambiguous function calls.
*/

class SystemDependenceGraph : public MergedDependenceGraph {

public:

  /*! \brief adds a PDG to our SDG

  Params:
    - ProgramDependenceGraph * pdg: The PDG to add to the SDG

  Side effects: Merges PDG in using _mergeGraph. Maps function PDG
    represents to the PDG itself in _funcs_map.
  */
  void addFunction(ProgramDependenceGraph * pdg);

  /*! \brief links all the functions together

  After the PDGs have been merged into the SDG, each call site is
  linked to the PDG associated with the function that it calls:
    - The callsite node is linked to the entry node with a "call" edge
    - Each actual-in node is linked to the formal-in node with a "call" edge
    - Each formal-out node is linked to the actual-out node with a "return" edge
  */
  void process();

  /*! \brief performs a backwards slice with slicing criterion node

  getSlice is defined according to the paper by Horowitz et al. as a
  two phase operation. The first operation does backwards reachability
  to "mark" nodes while not traversing return edges. Thus it ignores
  functin calls. The second phase does backwards reachability from all
  marked nodes while not traversing call edges. Thus it ignores
  calling functions. The final set of reachable nodes is the
  interprocedural slice.
  */
  virtual std::set<DependenceNode *> getSlice(DependenceNode * node);

  /*! \brief retrieve the PDGs in the graph

  Returns: a set of ProgramDependenceGraph that comprise the SystemDependenceGraph
  */
  std::set<ProgramDependenceGraph *> getPDGs();

private:

  /*! \brief links the call sites in PDG to the PDGs of the function it calls

  Params:
    - ProgramDependenceGraph * pdg: the PDG whose call sites we want to process

  Side effects: The interprocedural nodes for the call sites are
    linked to the interprocedural nodes for the appropriate entry
    sites
  */
  void _processFunction(ProgramDependenceGraph * pdg);


  /*! \brief get a list of functions that funcCall could be referring to

  Params:
    - SgFunctionCallExp * funcCall: the function call we are analyzing

  Return: A list of SgFunctionDeclarations that funcCall could
  potentially call.

  */
  std::list<SgFunctionDeclaration *> _getPossibleFuncs(SgFunctionCallExp * funcCall);

  //! a mapping of funciton declarations to the ProgramDependenceGraph that represents them
  std::map<SgFunctionDeclaration *, ProgramDependenceGraph *> _funcs_map;

};

/*! \class InterproceduralInfo

  This class holds information necessary to perform interprocedural
  slicing. As specified in the paper by Horowitz et al, every function
  in a program requires:
     - An "entry" node
     - A "formal-in" node for each parameter
     - A "formal-out" node for each return parameter

  Similarly, every function call in a procedure requires:
     - A "call-site" node
     - An "actual-in" node for every argument to the function
     - An "actual-out" node for every return variable

  There should be one InterproceduralInfo object associated with each
  procedure in a program. It is initialized by passing in a newly
  created object to the constructor of a ControlDependenceGraph. It
  must then be passed to the constructor of a DataDependenceGraph. At
  this point, it contains all the appropriate intraprocedural edges
  and nodes as specified above.

*/

class InterproceduralInfo {

public:

  /*! \brief extracs all the function calls from an AST subtree which
    may contain function calls

  Params:
    - SgNode * node: The root of the subtree we want to analyze

  Return: a list of all SgFunctionCallExp nodes that are in the subtree
  */
  static std::list<SgFunctionCallExp *> extractFunctionCalls(SgNode * node) {
    std::list<SgFunctionCallExp *> retval;
    std::list<SgNode *> calls = NodeQuery::querySubTree(node, V_SgFunctionCallExp);
    for (std::list<SgNode *>::iterator i = calls.begin(); i != calls.end(); i++) {
      SgFunctionCallExp * fce = isSgFunctionCallExp(*i);
      ROSE_ASSERT(fce != NULL);
      retval.push_back(fce);
    }

    return retval;
  }

  /*! \brief gets the function declaration that the InterproceduralInfo object is for

  Returns: The SgFunctionDeclaration node that is associated with this object
  */
  SgFunctionDeclaration * getFunctionDeclaration() {
    SgFunctionDeclaration * func;
    func = isSgFunctionDeclaration(procedureEntry.entry->getNode());
    ROSE_ASSERT(func != NULL);
    return func;
  }

  //! the nodes required to fully represent a call site in the PDG
  struct CallSiteStructure {
    //! the callsite - one per SgFunctionCallExp
    DependenceNode * callsite;
    //! the actual-in nodes - one per argument
    std::map<SgExpression *, DependenceNode *> actual_in;
    //! the actual-out nodes - one per argument
    std::map<SgExpression *, DependenceNode *> actual_out;
    //! a list which records the order of the function arguments
    std::list<SgExpression *> expr_order;
    //! an actual-out node representing the return value of the function call
    DependenceNode * actual_return;
  };

  //! maps function calls to the call site structure that represents them
  std::map<SgFunctionCallExp *, CallSiteStructure> callsite_map;
  
  //! the nodes required to fully represent a procedure entry in the PDG
  struct ProcedureEntryStructure {
    //! an entry node - one per function declaration
    DependenceNode * entry;
    //! the formal-in nodes - one per function parameter
    std::map<SgInitializedName *, DependenceNode *> formal_in;
    //! the formal-out nodes - one per function parameter
    std::map<SgInitializedName *, DependenceNode *> formal_out;
    //! a list which records the order of the parameters
    std::list<SgInitializedName *> arg_order;
    //! a formal out node representing the return value of the function
    DependenceNode * formal_return;
  };
  
  //! The entry node for a procedure
  ProcedureEntryStructure procedureEntry;
  
};

#endif
