// $Id: BaseGraph.C,v 1.1 2004/07/07 10:26:34 dquinlan Exp $
// -*-C++-*-
// * BeginRiceCopyright *****************************************************
// 
// Copyright ((c)) 2002, Rice University 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// 
// * Neither the name of Rice University (RICE) nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
// 
// This software is provided by RICE and contributors "as is" and any
// express or implied warranties, including, but not limited to, the
// implied warranties of merchantability and fitness for a particular
// purpose are disclaimed. In no event shall RICE or contributors be
// liable for any direct, indirect, incidental, special, exemplary, or
// consequential damages (including, but not limited to, procurement of
// substitute goods or services; loss of use, data, or profits; or
// business interruption) however caused and on any theory of liability,
// whether in contract, strict liability, or tort (including negligence
// or otherwise) arising in any way out of the use of this software, even
// if advised of the possibility of such damage. 
// 
// ******************************************************* EndRiceCopyright *

// Best seen in 120-column wide window (or print in landscape mode).
//--------------------------------------------------------------------------------------------------------------------
// This file is part of Mint.
// Arun Chauhan (achauhan@cs.rice.edu), Dept of Computer Science, Rice University, 2001.
//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
// BaseGraph is the abstract base class (the "interface") for a general graph.  It defines graph properties common to
// directed and undirected graphs.
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
// standard headers
#include <iostream>
using std::ostream;
using std::endl;

// Mint headers
#include "BaseGraph.h"
//--------------------------------------------------------------------------------------------------------------------

BaseGraph::~BaseGraph ()
{
  std::set<Node*>::iterator node_it;    
  for (node_it = node_set.begin(); node_it != node_set.end(); ++node_it) {
    delete *node_it;
  }
  node_set.clear();
  
  std::set<Edge*>::iterator edge_it;
  for (edge_it = edge_set.begin(); edge_it != edge_set.end(); ++edge_it) {
    delete *edge_it;
  }
  edge_set.clear();
  
  root_node = NULL;
}

//--------------------------------------------------------------------------------------------------------------------
// Exceptions
void
BaseGraph::DuplicateEdge::report (ostream& o) const
{
  o << "E!  Attempting to add an already existing edge ";
  offending_edge->dump(o);
  o << " (";
  offending_edge->n1->dump(o);
  o << ", ";
  offending_edge->n2->dump(o);
  o << ") to the graph." << endl;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** DFSIterator creates an iterator to enumerate nodes in the DFS order starting at the root.  If this is the first
    time a DFS iterator has been demanded for this graph, or the graph has changed since the last DFS traversal, a DFS
    is actually carried out to fill in the DFS pointers.  Otherwise, the nodes are simply traversed using the existing
    DFS pointers. */
BaseGraph::DFSIterator::DFSIterator (BaseGraph& g)
{
  if (g.root_node != 0) {
    if (g.DFS_needed) {
      // reset all the DFS succ links and call the recursive routine to re-create them
      std::set<Node*>::iterator ni = g.node_set.begin();
      while (ni != g.node_set.end()) {
	Node* n = *ni;
	n->dfs_succ = 0;
	++ni;
      }
      g.create_DFS_links(g.root_node);
      g.DFS_needed = false;
    }
  }
  p = g.root_node;
  cerr << "BaseGraph DFSIterator " << p << endl;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** DBFSIterator creates a node_iterator to enumerate nodes in the BFS order starting at the root.  If this is the
    first time a BFS iterator has been demanded for this graph, or the graph has changed since the last BFS traversal,
    a BFS is actually carried out to fill in the BFS pointers.  Otherwise, the nodes are simply traversed using the
    existing BFS pointers. */
BaseGraph::BFSIterator::BFSIterator (BaseGraph& g)
{
  if (g.root_node != 0) {
    if (!g.BFS_needed) {
      // reset all the BFS succ links and call the recursive routine to re-create them
      std::set<Node*>::iterator ni = g.node_set.begin();
      while (ni != g.node_set.end()) {
	Node* n = *ni;
	n->bfs_succ = 0;
	++ni;
      }
      g.create_BFS_links(g.root_node);
      g.BFS_needed = false;
    }
  }
  p = g.root_node;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** Add an edge, checking for duplication and making sure that the edge does not belong to another graph. */
void
BaseGraph::add (BaseGraph::Edge* e)
  throw (BaseGraph::DuplicateEdge, BaseGraph::EdgeInUse, BaseGraph::EmptyEdge,
         /* indirect: from BaseGraph::add(BaseGraph::Node) */
	 BaseGraph::DuplicateNode, BaseGraph::NodeInUse, BaseGraph::EmptyNode)

{
  if (e == 0)
    throw EmptyEdge();
  if (edge_set.find(e) != edge_set.end())
    throw DuplicateEdge(e);
  if (e->in_use)
    throw EdgeInUse(e);

  // insert the nodes if they don't already exist in the graph
  if (node_set.find(e->n1) == node_set.end())
    add(e->n1);
  if (node_set.find(e->n2) == node_set.end())
    add(e->n2);
  e->in_use = true;
  edge_set.insert(e);
  DFS_needed = BFS_needed = true;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** Add a node checking for duplication and making sure the node does not belong to another graph. */
void
BaseGraph::add (BaseGraph::Node* n)
  throw (BaseGraph::DuplicateNode, BaseGraph::NodeInUse, BaseGraph::EmptyNode)
{
  if (n == 0)
    throw EmptyNode();
  if (node_set.find(n) != node_set.end())
    throw DuplicateNode(n);
  if (n->in_use)
    throw NodeInUse(n);
  if (root_node == 0)
    root_node = n;
  n->in_use = true;
  node_set.insert(n);
  DFS_needed = BFS_needed = true;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** Remove an edge, checking for non-existence. */
void
BaseGraph::remove (BaseGraph::Edge* e)
  throw (BaseGraph::NonexistentEdge, BaseGraph::EmptyEdge)
{
  if (e == 0)
    throw EmptyEdge();
  if (edge_set.erase(e) == 0)
    throw NonexistentEdge(e); 
  e->in_use = false;
  DFS_needed = BFS_needed = true;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** Remove a node, checking for non-existence.  Throw DeletingRootOfNonEmptyGraph if attempted to delete the root node
    while the graph still has other nodes and edges. */
void
BaseGraph::remove (BaseGraph::Node* n)
  throw (BaseGraph::NonexistentNode, BaseGraph::DeletingRootOfNonSingletonGraph, BaseGraph::EmptyNode)
{
  if (n == 0)
    throw EmptyNode();
  if ((n == root_node) && (!edge_set.empty() || (node_set.size() > 1)))
    throw DeletingRootOfNonSingletonGraph(n);
  if (node_set.erase(n) == 0)
    throw NonexistentNode(n);
  n->in_use = false;
  DFS_needed = BFS_needed = true;
  if (n == root_node)
    root_node = 0;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
void
BaseGraph::dump (ostream& os)
{
  os << "List of Nodes:" << endl;
  std::set<Node*>::iterator nodes_iter = node_set.begin();
  while (nodes_iter != node_set.end()) {
    Node* n = *nodes_iter;
    os << "\t";
    n->dump(os);
    if (*nodes_iter == root_node)
      os << " (root)";
    os << endl;
    ++nodes_iter;
  }

  os << "List of Edges:" << endl;
  std::set<Edge*>::iterator edges_iter = edge_set.begin();
  while (edges_iter != edge_set.end()) {
    Edge* e = *edges_iter;
    os << "\t";
    e->dump(os);
    os << " (";
    e->n1->dump(os);
    os << " -- "; 
    e->n2->dump(os);
    os << ")" << endl;
    ++edges_iter;
  }
}
//--------------------------------------------------------------------------------------------------------------------
