// $Id: Tree.h,v 1.1 2004/07/07 10:26:35 dquinlan Exp $
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

#ifndef Tree_H
#define Tree_H

// STL headers
#include <deque>
#include <set>

// Mint headers
#include "Iterator.h"
#include "Exception.h"


//--------------------------------------------------------------------------------------------------------------------
/** Tree is the base class of a general tree where each node can have a variable number of child nodes.  All
    algorithms that operate on abstract trees should use this base class for maximum portability.

    Nodes and edges must be unique, meaning, a node or object cannot be added to the tree more than once.  The root
    node cannot be deleted if there are other nodes or edges remaining in the tree.  Finally, no node can have more
    than one incoming (parent) edge.  Edges and nodes are identified by their pointer values (Tree::Edge* and
    Tree::Node*).  An node is allowed to point to an empty edge since this comes handy in many situations where the
    order of outgoing edges matters.  Such an edge must be inserted using add_empty_edge method.

    Following exceptions are thrown by the class (all are subclasses of Exception):

    1.  Tree::EmptyEdge                      -- attempt to add, or remove, an empty edge (null pointer)

    2.  Tree::DuplicateEdge                  -- attempt to add an edge more than once

    3.  Tree::NonexistentEdge                -- attempt to remove an edge that does not belong to the tree

    4.  Tree::EdgeInUse                      -- attempt to add an edge that is already a part of another tree

    5.  Tree::SecondParent                   -- attempt to add a second incoming edge to a node

    6.  Tree::EmptyNode                      -- attempt to add, or remove, an empty node (null pointer)

    7.  Tree::DuplicateNode                  -- attempt to add a node more than once

    8.  Tree::NonexistentNode                -- attempt to remove a node that does not belong to the tree

    9.  Tree::NodeInUse                      -- attempt to add a node that is already a part of another tree

    10. Tree::DeletingRootOfNonSingletonTree -- attempt to delete the root node when tree has more nodes & edges

    NOTE ON friend CLASSES: Many classes (especially Tree::Node and Tree::Edge) have many friend classes.  This is
    *not* a kludge.  It is simulating "package" visiblity in Java.  We want a limited public interface to Node and
    Edge and yet give more permissions to methods within the Graph class.  */
//--------------------------------------------------------------------------------------------------------------------
class Tree {
 public:
  class Node;
  class Edge;
  class NodesIterator;
  class EdgesIterator;
  class PreOrderIterator;
  class PostOrderIterator;
  class ReversePostOrderIterator;
  class OutEdgesIterator;
  class ChildNodesIterator;
  friend class NodesIterator;
  friend class EdgesIterator;
  friend class PreOrderIterator;
  friend class PostOrderIterator;
  friend class ReversePostOrderIterator;
  //------------------------------------------------------------------------------------------------------------------
  /** EmptyEdge exception is thrown if an edge being added is null (0) */
  class EmptyEdge : public Exception {
  public:
    EmptyEdge () {}
    ~EmptyEdge () {}
    void report (std::ostream& o) const { o << "E!  Adding a null edge to a tree." << std::endl; }
  };
  //------------------------------------------------------------------------------------------------------------------
  /** DuplicateEdge exception is thrown if an edge being added is already a part of the tree. */
  class DuplicateEdge : public Exception {
  public:
    DuplicateEdge (Tree::Edge* e) { offending_edge = e; }
    ~DuplicateEdge () {}
    void report (std::ostream& o) const { o << "E!  Adding a duplicate edge to a tree." << std::endl; }
  private:
    Tree::Edge* offending_edge;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** NonexistentEdge exception is thrown if an edge being deleted is not a part of the tree. */
  class NonexistentEdge : public Exception {
  public:
    NonexistentEdge (Tree::Edge* e) { offending_edge = e; }
    ~NonexistentEdge () {}
    void report (std::ostream& o) const { o << "E!  Removing a non-existent edge from a tree." << std::endl; }
  private:
    Tree::Edge* offending_edge;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** EdgeInUse exception is thrown if an edge being added is already a part of another tree. */
  class EdgeInUse : public Exception {
  public:
    EdgeInUse (Tree::Edge* e) { offending_edge = e; }
    ~EdgeInUse () {}
    void report (std::ostream& o) const { o << "E!  Adding an edge that is already a part of another tree." << std::endl; }
  private:
    Tree::Edge* offending_edge;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** SecondParent exception is thrown if the sink (child) of the edge being added is already present in the tree,
      meaning that it already has a parent. */
  class SecondParent : public Exception {
  public:
    SecondParent (Tree::Edge* e) { offending_edge = e; }
    ~SecondParent () {}
    void report (std::ostream& o) const { o << "E!  Adding an edge that points to an existing node in the tree." << std::endl; }
  private:
    Tree::Edge* offending_edge;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** EmptyEndPoint exception is thrown if the one, or both, of the end points of an edge being added is null. */
  class EmptyEndPoint : public Exception {
  public:
    EmptyEndPoint (Tree::Edge* e) { offending_edge = e; }
    ~EmptyEndPoint () {}
    void report (std::ostream& o) const { o << "E!  Adding an edge that points to an empty node." << std::endl; }
  private:
    Tree::Edge* offending_edge;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** EmptyNode exception is thrown if a node being added is null (0) */
  class EmptyNode : public Exception {
  public:
    EmptyNode () {}
    ~EmptyNode () {}
    void report (std::ostream& o) const { o << "E!  Adding a null node to a tree." << std::endl; }
  };
  //------------------------------------------------------------------------------------------------------------------
  /** DuplicateNode exception is thrown if a node being added is already a part of the tree. */
  class DuplicateNode : public Exception {
  public:
    DuplicateNode (Tree::Node* n) { offending_node = n; }
    ~DuplicateNode () {}
    void report (std::ostream& o) const { o << "E!  Adding a duplicate node to a tree." << std::endl; }
  private:
    Tree::Node* offending_node;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** NonexistentNode exception is thrown if a node being deleted is not a part of the tree. */
  class NonexistentNode : public Exception {
  public:
    NonexistentNode (Tree::Node* n) { offending_node = n; }
    ~NonexistentNode () {}
    void report (std::ostream& o) const { o << "E!  Removing a non-existent node from the tree." << std::endl; }
  private:
    Tree::Node* offending_node;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** NodeInUse exception is thrown if a node being added is already a part of another tree. */
  class NodeInUse : public Exception {
  public:
    NodeInUse (Tree::Node* n) { offending_node = n; }
    ~NodeInUse () {}
    void report (std::ostream& o) const { o << "E!  Adding a node that is already a part of another tree." << std::endl; }
  private:
    Tree::Node* offending_node;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** DeletingRootOfNonSingletonTree is thrown when an attempt is made to remove the root of a tree that has other
      nodes and edges. */
  class DeletingRootOfNonSingletonTree : public Exception {
  public:
    DeletingRootOfNonSingletonTree (Tree::Node* n) { offending_node = n; }
    ~DeletingRootOfNonSingletonTree () {}
    void report (std::ostream& o) const { o << "E!  Deleting the root node of a non-singleton tree." << std::endl; }
  private:
    Tree::Node* offending_node;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** A tree node has a pointer for the incoming (parent) node, a list of outgoing (children) nodes and links for
      different traversal orders. */
  class Node {
  public:
    Node () { in_use = false; next_preorder = next_postorder = prev_postorder = 0;  incoming = 0; }
    virtual Node* new_copy () { Node* n = new Node(); copy(n); return n; }
    virtual ~Node () {}
    Node* parent () const { if (incoming != 0) return incoming->source(); else return 0; }
    Node* child (int i) const { if (outgoing[i] != 0) return outgoing[i]->child(); else return 0; }
    Edge* in_edge () const { return incoming; }
    Edge* out_edge (int i) const { return outgoing[i]; }
    int num_children () const { return outgoing.size(); }
    virtual void dump (std::ostream& os) { os << this; }
  protected:
    void copy (Node* n) {}
    Edge* incoming;
    std::deque<Edge*> outgoing;
    Node* next_preorder;
    Node* next_postorder;
    Node* prev_postorder;
    bool in_use;
    friend class Tree;
    friend class Tree::PreOrderIterator;
    friend class Tree::PostOrderIterator;
    friend class Tree::ReversePostOrderIterator;
    friend class Tree::OutEdgesIterator;
    friend class Tree::ChildNodesIterator;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** A tree edge has pointers to the nodes it links (parent and child). */
  class Edge {
  public:
    Edge (Node* _source, Node* _sink) { in_use = false; parent_node = _source; child_node = _sink; }
    virtual Edge* new_copy (Node* _src, Node* _snk) { Edge* e = new Edge(_src, _snk); copy(e); return e; }
    virtual ~Edge () {}
    Node* parent () const { return parent_node; }
    Node* source () const { return parent(); }
    Node* tail () const { return parent(); }
    Node* child () const { return child_node; }
    Node* sink () const { return child(); }
    Node* head () const { return child(); }
    virtual void dump (std::ostream& os) { os << this; }
  protected:
    void copy (Edge* e) {}
    Node* parent_node;
    Node* child_node;
    bool in_use;
    friend class Tree;
    friend class Tree::Node;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** The node iterator iterates over all the nodes in the tree in no particular order. */
  class NodesIterator : public Iterator {
  public:
    NodesIterator (Tree& t) { tr = &t;  iter = tr->node_set.begin(); }
    ~NodesIterator () {}
    void operator++ () { ++iter; }
    operator bool () { return (iter != tr->node_set.end()); }
    operator Node* () { return *iter; }
    Node* operator -> () { return *iter; }
  protected:
    std::set<Node*>::iterator iter;
    Tree* tr;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** The edge iterator iterates over all the edges in the tree in no particular order. */
  class EdgesIterator : public Iterator {
  public:
    EdgesIterator (Tree& t) { tr = &t;  iter = tr->edge_set.begin(); }
    ~EdgesIterator () {}
    void operator++ () { ++iter; }
    operator bool () { return (iter != tr->edge_set.end()); }
  protected:
    std::set<Edge*>::iterator iter;
    Tree* tr;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** OutEdgesIterator iterates over all the non-null outgoing nodes of a node. */
  class OutEdgesIterator : public Iterator {
  public:
    OutEdgesIterator (Node* n);
    virtual ~OutEdgesIterator () {}
    void operator++ ();
    operator bool () { return (iter != center->outgoing.end()); }
    operator Edge* () { return *iter; }
  private:
    Node* center;
    std::deque<Edge*>::iterator iter;
  };
  //------------------------------------------------------------------------------------------------------------------
  /** ChildNodesIterator iterates over all the child nodes of a node.  Thus, it skips the null edges. */
  class ChildNodesIterator : private OutEdgesIterator {
  public:
    ChildNodesIterator (Node* n) : OutEdgesIterator(n) {}
    ~ChildNodesIterator () {}
    void operator++ () { OutEdgesIterator::operator++(); }
    operator bool () { return OutEdgesIterator::operator bool(); }
    operator Node* () { Edge* e = (Edge*)(*this);  return e->sink(); }
  };
  //------------------------------------------------------------------------------------------------------------------
  /** Pre-order iterator enumerates the nodes in pre-order (a node is visited before all its sub-trees). */
  class PreOrderIterator : public Iterator {
  public:
    PreOrderIterator (Tree& t);
    virtual ~PreOrderIterator () {}
    void operator++ () { if (p != 0) p = p->next_preorder; }
    operator bool () { return (p != 0); }
    operator Node* () { return p; }
    Node* operator -> () { return p; }
  private:
    Node* p; // pointer to the last visited node
  };
  //------------------------------------------------------------------------------------------------------------------
  /** Post-order iterator enumerates the nodes in post-order (a node's sub-trees are visited before the node). */
  class PostOrderIterator : public Iterator {
  public:
    PostOrderIterator (Tree& t);
    virtual ~PostOrderIterator () {}
    void operator++ () { if (p != 0) p = p->next_postorder; }
    operator bool () { return (p != 0); }
    operator Node* () { return p; }
    Node* operator -> () { return p; }
  private:
    Node* p; // pointer to the last visited node
  };
  //------------------------------------------------------------------------------------------------------------------
  /** Reverse post-order iterator, as the name suggests, enumerates the nodes in the order that is reverse of
      post-order. */
  class ReversePostOrderIterator : public Iterator {
  public:
    ReversePostOrderIterator (Tree& t);
    virtual ~ReversePostOrderIterator () {}
    void operator++ () { if (p != 0) p = p->prev_postorder; }
    operator bool() { return (p != 0); }
    operator Node* () { return p; }
    Node* operator -> () { return p; }
  private:
    Node* p; // pointer to the last visited node
  };
  //------------------------------------------------------------------------------------------------------------------
  Tree () { root_node = 0; }
  Tree (Node* root) { root_node = 0; add(root); }
  virtual ~Tree () {}
  
  Node* getRoot () { return root_node; }

  void add (Edge*) throw (DuplicateEdge, EdgeInUse, EmptyEdge, SecondParent, EmptyEndPoint);
  void add (Node*) throw (DuplicateNode, NodeInUse, EmptyNode);
  void add_empty_edge (Node*) throw (EmptyNode);
  void remove (Edge*) throw (NonexistentEdge, EmptyEdge);
  void remove (Node*) throw (NonexistentNode, DeletingRootOfNonSingletonTree, EmptyNode);
  Node* clone (Node*);

protected:
  Node* root_node;
  std::set<Node*> node_set;
  std::set<Edge*> edge_set;
  bool preorder_needed;
  bool postorder_needed;
  bool rpostorder_needed;

private:
  Node* create_preorder_links (Node*);
  Node* create_postorder_links (Node*);
  Node* create_rpostrder_links (Node*);
};
//--------------------------------------------------------------------------------------------------------------------

#endif
