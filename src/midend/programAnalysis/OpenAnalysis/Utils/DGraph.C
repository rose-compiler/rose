// $Id: DGraph.C,v 1.1 2004/07/07 10:26:34 dquinlan Exp $
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
// DGraph is the base class (the "interface") for a general directed graph (DGraph).  Algorithms that operate upon
// abstract directed graphs should, normally, use only this base DGraph class for maximum portability.
//--------------------------------------------------------------------------------------------------------------------

#include "DGraph.h"


//--------------------------------------------------------------------------------------------------------------------
/** This routine must be called before a DFS iterator can be used.  It traverses the graph in DFS order and adds, in
    each node, a pointer to the next node in DFS ordering. */
BaseGraph::Node*
DGraph::create_DFS_links (BaseGraph::Node* n)
{
  SinkNodesIterator neigh(dynamic_cast<Node*>(n));
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
DGraph::create_BFS_links (BaseGraph::Node* n)
{
  return n;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** Add the given node to the set of nodes. */
void
DGraph::add (DGraph::Node* n)
{
  BaseGraph::add(n);
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** Add the edge to the set of edges, as well as to the list of incoming edges of its sink node and to the list of
    outoing edges of its source node.  Further, add the two nodes to each other's source and sink lists, as
    appropriate. */
void
DGraph::add (DGraph::Edge* e)
{
  BaseGraph::add(e);
  e->source()->outgoing_edges.push_back(e);
  e->sink()->incoming_edges.push_back(e);
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** Remove this edge from the BaseGraph as well as from the list of incoming or outgoing edges of the two nodes that
    form this edge. */
void
DGraph::remove (DGraph::Edge* e)
{
  BaseGraph::remove(e);

  // remove this edge from the incoming edges of the sink
  e->sink()->incoming_edges.remove(e);

  // remove this edge from the outgoing edges of the source
  e->source()->outgoing_edges.remove(e);
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** Remove the given node and all the incoming and outgoing edges incident on it. */
void
DGraph::remove (DGraph::Node* n)
{
  BaseGraph::remove(n);

  IncomingEdgesIterator incoming(n);
  while ((bool)incoming) {
    Edge* e = incoming;
    // remove this edge from the list of outgoing edges of the source node
    e->source()->outgoing_edges.remove(e);
    // remove this edge
    BaseGraph::remove(e);
    delete e; // an edge has no existence without its anchoring nodes
    ++incoming;
  }

  OutgoingEdgesIterator outgoing(n);
  while ((bool)outgoing) {
    Edge* e = outgoing;
    // remove this edge from the list of outgoing edges of the source node
    e->sink()->incoming_edges.remove(e);
    // remove this edge
    BaseGraph::remove(e);
    delete e; // an edge has no existence without its anchoring nodes
    ++outgoing;
  }
}
//--------------------------------------------------------------------------------------------------------------------
