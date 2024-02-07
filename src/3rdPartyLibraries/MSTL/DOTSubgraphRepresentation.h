/******************************************************************************
 *
 * DOT Graph output, similar to DOTGraphRepresentation
 * also supports subgraph clustering - inc constrast to above it
 * does buffer the graph information until writeDotoutStream is 
 * called
 *
 *****************************************************************************/
// Author: Markus Schordan, Nils Thuerey
// $Id: DOTSubgraphRepresentation.h,v 1.5 2006/04/24 00:21:27 dquinlan Exp $

#ifndef DOTSUBGRAPHREPRESENTATION_H
#define DOTSUBGRAPHREPRESENTATION_H

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include "DOTRepresentation.h"

// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
// using namespace std;

//! dot file representation with subgraphs
template<class NodeType>
class DOTSubgraphRepresentation : public DOTRepresentation<NodeType> {

  // pmp 09JUN05
  //   declare the use of doutout, so that gcc 3.4 can handle it
  //   since it is used as non template dependent.
  using DOTRepresentation<NodeType>::dotout;

public:
  typedef unsigned int TraceType;
  typedef unsigned int ContainerIndexType;

// DQ (8/16/2004): Added to refactor code in support of graph hierarcies (nested subgraphs)
  void resetWriteFlags( void );
  void writeOutSubgraph( typename std::map<int,std::string>::iterator i );

  void writeToFileAsGraph(std::string filename);
  void writeToFile(std::string filename);

  DOTSubgraphRepresentation();
  ~DOTSubgraphRepresentation();
    
  void addNode(NodeType node, std::string nodelabel, std::string option="");
  void addNode(NodeType node, std::string nodelabel, std::string option, int subgraph);

  void addEdge(NodeType node1, TraceType downtrace, TraceType uptrace, std::string edgelabel, NodeType node2, std::string option="");
  void addEdge(NodeType node1, TraceType downtrace, std::string edgelabel, NodeType node2, std::string option="");
  void addEdge(NodeType node1, std::string edgelabel, NodeType node2, std::string option="");

  void addNullValue(NodeType node, TraceType trace, std::string varname, std::string option="");
  void addNullValue(NodeType node, std::string nodelabel, std::string edgelabel, std::string option="");
  void addEmptyContainer(NodeType node, TraceType trace, std::string varname, std::string = "");

  std::string containerEdgeLabel(std::string containerVarName, ContainerIndexType count);

	//! add new subgraph, returns id of the subgraph
	int addSubgraph(std::string name);

	//! add new subgraph with manual id assignment
   // DQ (8/14/2004): Removed return value! fixes warning and implements better code
   // not clear if the semantics really should be to return a value.
   // int addSubgraph(int id, string name)
   void addSubgraph(int id, std::string name);

// DQ (2/8/2005): Moved this to the source file instead of placing it in the header 
//                file to try to fix linker error with installed libs.
#if 0
   void addSubgraph(int id, std::string name)
      {
        assert(!mAutoSubgraphIds);
        mManualSubgraphIds = true;
        mSubgraphNames[id] = name;

      };
#endif

	typedef struct{
		NodeType node1;
		NodeType node2;
		std::string label;
		std::string option;
		bool written; // internal flag for writing to dotout
	} EdgeStorageType;

	typedef struct{
		NodeType node;
		std::string label;
		std::string option;
		int subgraph;
		bool written; // internal flag for writing to dotout
	} NodeStorageType;

	typedef struct{
		NodeType node;
		std::string add;
		bool written; // internal flag for writing to dotout
	} NodeAdditionalStorage;

 protected:
 private:

	//! write stored nodes and edges to dotout stream
	void writeDotoutStream( void );

	//! store nodes for later addition
	std::vector<NodeStorageType> mNodes;

	//! store edges for later addition
	std::vector<EdgeStorageType> mEdges;

	//! store nodes for later addition
	std::vector<NodeAdditionalStorage> mAdds;

	//! store names and ID's of subgraphs
	std::map<int,std::string> mSubgraphNames;

   //! Child/Parent hierarchy
   std::map<std::string,std::string> childParentRelationship;

	//! last id of a subgraph
	int mSubgraphIds;

	//! use the automatically assigned subgraph id's ?, should not be mixed with manual assignments
	bool mAutoSubgraphIds;

	//! use the manual assigned of subgraph id's ?, should not be mixed with auto assignments
	bool mManualSubgraphIds;
};

#endif






