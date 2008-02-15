// $Id: Graph.h,v 1.1 2004/07/07 10:26:34 dquinlan Exp $
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

#ifndef Graph_H
#define Graph_H

// Mint headers
#include "BaseGraph.h"

//--------------------------------------------------------------------------------------------------------------------
// Graph
/** Graph is the base class for a general undirected graph (Graph) that is in turn derived from BaseGraph.  Algorithms
    that operate upon abstract undirected graphs should, normally, use only this base Graph class for maximum
    portability.
    
    No extra restrictions are placed on nodes and edges in addition to those imposed by BaseGraph.  This means that
    self-edges, and multiple edges between two nodes, are allowed.

    An undirected graph, Graph, extends BaseGraph by adding DFS and BFS iterators, as well as iterators to enumerate
    neighboring nodes and incident edges for a node.

    NOTE ON friend CLASSES: Many classes (especially Graph, Graph::Node, and Graph::Edge) have many friend classes.
    This is *not* a kludge.  It is simulating "package" visiblity in Java.  We want a limited public interface to Node
    and Edge and yet give more permissions to methods within the Graph class.  */
//--------------------------------------------------------------------------------------------------------------------
class Graph : public BaseGraph {
public:
  class Edge;
  class Node;
  class IncidentEdgesIterator;
  class NeighborNodesIterator;
  //------------------------------------------------------------------------------------------------------------------
  /** An node in an undirected graph has a list of neighboring nodes and a list of incident edges. */
  class Node : public BaseGraph::Node {
  public:
    Node () {}
    virtual ~Node () { incident_edges.clear(); }
  private:
    std::list<Edge*> incident_edges;
    friend class Graph;
    friend class Graph::Edge;
    friend class Graph::IncidentEdgesIterator;
    friend class Graph::NeighborNodesIterator;
  };
  //------------------------------------------------------------------------------------------------------------------
  class Edge : public BaseGraph::Edge {
  public:
    Edge (Node* n1, Node* n2) : BaseGraph::Edge(n1, n2) {}
    virtual ~Edge () {}
    Node* node1 () { return dynamic_cast<Node*>(n1); }
    Node* node2 () { return dynamic_cast<Node*>(n2); }
    friend class Graph;
    friend class Graph::NeighborNodesIterator;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** Incident edges iterator iterates over all the edges incident on a node. */
  class IncidentEdgesIterator : public Iterator {
  public:
    IncidentEdgesIterator (Node* n) { center = n;  iter = center->incident_edges.begin(); }
    ~IncidentEdgesIterator () {}
    void operator++ () { ++iter; }
    operator bool () { return (iter != center->incident_edges.end()); }
    Edge* operator-> () { return *iter; }
    operator Edge* () { return *iter; }
  private:
    Node*                      center;
    std::list<Edge*>::iterator iter;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** Node iterator iterates over all the neighboring nodes. */
  class NeighborNodesIterator : public Iterator {
  public:
    NeighborNodesIterator (Node* n) {   center = n; iter = center->incident_edges.begin(); }
    ~NeighborNodesIterator () {}
    void operator++ () { ++iter; }
    operator bool () { return (iter != center->incident_edges.end()); }
    Node* operator-> () { Edge* e = *iter; return (e->node1() == center) ? e->node2() : e->node1(); }
    operator Node* () { Edge* e = *iter; return  (e->node1() == center) ? e->node2() : e->node1(); }
  private:
    Node*                      center;
    std::list<Edge*>::iterator iter;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** The DFSIterator here is just an extension of BaseGraph::DFSIterator to allow proper casting. */
  class DFSIterator : public BaseGraph::DFSIterator {
  public:
    DFSIterator (Graph& g) : BaseGraph::DFSIterator(g) {}
    virtual ~DFSIterator () {}
    operator Node* () { return dynamic_cast<Node*>(p); }
    Node* operator -> () { return dynamic_cast<Node*>(p); }
   };
  //------------------------------------------------------------------------------------------------------------------
  /** The BFSIterator here is just an extension of BaseGraph::DFSIterator to allow proper casting. */
  class BFSIterator : public BaseGraph::BFSIterator {
  public:
    BFSIterator (Graph& g) : BaseGraph::BFSIterator(g) {}
    virtual ~BFSIterator () {}
    operator Node* () { return dynamic_cast<Node*>(p); }
    Node* operator -> () { return dynamic_cast<Node*>(p); }
   };
  //------------------------------------------------------------------------------------------------------------------
  /** The NodesIterator is just and extension of BaseGraph::NodesIterator to provide access to Graph nodes. */
  class NodesIterator : public BaseGraph::NodesIterator {
  public:
    NodesIterator (Graph& g) : BaseGraph::NodesIterator(g) {}
    virtual ~NodesIterator () {}
    operator Node* () { return dynamic_cast<Node*>(*iter); }
    Node* operator -> () { return dynamic_cast<Node*>(*iter); }
  };
  //------------------------------------------------------------------------------------------------------------------
  /** The BiDirNodesIterator is just an extension of BaseGraph::BiDirNodesIterator to provide access to Graph nodes. */
  class BiDirNodesIterator : public BaseGraph::BiDirNodesIterator {
  public:
    BiDirNodesIterator (Graph& g) : BaseGraph::BiDirNodesIterator(g) {}
    BiDirNodesIterator (Graph& g, dirType d) : BaseGraph::BiDirNodesIterator(g, d) {}
    virtual ~BiDirNodesIterator () {}
    operator Node* () { return dynamic_cast<Node*>(*iter); }
    Node* operator -> () { return dynamic_cast<Node*>(*iter); }
  };
  //------------------------------------------------------------------------------------------------------------------
  /** The EdgesIterator is just and extension of BaseGraph::EdgesIterator to provide access to Graph edges. */
  class EdgesIterator : public BaseGraph::EdgesIterator {
  public:
    EdgesIterator (Graph& g) : BaseGraph::EdgesIterator(g) {}
    virtual ~EdgesIterator () {}
    operator Edge* () { return dynamic_cast<Edge*>(*iter); }
    Edge* operator -> () { return dynamic_cast<Edge*>(*iter); }
  };
  //------------------------------------------------------------------------------------------------------------------
  Graph () : BaseGraph () {}
  Graph (Node* root) : BaseGraph (root) {}
  virtual ~Graph () {}
  virtual void add (Graph::Edge* e);
  virtual void add (Graph::Node* n);
  virtual void remove (Graph::Edge* e);
  virtual void remove (Graph::Node* n);

private:
  BaseGraph::Node* create_DFS_links (BaseGraph::Node* start_node);
  BaseGraph::Node* create_BFS_links (BaseGraph::Node* start_node);
};
//--------------------------------------------------------------------------------------------------------------------

#endif
