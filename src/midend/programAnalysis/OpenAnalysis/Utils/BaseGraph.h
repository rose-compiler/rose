// $Id: BaseGraph.h,v 1.3 2008/01/08 02:56:40 dquinlan Exp $
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

#ifndef BaseGraph_H
#define BaseGraph_H

// standard headers
#include <iostream>
using ::std::ostream;

// STL headers
#include <list>
#include <set>

// Mint headers
#include "Iterator.h"
#include "Exception.h"

namespace std {
  using namespace ::std;
}
//--------------------------------------------------------------------------------------------------------------------
// BaseGraph
/** BaseGraph is the abstract base class (the "interface") for a general graph.  It defines graph properties common to
    directed and undirected graphs.  It leaves out some loose ends in the interface:
    1. A node has no notion of incident edges or neighboring nodes because the number of kinds of incident edges or
       neighboring nodes is dependent upon the graph being directed or undirected.
    2. For the same reason, the method delete(node) cannot delete any incident edges.  Therefore the directed or
       undirected graph *must* override it with a more complete version.  Similarly, the method delete(edge) cannot
       delete the edge from the list(s) of the nodes involved.
    3. In an analogous manner, the method add(edge) must be overridden with a more complete version that adds the edge
       into the records of the nodes involved, if needed.

    The only restriction on nodes and edges is that they should be unique objects, meaning, an edge or a node cannot
    be shared between two graphs.  A node or edge can also not be inserted twice.  Nodes and edges are identified by
    their pointer values (BaseGraph::Node* and BaseGraph::Edge*).

    Following exceptions are thrown by the class (all are subclasses of Exception):
    1. BaseGraph::EmptyEdge                       -- attempt to add, or remove, an empty edge (null pointer)
    2. BaseGraph::DuplicateEdge                   -- attempt to add an edge more than once
    3. BaseGraph::NonexistentEdge                 -- attempt to remove an edge that does not belong to the graph
    4. BaseGraph::EdgeInUse                       -- attempt to add an edge that is already a part of another graph
    5. BaseGraph::EmptyNode                       -- attempt to add, or remove, an empty node (null pointer)
    6. BaseGraph::DuplicateNode                   -- attempt to add a node more than once
    7. BaseGraph::NonexistentNode                 -- attempt to remove a node that does not belong to the graph
    8. BaseGraph::NodeInUse                       -- attempt to add a node that is already a part of another graph
    9. BaseGraph::DeletingRootOfNonSingletonGraph -- attempt to delete the root node when graph has more nodes & edges
    
    NOTE ON friend CLASSES: Many classes (especially BaseGraph, BaseGraph::Node and BaseGraph::Edge) have many friend
    classes.  This is *not* a kludge.  It is simulating "package" visiblity in Java.  We want a limited public
    interface to Node and Edge and yet give more permissions to methods within the BaseGraph class.  */
//--------------------------------------------------------------------------------------------------------------------
class BaseGraph {
public:
  class Node;
  class Edge;
  class DFSIterator;
  class BFSIterator;
  class NodesIterator;
  class BiDirNodesIterator;
  class EdgesIterator;
  friend class DFSIterator;
  friend class BFSIterator;
  friend class NodesIterator;
  friend class BiDirNodesIterator;
  friend class EdgesIterator;
  //------------------------------------------------------------------------------------------------------------------
  /** EmptyEdge exception is thrown if an edge being added is null (0) */
  class EmptyEdge : public Exception {
  public:
    EmptyEdge () {}
    ~EmptyEdge () {}
    void report (std::ostream& o) const { o << "E!  Adding a null edge to a graph." << std::endl; }
  };
  //------------------------------------------------------------------------------------------------------------------
  /** DuplicateEdge exception is thrown if an edge being added is already a part of the graph. */
  class DuplicateEdge : public Exception {
  public:
    DuplicateEdge (BaseGraph::Edge* e) { offending_edge = e; }
    ~DuplicateEdge () {}
    void report (std::ostream& o) const;
  private:
    BaseGraph::Edge* offending_edge;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** NonexistentEdge exception is thrown if an edge being deleted is not a part of the graph. */
  class NonexistentEdge : public Exception {
  public:
    NonexistentEdge (BaseGraph::Edge* e) { offending_edge = e; }
    ~NonexistentEdge () {}
    void report (std::ostream& o) const { o << "E!  Removing a non-existent edge from a graph." << std::endl; }
  private:
    BaseGraph::Edge* offending_edge;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** EdgeInUse exception is thrown if an edge being added is already a part of another graph. */
  class EdgeInUse : public Exception {
  public:
    EdgeInUse (BaseGraph::Edge* e) { offending_edge = e; }
    ~EdgeInUse () {}
    void report (std::ostream& o) const { o << "E!  Adding an edge that is already a part of another graph." << std::endl; }
  private:
    BaseGraph::Edge* offending_edge;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** EmptyNode exception is thrown if a node being added is null (0) */
  class EmptyNode : public Exception {
  public:
    EmptyNode () {}
    ~EmptyNode () {}
    void report (std::ostream& o) const { o << "E!  Adding a null node to a graph." << std::endl; }
  };
  //------------------------------------------------------------------------------------------------------------------
  /** DuplicateNode exception is thrown if a node being added is already a part of the graph. */
  class DuplicateNode : public Exception {
  public:
    DuplicateNode (BaseGraph::Node* n) { offending_node = n; }
    ~DuplicateNode () {}
    void report (std::ostream& o) const { o << "E!  Adding a duplicate node to a graph." << std::endl; }
  private:
    BaseGraph::Node* offending_node;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** NonexistentNode exception is thrown if a node being deleted is not a part of the graph. */
  class NonexistentNode : public Exception {
  public:
    NonexistentNode (BaseGraph::Node* n) { offending_node = n; }
    ~NonexistentNode () {}
    void report (std::ostream& o) const { o << "E!  Removing a non-existent node from a graph." << std::endl; }
  private:
    BaseGraph::Node* offending_node;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** NodeInUse exception is thrown if a node being added is already a part of another graph. */
  class NodeInUse : public Exception {
  public:
    NodeInUse (BaseGraph::Node* n) { offending_node = n; }
    ~NodeInUse () {}
    void report (std::ostream& o) const { o << "E!  Addiing a node that is already a part of another graph." << std::endl; }
  private:
    BaseGraph::Node* offending_node;
  };
  //------------------------------------------------------------------------------------------------------------------
  class DeletingRootOfNonSingletonGraph : public Exception {
  public:
    DeletingRootOfNonSingletonGraph (BaseGraph::Node* n) { offending_node = n; }
    ~DeletingRootOfNonSingletonGraph () {}
    void report (std::ostream& o) const { o << "E!  Deleting the root node of a non-singleton graph." << std::endl; }
  private:
    BaseGraph::Node* offending_node;
  };
  //------------------------------------------------------------------------------------------------------------------
  class Node {
  public:
    Node () { dfs_succ = bfs_succ = 0; in_use = false; }
    virtual ~Node () { dfs_succ = NULL; bfs_succ = NULL; }
    virtual void dump (std::ostream& os) { os << this; }
  protected:
    bool in_use;
    Node* dfs_succ;
    Node* bfs_succ;
    friend class BaseGraph;
    friend class BaseGraph::DFSIterator;
    friend class BaseGraph::BFSIterator;
  };
  //------------------------------------------------------------------------------------------------------------------
  class Edge {
  public:
    Edge (Node* _n1, Node* _n2) { n1 = _n1; n2 = _n2; in_use = false; }
    virtual ~Edge () { n1 = NULL; n2 = NULL; }
    virtual void dump (std::ostream& os) { os << this; }
  protected:
    bool in_use;
    Node* n1;
    Node* n2;
    friend class BaseGraph;
    friend class BaseGraph::DFSIterator;
    friend class BaseGraph::BFSIterator;
    friend class BaseGraph::DuplicateEdge;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** The DFSiterator calls the virtual function create_DFS_links the first time it is called, or if the graph has
      been changed since the last call.  */
  class DFSIterator : public Iterator {
  public:
    DFSIterator (BaseGraph& g);
    ~DFSIterator () {}
    void operator++ () { if (p != 0) p = p->dfs_succ; std::cerr << "advance " << p << std::endl; }
    operator bool () { return (p != 0); }
  protected:
    Node* p;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** The BFSiterator calls the virtual function create_BFS_links the first time it is called, or if the graph has
      been changed since the last call. */
  class BFSIterator : public Iterator {
  public:
    BFSIterator (BaseGraph& g);
    ~BFSIterator () {}
    void operator++ () { if (p != 0) p = p->bfs_succ; }
    operator bool () { return (p != 0); }
  protected:
    Node* p;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** The node iterator iterates over all the nodes in the graph in no particular order. */
  class NodesIterator : public Iterator {
  public:
    NodesIterator (BaseGraph& g) { gr = &g;  iter = gr->node_set.begin(); }
    ~NodesIterator () {}
    void operator++ () { ++iter; }
    operator bool () { return (iter != gr->node_set.end()); }
  protected:
    std::set<Node*>::iterator iter;
    BaseGraph* gr;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** The bi-directional node iterator iterates over all the nodes in the graph in no particular order -- except
      that the Forward direction is guaranteed to be opposite of the Reverse direction.  */
  class BiDirNodesIterator : public Iterator {
  public:
    enum dirType { Forward, Reverse }; 
    // Default to forward direction if client doesn't specify the direction.
    BiDirNodesIterator (BaseGraph& g) { dir = Forward; iter = gr->node_set.begin(); } 
    BiDirNodesIterator (BaseGraph& g, dirType d) {
      gr = &g; dir = d;
      if (dir == Forward)
        iter = gr->node_set.begin();
      else {
        iter = gr->node_set.end();
        iter--;
        pre_begin = gr->node_set.begin();
        --pre_begin;
      }
    }
    ~BiDirNodesIterator () {}
    void operator++ () { ++iter; }
    void operator-- () { --iter; }
    operator bool () {
      if (dir == Forward)
        return iter != gr->node_set.end();
      else {
        return iter != pre_begin;
      }
    }
  protected:
    std::set<Node*>::iterator iter;
    std::set<Node*>::iterator pre_begin;
    BaseGraph* gr;
    dirType dir;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** The edge iterator iterates over all the edges in the graph in no particular order. */
  class EdgesIterator : public Iterator {
  public:
    EdgesIterator (BaseGraph& g) { gr = &g;  iter = gr->edge_set.begin(); }
    ~EdgesIterator () {}
    void operator++ () { ++iter; }
    operator bool () { return (iter != gr->edge_set.end()); }
  protected:
    std::set<Edge*>::iterator iter;
    BaseGraph* gr;
  };
  //------------------------------------------------------------------------------------------------------------------
  BaseGraph () { root_node = 0; }
  BaseGraph (Node* root) { root_node = 0; add(root); }
  virtual ~BaseGraph (); 
  
  Node* root () { return root_node; }
  void  set_root (Node* n) { root_node = n; }

  int num_nodes () { return node_set.size(); }
  int num_edges () { return edge_set.size(); }

  bool isempty () { return (root_node == 0); }

  void dump (std::ostream&);

protected:
  std::set<Node*> node_set;                                  // the set of all the graph nodes
  std::set<Edge*> edge_set;                                  // the set of all the graph edges
  Node* root_node;                                           // the root node
  bool DFS_needed, BFS_needed;                               // has a DFS / BFS been done on this graph?
#if __cplusplus < 201103L
  void add (Edge* e) throw (DuplicateEdge, EdgeInUse, EmptyEdge,
                            DuplicateNode, NodeInUse, EmptyNode);
  void add (Node* n) throw (DuplicateNode, NodeInUse, EmptyNode);
  void remove (Edge* e) throw (NonexistentEdge, EmptyEdge);
  void remove (Node* n) throw (NonexistentNode, DeletingRootOfNonSingletonGraph, EmptyNode);
#else
  void add (Edge* e);
  void add (Node* n);
  void remove (Edge* e);
  void remove (Node* n);
#endif
  virtual Node* create_DFS_links (Node* start_node) = 0;
  virtual Node* create_BFS_links (Node* start_node) = 0;
};
//--------------------------------------------------------------------------------------------------------------------

#endif
