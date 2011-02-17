// $Id: Tree.C,v 1.1 2004/07/07 10:26:35 dquinlan Exp $
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
// Tree is the base class of a general tree where each node can have a variable number of child nodes.  All
// algorithms that operate on abstract trees should use this base class for maximum portability.
//--------------------------------------------------------------------------------------------------------------------


// Mint headers
#include "Tree.h"

template <class T> void deque_erase (std::deque<T> d, T elt);

//--------------------------------------------------------------------------------------------------------------------
/** OutEdgesIterator iterates over all the non-null outgoing edges of a node.  The constructor initialized the
    iterator to point to the first non-null edge. */
Tree::OutEdgesIterator::OutEdgesIterator (Node* n)
{
  center = n;
  iter = center->outgoing.begin();
  Edge* e = 0;
  if (iter != center->outgoing.end())
    e = *iter;
  while ((e == 0) && (iter != center->outgoing.end())) {
    ++iter;
    e = *iter;
  }
}
//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
/** The ++ operator for OutEdgesIterator skips all null edges. */
void
Tree::OutEdgesIterator::operator ++ ()
{
  ++iter;
  Edge* e = 0;
  if (iter != center->outgoing.end())
    e = *iter;
  while ((e == 0) && (iter != center->outgoing.end())) {
    ++iter;
    e = *iter;
  }
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** PreOrderIterator creates an iterator to enumerate the tree nodes in pre-order.  If this is the first time a
    pre-order traversal has been demanded for this tree, or the tree has changed since the last pre-order traversal, a
    pre-order walk is carried out using the recursive method create_preorder_links.  This links up the nodes in
    pre-order and subsequent pre-order traversals simply follow the links. */
Tree::PreOrderIterator::PreOrderIterator (Tree& t)
{
  if (t.root_node != 0) {
    if (t.preorder_needed) {
      // reset all the preoder_next links
      std::set<Node*>::iterator ni = t.node_set.begin();
      while (ni != t.node_set.end()) {
        Node* n = *ni;
        n->next_preorder = 0;
        ++ni;
      }
      t.create_preorder_links(t.root_node);
      t.preorder_needed = false;
    }
  }
  p = t.root_node;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** This method must be called before a pre-order traversal can be carried out in a new, or altered, graph.  The
    method recursively traverses the graph in pre-order and updates the nodes' next_preorder links. */
Tree::Node*
Tree::create_preorder_links (Tree::Node* n)
{
  Node* last;

  ChildNodesIterator iter(n);
  if (!(bool)iter) {
    n->next_preorder = 0;
    last = n;
  }
  else {
    last = n;
    do {
      last->next_preorder = (Node*)iter;
      last = create_preorder_links((Node*)iter);
      ++iter;
    } while ((bool)iter);
  }
  return last;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** An edge to be added to the tree must not belong to any tree (including this one). */
void
Tree::add (Tree::Edge* e)
  throw (Tree::DuplicateEdge, Tree::EdgeInUse, Tree::EmptyEdge, Tree::SecondParent, Tree::EmptyEndPoint)
{
  if (e == 0)
    throw EmptyEdge();
  if (edge_set.find(e) != edge_set.end())
    throw DuplicateEdge(e);
  if (e->in_use)
    throw EdgeInUse(e);
  if ((e->child_node == 0) || (e->parent_node == 0))
    throw EmptyEndPoint(e);
  if (e->child_node->incoming != 0)
      throw SecondParent(e);

  // insert the nodes if they don't already exist in the graph
  if (node_set.find(e->parent_node) == node_set.end())
    add(e->parent_node);
  if (node_set.find(e->child_node) == node_set.end())
    add(e->child_node);

  // insert the edge in the corresponding lists of the two nodes
  e->parent_node->outgoing.push_back(e);
  e->child_node->incoming = e;

  // finally, insert the edge itself in the tree
  e->in_use = true;
  edge_set.insert(e);
  preorder_needed = postorder_needed = rpostorder_needed = true;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** A node to be added to the tree must not belong to any tree (including this one). */
void
Tree::add (Tree::Node* n)
  throw (Tree::DuplicateNode, Tree::NodeInUse, Tree::EmptyNode)
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
  preorder_needed = postorder_needed = rpostorder_needed = true;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** It is often useful to be able to have an empty outgoing edge to indicate the absence of a sub-tree.  This method
    adds an empty edge to the given node. */
void
Tree::add_empty_edge (Tree::Node *n)
  throw (Tree::EmptyNode)
{
  if (n == 0)
    throw EmptyNode();
  n->outgoing.push_back(0);
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
void
Tree::remove (Tree::Edge* e)
  throw (Tree::NonexistentEdge, Tree::EmptyEdge)
{
  if (e == 0)
    throw EmptyEdge();
  if (edge_set.erase(e) == 0)
    throw NonexistentEdge(e);
  preorder_needed = postorder_needed = rpostorder_needed = true;
  e->in_use = false;
  deque_erase<Edge*>(e->parent_node->outgoing, e);
  e->child_node->incoming = 0;
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
void
Tree::remove (Tree::Node* n)
  throw (Tree::NonexistentNode, Tree::DeletingRootOfNonSingletonTree, Tree::EmptyNode)
{
  if (n == 0)
    throw EmptyNode();
  if ((n == root_node) && (!edge_set.empty() || (node_set.size() > 1)))
    throw DeletingRootOfNonSingletonTree(n);
  if (node_set.erase(n) == 0)
    throw NonexistentNode(n);
  preorder_needed = postorder_needed = rpostorder_needed = true;
  n->in_use = false;
  if (n == root_node)
    root_node = 0;

  if (n->incoming != 0) {
    edge_set.erase(n->incoming);
    deque_erase<Edge*>(n->incoming->parent_node->outgoing, n->incoming);
    delete n->incoming; // an edge has no existence without its anchoring nodes
  }

  OutEdgesIterator out(n);
  while ((bool)out) {
    Edge* e = (Edge*)out;
    e->child_node->incoming = 0;
    edge_set.erase(e);
    delete e; // an edge has no existence without its anchoring nodes
    ++out;
  }
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** Since std::deque does not support random deletion of elements, we must search for an element and then erase it. */
template <class T>
void
deque_erase (std::deque<T> d, T elt)
{
  typename std::deque<T>::iterator iter = d.begin();
  while (iter != d.end()) {
    T k = *iter;
    if (k == elt) {
      d.erase(iter);
      break;
    }
  }
}
//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
/** Clone (as the name suggests) clones an entire subtree.  The parent of the new subtree is left undefined, but, all
    of it is added to the Tree.  It returns a pointer to the root of the newly created subtree.  Cloning proceeds by
    first creating a new copy of the root of the subtree -- this uses the new_copy virtual function for Node.  Next,
    the children of this root node are recursively cloned and clones of the existing edges are added to link the root
    with all the newly created children -- this time, using the new_copy virtual function for Edge.  */
Tree::Node*
Tree::clone (Tree::Node* subtree)
{
  Node* new_root = subtree->new_copy();
  OutEdgesIterator edge_iter(subtree);
  while ((bool)edge_iter) {
    Edge* out_edge = edge_iter;
    if (out_edge != 0) {
      Node* new_child = clone(out_edge->sink());
      add(out_edge->new_copy(new_root, new_child));
    }
    else
      new_root->outgoing.push_back(0);
    ++edge_iter;
  }
  return new_root;
}
//--------------------------------------------------------------------------------------------------------------------
