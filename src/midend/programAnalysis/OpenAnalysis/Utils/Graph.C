// $Id: Graph.C,v 1.1 2004/07/07 10:26:34 dquinlan Exp $
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
// Graph is the base class (the "interface") for a general undirected graph (Graph).  Algorithms that operate upon
// abstract undirected graphs should, normally, use only this base Graph class for maximum portability.
//--------------------------------------------------------------------------------------------------------------------

#include "Graph.h"


//--------------------------------------------------------------------------------------------------------------------
/** This routine must be called before a DFS iterator can be used.  It traverses the graph in DFS order and adds, in
    each node, a pointer to the next node in DFS ordering. */
BaseGraph::Node*
Graph::create_DFS_links (BaseGraph::Node* n)
{
  NeighborNodesIterator neigh(dynamic_cast<Node*>(n));
  BaseGraph::Node* last = n;
  while ((bool)neigh) {
    if (neigh->dfs_succ == 0) {
      (dynamic_cast<Node*>(last))->dfs_succ = neigh; // this cast is needed to access private members of last
      last = create_DFS_links(neigh);
    }
    ++neigh;
  }
  (dynamic_cast<Node*>(last))->dfs_succ = 0;         // this cast is needed to access private members of last
  return last;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** This routine must be called before a BFS iterator can be used.  It traverses the graph in BFS order and adds, in
    each node, a pointer to the next node in BFS ordering. */
BaseGraph::Node*
Graph::create_BFS_links (BaseGraph::Node* n)
{
  return n;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** Add the given node to the set of nodes. */
void
Graph::add (Graph::Node* n)
{
  BaseGraph::add(n);
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** Add the edge to the set of edges, as well as to the list of incident edges in both the nodes involved.  Further,
    add the two nodes to each other's neighbor lists. */
void
Graph::add (Graph::Edge* e)
{
  BaseGraph::add(e);
  e->node1()->incident_edges.push_back(e);
  e->node2()->incident_edges.push_back(e);
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** Remove this edge from the BaseGraph as well as from the list of incident edges of the two nodes that form this
    edge. */
void
Graph::remove (Graph::Edge* e)
{
  BaseGraph::remove(e);

  // remove this edge from the list of incident edges of the two nodes involved
  e->node1()->incident_edges.remove(e);
  e->node2()->incident_edges.remove(e);
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** Remove the given node and all the edges incident on it. */
void
Graph::remove (Graph::Node* n)
{
  BaseGraph::remove(n);

  // remove all the edges incident on this node
  IncidentEdgesIterator incident(n);
  while ((bool)incident) {
    Edge* e = incident;
    // remove this edge from the neighboring node's list of incident edges
    if (e->node1() == n)
      e->node2()->incident_edges.remove(e);
    else
      e->node1()->incident_edges.remove(e);
    // remove this edge
    BaseGraph::remove(e);
    delete e; // an edge has no existence without its anchoring nodes
    ++incident;
  }
}
//--------------------------------------------------------------------------------------------------------------------
