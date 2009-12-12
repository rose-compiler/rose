
#ifndef TRANSDEP_GRAPH
#define TRANSDEP_GRAPH

#include <DepGraph.h>
#include <TransAnalysis.h>
#include <iostream>

template <class Node> class TransDepAnalImpl;
template <class Node>
class TransDepGraphCreate 
   : public VirtualGraphCreateTemplate<Node,DepInfoSetEdge>, 
     public TransInfoGraph<DepInfoSet>
{
  GraphAccessWrapTemplate<GraphAccessInterface::Node,
                          GraphAccessInterface::Edge, 
                          GraphAccessTemplate<Node,DepInfoEdge> >
     access;
  GraphTransAnalysis<DepInfoSet> anal;
  TransDepAnalImpl<Node> *analop;

  bool TransInfoComputed(const GraphAccessInterface::Node *src, const GraphAccessInterface::Node *snk);
  DepInfoSet GetTransInfo( const GraphAccessInterface::Node *src, const GraphAccessInterface::Node *snk);
  void SetTransInfo( GraphAccessInterface::Node *src, GraphAccessInterface::Node *snk, DepInfoSet info) ;
 public:
  TransDepGraphCreate(GraphAccessTemplate<Node,DepInfoEdge> *g, 
                       int splitlimit=20, 
                       BaseGraphCreate *bc = 0);
  ~TransDepGraphCreate(); 

  DepInfoSetEdge* GetTransDep( Node *n1, Node *n2);
};
  
#endif

