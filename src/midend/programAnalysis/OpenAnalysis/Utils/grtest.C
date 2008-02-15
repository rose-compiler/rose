// $Id: grtest.C,v 1.1 2004/07/07 10:26:35 dquinlan Exp $
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

#include "DGraph.h"

class myGraph : public DGraph {
public:
  class Node : public DGraph::Node {
  public:
    Node (int v) : DGraph::Node() { val = v; }
    int val;
  };
  class Edge : public DGraph::Edge {
  public:
    Edge (Node* src, Node* sink, int v) : DGraph::Edge(src, sink) { val = v; }
    int val;
  };

  myGraph (Node* root) : DGraph (root) {}

  int edge_count () { return edge_set.size(); }
  int node_count () { return node_set.size(); }
};


void list_DFS_nodes (myGraph&);
void dump_graph (myGraph&);

int
main (int argc, char* argv[])
{
  try {
    myGraph::Node n1(1);
    myGraph::Node n2(2);
    myGraph::Node n3(3);
    myGraph::Node n4(4);
    myGraph::Edge e1(&n1, &n2, 1);
    myGraph::Edge e2(&n1, &n3, 2);
    myGraph::Edge e3(&n2, &n4, 3);
    //    myGraph::Edge e4(&n3, &n4, 4);

    myGraph g(&n1);

    g.add(&e1);
    g.add(&e2);
    g.add(&e3);
    //    g.add(&e4);

    dump_graph(g);
    list_DFS_nodes(g);
//    cout << "Initial graph" << endl;
//    dump_graph(g);
    g.remove(&n2);
    cout << "After deleting node 2" << endl;
//    //list_DFS_nodes(g);
    dump_graph(g);

    cout << "Graph 2" << endl;
    myGraph::Node nn(5);
    //  myGraph2::node nn2(1.3);
    myGraph g2(&nn);
    cout << "number of edges in g = " << g.edge_count() << ", number of edges in g2 = " << g2.edge_count() << endl;
    cout << "number of nodes in g = " << g.node_count() << ", number of nodes in g2 = " << g2.node_count() << endl;

    //  g2.add_node(&nn2);
    //  g2.add_edge(new myGraph2::edge(&nn, &nn2, 2.5));
    //list_DFS_nodes(g2);
    dump_graph(g2);

    //    g2.add(&e4);
    g2.add(&n3);

  }
  catch (Exception& e) {
    e.report(cerr);
  }

}


void
list_DFS_nodes (myGraph& g)
{
  cout << "Enumerating Nodes in DFS order:" << endl;
  myGraph::DFSIterator iter(g);
  while ((bool)iter) {
    myGraph::Node* n = dynamic_cast<myGraph::Node*>((DGraph::Node*)iter);
    cout << "\tnode " << n->val << endl;
    ++iter;
  }
}


void
dump_graph (myGraph& g)
{
  cout << "Enumerating all Nodes in arbitrary order:" << endl;
  myGraph::NodesIterator n_iter(g);
  while ((bool)n_iter) {
    myGraph::Node* n = dynamic_cast<myGraph::Node*>((DGraph::Node*)n_iter);
    cout << "\tnode " << n << " = " << n->val << endl;
    ++n_iter;
  }
  cout << "Enumerating all Edges in arbitrary order:" << endl;
  myGraph::EdgesIterator e_iter(g);
  while ((bool)e_iter) {
    myGraph::Edge* e = dynamic_cast<myGraph::Edge*>((DGraph::Edge*)e_iter);
    cout << "\tedge " << e << " = " << e->val << endl;
    ++e_iter;
  }
}
