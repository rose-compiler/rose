// $Id: trtest.C,v 1.1 2004/07/07 10:26:35 dquinlan Exp $
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


#include <iostream>

using std::endl;
using std::cout;
using std::cerr;

#include "Tree.h"


class myTree : public Tree {
public:
  class Node : public Tree::Node {
  public:
    Node (int v) : Tree::Node() { val = v; }
    virtual ~Node () {}
    int val;
  };
  class Edge : public Tree::Edge {
  public:
    Edge (int v, Node* parent, Node* child) : Tree::Edge (parent, child) { val = v; }
    int val;
  };
  myTree () : Tree () {}
  myTree (Node* n) : Tree (n) {}
};

static void print_preorder (myTree& t);


int
main (int argc, char* argv[])
{
  try {
    myTree::Node n1(1);
    myTree::Node n2(2);
    myTree::Node n3(3);
    myTree::Edge e1(1, &n1, &n2);
    myTree::Edge e2(2, &n1, &n3);

    myTree t(&n1);
    t.add_empty_edge(&n1);
    t.add(&e2);
    t.add(&e1);

    print_preorder(t);
  }
  catch (Exception& e) {
    e.report(cerr);
  }
}


void
print_preorder (myTree& t)
{
  myTree::PreOrderIterator iter(t);
  cout << "Tree nodes in pre-order:" << endl;
  while ((bool)iter) {
    cout << "\tnode " << (dynamic_cast<myTree::Node*>((Tree::Node*)iter))->val << endl;
    ++iter;
  }
}
