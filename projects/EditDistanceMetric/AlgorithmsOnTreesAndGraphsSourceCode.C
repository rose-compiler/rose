// http://www.lsi.upc.es/~valiente/algorithm/combin.cpp

// Source code for the book "Algorithms on Trees and Graphs" by Gabriel
// Valiente (Berlin: Springer-Verlag, 2002), ISBN 3-540-43550-6.
// 
// Copyright (C) 2002  Gabriel Valiente
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <LEDA/graph.h>
#include <LEDA/graph_alg.h>
#include <LEDA/graphwin.h>

#include <LEDA/array2.h>
#include <LEDA/d_array.h>
#include <LEDA/h_array.h>
#include <LEDA/node_pq.h>
#include <LEDA/set.h>

using namespace leda;

bool is_tree(
  const graph& G)
{ 
  node v;
  forall_nodes(v,G)
    if ( G.indeg(v) > 1 ) return false; // nonunique parent
  return ( G.number_of_nodes() == G.number_of_edges() + 1
    && Is_Connected(G) );
}

class tree : public graph {
  public:
    tree() : graph() { }

    tree( const graph& G ) : graph( G ) {
      if ( !is_tree(G) )
        error_handler(1,"Graph is not a tree");
    }

    node parent( const node v ) const;
    bool is_root( const node v ) const;
    bool is_leaf( const node v ) const;
    node root() const;
    node first_child( const node v ) const;
    node last_child( const node v ) const;
    node next_sibling( const node v ) const;
    node previous_sibling( const node v ) const;
    bool is_first_child( const node v ) const;
    bool is_last_child( const node v ) const;
    int number_of_children( const node v ) const;
};

node tree::parent( const node v ) const {
  if ( (*this).indeg(v) == 0 ) return nil;
  return (*this).source((*this).first_in_edge(v));
}

bool tree::is_root( const node v ) const {
  return (*this).indeg(v) == 0;
}

bool tree::is_leaf( const node v ) const {
  return (*this).outdeg(v) == 0;
}

node tree::root() const {
  node v = (*this).choose_node();
  while ( !is_root(v) )
    v = parent(v);
  return v;
}

node tree::first_child( const node v ) const {
  if ( (*this).outdeg(v) == 0 ) return nil;
  return (*this).target((*this).first_adj_edge(v));
}

node tree::last_child( const node v ) const {
  if ( (*this).outdeg(v) == 0 ) return nil;
  return (*this).target((*this).last_adj_edge(v));
}

node tree::next_sibling( const node v ) const {
  if ( (*this).indeg(v) == 0 ) return nil;
  edge e = (*this).adj_succ((*this).first_in_edge(v));
  if ( e == nil ) return nil;
  return (*this).target(e);
}

node tree::previous_sibling( const node v ) const {
  if ( (*this).indeg(v) == 0 ) return nil;
  edge e = (*this).adj_pred((*this).first_in_edge(v));
  if ( e == nil ) return nil;
  return (*this).target(e);
}

bool tree::is_first_child( const node v ) const {
  if ( (*this).is_root(v) ) return false;
  return (*this).first_child((*this).parent(v)) == v;
}

bool tree::is_last_child( const node v ) const {
  if ( (*this).is_root(v) ) return true;
  return (*this).last_child((*this).parent(v)) == v;
}

int tree::number_of_children( const node v ) const {
  return (*this).outdeg(v);
}

#define forall_children(w,v) forall_adj_nodes(w,v)

template<class V,class E>
class TREE : public GRAPH<V,E> {
public:
  TREE<V,E>() { }

  TREE( const GRAPH<V,E>& G ) : GRAPH<V,E>( G ) {
    if ( !is_tree(G) )
      error_handler(1,"Graph is not a tree");
  }

  node parent( const node v ) const {
    if ( (*this).indeg(v) == 0 ) return nil;
    return (*this).source((*this).first_in_edge(v));
  }

  bool is_root( const node v ) const {
    return (*this).indeg(v) == 0;
  }

  bool is_leaf( const node v ) const {
    return (*this).outdeg(v) == 0;
  }

  node root() const {
    node v = (*this).choose_node();
    while ( !is_root(v) )
      v = parent(v);
    return v;
  }

  node first_child( const node v ) const {
    if ( (*this).outdeg(v) == 0 ) return nil;
    return (*this).target((*this).first_adj_edge(v));
  }

  node last_child( const node v ) const {
    if ( (*this).outdeg(v) == 0 ) return nil;
    return (*this).target((*this).last_adj_edge(v));
  }

  node next_sibling( const node v ) const {
    if ( (*this).indeg(v) == 0 ) return nil;
    edge e = (*this).adj_succ((*this).first_in_edge(v));
    if ( e == nil ) return nil;
    return (*this).target(e);
  }

  node previous_sibling( const node v ) const {
    if ( (*this).indeg(v) == 0 ) return nil;
    edge e = (*this).adj_pred((*this).first_in_edge(v));
    if ( e == nil ) return nil;
    return (*this).target(e);
  }

  bool is_first_child( const node v ) const {
    if ( (*this).is_root(v) ) return false;
    return (*this).first_child((*this).parent(v)) == v;
  }

  bool is_last_child( const node v ) const {
    if ( (*this).is_root(v) ) return true;
    return (*this).last_child((*this).parent(v)) == v;
  }

  int number_of_children( const node v ) const {
    return (*this).outdeg(v);
  }

};

// #include "combin.h"

void preorder_tree_traversal(
  const tree& T,
  node_array<int>& order);
void preorder_tree_depth(
  const tree& T,
  node_array<int>& order,
  node_array<int>& depth);
void rearrange_tree_in_preorder(
  tree& T);
void extend_backtracking_tree_edit(
  const tree& T1,
  const tree& T2,
  const node_array<int>& num1,
  const node_array<int>& num2,
  const node_array<int>& depth1,
  const node_array<int>& depth2,
  node_array<list<node> >& C,
  node v,
  node_array<node>& M,
  list<node_array<node> >& L);
void extend_branch_and_bound_tree_edit(
  const tree& T1,
  const tree& T2,
  const node_array<int>& num1,
  const node_array<int>& num2,
  const node_array<int>& depth1,
  const node_array<int>& depth2,
  node_array<list<node> >& C,
  node v,
  node_array<node>& M,
  int cost,
  node_array<node>& A,
  int& lowcost);
int divide_and_conquer_tree_edit(
  const tree& T1,
  const node_array<int>& order1,
  node v1,
  node v2,
  const tree& T2,
  const node_array<int>& order2,
  node w1,
  node w2,
  node_array<node>& M);
node predecessor_of_last_child(
  const tree& T,
  const node_array<int>& order,
  node v1,
  node v2);
int top_down_dyn_prog_tree_edit(
  const tree& T1,
  const node_array<int>& order1,
  node v1,
  node v2,
  const tree& T2,
  const node_array<int>& order2,
  node w1,
  node w2,
  h_array<four_tuple<node,node,node,node>,two_tuple<int,node_array<node> > >& S,
  node_array<node>& M);
int bottom_up_dyn_prog_tree_edit(
  const tree& T1,
  const node_array<array<node> >& child1,
  node r1,
  const tree& T2,
  const node_array<array<node> >& child2,
  node r2,
  list<two_tuple<node,node> >& L);

void rec_preorder_subtree_traversal(
  const tree& T,
  const node v,
  node_array<int>& order,
  int& num);
void rec_postorder_subtree_traversal(
  const tree& T,
  const node v,
  node_array<int>& order,
  int& num);
void rec_preorder_node_depth(
  const tree& T,
  const node v,
  node_array<int>& depth,
  node& deepest,
  int& num);
void rec_top_down_subtree_traversal(
  const tree& T,
  const node v,
  node_array<int>& order,
  int& num,
  const int d);

int compare(
    const list<int>& L1,
    const list<int>& L2);
bool operator<(
  const two_tuple<int,int>& p,
  const two_tuple<int,int>& q);
int Hash(
  const list<int>& L);
inline int id(
  const int& x);
template<class E>
void list_to_array(
  const list<E>& L,
  array<E>& A);
void preorder_tree_list_traversal(
  const TREE<string,string>& T,
  list<node>& L);
void postorder_tree_list_traversal(
  const TREE<string,string>& T,
  list<node>& L);
void radix_sort(
  list<list<int> >& S);
void subtree_nodes(
  const TREE<string,string>& T,
  const node r,
  list<node>& L);
void berztiss_ordered_subtree_isomorphism_mapping(
    const tree& T1,
    const tree& T2,
    const node v,
    node_array<node>& M);
bool is_ordered_tree_isomorphism(
  const tree& T1,
  const tree& T2,
  const node_array<node>& M);
bool map_ordered_tree(
  const TREE<string,string>& T1,
  const node r1,
  const TREE<string,string>& T2,
  const node r2,
  node_array<node>& M);
bool map_ordered_subtree(
  const TREE<string,string>& T1,
  const node r1,
  const TREE<string,string>& T2,
  const node r2,
  node_array<node>& M);
void map_unordered_subtree(
  const TREE<string,string>& T1,
  const node r1,
  const TREE<string,string>& T2,
  const node r2,
  const node_array<int>& code1,
  const node_array<int>& code2,
  node_array<node>& M);
void sort_isomorphism_codes(
  list<int>& L);
bool map_ordered_common_subtree(
  const TREE<string,string>& T1,
  const node r1,
  const TREE<string,string>& T2,
  const node r2,
  node_array<node>& M);
void preorder_subtree_list_traversal(
  const TREE<string,string>& T,
  const node r,
  list<node>& L);

void leftmost_depth_first_traversal(
  const graph& G,
  node_array<bool>& node_visited,
  edge_array<bool>& edge_visited,
  edge e,
  list<edge>& L);

void next_clique(
  const graph& G,
  const node_matrix<bool>& A,
  set<node>& C,
  set<node>& P,
  list<set<node> >& L);
void simple_next_maximal_clique(
  const graph& G,
  const node_matrix<bool>& A,
  set<node>& C,
  set<node>& P,
  set<node>& S,
  list<set<node> >& L);
void next_maximal_clique(
  const graph& G,
  const node_matrix<bool>& A,
  set<node>& C,
  set<node>& P,
  set<node>& S,
  list<set<node> >& L);
void all_maximal_cliques(
  const graph& G,
  const node_matrix<bool>& A,
  list<set<node> >& L);
inline int clique_size(
  const set<node>& C);
void next_maximum_clique(
  const graph& G,
  const node_matrix<bool>& A,
  set<node>& C,
  set<node>& P,
  set<node>& S,
  const int max_deg,
  set<node>& MAX);
void graph_complement(
  GRAPH<string,string>& G);

bool extend_graph_isomorphism(
    const GRAPH<string,string>& G1,
    const GRAPH<string,string>& G2,
    const node_matrix<edge>& A1,
    const node_matrix<edge>& A2,
    node_array<node> M,
    node v,
    list<node_array<node> >& L);
bool preserves_adjacencies(
    const GRAPH<string,string>& G1,
    const GRAPH<string,string>& G2,
    const node_matrix<edge>& A1,
    const node_matrix<edge>& A2,
    const node_array<node>& M,
    node v,
    node w);
void graph_isomorphism(
    const GRAPH<string,string>& G1,
    const GRAPH<string,string>& G2,
    node_array<node>& M,
    list<node_array<node> >& L);
bool refine_subgraph_isomorphism(
    const GRAPH<string,string>& G1,
    const GRAPH<string,string>& G2,
    const node_matrix<edge>& A1,
    const node_matrix<edge>& A2,
    node_array<set<node> >& CAN,
    node v,
    node w);
void extend_subgraph_isomorphism(
    const GRAPH<string,string>& G1,
    const GRAPH<string,string>& G2,
    const node_matrix<edge>& A1,
    const node_matrix<edge>& A2,
    node_array<set<node> >& CAN,
    node v,
    node_array<node>& MAP,
    list<node_array<node> >& L);
void subgraph_isomorphism(
    const GRAPH<string,string>& G1,
    const GRAPH<string,string>& G2,
    node_array<node>& MAP,
    list<node_array<node> >& L);
void depth_first_prefix_rightmost_tree_traversal(
  const tree& T,
  node_array<int>& order);

int compare(
  const array<int>& A1,
  const array<int>& A2)
{ 
  int n = leda_min(A1.size(),A2.size());
  for ( int i = 1; i <= n; i++ ) {
    if ( A1[i] < A2[i] ) return -1;
    if ( A1[i] > A2[i] ) return 1;
  }
  if ( A1.size() < A2.size() ) return -1;
  if ( A1.size() > A2.size() ) return 1;
  return 0;
}

bool operator>(
  const array<int>& A1,
  const array<int>& A2)
{ 
  return compare(A1,A2) == 1;
}

void adjacency_matrix(
  const graph& G,
  node_matrix<bool>& A)
{ 
  A.init(G,nil);
  edge e;
  forall_edges(e,G) {
    A(G.source(e),G.target(e)) = true;
} }

bool adjacent(
  node v,
  node w,
  const node_matrix<bool>& A)
{ 
  return A(v,w);
}

bool adjacent(
  node v,
  node w)
{ 
  node u;
  forall_adj_nodes(u,v)
    if ( u == w ) return true;
  return false;
}

template<class E>
void straight_bucket_sort(
  list<E>& L,
  int i,
  int j,
  int (*ord)(const E&) = id)
{ 
  int n = j - i + 1; // number of buckets needed
  array<list<E> > bucket(1,n);
  while ( !L.empty() ) {
    E x = L.pop();
    int k = ord(x) - i + 1; // element [[x]] belongs in bucket [[k]]
    if ( k >= 1 && k <= n ) {
      bucket[k].append(x);
    } else {
      error_handler(1,"bucket sort: value out of range");
  } }
  for ( i = 1; i <= n; i++ )
    L.conc(bucket[i]); // destructive
  { list_item it;
    forall_items(it,L) {
      if ( it != L.last() && ord(L[it]) > ord(L[L.succ(it)]) ) {
        error_handler(1,"Wrong implementation of bucket sort");
  } } }

}

template<class E>
void straight_bucket_sort(
  list<E>& L,
  int (*ord)(const E&) = id)
{ 
  if ( L.empty() ) return;
  int i = ord(L.head());
  int j = i;
  E x;
  forall(x,L) {
    int k = ord(x);
    if ( k < i ) i = k;
    if ( k > j ) j = k;
  }
  straight_bucket_sort(L,i,j,ord);
}

void radix_sort(
  list<array<int> >& L,
  int min,
  int max)
{ 
  if ( L.empty() ) return;
  int maxlen = 0;
  array<int> A,T;
  forall(A,L)
    maxlen = leda_max(maxlen,A.size());
  int n = leda_max(max - min + 1,maxlen);

  array<list<array<int> > > bucket(1,n);
  list<array<int> > P;
  forall(A,L) {
    for ( int i = 1; i <= A.size(); i++ ) {
      array<int> T(1,i,A[i]);
      P.append(T);
  } }

  while ( !P.empty() ) {
    T = P.pop();
    int k = T[2] - min + 1; // [[T]] belongs in bucket [[k]]
    if ( k >= 1 && k <= n ) {
      bucket[k].append(T);
    } else {
      error_handler(1,"radix sort: value out of range");
  } }
  for ( int i = 1; i <= n; i++ )
    P.conc(bucket[i]); // destructive

  while ( !P.empty() ) {
    T = P.pop();
    int k = T[1]; // [[T]] belongs in bucket [[k]]
    if ( k >= 1 && k <= n ) {
      bucket[k].append(T);
    } else {
      error_handler(1,"radix sort: value out of range");
  } }
  for ( int i = 1; i <= n; i++ )
    P.conc(bucket[i]); // destructive

  array<list<int> > FULL(1,maxlen);
  forall(A,P)
    FULL[A[1]].append(A[2]);
  for ( int i = 1; i <= maxlen; i++ )
    FULL[i].unique();

  array<list<array<int> > > LEN(1,maxlen);
  while ( !L.empty() ) {
    A = L.pop();
    LEN[A.size()].append(A);
  }

  for ( int i = maxlen; i >= 1; i-- ) {
    L.conc(LEN[i],LEDA::before); // destructive
    while ( !L.empty() ) {
      A = L.pop();
      int k = A[i] - min + 1; // array [[A]] belongs in bucket [[k]]
      if ( k >= 1 && k <= n ) {
        bucket[k].append(A);
      } else {
        error_handler(1,"radix sort: value out of range");
    } }
    int x;
    forall(x,FULL[i])
      L.conc(bucket[x - min + 1]); // destructive

  }
  { list_item it;
    forall_items(it,L) {
      if ( it != L.last() && L[it] > L[L.succ(it)] ) {
        error_handler(1,"Wrong implementation of radix sort");
  } } }

}

void radix_sort(
  list<array<int> >& L)
{ 
  if ( L.length() <= 1 ) return;
  if ( L.head().size() == 0 ) return;
  int i = L.head()[1];
  int j = i;
  array<int> A;
  int x;
  forall(A,L) {
    forall(x,A) {
      i = leda_min(i,x);
      j = leda_max(j,x);
  } }
  radix_sort(L,i,j);
}

static void make_proof_panel(
    panel& P,
    string s,
    bool proof = false)
{ 
  P.text_item("\\bf \\blue " + s + ".");
  P.button("ok");
  if ( proof ) P.fbutton("proof",1);
}

static void make_yes_no_panel(
    panel& P,
    string s,
    bool no = false)
{ 
  P.text_item("\\bf \\blue " + s + "?");
  P.button("yes");
  if ( no ) P.fbutton("no",1);
}

static void make_proof_maximum_panel(
    panel& P,
    string s,
    bool proof = false)
{ 
  P.text_item("\\bf \\blue " + s + ".");
  P.button("ok",0);
  if ( proof ) P.button("proof",1);
  P.fbutton("maximum",2);
}

void double_check_tree_edit(
  const tree& T1,
  const tree& T2,
  const node_array<node>& M)
{ 
  node_array<int> num1(T1);
  node_array<int> depth1(T1);
  preorder_tree_depth(T1,num1,depth1);
  node_array<int> num2(T2);
  node_array<int> depth2(T2);
  preorder_tree_depth(T2,num2,depth2);

  node v = T1.root();
  node w = T2.root();
  if ( M[v] != nil && M[v] != w )
    error_handler(1,"Wrong implementation of tree edit");

  forall_nodes(v,T1) {
    if ( M[v] != nil ) {
      if ( depth1[v] != depth2[M[v]] )
        error_handler(1,"Wrong implementation of tree edit");
      if ( !T1.is_root(v) && T2.parent(M[v]) != M[T1.parent(v)] )
        error_handler(1,"Wrong implementation of tree edit");
      forall_nodes(w,T1)
        if ( M[w] != nil && num1[v] < num1[w] && num2[M[v]] >= num2[M[w]] )
          error_handler(1,"Wrong implementation of tree edit");
} } }

void tree_edit_graph(
  tree& T1,
  tree& T2,
  GRAPH<string,string>& G)
{ 
  G.clear();
  rearrange_tree_in_preorder(T1);
  rearrange_tree_in_preorder(T2);
  int n1 = T1.number_of_nodes();
  int n2 = T2.number_of_nodes();

  node_array<int> num1(T1), num2(T2), depth1(T1), depth2(T2);
  preorder_tree_depth(T1,num1,depth1);
  preorder_tree_depth(T2,num2,depth2);
  node v,w;
  array<int> d1(1,n1);
  forall_nodes(v,T1)
    d1[num1[v]] = depth1[v];
  array<int> d2(1,n2);
  forall_nodes(w,T2)
    d2[num2[w]] = depth2[w];

  array2<node> A(0,n1,0,n2);
  for ( int i = 0; i <= n1; i++ ) {
    for ( int j = 0; j <= n2; j++ ) {
      A(i,j) = v = G.new_node(string("%i:%i",i,j));
  } }

  for ( int i = 0; i < n1; i++ ) G.new_edge(A(i,n2),A(i+1,n2),"del");
  for ( int j = 0; j < n2; j++ ) G.new_edge(A(n1,j),A(n1,j+1),"ins");
  for ( int i = 0; i < n1; i++ ) {
    for ( int j = 0; j < n2; j++ ) {
      if ( d1[i+1] >= d2[j+1] ) G.new_edge(A(i,j),A(i+1,j),"del");
      if ( d1[i+1] == d2[j+1] ) G.new_edge(A(i,j),A(i+1,j+1),"sub");
      if ( d1[i+1] <= d2[j+1] ) G.new_edge(A(i,j),A(i,j+1),"ins");
} } }

void tree_edit(
  tree& T1,
  tree& T2,
  GRAPH<string,string>& G,
  list<edge>& P)
{ 
  tree_edit_graph(T1,T2,G);
  node s = G.first_node(); // top-left corner
  node t = G.last_node(); // bottom-right corner

  edge_array<double> cost(G,1); // cost of elementary edit operations
  node_array<edge> pred(G);
  node_array<double> dist(G);
  ACYCLIC_SHORTEST_PATH(G,s,cost,dist,pred);

  P.clear();
  node v = t;
  edge e;
  while ( pred[v] != nil ) {
    e = pred[v];
    P.push(e);
    v = G.source(e);
} }

void rearrange_tree_in_preorder(
  tree& T)
{ 
  node_array<int> order(T);
  node_array<int> depth(T);
  preorder_tree_depth(T,order,depth);
  T.sort_nodes(order);
}

void backtracking_tree_edit(
  tree& T1,
  tree& T2,
  node_array<node>& M,
  list<node_array<node> >& L)
{ 
  L.clear();
  rearrange_tree_in_preorder(T1);
  rearrange_tree_in_preorder(T2);
  node_array<int> num1(T1), num2(T2), depth1(T1), depth2(T2);
    node_array<list<node> > C(T1);
  { preorder_tree_depth(T1,num1,depth1);
    preorder_tree_depth(T2,num2,depth2);
    node v,w;
    forall_nodes(v,T1) {
      C[v].append(nil); // dummy node, representing deletion of node [[v]]
      forall_nodes(w,T2) {
        if ( depth1[v] == depth2[w] ) {
          C[v].append(w);
  } } } }

  node v = T1.first_node();
  extend_backtracking_tree_edit(T1,T2,num1,num2,depth1,depth2,C,v,M,L);
}

void extend_backtracking_tree_edit(
  const tree& T1,
  const tree& T2,
  const node_array<int>& num1,
  const node_array<int>& num2,
  const node_array<int>& depth1,
  const node_array<int>& depth2,
  node_array<list<node> >& C,
  node v,
  node_array<node>& M,
  list<node_array<node> >& L)
{ 
  node w,x,y;
  forall(w,C[v]) {
    M[v] = w;
    node_array<list<node> > N(T1);
    N = C;
    list_item it;
    if ( w != nil ) {
      forall_nodes(x,T1) {
        forall_items(it,N[x]) {
          y = N[x].contents(it);
          if ( y == w ) {
            N[x].del(it);
    } } } }

    forall_children(x,v) {
      forall_items(it,N[x]) {
        y = N[x].contents(it);
        if ( y != nil && T2.parent(y) != w ) {
          N[x].del(it);
    } } }

    if ( !T1.is_root(v) && w != nil ) {
      forall_children(x,T1.parent(v)) {
        if ( num1[v] < num1[x] ) {
          forall_items(it,N[x]) {
            y = N[x].contents(it);
            if ( y != nil && num2[w] > num2[y] ) {
              N[x].del(it);
    } } } } }


    if ( v == T1.last_node() ) {
      double_check_tree_edit(T1,T2,M);
      L.append(M);
    } else {
      extend_backtracking_tree_edit
        (T1,T2,num1,num2,depth1,depth2,N,T1.succ_node(v),M,L);
} } }

void preorder_tree_depth(
  const tree& T,
  node_array<int>& order,
  node_array<int>& depth)
{ 
  stack<node> S;
  node v,w;
  S.push(T.root());
  int num = 1;
  do {
    v = S.pop();
    order[v] = num++; // visit node [[v]]
    if ( T.is_root(v) ) {
      depth[v] = 0;
    } else {
      depth[v] = depth[T.parent(v)] + 1;
    }
    w = T.last_child(v);
    while ( w != nil ) {
      S.push(w);
      w = T.previous_sibling(w);
    }
  } while ( !S.empty() );
}

void branch_and_bound_tree_edit(
  tree& T1,
  tree& T2,
  node_array<node>& M)
{ 
  rearrange_tree_in_preorder(T1);
  rearrange_tree_in_preorder(T2);
  node_array<int> num1(T1), num2(T2), depth1(T1), depth2(T2);
    node_array<list<node> > C(T1);
  { preorder_tree_depth(T1,num1,depth1);
    preorder_tree_depth(T2,num2,depth2);
    node v,w;
    forall_nodes(v,T1) {
      C[v].append(nil); // dummy node, representing deletion of node [[v]]
      forall_nodes(w,T2) {
        if ( depth1[v] == depth2[w] ) {
          C[v].append(w);
  } } } }

  node v = T1.first_node();
  int cost = 0;
  node_array<node> A;
  int lowcost = T1.number_of_nodes();
  extend_branch_and_bound_tree_edit
    (T1,T2,num1,num2,depth1,depth2,C,v,M,cost,A,lowcost);
  M = A; // return least-cost solution found
}

void extend_branch_and_bound_tree_edit(
  const tree& T1,
  const tree& T2,
  const node_array<int>& num1,
  const node_array<int>& num2,
  const node_array<int>& depth1,
  const node_array<int>& depth2,
  node_array<list<node> >& C,
  node v,
  node_array<node>& M,
  int cost,
  node_array<node>& A,
  int& lowcost)
{ 
  node w,x,y;
  forall(w,C[v]) {
    M[v] = w;
    node_array<list<node> > N(T1);
    N = C;
    list_item it;
    if ( w != nil ) {
      forall_nodes(x,T1) {
        forall_items(it,N[x]) {
          y = N[x].contents(it);
          if ( y == w ) {
            N[x].del(it);
    } } } }

    forall_children(x,v) {
      forall_items(it,N[x]) {
        y = N[x].contents(it);
        if ( y != nil && T2.parent(y) != w ) {
          N[x].del(it);
    } } }

    if ( !T1.is_root(v) && w != nil ) {
      forall_children(x,T1.parent(v)) {
        if ( num1[v] < num1[x] ) {
          forall_items(it,N[x]) {
            y = N[x].contents(it);
            if ( y != nil && num2[w] > num2[y] ) {
              N[x].del(it);
    } } } } }


    if ( w == nil ) cost++; // deletion of node [[v]]
    if ( cost < lowcost ) { // lower-cost (partial) solution found
      if ( v == T1.last_node() ) {
        double_check_tree_edit(T1,T2,M);
        A = M; // remember lowest-cost solution so far
        lowcost = cost;
      } else {
        extend_branch_and_bound_tree_edit
          (T1,T2,num1,num2,depth1,depth2,N,T1.succ_node(v),M,cost,A,lowcost);
    } }
    if ( w == nil ) cost--; // deletion of node [[v]]
} }

int divide_and_conquer_tree_edit(
  tree& T1,
  tree& T2,
  node_array<node>& M)
{ 
  rearrange_tree_in_preorder(T1);
  rearrange_tree_in_preorder(T2);
  node v,w;
  node_array<int> order1(T1);
  node_array<int> order2(T2);
  int i = 1;
  forall_nodes(v,T1)
    order1[v] = i++;
  int j = 1;
  forall_nodes(w,T2)
    order2[w] = j++;

  return divide_and_conquer_tree_edit(T1,order1,
    T1.first_node(),T1.last_node(),T2,order2,T2.first_node(),T2.last_node(),M);
}

int divide_and_conquer_tree_edit(
  const tree& T1,
  const node_array<int>& order1,
  node v1,
  node v2,
  const tree& T2,
  const node_array<int>& order2,
  node w1,
  node w2,
  node_array<node>& M)
{ 
  int dist,del,ins,pre,pos;
  node k1,k2,l1,l2;

  if ( v1 == v2 ) {
    if ( w1 == w2 ) { // substitution of [[v1]] by [[w1]]

      M[v1] = w1;
      dist = 1;

    } else { // insertion of [[k2]]

      k2 = predecessor_of_last_child(T2,order2,w1,w2);
      dist = divide_and_conquer_tree_edit(T1,order1,v1,v2,T2,order2,w1,k2,M);

    }
  } else {
    if ( w1 == w2 ) { // deletion of [[k1]]

      k1 = predecessor_of_last_child(T1,order1,v1,v2);
      dist = divide_and_conquer_tree_edit(T1,order1,v1,k1,T2,order2,w1,w2,M);

    } else { // substitution of [[k1]] by [[k2]]

      k1 = predecessor_of_last_child(T1,order1,v1,v2);
      node_array<node> M1(T1); M1 = M;
      del = divide_and_conquer_tree_edit(T1,order1,v1,k1,T2,order2,w1,w2,M1);

      k2 = predecessor_of_last_child(T2,order2,w1,w2);
      node_array<node> M2(T1); M2 = M;
      ins = divide_and_conquer_tree_edit(T1,order1,v1,v2,T2,order2,w1,k2,M2);

      node_array<node> M3(T1); M3 = M;
      pre = divide_and_conquer_tree_edit(T1,order1,v1,k1,T2,order2,w1,k2,M3);
      pos = divide_and_conquer_tree_edit(T1,order1,
        T1.succ_node(k1),v2,T2,order2,T2.succ_node(k2),w2,M3);

      dist = leda_max(leda_max(del,ins),pre+pos);
      if ( dist == del ) M = M1;
      else if ( dist == ins ) M = M2;
      else M = M3;
  } }

  return dist;
}

node predecessor_of_last_child(
  const tree& T,
  const node_array<int>& order,
  node v1,
  node v2)
{ 
  node k = T.last_child(v1);
  while ( order[k] > order[v2] )
    k = T.previous_sibling(k);
  return T.pred_node(k);
}

int top_down_dynamic_programming_tree_edit(
  tree& T1,
  tree& T2,
  node_array<node>& M)
{ 
  rearrange_tree_in_preorder(T1);
  rearrange_tree_in_preorder(T2);
  node v,w;
  node_array<int> order1(T1);
  node_array<int> order2(T2);
  int i = 1;
  forall_nodes(v,T1)
    order1[v] = i++;
  int j = 1;
  forall_nodes(w,T2)
    order2[w] = j++;
  h_array<four_tuple<node,node,node,node>,
    two_tuple<int,node_array<node> > > S;
  return top_down_dyn_prog_tree_edit(T1,order1,T1.first_node(),T1.last_node(),
    T2,order2,T2.first_node(),T2.last_node(),S,M);
}

int top_down_dyn_prog_tree_edit(
  const tree& T1,
  const node_array<int>& order1,
  node v1,
  node v2,
  const tree& T2,
  const node_array<int>& order2,
  node w1,
  node w2,
  h_array<four_tuple<node,node,node,node>,
    two_tuple<int,node_array<node> > >& S,
  node_array<node>& M)
{ 
  int dist;
  four_tuple<node,node,node,node> N(v1,v2,w1,w2);
  if ( S.defined(N) ) { // problem [[v1,v2,w1,w2]] was already solved

    dist = S[N].first();
    M = S[N].second();

  } else { // solve problem [[v1,v2,w1,w2]]

    int del,ins,pre,pos;
    node k1,k2,l1,l2;

    if ( v1 == v2 ) {
      if ( w1 == w2 ) { // substitution of [[v1]] by [[w1]]

        M[v1] = w1;
        dist = 1;

      } else { // insertion of [[k2]]

        k2 = predecessor_of_last_child(T2,order2,w1,w2);
        dist = top_down_dyn_prog_tree_edit(T1,order1,v1,v2,
          T2,order2,w1,k2,S,M);
      }
    } else {
      if ( w1 == w2 ) { // deletion of [[k1]]

        k1 = predecessor_of_last_child(T1,order1,v1,v2);
        dist = top_down_dyn_prog_tree_edit(T1,order1,v1,k1,
          T2,order2,w1,w2,S,M);

      } else { // substitution of [[k1]] by [[k2]]

        k1 = predecessor_of_last_child(T1,order1,v1,v2);
        node_array<node> M1(T1); M1 = M;
        del = top_down_dyn_prog_tree_edit(T1,order1,v1,k1,
          T2,order2,w1,w2,S,M1);

        k2 = predecessor_of_last_child(T2,order2,w1,w2);
        node_array<node> M2(T1); M2 = M;
        ins = top_down_dyn_prog_tree_edit(T1,order1,v1,v2,
          T2,order2,w1,k2,S,M2);

        node_array<node> M3(T1); M3 = M;
        pre = top_down_dyn_prog_tree_edit(T1,order1,v1,k1,
          T2,order2,w1,k2,S,M3);
        pos = top_down_dyn_prog_tree_edit(T1,order1,T1.succ_node(k1),v2,
          T2,order2,T2.succ_node(k2),w2,S,M3);

        dist = leda_max(leda_max(del,ins),pre+pos);
        if ( dist == del ) M = M1;
        else if ( dist == ins ) M = M2;
        else M = M3;
    } }

    two_tuple<int,node_array<node> > R(dist,M);
    S[N] = R; // save solution to problem [[v1,v2,w1,w2]] for later lookup
  }

  return dist;
}

int bottom_up_dynamic_programming_tree_edit(
  const tree& T1,
  const tree& T2,
  node_array<node>& M)
{ 
  node_array<array<node> > child1(T1);
  { node v,w;
    forall_nodes(v,T1) {
      if ( !T1.is_leaf(v) ) {
        array<node> CHILD(1,T1.number_of_children(v));
        int k = 1;
        forall_children(w,v)
          CHILD[k++] = w;
        child1[v] = CHILD;
  } } }
  node_array<array<node> > child2(T2);
  { node v,w;
    forall_nodes(v,T2) {
      if ( !T2.is_leaf(v) ) {
        array<node> CHILD(1,T2.number_of_children(v));
        int k = 1;
        forall_children(w,v)
          CHILD[k++] = w;
        child2[v] = CHILD;
  } } }

  list<two_tuple<node,node> > L;
  bottom_up_dyn_prog_tree_edit(T1,child1,T1.root(),T2,child2,T2.root(),L);
  two_tuple<node,node> EDIT;
  forall(EDIT,L) {
    node v = EDIT.first();
    node w = EDIT.second();
    if ( v != nil ) { // deletion or substitution
      M[v] = w;
} } }

int bottom_up_dyn_prog_tree_edit(
  const tree& T1,
  const node_array<array<node> >& child1,
  node r1,
  const tree& T2,
  const node_array<array<node> >& child2,
  node r2,
  list<two_tuple<node,node> >& L)
{ 
  int m = T1.number_of_children(r1);
  int n = T2.number_of_children(r2);
  array2<int> D(0,m,0,n);
  array2<list<two_tuple<node,node> > > E(0,m,0,n);
  D(0,0) = 0;
  for ( int i = 1; i <= m; i++ ) {
    D(i,0) = D(i-1,0);
    two_tuple<node,node> DEL(child1[r1][i],nil);
    E(i,0) = E(i-1,0);
    E(i,0).append(DEL);
  }
  for ( int j = 1; j <= n; j++ ) {
    D(0,j) = D(0,j-1);
    two_tuple<node,node> INS(nil,child2[r2][j]);
    E(0,j) = E(0,j-1);
    E(0,j).append(INS);
  }
  for ( int i = 1; i <= m; i++ ) {
    for ( int j = 1; j <= n; j++ ) {
      node rr1 = child1[r1][i];
      node rr2 = child2[r2][j];
      int del = D(i-1,j);
      int ins = D(i,j-1);
      D(i,j) = leda_max(del,ins);
      list<two_tuple<node,node> > LL;
      int subst = bottom_up_dyn_prog_tree_edit(T1,child1,rr1,T2,child2,rr2,LL);
      if ( del >= D(i-1,j-1) + subst ) { // delete
        D(i,j) = del;
        two_tuple<node,node> DEL(rr1,nil);
        E(i,j) = E(i-1,j);
        E(i,j).append(DEL);
      } else {
        if ( ins >= D(i-1,j-1) + subst ) { // insert
          D(i,j) = ins;
          two_tuple<node,node> INS(nil,rr2);
          E(i,j) = E(i,j-1);
          E(i,j).append(INS);
        } else { // substitute
          D(i,j) = D(i-1,j-1) + subst;
          E(i,j) = E(i-1,j-1);
          E(i,j).conc(LL);
  } } } }
  two_tuple<node,node> SUBST(r1,r2);
  L = E(m,n);
  L.append(SUBST);
  return D(m,n)+1;
}

int Hash(
  const four_tuple<node,node,node,node>& N)
{
  int sum = index(N.first());
  sum += index(N.second());
  sum += index(N.third());
  sum += index(N.fourth());
  return sum;
}

void gw_tree_edit(
  GraphWin& gw1)
{ 
  graph& G1 = gw1.get_graph();
  tree T1(G1);

  graph G2;
  GraphWin gw2(G2,500,500,"Tree Edit");
  gw2.display();
  gw2.message("Enter second tree. Press done when finished");
  gw2.edit();
  gw2.del_message();

  tree T2(G2);

  GRAPH<string,string> G;
  list<edge> P;
  tree_edit(T1,T2,G,P);
  node_array<double> xcoord(G);
  node_array<double> ycoord(G);
  { int n1 = T1.number_of_nodes();
    int n2 = T2.number_of_nodes();
    int N = (n1 + 1) * (n2 + 1);
    xcoord.init(G,N,0);
    ycoord.init(G,N,0);
    double d = 1.0 / ((n1 + 1) + (n2 + 1) + 1);
    int i,j;
    node v;
    forall_nodes(v,G) {
      i = index(v) / (n2 + 1);
      j = index(v) - i * (n2 + 1);
      xcoord[v] = (j + 1) * d;
      ycoord[v] = - (i + 1) * d;
  } }


  gw1.save_all_attributes();
  gw1.set_node_label_type(data_label);
  gw2.set_node_label_type(data_label);
  node_array<int> order1(T1);
  preorder_tree_traversal(T1,order1);
  node_array<int> order2(T2);
  preorder_tree_traversal(T2,order2);
  node v,w;
  forall_nodes(v,T1)
    gw1.set_label(v,string("%i",order1[v]));
  forall_nodes(w,T2)
    gw2.set_label(w,string("%i",order2[w]));
 
  GraphWin gw(G,500,500,"Tree Edit");
  gw.set_node_label_type(data_label);
  gw.display(window::center,window::center);
  gw.adjust_coords_to_win(xcoord,ycoord);
  gw.set_layout(xcoord,ycoord);

  edge e;
  forall(e,P) {
    v = G.target(e);
    if ( G[e] == "del" ) gw.set_color(v,red); // deletion
    if ( G[e] == "sub" ) gw.set_color(v,green); // substitution
    if ( G[e] == "ins" ) gw.set_color(v,blue); // insertion
    gw.set_width(e,3);
  }

  gw.wait();
  gw1.restore_all_attributes();
}

void gw_backtracking_tree_edit(
  GraphWin& gw1)
{ 
  graph& G1 = gw1.get_graph();
  tree T1(G1);

  graph G2;
  GraphWin gw2(G2,500,500,"Backtracking Tree Edit");
  gw2.display();
  gw2.message("Enter second tree. Press done when finished");
  gw2.edit();
  gw2.del_message();

  tree T2(G2);

  node_array<node> M(T1);
  list<node_array<node> > L;
  backtracking_tree_edit(T1,T2,M,L);

  panel P;
  make_proof_panel(P,
    string("There are %i valid tree transformations",L.length()),true);
  if ( gw1.open_panel(P) ) { // proof button pressed
    forall(M,L) {
      gw1.save_all_attributes();
      gw2.save_all_attributes();
      node v;
      forall_nodes(v,T1) {
        if ( M[v] != nil ) {
          gw1.set_color(v,blue);
          gw2.set_color(M[v],blue);
      } }
      panel Q;
      make_yes_no_panel(Q,"Continue",true);
      if ( gw1.open_panel(Q) ) { // no button pressed
        gw1.restore_all_attributes();
        break;
      }
      gw1.restore_all_attributes();
      gw2.restore_all_attributes();
} } }

void gw_branch_and_bound_tree_edit(
  GraphWin& gw1)
{ 
  graph& G1 = gw1.get_graph();
  tree T1(G1);

  graph G2;
  GraphWin gw2(G2,500,500,"Branch-and-Bound Tree Edit");
  gw2.display();
  gw2.message("Enter second tree. Press done when finished");
  gw2.edit();
  gw2.del_message();

  tree T2(G2);

  node_array<node> M(T1);
  branch_and_bound_tree_edit(T1,T2,M);

  gw1.save_all_attributes();
  int del = 0;
  int ins = T2.number_of_nodes();
  node v;
  forall_nodes(v,T1) {
    if ( M[v] == nil ) {
      del++;
    } else {
      ins--;
      gw1.set_color(v,blue);
      gw2.set_color(M[v],blue);
  } }
  gw1.message(string("Least-cost transformation makes %i node deletions and %i node insertions",del,ins));
  gw1.wait();
  gw1.del_message();
  gw1.restore_all_attributes();
}

void gw_divide_and_conquer_tree_edit(
  GraphWin& gw1)
{ 
  graph& G1 = gw1.get_graph();
  tree T1(G1);

  graph G2;
  GraphWin gw2(G2,500,500,"Divide-and-Conquer Tree Edit");
  gw2.display();
  gw2.message("Enter second tree. Press done when finished");
  gw2.edit();
  gw2.del_message();

  tree T2(G2);

  node_array<node> M(T1);
  int dist = divide_and_conquer_tree_edit(T1,T2,M);

  gw1.save_all_attributes();
  int del = 0;
  int ins = T2.number_of_nodes();
  node v;
  forall_nodes(v,T1) {
    if ( M[v] == nil ) {
      del++;
    } else {
      ins--;
      gw1.set_color(v,blue);
      gw2.set_color(M[v],blue);
  } }
  gw1.message(string("Least-cost transformation makes %i node deletions and %i node insertions",del,ins));
  gw1.wait();
  gw1.del_message();
  gw1.restore_all_attributes();
}

void gw_top_down_dynamic_programming_tree_edit(
  GraphWin& gw1)
{ 
  graph& G1 = gw1.get_graph();
  tree T1(G1);

  graph G2;
  GraphWin gw2(G2,500,500,"Top-Down Dynamic Programming Tree Edit");
  gw2.display();
  gw2.message("Enter second tree. Press done when finished");
  gw2.edit();
  gw2.del_message();

  tree T2(G2);

  node_array<node> M(T1);
  int dist = top_down_dynamic_programming_tree_edit(T1,T2,M);

  gw1.save_all_attributes();
  int del = 0;
  int ins = T2.number_of_nodes();
  node v;
  forall_nodes(v,T1) {
    if ( M[v] == nil ) {
      del++;
    } else {
      ins--;
      gw1.set_color(v,blue);
      gw2.set_color(M[v],blue);
  } }
  gw1.message(string("Least-cost transformation makes %i node deletions and %i node insertions",del,ins));
  gw1.wait();
  gw1.del_message();
  gw1.restore_all_attributes();
}

void gw_bottom_up_dynamic_programming_tree_edit(
  GraphWin& gw1)
{ 
  graph& G1 = gw1.get_graph();
  tree T1(G1);

  graph G2;
  GraphWin gw2(G2,500,500,"Bottom-Up Dynamic Programming Tree Edit");
  gw2.display();
  gw2.message("Enter second tree. Press done when finished");
  gw2.edit();
  gw2.del_message();

  tree T2(G2);

  node_array<node> M(T1);
  int dist = bottom_up_dynamic_programming_tree_edit(T1,T2,M);

  gw1.save_all_attributes();
  int del = 0;
  int ins = T2.number_of_nodes();
  node v;
  forall_nodes(v,T1) {
    if ( M[v] == nil ) {
      del++;
    } else {
      ins--;
      gw1.set_color(v,blue);
      gw2.set_color(M[v],blue);
  } }
  gw1.message(string("Least-cost transformation makes %i node deletions and %i node insertions",del,ins));
  gw1.wait();
  gw1.del_message();
  gw1.restore_all_attributes();
}


void subtree_size(
  const tree& T,
  const node v,
  node_array<int>& size)
{ 
  size[v] = 1;
  if ( !T.is_leaf(v) ) {
    node w;
    forall_children(w,v) {
      subtree_size(T,w,size);
      size[v] += size[w];
} } }

void double_check_preorder_tree_traversal(
  const tree& T,
  node_array<int>& order)
{ 
  node_array<int> size(T);
  subtree_size(T,T.root(),size);
  if ( order[T.root()] != 1 )
    error_handler(1,
      "Wrong implementation of preorder tree traversal");
  node v;
  forall_nodes(v,T) {
    if ( !T.is_leaf(v) && order[T.first_child(v)] != order[v] + 1 )
      error_handler(1,
        "Wrong implementation of preorder tree traversal");
    if ( !T.is_last_child(v) &&
        order[T.next_sibling(v)] != order[v] + size[v] )
      error_handler(1,
        "Wrong implementation of preorder tree traversal");
} }

void rec_preorder_tree_traversal(
  const tree& T,
  node_array<int>& order)
{ 
  int num = 1;
  rec_preorder_subtree_traversal(T,T.root(),order,num);
  double_check_preorder_tree_traversal(T,order);
}

void rec_preorder_subtree_traversal(
  const tree& T,
  const node v,
  node_array<int>& order,
  int& num)
{ 
  order[v] = num++; // visit node [[v]]
  node w;
  forall_children(w,v)
    rec_preorder_subtree_traversal(T,w,order,num);
}

void preorder_tree_traversal(
  const tree& T,
  node_array<int>& order)
{ 
  stack<node> S;
  node v,w;
  S.push(T.root());
  int num = 1;
  do {
    v = S.pop();
    order[v] = num++; // visit node [[v]]
    w = T.last_child(v);
    while ( w != nil ) {
      S.push(w);
      w = T.previous_sibling(w);
    }
  } while ( !S.empty() );
  double_check_preorder_tree_traversal(T,order);
}

void preorder_tree_depth(
  const tree& T,
  node_array<int>& depth,
  node& deepest)
{ 
  deepest = T.first_node();

  stack<node> S;
  node v,w;
  S.push(T.root());

  do {
    v = S.pop();

    if ( T.is_root(v) ) {
      depth[v] = 0;
    } else {
      depth[v] = depth[T.parent(v)] + 1;
      if ( depth[v] > depth[deepest] ) deepest = v;
    }

    w = T.last_child(v);
    while ( w != nil ) {
      S.push(w);
      w = T.previous_sibling(w);
    }
  } while ( !S.empty() );
}

void double_check_postorder_tree_traversal(
    const tree& T,
    node_array<int>& order)
{ 
  node_array<int> size(T);
  subtree_size(T,T.root(),size);
  if ( order[T.root()] != T.number_of_nodes() )
    error_handler(1,
      "Wrong implementation of postorder tree traversal");
  node v;
  forall_nodes(v,T) {
    if ( !T.is_leaf(v) && order[T.last_child(v)] != order[v] - 1 )
      error_handler(1,
        "Wrong implementation of postorder tree traversal");
    if ( !T.is_last_child(v) &&
        order[T.next_sibling(v)] !=
          order[v] + size[T.next_sibling(v)] )
      error_handler(1,
        "Wrong implementation of postorder tree traversal");
} }

void rec_postorder_tree_traversal(
  const tree& T,
  node_array<int>& order)
{ 
  int num = 1;
  rec_postorder_subtree_traversal(T,T.root(),order,num);
  double_check_postorder_tree_traversal(T,order);
}

void rec_postorder_subtree_traversal(
  const tree& T,
  const node v,
  node_array<int>& order,
  int& num)
{ 
  node w;
  forall_children(w,v)
    rec_postorder_subtree_traversal(T,w,order,num);
  order[v] = num++; // visit node [[v]]
}

void postorder_tree_traversal(
  const tree& T,
  node_array<int>& order)
{ 
  stack<node> S;
  node v,w;
  S.push(T.root());
  int num = 1;
  do {
    v = S.pop();
    order[v] = T.number_of_nodes() - num++ + 1; // visit node [[v]]
    forall_children(w,v)
      S.push(w);
  } while ( !S.empty() );
  double_check_postorder_tree_traversal(T,order);
}

void double_check_top_down_tree_traversal(
    const tree& T,
    node_array<int>& order)
{ 
    node_array<int> depth(T);
    node_array<int> rank(T);
  { stack<node> S;
    node v,w;
    S.push(T.root());
    int num = 1;
    do {
      v = S.pop();

      rank[v] = num++;
      if ( T.is_root(v) ) {
        depth[v] = 0;
      } else {
        depth[v] = depth[T.parent(v)] + 1;
      }

      w = T.last_child(v);
      while ( w != nil ) {
        S.push(w);
        w = T.previous_sibling(w);
      }
    } while ( !S.empty() );
    double_check_preorder_tree_traversal(T,rank);
  }


  node v,w;
  forall_nodes(v,T) {
    forall_nodes(w,T) {
      if ( order[v] < order[w] && depth[v] > depth[w] )
        error_handler(1,
          "Wrong implementation of top-down tree traversal");
      if ( depth[v] == depth[w] && rank[v] < rank[w] &&
           order[v] >= order[w] )
        error_handler(1,
          "Wrong implementation of top-down tree traversal");
} } }

void top_down_tree_traversal(
  const tree& T,
  node_array<int>& order)
{ 
  queue<node> Q;
  node v,w;
  Q.append(T.root());
  int num = 1;
  do {
    v = Q.pop();
    order[v] = num++; // visit node [[v]]
    forall_children(w,v)
      Q.append(w);
  } while ( !Q.empty() );
  double_check_top_down_tree_traversal(T,order);
}

void double_check_bottom_up_tree_traversal(
    const tree& T,
    node_array<int>& order)
{ 
    node_array<int> depth(T);
    node_array<int> rank(T);
  { stack<node> S;
    node v,w;
    S.push(T.root());
    int num = 1;
    do {
      v = S.pop();

      rank[v] = num++;
      if ( T.is_root(v) ) {
        depth[v] = 0;
      } else {
        depth[v] = depth[T.parent(v)] + 1;
      }

      w = T.last_child(v);
      while ( w != nil ) {
        S.push(w);
        w = T.previous_sibling(w);
      }
    } while ( !S.empty() );
    double_check_preorder_tree_traversal(T,rank);
  }

    node_array<int> height(T);
  { list<node> L;

    stack<node> S;
    node v,w;
    S.push(T.root());
    do {
      v = S.pop();
      L.push(v); // visit node [[v]]
      forall_children(w,v)
        S.push(w);
    } while ( !S.empty() );

    forall(v,L) {
      if ( T.is_leaf(v) ) {
        height[v] = 0;
      } else {
        forall_children(w,v)
          height[v] = leda_max(height[w],height[v]);
        height[v]++;
  } } }


  node v,w;
  forall_nodes(v,T) {
    forall_nodes(w,T) {
      if ( order[v] < order[w] && height[v] > height[w] )
        error_handler(1,
          "Wrong implementation of bottom-up tree traversal");
      if ( height[v] == height[w] && depth[v] < depth[w] &&
           order[v] >= order[w] )
        error_handler(1,
          "Wrong implementation of bottom-up tree traversal");
      if ( height[v] == height[w] && depth[v] == depth[w] && 
           rank[v] < rank[w] && order[v] > order[w] )
        error_handler(1,
          "Wrong implementation of bottom-up tree traversal");
} } }

void bottom_up_tree_traversal(
  const tree& T,
  node_array<int>& order)
{ 
  queue<node> Q;
  node_array<int> children(T,0);
  edge e;
  forall_edges(e,T)
    children[T.source(e)]++;
  node v,w;

  queue<node> R;
  R.append(T.root());
  do {
    v = R.pop();
    forall_children(w,v) {
      if ( T.is_leaf(w) ) {
        Q.append(w);
      } else {
        R.append(w);
    } }
  } while ( !R.empty() );


  int num = 1;
  do {
    v = Q.pop();
    order[v] = num++; // visit node [[v]]
    if ( !T.is_root(v) ) {
      children[T.parent(v)]--;
      if ( children[T.parent(v)] == 0 )
        Q.append(T.parent(v));
    }
  } while ( !Q.empty() );
  double_check_bottom_up_tree_traversal(T,order);
}

void bottom_up_tree_height(
  const tree& T,
  node_array<int>& height)
{ 
  queue<node> Q;
  node_array<int> children(T,0);
  edge e;
  forall_edges(e,T)
    children[T.source(e)]++;
  node v,w;
  forall_nodes(v,T)
    if ( children[v] == 0 )
      Q.append(v);
  do {
    v = Q.pop();

    if ( T.is_leaf(v) ) {
      height[v] = 0;
    } else {
      forall_children(w,v)
        height[v] = leda_max(height[w],height[v]);
      height[v]++;
    }

    if ( !T.is_root(v) ) {
      w = T.parent(v);
      children[w]--;
      if ( children[w] == 0 )
        Q.append(w);
    }
  } while ( !Q.empty() );
}

void layered_tree_layout(
    const tree& T,
    node_array<double>& x,
    node_array<double>& y)
{ 
  node_array<int> depth(T);
  { stack<node> S;
    node v,w;
    S.push(T.root());
    do {
      v = S.pop();

      if ( T.is_root(v) ) {
        depth[v] = 0;
      } else {
        depth[v] = depth[T.parent(v)] + 1;
      }

      w = T.last_child(v);
      while ( w != nil ) {
        S.push(w);
        w = T.previous_sibling(w);
      }
    } while ( !S.empty() );
  }

  node_array<int> breadth(T);
  { queue<node> Q;
    node_array<int> children(T,0);
    edge e;
    forall_edges(e,T)
      children[T.source(e)]++;
    node v;
    forall_nodes(v,T)
      if ( children[v] == 0 )
        Q.append(v);
    do {
      v = Q.pop();
      if ( T.is_leaf(v) )
        breadth[v] = 1;
      if ( !T.is_root(v) ) {
        breadth[T.parent(v)] += breadth[v];
        children[T.parent(v)]--;
        if ( children[T.parent(v)] == 0 )
          Q.append(T.parent(v));
      }
    } while ( !Q.empty() );
  }

  { queue<node> Q;
    node v,w;
    Q.append(T.root());
    do {
      v = Q.pop();

      if ( T.is_root(v) ) {
        x[v] = 0;
      } else {
        if ( T.is_first_child(v) ) {
          x[v] = x[T.parent(v)];
        } else {
          x[v] = x[T.previous_sibling(v)]
               + breadth[T.previous_sibling(v)];
      } }

      y[v] = - depth[v];

      forall_children(w,v)
        Q.append(w);
    } while ( !Q.empty() );
  }

  { queue<node> Q;
    node_array<int> children(T,0);
    edge e;
    forall_edges(e,T)
      children[T.source(e)]++;
    node v;
    forall_nodes(v,T)
      if ( children[v] == 0 )
        Q.append(v);
    do {
      v = Q.pop();

      if ( !T.is_leaf(v) )
        x[v] = ( x[T.first_child(v)] + x[T.last_child(v)] ) / 2;

      if ( !T.is_root(v) ) {
        children[T.parent(v)]--;
        if ( children[T.parent(v)] == 0 )
          Q.append(T.parent(v));
      }
    } while ( !Q.empty() );
  }

}


void gw_rec_preorder_tree_traversal(
  GraphWin& gw)
{ 
  graph& G = gw.get_graph();
  tree T(G);
  node_array<int> order(T);
  rec_preorder_tree_traversal(T,order);
  { gw.save_all_attributes();

    node v;
    forall_nodes(v,T)
      gw.set_label(v,string("%i",order[v]));

    int n = T.number_of_nodes();
    array<node> disorder(1,n);
    forall_nodes(v,T)
      disorder[order[v]] = v;

    for ( int i = 1; i <= n; i++ ) {
      gw.set_color(disorder[i],blue);
      leda_wait(0.5);
    }

    gw.wait();
    gw.restore_all_attributes();
  }

}

void gw_preorder_tree_traversal(
  GraphWin& gw)
{ 
  graph& G = gw.get_graph();
  tree T(G);
  node_array<int> order(T);
  preorder_tree_traversal(T,order);
  { gw.save_all_attributes();

    node v;
    forall_nodes(v,T)
      gw.set_label(v,string("%i",order[v]));

    int n = T.number_of_nodes();
    array<node> disorder(1,n);
    forall_nodes(v,T)
      disorder[order[v]] = v;

    for ( int i = 1; i <= n; i++ ) {
      gw.set_color(disorder[i],blue);
      leda_wait(0.5);
    }

    gw.wait();
    gw.restore_all_attributes();
  }

}

void gw_preorder_tree_depth(
  GraphWin& gw)
{ 
  graph& G = gw.get_graph();
  tree T(G);
  node_array<int> depth(T);
  node deepest;
  preorder_tree_depth(T,depth,deepest);

  gw.save_all_attributes();
  node v;
  forall_nodes(v,T)
    gw.set_label(v,string("%i",depth[v]));

  panel P;
  make_proof_panel(P,
    string("The depth of the tree is %i",depth[deepest]),true);
  if ( gw.open_panel(P) ) { // proof button pressed
    gw.set_color(deepest,blue);
    int num = depth[deepest];
    edge e;
    while ( !T.is_root(deepest) ) {
      e = T.first_in_edge(deepest);
      v = T.source(e);
      gw.set_color(v,blue);
      gw.set_color(e,blue);
      gw.set_label(e,string("%i",num--));
      gw.set_width(e,2);
      deepest = v;
    }
    gw.wait();
  }
  gw.restore_all_attributes();
}

void gw_rec_postorder_tree_traversal(
  GraphWin& gw)
{ 
  graph& G = gw.get_graph();
  tree T(G);
  node_array<int> order(T);
  rec_postorder_tree_traversal(T,order);
  { gw.save_all_attributes();

    node v;
    forall_nodes(v,T)
      gw.set_label(v,string("%i",order[v]));

    int n = T.number_of_nodes();
    array<node> disorder(1,n);
    forall_nodes(v,T)
      disorder[order[v]] = v;

    for ( int i = 1; i <= n; i++ ) {
      gw.set_color(disorder[i],blue);
      leda_wait(0.5);
    }

    gw.wait();
    gw.restore_all_attributes();
  }

}

void gw_postorder_tree_traversal(
  GraphWin& gw)
{ 
  graph& G = gw.get_graph();
  tree T(G);
  node_array<int> order(T);
  postorder_tree_traversal(T,order);
  { gw.save_all_attributes();

    node v;
    forall_nodes(v,T)
      gw.set_label(v,string("%i",order[v]));

    int n = T.number_of_nodes();
    array<node> disorder(1,n);
    forall_nodes(v,T)
      disorder[order[v]] = v;

    for ( int i = 1; i <= n; i++ ) {
      gw.set_color(disorder[i],blue);
      leda_wait(0.5);
    }

    gw.wait();
    gw.restore_all_attributes();
  }

}

void gw_top_down_tree_traversal(
  GraphWin& gw)
{ 
  graph& G = gw.get_graph();
  tree T(G);
  node_array<int> order(T);
  top_down_tree_traversal(T,order);
  { gw.save_all_attributes();

    node v;
    forall_nodes(v,T)
      gw.set_label(v,string("%i",order[v]));

    int n = T.number_of_nodes();
    array<node> disorder(1,n);
    forall_nodes(v,T)
      disorder[order[v]] = v;

    for ( int i = 1; i <= n; i++ ) {
      gw.set_color(disorder[i],blue);
      leda_wait(0.5);
    }

    gw.wait();
    gw.restore_all_attributes();
  }

}

void gw_bottom_up_tree_traversal(
  GraphWin& gw)
{ 
  graph& G = gw.get_graph();
  tree T(G);
  node_array<int> order(T);
  bottom_up_tree_traversal(T,order);
  { gw.save_all_attributes();

    node v;
    forall_nodes(v,T)
      gw.set_label(v,string("%i",order[v]));

    int n = T.number_of_nodes();
    array<node> disorder(1,n);
    forall_nodes(v,T)
      disorder[order[v]] = v;

    for ( int i = 1; i <= n; i++ ) {
      gw.set_color(disorder[i],blue);
      leda_wait(0.5);
    }

    gw.wait();
    gw.restore_all_attributes();
  }

}

void gw_bottom_up_tree_height(
  GraphWin& gw)
{ 
  graph& G = gw.get_graph();
  tree T(G);
  node_array<int> height(T);
  bottom_up_tree_height(T,height);

  gw.save_all_attributes();
  node v;
  forall_nodes(v,T)
    gw.set_label(v,string("%i",height[v]));
  node r = T.root();

  panel P;
  make_proof_panel(P,string("The height of the tree is %i",height[r]),true);
  if ( gw.open_panel(P) ) { // proof button pressed
    gw.set_color(r,blue);

    node v = r;
    node w;
    edge e,aux;
    int num = 1;
    while ( !T.is_leaf(v) ) {
      forall_out_edges(aux,v) {
        w = T.opposite(v,aux);
        if ( height[w] == height[v] - 1 ) {
          v = w;
          e = aux;
          break;
      } }
      gw.set_color(v,blue);
      gw.set_color(e,blue);
      gw.set_label(e,string("%i",num++));
      gw.set_width(e,2);
    }
    gw.wait();
  }
  gw.restore_all_attributes();
}

void gw_layered_tree_layout(
  GraphWin& gw)
{ 
  graph& G = gw.get_graph();
  tree T(G);

  node_array<double> xcoord(T);
  node_array<double> ycoord(T);
  layered_tree_layout(T,xcoord,ycoord);
  gw.adjust_coords_to_win(xcoord,ycoord);
  gw.set_layout(xcoord,ycoord);
  gw.display();
}


bool simple_ordered_tree_isomorphism(
  const TREE<string,string>& T1,
  const TREE<string,string>& T2,
  node_array<node>& M)
{ 
  int n = T2.number_of_nodes();
  if ( T1.number_of_nodes() != n ) return false;

  node_array<int> order1(T1);
  node_array<int> order2(T2);
  top_down_tree_traversal(T1,order1);
  top_down_tree_traversal(T2,order2);

  array<node> disorder2(1,n);
  node v,w;
  forall_nodes(v,T2)
    disorder2[order2[v]] = v;
  forall_nodes(v,T1)
    M[v] = disorder2[order1[v]];

  forall_nodes(v,T1) {
    w = M[v];
    if ( T1[v] != T2[w] ) return false;
    if ( !T1.is_leaf(v) &&
         ( T2.is_leaf(w) ||
           ( M[T1.first_child(v)] != T2.first_child(w) ) ) )
      return false;
    if ( !T1.is_last_child(v) &&
         ( T2.is_last_child(w) ||
           ( M[T1.next_sibling(v)] != T2.next_sibling(w) ) ) )
      return false;
  }
  return true;
}

bool ordered_tree_isomorphism(
  const TREE<string,string>& T1,
  const TREE<string,string>& T2,
  node_array<node>& M)
{ 
  if ( T1.number_of_nodes() != T2.number_of_nodes() ) return false;
  if ( map_ordered_tree(T1,T1.root(),T2,T2.root(),M) ) {
    { node v;
      forall_nodes(v,T1) {
        if ( M[v] == nil || T1[v] != T2[M[v]] )
          error_handler(1,
            "Wrong implementation of ordered tree isomorphism");
        if ( !T1.is_leaf(v) &&
             ( T2.is_leaf(M[v]) ||
               ( M[T1.first_child(v)] != T2.first_child(M[v]) ) ) )
          error_handler(1,
            "Wrong implementation of ordered tree isomorphism");
        if ( !T1.is_last_child(v) &&
             ( T2.is_last_child(M[v]) ||
               ( M[T1.next_sibling(v)] != T2.next_sibling(M[v]) ) ) )
          error_handler(1,
            "Wrong implementation of ordered tree isomorphism");
    } }

    return true;
  }
  return false;
}

bool map_ordered_tree(
  const TREE<string,string>& T1,
  const node r1,
  const TREE<string,string>& T2,
  const node r2,
  node_array<node>& M)
{ 
  if ( T1[r1] != T2[r2] ) return false;
  M[r1] = r2;
  int d1 = T1.number_of_children(r1);
  int d2 = T2.number_of_children(r2);
  if ( d1 != d2 ) return false;
  node v1,v2;
  if ( !T1.is_leaf(r1) ) {
    v1 = T1.first_child(r1);
    v2 = T2.first_child(r2);
    if ( !map_ordered_tree(T1,v1,T2,v2,M) ) return false;
    for ( int i = 2; i <= d1; i++ ) {
      v1 = T1.next_sibling(v1);
      v2 = T2.next_sibling(v2);
      if ( !map_ordered_tree(T1,v1,T2,v2,M) ) return false;
  } }
  return true;
}

bool unordered_tree_isomorphism(
  const TREE<string,string>& T1,
  const TREE<string,string>& T2,
  node_array<node>& M)
{ 
  if ( T1.number_of_nodes() != T2.number_of_nodes() ) return false;

  node_array<list<int> > code1(T1);
  node_array<list<int> > code2(T2);

  { node v,w;
    array<int> A;
    list<array<int> > L;
    int code;

    list<node> L1;
    postorder_tree_list_traversal(T1,L1);

    forall(v,L1) {
      if ( T1.is_leaf(v) ) {
        code1[v].append(1);
      } else {
        L.clear();
        code = 1;
        forall_children(w,v) {
          code += code1[w].head();
          list_to_array(code1[w],A);
          L.append(A);
        }
        radix_sort(L);
        code1[v].append(code);
        forall(A,L)
          forall(code,A)
            code1[v].append(code);
    } }

    list<node> L2;
    postorder_tree_list_traversal(T2,L2);

    forall(v,L2) {
      if ( T2.is_leaf(v) ) {
        code2[v].append(1);
      } else {
        L.clear();
        code = 1;
        forall_children(w,v) {
          code += code2[w].head();
          list_to_array(code2[w],A);
          L.append(A);
        }
        radix_sort(L);
        code2[v].append(code);
        forall(A,L)
          forall(code,A)
            code2[v].append(code);
  } } }


  if ( code1[T1.root()] == code2[T2.root()] ) {
    { node v = T1.root();
      node w = T2.root();
      M[v] = w;
      node_array<bool> mapped_to(T2,false);
      list<node> L1;
      preorder_tree_list_traversal(T1,L1);
      L1.pop(); // node [[v]] already mapped
      forall(v,L1) {
        forall_children(w,M[T1.parent(v)]) {
          if ( code1[v] == code2[w] && !mapped_to[w] &&
               ( T2.is_root(w) || M[T1.parent(v)] == T2.parent(w) ) ) {
            M[v] = w;
            mapped_to[w] = true;
            break;
    } } } }

    { node v;
      forall_nodes(v,T1) {
        if ( M[v] == nil || T1[v] != T2[M[v]] )
          error_handler(1,
            "Wrong implementation of unordered tree isomorphism");
        if ( !T1.is_root(v) &&
             ( T2.is_root(M[v]) ||
               ( M[T1.parent(v)] != T2.parent(M[v]) ) ) )
          error_handler(1,
            "Wrong implementation of unordered tree isomorphism");
    } }

    return true;
  } else {
    return false; // [[T1]] and [[T2]] are not isomorphic
} }

void postorder_tree_list_traversal(
  const TREE<string,string>& T,
  list<node>& L)
{ 
  L.clear();
  stack<node> S;
  S.push(T.root());
  node v,w;
  do {
    v = S.pop();
    L.push(v);
    forall_children(w,v)
      S.push(w);
  } while ( !S.empty() );
}

void preorder_tree_list_traversal(
  const TREE<string,string>& T,
  list<node>& L)
{ 
  L.clear();
  stack<node> S;
  S.push(T.root());
  node v,w;
  do {
    v = S.pop();
    L.push(v);
    w = T.last_child(v);
    while ( w != nil ) {
      S.push(w);
      w = T.previous_sibling(w);
    }
  } while ( !S.empty() );
  L.reverse();
}

template<class E>
void list_to_array(
  const list<E>& L,
  array<E>& A)
{ 
  A.resize(1,L.length());
  int i = 1;
  E x;
  forall(x,L) A[i++] = x;
}

int compare(
  const list<int>& L1,
  const list<int>& L2)
{ 
  list_item p = L1.first();
  list_item q = L2.first();
  while ( p != nil && q != nil ) {
    if ( L1[p] < L2[q] ) return -1;
    if ( L1[p] > L2[q] ) return 1;
    p = L1.succ(p);
    q = L2.succ(q);
  }
  if ( q != nil ) return -1; // p == nil && q != nil
  if ( p != nil ) return 1;  // p != nil && q == nil
  return 0;                  // p == nil && q == nil
}

bool map_ordered_subtree(
  const TREE<string,string>& T1,
  const node r1,
  const TREE<string,string>& T2,
  const node r2,
  node_array<node>& M)
{ 
  if ( T1[r1] != T2[r2] ) return false;
  M[r1] = r2;
  int d1 = T1.number_of_children(r1);
  int d2 = T2.number_of_children(r2);
  if ( d1 > d2 ) return false;
  node v1,v2;
  if ( !T1.is_leaf(r1) ) {
    v1 = T1.first_child(r1);
    v2 = T2.first_child(r2);
    if ( !map_ordered_subtree(T1,v1,T2,v2,M) ) return false;
    for ( int i = 2; i <= d1; i++ ) {
      v1 = T1.next_sibling(v1);
      v2 = T2.next_sibling(v2);
      if ( !map_ordered_subtree(T1,v1,T2,v2,M) ) return false;
  } }
  return true;
}

bool top_down_ordered_subtree_isomorphism(
  const TREE<string,string>& T1,
  const TREE<string,string>& T2,
  node_array<node>& M)
{ 
  if ( T1.number_of_nodes() > T2.number_of_nodes() ) return false;
  if ( map_ordered_subtree(T1,T1.root(),T2,T2.root(),M) ) {
    { node v;
      if ( M[T1.root()] != T2.root() )
        error_handler(1,
          "Wrong implementation of top-down subtree isomorphism");
      forall_nodes(v,T1) {
        if ( M[v] == nil || T1[v] != T2[M[v]] )
          error_handler(1,
            "Wrong implementation of top-down subtree isomorphism");
        if ( !T1.is_leaf(v) &&
             ( T2.is_leaf(M[v]) ||
               ( M[T1.first_child(v)] != T2.first_child(M[v]) ) ) )
          error_handler(1,
            "Wrong implementation of top-down subtree isomorphism");
        if ( !T1.is_last_child(v) &&
             ( T2.is_last_child(M[v]) ||
               ( M[T1.next_sibling(v)] != T2.next_sibling(M[v]) ) ) )
          error_handler(1,
            "Wrong implementation of top-down subtree isomorphism");
    } }

    return true;
  } 
  return false;
}

bool top_down_unordered_subtree_isomorphism(
  const TREE<string,string>& T1,
  node r1,
  node_array<int> height1,
  node_array<int> size1,
  const TREE<string,string>& T2,
  node r2,
  node_array<int> height2,
  node_array<int> size2,
  node_array<set<node> >& B)
{ 
  if ( T1[r1] != T2[r2] ) return false;

  if ( T1.is_leaf(r1) ) return true;

  int p = T1.number_of_children(r1);
  int q = T2.number_of_children(r2);

  if ( p > q || height1[r1] > height2[r2] || size1[r1] > size2[r2] )
    return false;

  node_map<node> T1G(T1);
  node_map<node> T2G(T2);
  graph G;
  node_map<node> GT(G);

  node v1,v2;
  forall_children(v1,r1) {
    node v = G.new_node();
    GT[v] = v1;
    T1G[v1] = v;
  }
  forall_children(v2,r2) {
    node w = G.new_node();
    GT[w] = v2;
    T2G[v2] = w;
  }
  forall_children(v1,r1) {
    forall_children(v2,r2) {
      if ( top_down_unordered_subtree_isomorphism
             (T1,v1,height1,size1,T2,v2,height2,size2,B) )
        G.new_edge(T1G[v1],T2G[v2]);
  } }

  list<edge> L = MAX_CARD_BIPARTITE_MATCHING(G);

  if ( L.length() == p ) {
    edge e;
    forall(e,L)
      B[GT[G.source(e)]].insert(GT[G.target(e)]);
    return true;
  } else {
    return false;
} }

bool top_down_unordered_subtree_isomorphism(
  const TREE<string,string>& T1,
  const TREE<string,string>& T2,
  node_array<node>& M)
{ 
  node r1 = T1.root();
  node r2 = T2.root();
  node_array<int> height1(T1);
  node_array<int> size1(T1);
  { node v,w;
    list<node> L;
    postorder_tree_list_traversal(T1,L);
    forall(v,L) {
      height1[v] = 0; // leaves have height equal to zero
      size1[v] = 1;   // and size equal to one
      if ( !T1.is_leaf(v) ) {
        forall_children(w,v) {
          height1[v] = leda_max(height1[v],height1[w]);
          size1[v] += size1[w];
        }
        height1[v]++; // one plus the largest height among the children
  } } }

  node_array<int> height2(T2);
  node_array<int> size2(T2);
  { node v,w;
    list<node> L;
    postorder_tree_list_traversal(T2,L);
    forall(v,L) {
      height2[v] = 0; // leaves have height equal to zero
      size2[v] = 1;   // and size equal to one
      if ( !T2.is_leaf(v) ) {
        forall_children(w,v) {
          height2[v] = leda_max(height2[v],height2[w]);
          size2[v] += size2[w];
        }
        height2[v]++; // one plus the largest height among the children
  } } }

  node_array<set<node> > B(T1);
  bool isomorph = top_down_unordered_subtree_isomorphism
                    (T1,r1,height1,size1,T2,r2,height2,size2,B);
  if ( isomorph ) {
    M[r1] = r2;
    { node v,w;
      list<node> L;
      preorder_tree_list_traversal(T1,L);
      forall(v,L) {
        if ( !T1.is_root(v) ) {
          forall(w,B[v]) {
            if ( M[T1.parent(v)] == T2.parent(w) ) {
              M[v] = w;
              break;
    } } } } }

    { node v,w;
      if ( M[T1.root()] != T2.root() )
        error_handler(1,
          "Wrong implementation of top-down subtree isomorphism");
      forall_nodes(v,T1) {
        if ( M[v] == nil || T1[v] != T2[M[v]] )
          error_handler(1,
            "Wrong implementation of top-down subtree isomorphism");
        if ( !T1.is_root(v) &&
             ( T2.is_root(M[v]) ||
               ( M[T1.parent(v)] != T2.parent(M[v]) ) ) )
          error_handler(1,
            "Wrong implementation of top-down subtree isomorphism");
        forall_nodes(w,T1) {
          if ( v != w && M[v] == M[w] )
            error_handler(1,
              "Wrong implementation of top-down subtree isomorphism");
    } } }

  }
  return isomorph;
}

void bottom_up_ordered_subtree_isomorphism(
  const TREE<string,string>& T1,
  const TREE<string,string>& T2,
  list<node_array<node> >& L)
{ 
  node r1 = T1.root();

  int h1;
  { node_array<int> height(T1);
    bottom_up_tree_height(T1,height);
    h1 = height[r1];
  }

  node_array<int> height2(T2);
  node_array<int> size2(T2);
  { node v,w;
    list<node> L;
    postorder_tree_list_traversal(T2,L);
    forall(v,L) {
      height2[v] = 0; // leaves have height equal to zero
      size2[v] = 1;   // and size equal to one
      if ( !T2.is_leaf(v) ) {
        forall_children(w,v) {
          height2[v] = leda_max(height2[v],height2[w]);
          size2[v] += size2[w];
        }
        height2[v]++; // one plus the largest height among the children
  } } }


  node_array<node> M(T1);
  L.clear();

  node v;
  forall_nodes(v,T2) {
    if ( height2[v] == h1 && size2[v] == T1.number_of_nodes() ) {
      if ( map_ordered_subtree(T1,r1,T2,v,M) ) {
        { node v;
          forall_nodes(v,T1) {
            if ( M[v] == nil || T1[v] != T2[M[v]] )
              error_handler(1,
                "Wrong implementation of bottom-up subtree isomorphism");
            if ( !T1.is_leaf(v) &&
                 ( T2.is_leaf(M[v]) ||
                   ( M[T1.first_child(v)] != T2.first_child(M[v]) ) ) )
              error_handler(1,
                "Wrong implementation of bottom-up subtree isomorphism");
            if ( !T1.is_last_child(v) &&
                 ( T2.is_last_child(M[v]) ||
                   ( M[T1.next_sibling(v)] != T2.next_sibling(M[v]) ) ) )
              error_handler(1,
                "Wrong implementation of bottom-up subtree isomorphism");
            if ( T1.is_leaf(v) && !T2.is_leaf(M[v]) )
              error_handler(1,
                "Wrong implementation of bottom-up subtree isomorphism");
        } }

        L.append(M);
} } } }

void bottom_up_unordered_subtree_isomorphism(
  const TREE<string,string>& T1,
  const TREE<string,string>& T2,
  node_array<int>& code1,
  node_array<int>& code2,
  list<node_array<node> >& L)
{ 
  list<node> L1;
  postorder_tree_list_traversal(T1,L1);

  list<node> L2;
  postorder_tree_list_traversal(T2,L2);

  int num = 1;
  h_array<list<int>,int> CODE;

  bool unordered = true;
  { list<int> L;
    node v,w;
    forall(v,L1) {
      if ( T1.is_leaf(v) ) {
        code1[v] = 1;
      } else {

        L.clear();
        forall_children(w,v)
          L.append(code1[w]);
        if ( unordered ) sort_isomorphism_codes(L);

        if ( CODE.defined(L) ) {
          code1[v] = CODE[L];
        } else {
          CODE[L] = ++num;
          code1[v] = num;
  } } } }

  { list<int> L;
    node v,w;
    forall(v,L2) {
      if ( T2.is_leaf(v) ) {
        code2[v] = 1;
      } else {

        L.clear();
        forall_children(w,v)
          L.append(code2[w]);
        if ( unordered ) sort_isomorphism_codes(L);

        if ( CODE.defined(L) ) {
          code2[v] = CODE[L];
        } else {
          CODE[L] = ++num;
          code2[v] = num;
  } } } }


  node r1 = T1.root();
  L.clear();
  node_array<node> M(T1);
  node v;
  forall(v,L2) {
    if ( code1[r1] == code2[v] ) {
      M[r1] = v;
      map_unordered_subtree(T1,r1,T2,v,code1,code2,M);

      L.append(M);
} } }

void sort_isomorphism_codes(
  list<int>& L)
{ 
  int x,y,z;
  switch ( L.length() ) {
    case 0:
      break;
    case 1:
      break;
    case 2:
      x = L.pop();
      y = L.pop();
      if ( x <= y ) { L.append(x); L.append(y); }
      else { L.append(y); L.append(x); }
      break;
    case 3:
      x = L.pop();
      y = L.pop();
      z = L.pop();
      if ( x <= y ) {
        if ( y <= z ) { L.append(x); L.append(y); L.append(z); }
        else {
          if ( x <= z ) { L.append(x); L.append(z); L.append(y); }
          else { L.append(z); L.append(x); L.append(y); }
        }
      } else {
        if ( y <= z ) {
          if ( x <= z ) { L.append(y); L.append(x); L.append(z); }
          else { L.append(y); L.append(z); L.append(x); }
        } else { L.append(z); L.append(y); L.append(x); }
      }
      break;
    default:
      L.bucket_sort(id);
      break;
} }

void map_unordered_subtree(
  const TREE<string,string>& T1,
  const node r1,
  const TREE<string,string>& T2,
  const node r2,
  const node_array<int>& code1,
  const node_array<int>& code2,
  node_array<node>& M)
{ 
  node v,w;
  list<node> L2;
  forall_children(w,r2) L2.append(w);
  forall_children(v,r1) {
    list_item it;
    forall_items(it,L2) {
      w = L2[it];
      if ( code1[v] == code2[w] ) {
        M[v] = w;
        L2.del(it); // node [[w]] already mapped to
        map_unordered_subtree(T1,v,T2,w,code1,code2,M);
        break;
} } } }

namespace leda {
int Hash(
  const list<int>& L)
{
  int sum = 0;
  int x;
  forall(x,L) sum += x;
  return sum;
}
}

inline int id(
  const int& x)
{ 
  return x;
}

void top_down_ordered_max_common_subtree_isomorphism(
  const TREE<string,string>& T1,
  const TREE<string,string>& T2,
  node_array<node>& M)
{ 
  if ( map_ordered_common_subtree(T1,T1.root(),T2,T2.root(),M) ) {
    { node v;
      if ( M[T1.root()] != T2.root() )
        error_handler(1,
          "Wrong implementation of common subtree isomorphism");
      forall_nodes(v,T1) {
        if ( M[v] != nil && T1[v] != T2[M[v]] )
          error_handler(1,
            "Wrong implementation of common subtree isomorphism");
        if ( M[v] != nil &&
             !T1.is_leaf(v) &&
             !T2.is_leaf(M[v]) &&
             M[T1.first_child(v)] != nil &&
             M[T1.first_child(v)] != T2.first_child(M[v]) )
          error_handler(1,
            "Wrong implementation of common subtree isomorphism");
        if ( M[v] != nil &&
             !T1.is_last_child(v) &&
             !T2.is_last_child(M[v]) &&
             M[T1.next_sibling(v)] != nil &&
             M[T1.next_sibling(v)] != T2.next_sibling(M[v]) )
          error_handler(1,
            "Wrong implementation of common subtree isomorphism");
    } }

    { node x;
      forall_nodes(x,T1) {
        if ( M[x] != nil ) {
          if ( !T1.is_leaf(x) &&
               M[T1.first_child(x)] == nil &&
               !T2.is_leaf(M[x]) &&
               T1[T1.first_child(x)] == T2[T2.first_child(M[x])] )
            error_handler(1,
              "Wrong implementation of common subtree isomorphism");
          if ( !T1.is_last_child(x) &&
               M[T1.next_sibling(x)] == nil &&
               !T2.is_last_child(M[x]) &&
               T1[T1.next_sibling(x)] == T2[T2.next_sibling(M[x])] )
            error_handler(1,
              "Wrong implementation of common subtree isomorphism");
    } } }

} }

bool map_ordered_common_subtree(
  const TREE<string,string>& T1,
  const node r1,
  const TREE<string,string>& T2,
  const node r2,
  node_array<node>& M)
{ 
  if ( T1[r1] != T2[r2] ) {
    return false;
  } else {
    M[r1] = r2;
    if ( !T1.is_leaf(r1) && !T2.is_leaf(r2) ) {
      node v = T1.first_child(r1);
      node w = T2.first_child(r2);
      bool res = true;
      while ( res ) {
        res = map_ordered_common_subtree(T1,v,T2,w,M);
        if ( T1.is_last_child(v) ) break;
        v = T1.next_sibling(v);
        if ( T2.is_last_child(w) ) break;
        w = T2.next_sibling(w);
    } }
    return true;
} }

int top_down_unordered_max_common_subtree_isomorphism(
  const TREE<string,string>& T1,
  node r1,
  const TREE<string,string>& T2,
  node r2,
  node_array<set<node> >& B)
{ 
  if ( T1[r1] != T2[r2] ) return 0;

  if ( T1.is_leaf(r1) || T2.is_leaf(r2) ) return T1[r1] == T2[r2];

  int p = T1.number_of_children(r1);
  int q = T2.number_of_children(r2);

  node_map<node> T1G(T1);
  node_map<node> T2G(T2);
  graph G;
  node_map<node> GT(G);
  edge_map<int> WEIGHT(G);

  list<node> U;
  node v1,v2;
  forall_children(v1,r1) {
    node v = G.new_node();
    U.append(v);
    GT[v] = v1;
    T1G[v1] = v;
  }
  list<node> W;
  forall_children(v2,r2) {
    node w = G.new_node();
    W.append(w);
    GT[w] = v2;
    T2G[v2] = w;
  }
  edge e;
  forall_children(v1,r1) {
    forall_children(v2,r2) {
      int res = top_down_unordered_max_common_subtree_isomorphism
                (T1,v1,T2,v2,B);
      if ( res != 0 ) {
        e = G.new_edge(T1G[v1],T2G[v2]);
        WEIGHT[e] = res;
  } } }

  node_array<int> POT(G);
  list<edge> L = MAX_WEIGHT_BIPARTITE_MATCHING(G,U,W,WEIGHT,POT);

  int res = 1; // mapping of [[r1]] to [[r2]]
  forall(e,L) {
    B[GT[G.source(e)]].insert(GT[G.target(e)]);
    res += WEIGHT[e];
  }
  return res;

}

void top_down_unordered_max_common_subtree_isomorphism(
  const TREE<string,string>& T1,
  const TREE<string,string>& T2,
  node_array<node>& M)
{ 
  node r1 = T1.root();
  node r2 = T2.root();
  if ( T1[r1] == T2[r2] ) {
    node_array<set<node> > B(T1);
    B[r1].insert(r2);
    top_down_unordered_max_common_subtree_isomorphism(T1,r1,T2,r2,B);
    M[r1] = r2;
    { node v,w;
      list<node> L;
      preorder_tree_list_traversal(T1,L);
      L.pop(); // node [[r1]] already mapped
      forall(v,L) {
        forall(w,B[v]) {
          if ( M[T1.parent(v)] == T2.parent(w) ) {
            M[v] = w;
            break;
    } } } }

    { node v,w;
      if ( M[T1.root()] != T2.root() )
        error_handler(1,
          "Wrong implementation of common subtree isomorphism");
      forall_nodes(v,T1) {
        if ( M[v] != nil ) {
          if ( T1[v] != T2[M[v]] )
            error_handler(1,
              "Wrong implementation of common subtree isomorphism");
          if ( !T1.is_root(v) &&
               !T2.is_root(M[v]) &&
               M[T1.parent(v)] != T2.parent(M[v]) )
            error_handler(1,
              "Wrong implementation of common subtree isomorphism");
          forall_nodes(w,T1) {
            if ( v != w && M[v] == M[w] )
              error_handler(1,
                "Wrong implementation of common subtree isomorphism");
    } } } }

} }

void bottom_up_ordered_max_common_subtree_isomorphism(
  const TREE<string,string>& T1,
  const TREE<string,string>& T2,
  node_array<int>& code1,
  node_array<int>& code2,
  node_array<node>& M)
{ 
  list<node> L1;
  postorder_tree_list_traversal(T1,L1);

  list<node> L2;
  postorder_tree_list_traversal(T2,L2);

  int num = 1;
  h_array<list<int>,int> CODE;

  bool unordered = false;
  { list<int> L;
    node v,w;
    forall(v,L1) {
      if ( T1.is_leaf(v) ) {
        code1[v] = 1;
      } else {

        L.clear();
        forall_children(w,v)
          L.append(code1[w]);
        if ( unordered ) sort_isomorphism_codes(L);

        if ( CODE.defined(L) ) {
          code1[v] = CODE[L];
        } else {
          CODE[L] = ++num;
          code1[v] = num;
  } } } }

  { list<int> L;
    node v,w;
    forall(v,L2) {
      if ( T2.is_leaf(v) ) {
        code2[v] = 1;
      } else {

        L.clear();
        forall_children(w,v)
          L.append(code2[w]);
        if ( unordered ) sort_isomorphism_codes(L);

        if ( CODE.defined(L) ) {
          code2[v] = CODE[L];
        } else {
          CODE[L] = ++num;
          code2[v] = num;
  } } } }


  node v,w;
  node_array<int> size1(T1);
  node_array<int> size2(T2);
  { node_pq<two_tuple<int,int> > Q1(T1);
    node_pq<two_tuple<int,int> > Q2(T2);

    forall(v,L1) {
      size1[v] = 1; // leaves have size equal to one
      if ( !T1.is_leaf(v) ) {
        forall_children(w,v) {
          size1[v] += size1[w];
      } }
      two_tuple<int,int> prio(-size1[v],code1[v]);
      Q1.insert(v,prio);
    }

    forall(v,L2) {
      size2[v] = 1; // leaves have size equal to one
      if ( !T2.is_leaf(v) ) {
        forall_children(w,v) {
          size2[v] += size2[w];
      } }
      two_tuple<int,int> prio(-size2[v],code2[v]);
      Q2.insert(v,prio);
    }

    while ( !Q1.empty() && !Q2.empty() ) {
      v = Q1.find_min();
      w = Q2.find_min();
      if ( code1[v] == code2[w] ) break;
      if ( Q1.prio(v) < Q2.prio(w) ) {
        Q1.del_min();
      } else {
        Q2.del_min();
    } }

    M[v] = w;

  }

  map_ordered_subtree(T1,v,T2,w,M);
  { node v,w;
    forall_nodes(v,T1) {
      if ( M[v] != nil &&
           !T1.is_leaf(v) &&
           !T2.is_leaf(M[v]) &&
           M[T1.first_child(v)] != nil &&
           M[T1.first_child(v)] != T2.first_child(M[v]) )
        error_handler(1,
          "Wrong implementation of common subtree isomorphism");
      if ( M[v] != nil &&
           !T1.is_last_child(v) &&
           !T2.is_last_child(M[v]) &&
           M[T1.next_sibling(v)] != nil &&
           M[T1.next_sibling(v)] != T2.next_sibling(M[v]) )
        error_handler(1,
          "Wrong implementation of common subtree isomorphism");
  } }

  { node x,y;
    forall_nodes(x,T1) {
      forall_nodes(y,T2) {
        if ( code1[x] == code2[y] && size1[x] > size1[v] )
          error_handler(1,
            "Wrong implementation of common subtree isomorphism");
  } } }

}

bool operator<(
  const two_tuple<int,int>& p,
  const two_tuple<int,int>& q)
{ 
  return compare(p,q) == -1;
}

void bottom_up_unordered_max_common_subtree_isomorphism(
  const TREE<string,string>& T1,
  const TREE<string,string>& T2,
  node_array<int>& code1,
  node_array<int>& code2,
  node_array<node>& M)
{ 
  list<node> L1;
  postorder_tree_list_traversal(T1,L1);

  list<node> L2;
  postorder_tree_list_traversal(T2,L2);

  int num = 1;
  h_array<list<int>,int> CODE;

  bool unordered = true;
  { list<int> L;
    node v,w;
    forall(v,L1) {
      if ( T1.is_leaf(v) ) {
        code1[v] = 1;
      } else {

        L.clear();
        forall_children(w,v)
          L.append(code1[w]);
        if ( unordered ) sort_isomorphism_codes(L);

        if ( CODE.defined(L) ) {
          code1[v] = CODE[L];
        } else {
          CODE[L] = ++num;
          code1[v] = num;
  } } } }

  { list<int> L;
    node v,w;
    forall(v,L2) {
      if ( T2.is_leaf(v) ) {
        code2[v] = 1;
      } else {

        L.clear();
        forall_children(w,v)
          L.append(code2[w]);
        if ( unordered ) sort_isomorphism_codes(L);

        if ( CODE.defined(L) ) {
          code2[v] = CODE[L];
        } else {
          CODE[L] = ++num;
          code2[v] = num;
  } } } }


  node v,w;
  node_array<int> size1(T1);
  node_array<int> size2(T2);
  { node_pq<two_tuple<int,int> > Q1(T1);
    node_pq<two_tuple<int,int> > Q2(T2);

    forall(v,L1) {
      size1[v] = 1; // leaves have size equal to one
      if ( !T1.is_leaf(v) ) {
        forall_children(w,v) {
          size1[v] += size1[w];
      } }
      two_tuple<int,int> prio(-size1[v],code1[v]);
      Q1.insert(v,prio);
    }

    forall(v,L2) {
      size2[v] = 1; // leaves have size equal to one
      if ( !T2.is_leaf(v) ) {
        forall_children(w,v) {
          size2[v] += size2[w];
      } }
      two_tuple<int,int> prio(-size2[v],code2[v]);
      Q2.insert(v,prio);
    }

    while ( !Q1.empty() && !Q2.empty() ) {
      v = Q1.find_min();
      w = Q2.find_min();
      if ( code1[v] == code2[w] ) break;
      if ( Q1.prio(v) < Q2.prio(w) ) {
        Q1.del_min();
      } else {
        Q2.del_min();
    } }

    M[v] = w;

  }


  preorder_subtree_list_traversal(T1,v,L1);
  L1.pop(); // node [[v]] already mapped
  node_array<bool> mapped_to(T2,false);
  { node v,w;
    forall(v,L1) {
      forall_children(w,M[T1.parent(v)]) {
        if ( code1[v] == code2[w] && !mapped_to[w] ) {
          M[v] = w;
          mapped_to[w] = true;
          break;
  } } } }

  { node v,w,z;
    bool found;
    forall_nodes(v,T1) {
      if ( M[v] != nil ) {
        forall_nodes(w,T1)
          if ( v != w && M[v] == M[w] )
            error_handler(1,
              "Wrong implementation of common subtree isomorphism");
        forall_nodes(w,T2) {
          if ( !T1.is_root(v) &&
               !T2.is_root(w) &&
               M[T1.parent(v)] == T2.parent(w) ) {
            found = false;
            forall_children(z,T2.parent(w))
              if ( M[v] == z ) found = true;
            if ( !found ) {
              error_handler(1,
                "Wrong implementation of common subtree isomorphism");
  } } } } } }

  { node x,y;
    forall_nodes(x,T1) {
      forall_nodes(y,T2) {
        if ( code1[x] == code2[y] && size1[x] > size1[v] )
          error_handler(1,
            "Wrong implementation of common subtree isomorphism");
  } } }


}

void preorder_subtree_list_traversal(
  const TREE<string,string>& T,
  const node r,
  list<node>& L)
{ 
  L.clear();
  stack<node> S;
  S.push(r);
  node v,w;
  do {
    v = S.pop();
    L.push(v);
    w = T.last_child(v);
    while ( w != nil ) {
      S.push(w);
      w = T.previous_sibling(w);
    }
  } while ( !S.empty() );
  L.reverse();
}

void gw_ordered_tree_isomorphism(
  GraphWin& gw1)
{ 
  GRAPH<string,string>& G1 = gw1.get_graph();
  TREE<string,string> T1(G1); // tree T1(G1);

  GRAPH<string,string> G2;
  GraphWin gw2(G2,500,500,"Ordered Tree Isomorphism");
  gw2.display();
  gw2.message("Enter second tree. Press done when finished");
  gw2.edit();
  gw2.del_message();

  TREE<string,string> T2(G2); // tree T2(G2);

  node_array<node> M(T1);
  bool isomorph = ordered_tree_isomorphism(T1,T2,M);

  panel P;
  if ( isomorph ) {
    make_proof_panel(P,"These trees are isomorphic",true);
    if ( gw1.open_panel(P) ) { // proof button pressed
      gw1.save_all_attributes();
      node_array<point> pos1(T1);
      node_array<point> pos2(T2);
      gw1.get_position(pos1);
      gw1.set_layout(pos1); // remove edge bends
      gw2.get_position(pos2);
      gw2.set_layout(pos2); // remove edge bends
      node v;
      forall_nodes(v,T1)
        pos2[M[v]] = pos1[v];
      gw2.set_position(pos2);
      gw2.set_layout(pos2);

      gw1.wait();
      gw1.restore_all_attributes();
  } } else {
    make_proof_panel(P,"These trees are \\red not isomorphic",false);
    gw1.open_panel(P);
} }

void gw_unordered_tree_isomorphism(
  GraphWin& gw1)
{ 
  GRAPH<string,string>& G1 = gw1.get_graph();
  TREE<string,string> T1(G1);

  GRAPH<string,string> G2;
  GraphWin gw2(G2,500,500,"Unordered Tree Isomorphism");
  gw2.display();
  gw2.message("Enter second tree. Press done when finished");
  gw2.edit();
  gw2.del_message();

  TREE<string,string> T2(G2);

  node_array<node> M(T1);
  bool isomorph = unordered_tree_isomorphism(T1,T2,M);

  panel P;
  if ( isomorph ) {
    make_proof_panel(P,"These trees are isomorphic",true);
    if ( gw1.open_panel(P) ) { // proof button pressed
      gw1.save_all_attributes();
      node_array<point> pos1(T1);
      node_array<point> pos2(T2);
      gw1.get_position(pos1);
      gw1.set_layout(pos1); // remove edge bends
      gw2.get_position(pos2);
      gw2.set_layout(pos2); // remove edge bends
      node v;
      forall_nodes(v,T1)
        pos2[M[v]] = pos1[v];
      gw2.set_position(pos2);
      gw2.set_layout(pos2);

      gw1.wait();
      gw1.restore_all_attributes();
  } } else {
    make_proof_panel(P,"These trees are \\red not isomorphic",false);
    gw1.open_panel(P);
} }

void gw_top_down_ordered_subtree_isomorphism(
  GraphWin& gw1)
{ 
  GRAPH<string,string>& G1 = gw1.get_graph();
  TREE<string,string> T1(G1);

  GRAPH<string,string> G2;
  GraphWin gw2(G2,500,500,
    "Top-Down Ordered Subtree Isomorphism");
  gw2.display();
  gw2.message("Enter second tree. Press done when finished");
  gw2.edit();
  gw2.del_message();

  TREE<string,string> T2(G2);

  node_array<node> M(T1);
  bool isomorph = top_down_ordered_subtree_isomorphism(T1,T2,M);

  panel P;
  if ( isomorph ) {
    make_proof_panel(P,"There is an isomorphic subtree",true);
    if ( gw1.open_panel(P) ) { // proof button pressed
      gw1.save_all_attributes();
      node_array<point> pos1(T1);
      node_array<point> pos2(T2);
      { node v;
        forall_nodes(v,T1)
          gw2.set_color(M[v],blue);
        edge e;
        forall_edges(e,T2) {
          if ( gw2.get_color(T2.source(e)) == blue &&
               gw2.get_color(T2.target(e)) == blue ) {
            gw2.set_color(e,blue);
            gw2.set_width(e,2);
        } }

        gw1.get_position(pos1);
        gw1.set_layout(pos1); // remove edge bends
        gw2.get_position(pos2);
        gw2.set_layout(pos2); // remove edge bends
        forall_nodes(v,T1)
          pos1[v] = pos2[M[v]];
        gw1.set_position(pos1);
        gw1.set_layout(pos1);
      }

      gw1.wait();
      gw1.restore_all_attributes();
  } } else {
    make_proof_panel(P,"There is \\red no isomorphic subtree",false);
    gw1.open_panel(P);
} }

void gw_top_down_unordered_subtree_isomorphism(
  GraphWin& gw1)
{ 
  GRAPH<string,string>& G1 = gw1.get_graph();
  TREE<string,string> T1(G1);

  GRAPH<string,string> G2;
  GraphWin gw2(G2,500,500,
    "Top-Down Unordered Subtree Isomorphism");
  gw2.display();
  gw2.message("Enter second tree. Press done when finished");
  gw2.edit();
  gw2.del_message();

  TREE<string,string> T2(G2);

  node_array<node> M(T1);
  bool isomorph = top_down_unordered_subtree_isomorphism(T1,T2,M);

  panel P;
  if ( isomorph ) {
    make_proof_panel(P,"There is an isomorphic subtree",true);
    if ( gw1.open_panel(P) ) { // proof button pressed
      gw1.save_all_attributes();
      node_array<point> pos1(T1);
      node_array<point> pos2(T2);
      { node v;
        forall_nodes(v,T1)
          gw2.set_color(M[v],blue);
        edge e;
        forall_edges(e,T2) {
          if ( gw2.get_color(T2.source(e)) == blue &&
               gw2.get_color(T2.target(e)) == blue ) {
            gw2.set_color(e,blue);
            gw2.set_width(e,2);
        } }

        gw1.get_position(pos1);
        gw1.set_layout(pos1); // remove edge bends
        gw2.get_position(pos2);
        gw2.set_layout(pos2); // remove edge bends
        forall_nodes(v,T1)
          pos1[v] = pos2[M[v]];
        gw1.set_position(pos1);
        gw1.set_layout(pos1);
      }

      gw1.wait();
      gw1.restore_all_attributes();
  } } else {
    make_proof_panel(P,"There is \\red no isomorphic subtree",false);
    gw1.open_panel(P);
} }

void gw_bottom_up_ordered_subtree_isomorphism(
  GraphWin& gw1)
{ 
  GRAPH<string,string>& G1 = gw1.get_graph();
  TREE<string,string> T1(G1);

  GRAPH<string,string> G2;
  GraphWin gw2(G2,500,500,
    "Bottom-Up Ordered Subtree Isomorphism");
  gw2.display();
  gw2.message("Enter second tree. Press done when finished");
  gw2.edit();
  gw2.del_message();

  TREE<string,string> T2(G2);

  list<node_array<node> > L;
  bottom_up_ordered_subtree_isomorphism(T1,T2,L);

  gw2.message(string("There are %i isomorphic subtrees",
    L.size()));
  node v;
  node_array<node> M(T1);
  forall(M,L) {
    forall_nodes(v,T1) {
      gw2.set_color(M[v],blue);
  } }
  edge e;
  forall_edges(e,T2) {
    if ( gw2.get_color(T2.source(e)) == blue &&
         gw2.get_color(T2.target(e)) == blue ) {
      gw2.set_color(e,blue);
      gw2.set_width(e,2);
  } }

  gw2.wait();
  gw2.del_message();
}

void gw_bottom_up_unordered_subtree_isomorphism(
  GraphWin& gw1)
{ 
  GRAPH<string,string>& G1 = gw1.get_graph();
  TREE<string,string> T1(G1);

  GRAPH<string,string> G2;
  GraphWin gw2(G2,500,500,
    "Bottom-up Unordered Subtree Isomorphism");
  gw2.display();
  gw2.message("Enter second tree. Press done when finished");
  gw2.edit();
  gw2.del_message();

  TREE<string,string> T2(G2);

  node_array<int> C1(T1);
  node_array<int> C2(T2);
  list<node_array<node> > L;
  bottom_up_unordered_subtree_isomorphism(T1,T2,C1,C2,L);

  gw2.message(string("There are %i isomorphic subtrees",
    L.size()));

  node v,w;
  forall_nodes(v,T1)
    gw1.set_label(v,string("%i",C1[v]));
  forall_nodes(w,T2)
    gw2.set_label(w,string("%i",C2[w]));

  node_array<node> M(T1);
  forall(M,L) {
    forall_nodes(v,T1) {
      if ( M[v] != nil ) {
        gw2.set_color(M[v],blue);
  } } }

  edge e;
  forall_edges(e,T2) {
    if ( gw2.get_color(T2.source(e)) == blue &&
         gw2.get_color(T2.target(e)) == blue ) {
      gw2.set_color(e,blue);
      gw2.set_width(e,2);
  } }

  gw2.wait();
  gw2.del_message();
}

void gw_top_down_ordered_maximum_common_subtree_isomorphism(
  GraphWin& gw1)
{ 
  GRAPH<string,string>& G1 = gw1.get_graph();
  TREE<string,string> T1(G1);

  GRAPH<string,string> G2;
  GraphWin gw2(G2,500,500,
    "Top-Down Ordered Maximum Common Subtree Isomorphism");
  gw2.display();
  gw2.message("Enter second tree. Press done when finished");
  gw2.edit();
  gw2.del_message();

  TREE<string,string> T2(G2);

  node_array<node> M(T1,nil);
  top_down_ordered_max_common_subtree_isomorphism(T1,T2,M);

  gw1.save_all_attributes();
  { node v;
    forall_nodes(v,T1) {
      if ( M[v] != nil ) {
        gw1.set_color(v,blue);
        gw2.set_color(M[v],blue);
    } }
    edge e;
    forall_edges(e,T1) {
      if ( gw1.get_color(T1.source(e)) == blue &&
           gw1.get_color(T1.target(e)) == blue ) {
        gw1.set_color(e,blue);
        gw1.set_width(e,2);
    } }
    forall_edges(e,T2) {
      if ( gw2.get_color(T2.source(e)) == blue &&
           gw2.get_color(T2.target(e)) == blue ) {
        gw2.set_color(e,blue);
        gw2.set_width(e,2);
  } } }

  gw2.wait();
  gw1.restore_all_attributes();
}

void gw_top_down_unordered_maximum_common_subtree_isomorphism(
  GraphWin& gw1)
{ 
  GRAPH<string,string>& G1 = gw1.get_graph();
  TREE<string,string> T1(G1);

  GRAPH<string,string> G2;
  GraphWin gw2(G2,500,500,
    "Top-Down Unordered Maximum Common Subtree Isomorphism");
  gw2.display();
  gw2.message("Enter second tree. Press done when finished");
  gw2.edit();
  gw2.del_message();

  TREE<string,string> T2(G2);

  node_array<node> M(T1,nil);
  top_down_unordered_max_common_subtree_isomorphism(T1,T2,M);

  gw1.save_all_attributes();
  { node v;
    forall_nodes(v,T1) {
      if ( M[v] != nil ) {
        gw1.set_color(v,blue);
        gw2.set_color(M[v],blue);
    } }
    edge e;
    forall_edges(e,T1) {
      if ( gw1.get_color(T1.source(e)) == blue &&
           gw1.get_color(T1.target(e)) == blue ) {
        gw1.set_color(e,blue);
        gw1.set_width(e,2);
    } }
    forall_edges(e,T2) {
      if ( gw2.get_color(T2.source(e)) == blue &&
           gw2.get_color(T2.target(e)) == blue ) {
        gw2.set_color(e,blue);
        gw2.set_width(e,2);
  } } }

  gw2.wait();
  gw1.restore_all_attributes();
}

void gw_bottom_up_ordered_maximum_common_subtree_isomorphism(
  GraphWin& gw1)
{ 
  GRAPH<string,string>& G1 = gw1.get_graph();
  TREE<string,string> T1(G1);

  GRAPH<string,string> G2;
  GraphWin gw2(G2,500,500,
    "Bottom-Up Ordered Maximum Common Subtree Isomorphism");
  gw2.display();
  gw2.message("Enter second tree. Press done when finished");
  gw2.edit();
  gw2.del_message();

  TREE<string,string> T2(G2);

  node_array<int> C1(T1);
  node_array<int> C2(T2);
  node_array<node> M(T1,nil);
  bottom_up_ordered_max_common_subtree_isomorphism(T1,T2,C1,C2,M);

  gw1.save_all_attributes();
  node v,w;
  forall_nodes(v,T1)
    gw1.set_label(v,string("%i",C1[v]));
  forall_nodes(w,T2)
    gw2.set_label(w,string("%i",C2[w]));
  { node v;
    forall_nodes(v,T1) {
      if ( M[v] != nil ) {
        gw1.set_color(v,blue);
        gw2.set_color(M[v],blue);
    } }
    edge e;
    forall_edges(e,T1) {
      if ( gw1.get_color(T1.source(e)) == blue &&
           gw1.get_color(T1.target(e)) == blue ) {
        gw1.set_color(e,blue);
        gw1.set_width(e,2);
    } }
    forall_edges(e,T2) {
      if ( gw2.get_color(T2.source(e)) == blue &&
           gw2.get_color(T2.target(e)) == blue ) {
        gw2.set_color(e,blue);
        gw2.set_width(e,2);
  } } }

  gw2.wait();
  gw1.restore_all_attributes();
}

void gw_bottom_up_unordered_maximum_common_subtree_isomorphism(
  GraphWin& gw1)
{ 
  GRAPH<string,string>& G1 = gw1.get_graph();
  TREE<string,string> T1(G1);

  GRAPH<string,string> G2;
  GraphWin gw2(G2,500,500,
    "Bottom-Up Unordered Maximum Common Subtree Isomorphism");
  gw2.display();
  gw2.message("Enter second tree. Press done when finished");
  gw2.edit();
  gw2.del_message();

  TREE<string,string> T2(G2);

  node_array<int> C1(T1);
  node_array<int> C2(T2);
  node_array<node> M(T1,nil);
  bottom_up_unordered_max_common_subtree_isomorphism(T1,T2,C1,C2,M);

  gw1.save_all_attributes();
  node v,w;
  forall_nodes(v,T1)
    gw1.set_label(v,string("%i",C1[v]));
  forall_nodes(w,T2)
    gw2.set_label(w,string("%i",C2[w]));
  { node v;
    forall_nodes(v,T1) {
      if ( M[v] != nil ) {
        gw1.set_color(v,blue);
        gw2.set_color(M[v],blue);
    } }
    edge e;
    forall_edges(e,T1) {
      if ( gw1.get_color(T1.source(e)) == blue &&
           gw1.get_color(T1.target(e)) == blue ) {
        gw1.set_color(e,blue);
        gw1.set_width(e,2);
    } }
    forall_edges(e,T2) {
      if ( gw2.get_color(T2.source(e)) == blue &&
           gw2.get_color(T2.target(e)) == blue ) {
        gw2.set_color(e,blue);
        gw2.set_width(e,2);
  } } }

  gw2.wait();
  gw1.restore_all_attributes();
}


void depth_first_traversal(
  const graph& G,
  node_array<int>& order)
{ 
  node v,w;
  edge e;
  forall_nodes(v,G)
    order[v] = -1;

  int num = 1;
  stack<node> S;
  forall_nodes(v,G) {
    if ( order[v] == -1 ) {
      S.push(v);
      while ( !S.empty() ) {
        v = S.pop();
        if ( order[v] == -1 ) {
          order[v] = num++; // visit vertex [[v]]
          e = G.last_adj_edge(v);
          while ( e != nil ) {
            w = G.opposite(v,e);
            if ( order[w] == -1 ) S.push(w);
            e = G.adj_pred(e);
  } } } } }

  { node v,w,x,y;
    edge e,ee;
    node_array<node> root(G);
    list<edge> S;

    { int n = G.number_of_nodes();
      array<node> disorder(1,n);
      forall_nodes(v,G)
        disorder[order[v]] = v;

      root[disorder[1]] = disorder[1];
      for ( int i = 2; i <= n; i++ ) {
        v = disorder[i];
        if ( G.indeg(v) == 0 ) {
          root[v] = v;
        } else {
          bool nonroot = false;
          forall_in_edges(e,v) {
            if ( order[G.source(e)] < order[v] ) {
              nonroot = true;
              break;
          } }
          if ( !nonroot ) {
            root[v] = v;
          } else {
            forall_in_edges(e,v)
              if ( order[G.source(e)] < order[v] )
                break;
            forall_in_edges(ee,v)
              if ( order[G.source(ee)] > order[G.source(e)] &&
                   order[G.source(ee)] < order[v] )
                e = ee;
            S.append(e);
            root[v] = root[G.source(e)];
    } } } }


    forall_edges(e,G) {
      v = G.source(e);
      w = G.target(e);
      if ( order[root[v]] < order[root[w]] ) {
        error_handler(1,
          "Wrong implementation of depth-first traversal");
    } }

    forall(e,S) {
      v = G.source(e);
      w = G.target(e);
      forall_edges(ee,G) {
        x = G.source(ee);
        y = G.target(ee);
        if ( order[v] < order[x] &&
             order[x] < order[w] &&
             order[w] <= order[y] )
          error_handler(1,
            "Wrong implementation of depth-first traversal");
  } } }


}

void depth_first_spanning_subtree(
  const graph& G,
  node v,
  node_array<int>& order,
  set<node>& W,
  set<edge>& S)
{ 
  node w;
  edge e;
  forall_nodes(w,G)
    order[w] = -1;

  int num = 1;
  W.insert(v);
  order[v] = num++; // visit vertex [[v]]

  stack<edge> Z;
  e = G.last_adj_edge(v);
  while ( e != nil ) {
    w = G.opposite(v,e);
      Z.push(e);
    e = G.adj_pred(e);
  }

  while ( !Z.empty() ) {
    e = Z.pop();
    v = G.target(e);
    if ( order[v] == -1 ) {
      W.insert(v);
      S.insert(e);
      order[v] = num++; // visit vertex [[v]]
      e = G.last_adj_edge(v);
      while ( e != nil ) {
        w = G.opposite(v,e);
        if ( order[w] == -1 )
          Z.push(e);
        e = G.adj_pred(e);
  } } }

  { node v,w,x,y;
    edge e,ee;
    forall(e,S) {
      forall_edges(ee,G) {
        v = G.source(e);
        w = G.target(e);
        x = G.source(ee);
        y = G.target(ee);
        if ( order[v] < order[x] &&
             order[x] < order[w] &&
             order[w] <= order[y] )
          error_handler(1,
            "Wrong implementation of depth-first traversal");
  } } }


}

void leftmost_depth_first_traversal(
  const graph& G,
  edge e,
  list<edge>& L)
{ 
  node_array<bool> node_visited(G,false);
  edge_array<bool> edge_visited(G,false);
  node_visited[G.source(e)] = true;
  leftmost_depth_first_traversal(G,
    node_visited,edge_visited,e,L);
}

void leftmost_depth_first_traversal(
  const graph& G,
  node_array<bool>& node_visited,
  edge_array<bool>& edge_visited,
  edge e,
  list<edge>& L)
{ 
  node v = G.target(e);
  edge eprime;
  edge erev = G.reversal(e);
  L.append(e);
  if ( node_visited[v] ) {
    if ( edge_visited[erev] ) {
      eprime = erev;
      do {
        eprime = G.cyclic_adj_succ(eprime);
      } while ( eprime != erev && edge_visited[eprime] );
      if ( edge_visited[eprime] ) return;
    }
    else {
      eprime = erev;
    }
  } else {
    eprime = G.cyclic_adj_succ(erev);
  }
  edge_visited[e] = true;
  node_visited[v] = true;
  return leftmost_depth_first_traversal(G,
    node_visited,edge_visited,eprime,L);
}

void breadth_first_traversal(
  const graph& G,
  node_array<int>& order)
{ 
  node v,w;
  forall_nodes(v,G)
    order[v] = -1;

  int num = 1;
  queue<node> Q;
  forall_nodes(v,G) {
    if ( order[v] == -1 ) {
      Q.append(v);
      order[v] = num++; // visit vertex [[v]]
      while ( !Q.empty() ) {
        v = Q.pop();
        forall_adj_nodes(w,v) {
          if ( order[w] == -1 ) {
            Q.append(w);
            order[w] = num++; // visit vertex [[w]]
  } } } } }

  { node v,w,x,y;
    edge e,ee;
    node_array<node> root(G);
    list<edge> S;

    { int n = G.number_of_nodes();
      array<node> disorder(1,n);
      forall_nodes(v,G)
        disorder[order[v]] = v;

      root[disorder[1]] = disorder[1];
      for ( int i = 2; i <= n; i++ ) {
        v = disorder[i];
        if ( G.indeg(v) == 0 ) {
          root[v] = v;
        } else {
          bool nonroot = false;
          forall_in_edges(e,v) {
            if ( order[G.source(e)] < order[v] ) {
              nonroot = true;
              break;
          } }
          if ( !nonroot ) {
            root[v] = v;
          } else {
            forall_in_edges(e,v)
              if ( order[G.source(e)] < order[v] )
                break;
            forall_in_edges(ee,v)
              if ( order[G.source(ee)] < order[G.source(e)] &&
                   order[G.source(ee)] < order[v] )
                e = ee;
            S.append(e);
            root[v] = root[G.source(e)];
    } } } }


    forall_edges(e,G) {
      v = G.source(e);
      w = G.target(e);
      if ( order[root[v]] < order[root[w]] ) {
        error_handler(1,
          "Wrong implementation of breadth-first traversal");
    } }

    forall(e,S) {
      v = G.source(e);
      w = G.target(e);
      forall_edges(ee,G) {
        x = G.source(ee);
        y = G.target(ee);
        if ( order[x] < order[v] &&
             order[v] < order[w] &&
             order[w] <= order[y] )
          error_handler(1,
            "Wrong implementation of breadth-first traversal");
  } } }


}

void breadth_first_spanning_subtree(
  const graph& G,
  node v,
  node_array<int>& order,
  set<node>& W,
  set<edge>& S)
{ 
  node w;
  forall_nodes(w,G)
    order[w] = -1;
  int num = 1;

  W.insert(v);
  order[v] = num++; // visit vertex [[v]]
  edge e;
  queue<edge> Q;
  forall_adj_edges(e,v)
    if ( G.target(e) != v )
      Q.append(e);
  while ( !Q.empty() ) {
    e = Q.pop();
    w = G.target(e);
    if ( !W.member(w) ) {
      order[w] = num++; // visit vertex [[v]]
      W.insert(w);
      S.insert(e);
      forall_adj_edges(e,w)
        if ( !W.member(G.target(e)) )
          Q.append(e);
  } }

  { node v,w,x,y;
    edge e,ee;
    forall(e,S) {
      forall_edges(ee,G) {
        v = G.source(e);
        w = G.target(e);
        x = G.source(ee);
        y = G.target(ee);
        if ( order[x] != -1 &&
             order[x] < order[v] &&
             order[v] < order[w] &&
             order[w] <= order[y] )
          error_handler(1,
            "Wrong implementation of breadth-first traversal");
  } } }


}

bool ordered_graph_isomorphism(
  const graph& G1,
  const graph& G2,
  node_array<node>& M)
{ 
  if ( G1.number_of_nodes() != G2.number_of_nodes() ||
       G1.number_of_edges() != G2.number_of_edges() )
    return false;

  if ( G1.number_of_edges() == 0 ) {
    node v = G1.first_node();
    node w = G2.first_node();
    while ( v != nil ) { // && w != nil
      M[v] = w;
      v = G1.succ_node(v);
      w = G2.succ_node(w);
    }
    return true;
  }

  edge e1 = G1.first_edge();
  list<edge> L;
  leftmost_depth_first_traversal(G1,e1,L);

  node_array<int> order1(G1);
  { node v;
    forall_nodes(v,G1)
      order1[v] = -1;
    edge e = L.head();
    int num = 1;
    order1[G1.source(e)] = num++;
    forall(e,L) {
      if ( order1[G1.target(e)] == -1 ) {
        order1[G1.target(e)] = num++;
  } } }


  edge e2;
  forall_edges(e2,G2) {
    L.clear();
    leftmost_depth_first_traversal(G2,e2,L);

    node_array<int> order2(G2);
    { node v;
      forall_nodes(v,G2)
        order2[v] = -1;
      edge e = L.head();
      int num = 1;
      order2[G2.source(e)] = num++;
      forall(e,L) {
        if ( order2[G2.target(e)] == -1 ) {
          order2[G2.target(e)] = num++;
    } } }

    { int n = G1.number_of_nodes();
      array<node> disorder1(1,n);
      node v;
      forall_nodes(v,G1)
        disorder1[order1[v]] = v;
      array<node> disorder2(1,n);
      forall_nodes(v,G2)
        disorder2[order2[v]] = v;
      for ( int i = 1; i <= n; i++ ) {
        M[disorder1[i]] = disorder2[i];
    } }

    bool isomorph = true;
    { node v,w;
      list<node> L1,L2;
      forall_nodes(v,G1) {
        L1 = G1.adj_nodes(v);
        L2 = G2.adj_nodes(M[v]);

        w = M[L1.front()];
        for ( int i = 1; i <= L1.length(); i++ ) {
          if ( w == L2.front() ) {
            break;
          } else {
            L2.move_to_rear(L2.first());
        } }

        list_item it1 = L1.first();
        list_item it2 = L2.first();
        while ( it1 != nil ) { // && it2 != nil
          if ( M[L1.contents(it1)] != L2.contents(it2) ) {
            isomorph = false;
            break;
          }
          it1 = L1.succ(it1);
          it2 = L2.succ(it2);
    } } }


    if ( isomorph ) return true;
  }
  return false;
}

void gw_depth_first_traversal(
  GraphWin& gw)
{ 
  graph& G = gw.get_graph();
  { edge e;
    edge_array<int> S(G),T(G);
    forall_edges(e,G) {
      S[e] = index(G.source(e));
      T[e] = index(G.target(e));
    }
    G.bucket_sort_edges(T);
    G.bucket_sort_edges(S);
  }

  gw.update_graph();
  node_array<int> order(G);
  depth_first_traversal(G,order);
  { gw.save_all_attributes();
    color gray = grey3;

    node v,w,x;
    forall_nodes(v,G) {
      gw.set_color(v,white);
      gw.set_label(v,string("%i",order[v]));
    }

    int n = G.number_of_nodes();
    array<node> disorder(1,n);
    forall_nodes(v,G)
      disorder[order[v]] = v;

    edge e,ee;
    for ( int i = 1; i <= n; i++ ) {
      v = disorder[i];

      gw.set_color(v,gray);
      { bool finished = true;
        forall_adj_nodes(w,v)
          if ( gw.get_color(w) == white ) finished = false;
        if ( finished ) {
          gw.set_color(v,black);
          leda_wait(0.5);
      } }

      { int pred_num = 1;
        bool pred = false;
        forall_in_edges(e,v) {
          w = G.opposite(v,e);
          if ( gw.get_color(w) != white &&
               order[w] >= pred_num ) {
            ee = e;
            pred_num = order[w];
            pred = true;
        } }
        if ( pred ) gw.set_width(ee,5);
        leda_wait(0.5);
      }

      forall_in_edges(e,v) {
        w = G.opposite(v,e);
        if ( gw.get_color(w) == gray ) {
          bool finished = true;
          forall_out_edges(ee,w)
            if ( gw.get_color(G.opposite(w,ee)) == white )
              finished = false;
          if ( finished ) {
            gw.set_color(w,black);
            leda_wait(0.5);
      } } }


      leda_wait(0.5);
    }

    gw.wait();
    gw.restore_all_attributes();
  }

}

void gw_depth_first_spanning_subtree(
  GraphWin& gw)
{ 
  graph& G = gw.get_graph();
  { edge e;
    edge_array<int> S(G),T(G);
    forall_edges(e,G) {
      S[e] = index(G.source(e));
      T[e] = index(G.target(e));
    }
    G.bucket_sort_edges(T);
    G.bucket_sort_edges(S);
  }

  gw.update_graph();
  node v = gw.ask_node();
  node_array<int> order(G);
  set<node> W;
  set<edge> S;
  depth_first_spanning_subtree(G,v,order,W,S);
  { gw.save_all_attributes();
    color gray = grey3;
    forall_nodes(v,G)
      gw.set_label(v,"");
    forall(v,W) {
      gw.set_color(v,gray);
      gw.set_label(v,string("%i",order[v]));
    }
    edge e;
    forall(e,S)
      gw.set_width(e,3);

    GRAPH<node,edge> H;
    CopyGraph(H,G);

    forall_nodes(v,H)
      if ( !W.member(H[v]) )
        H.del_node(v);
    forall_edges(e,H)
      if ( !S.member(H[e]) )
        H.del_edge(e);

    tree T(H);
    GraphWin gw2(T,500,500,"Depth-First Spanning Subtree");
    gw2.display();

    forall_nodes(v,T)
      gw2.set_label(v,gw.get_label(H[v]));

    node_array<double> xcoord(T);
    node_array<double> ycoord(T);
    layered_tree_layout(T,xcoord,ycoord);
    gw2.adjust_coords_to_win(xcoord,ycoord);
    gw2.set_layout(xcoord,ycoord);
    gw2.display();

    gw.wait();
    gw.restore_all_attributes();
  }

}

void gw_leftmost_depth_first_traversal(
  GraphWin& gw)
{ 
  graph& G = gw.get_graph();

  panel P;
  if ( !Is_Bidirected(G) ) {
    make_proof_panel(P,"This graph is \\red not bidirected",false);
    gw.open_panel(P);
    return;
  }

  G.make_map(); // set edge reversal information

  list<edge> L;
  edge e = gw.ask_edge();
  leftmost_depth_first_traversal(G,e,L);
  { gw.save_all_attributes();

    node_array<int> order(G);
    node v;
    forall_nodes(v,G)
      order[v] = -1;
    edge e = L.head();
    int num = 1;
    order[G.source(e)] = num++;
    forall(e,L)
      if ( order[G.target(e)] == -1 )
        order[G.target(e)] = num++;

    forall_nodes(v,G) {
      gw.set_color(v,white);
      gw.set_label(v,string("%i",order[v]));
    }

    gw.set_color(G.source(L.front()),blue);
    forall(e,L) {
      gw.set_width(e,2);
      gw.set_color(e,red);
      if ( gw.get_color(G.target(e)) == white )
        gw.set_color(G.target(e),blue);
      leda_wait(0.5);
    }

    gw.wait();
    gw.restore_all_attributes();
  }


}

void gw_breadth_first_traversal(
  GraphWin& gw)
{ 
  graph& G = gw.get_graph();
  { edge e;
    edge_array<int> S(G),T(G);
    forall_edges(e,G) {
      S[e] = index(G.source(e));
      T[e] = index(G.target(e));
    }
    G.bucket_sort_edges(T);
    G.bucket_sort_edges(S);
  }

  gw.update_graph();
  node_array<int> order(G);
  breadth_first_traversal(G,order);
  { gw.save_all_attributes();
    color gray = grey3;

    node v,w,x;
    forall_nodes(v,G) {
      gw.set_color(v,white);
      gw.set_label(v,string("%i",order[v]));
    }

    int n = G.number_of_nodes();
    array<node> disorder(1,n);
    forall_nodes(v,G)
      disorder[order[v]] = v;

    edge e,ee;
    for ( int i = 1; i <= n; i++ ) {
      v = disorder[i];

      gw.set_color(v,gray);
      { bool finished = true;
        forall_adj_nodes(w,v)
          if ( gw.get_color(w) == white ) finished = false;
        if ( finished ) {
          gw.set_color(v,black);
          leda_wait(0.5);
      } }

      { int pred_num = n;
        bool pred = false;
        forall_in_edges(e,v) {
          w = G.opposite(v,e);
          if ( gw.get_color(w) != white &&
               order[w] < pred_num ) {
            ee = e;
            pred_num = order[w];
            pred = true;
        } }
        if ( pred ) gw.set_width(ee,5);
        leda_wait(0.5);
      }

      forall_in_edges(e,v) {
        w = G.opposite(v,e);
        if ( gw.get_color(w) == gray ) {
          bool finished = true;
          forall_out_edges(ee,w)
            if ( gw.get_color(G.opposite(w,ee)) == white )
              finished = false;
          if ( finished ) {
            gw.set_color(w,black);
            leda_wait(0.5);
      } } }


      leda_wait(0.5);
    }

    gw.wait();
    gw.restore_all_attributes();
  }

}

void gw_breadth_first_spanning_subtree(
  GraphWin& gw)
{ 
  graph& G = gw.get_graph();
  { edge e;
    edge_array<int> S(G),T(G);
    forall_edges(e,G) {
      S[e] = index(G.source(e));
      T[e] = index(G.target(e));
    }
    G.bucket_sort_edges(T);
    G.bucket_sort_edges(S);
  }

  gw.update_graph();
  node v = gw.ask_node();
  node_array<int> order(G);
  set<node> W;
  set<edge> S;
  breadth_first_spanning_subtree(G,v,order,W,S);
  { gw.save_all_attributes();
    color gray = grey3;
    forall_nodes(v,G)
      gw.set_label(v,"");
    forall(v,W) {
      gw.set_color(v,gray);
      gw.set_label(v,string("%i",order[v]));
    }
    edge e;
    forall(e,S)
      gw.set_width(e,3);

    GRAPH<node,edge> H;
    CopyGraph(H,G);

    forall_nodes(v,H)
      if ( !W.member(H[v]) )
        H.del_node(v);
    forall_edges(e,H)
      if ( !S.member(H[e]) )
        H.del_edge(e);

    tree T(H);
    GraphWin gw2(T,500,500,"Breadth-First Spanning Subtree");
    gw2.display();

    forall_nodes(v,T)
      gw2.set_label(v,gw.get_label(H[v]));

    node_array<double> xcoord(T);
    node_array<double> ycoord(T);
    layered_tree_layout(T,xcoord,ycoord);
    gw2.adjust_coords_to_win(xcoord,ycoord);
    gw2.set_layout(xcoord,ycoord);
    gw2.display();

    gw.wait();
    gw.restore_all_attributes();
  }

}

void gw_ordered_graph_isomorphism(
  GraphWin& gw1)
{ 
  graph& G1 = gw1.get_graph();

  panel P;
  if ( !Is_Bidirected(G1) ) {
    make_proof_panel(P,"This graph is \\red not bidirected",false);
    gw1.open_panel(P);
    return;
  }

  G1.make_map(); // set edge reversal information

  node_array<node> M(G1,nil);

  graph G2;
  GraphWin gw2(G2,500,500,"Ordered Graph Isomorphism");
  gw2.display();
  gw2.message("Enter second graph. Press done when finished");
  gw2.edit();
  gw2.del_message();

  if ( !Is_Bidirected(G2) ) {
    make_proof_panel(P,"This graph is \\red not bidirected",false);
    gw2.open_panel(P);
    return;
  }

  G2.make_map(); // set edge reversal information

  if ( ordered_graph_isomorphism(G1,G2,M) ) {

    make_proof_panel(P,"These graphs are isomorphic",true);
    if ( gw1.open_panel(P) ) { // proof button pressed
      gw1.save_all_attributes();
      node_array<point> pos1(G1);
      node_array<point> pos2(G2);
      gw1.get_position(pos1);
      gw1.set_layout(pos1); // remove edge bends
      node v;
      forall_nodes(v,G1)
        pos2[M[v]] = pos1[v];
      gw2.set_position(pos2);
      gw2.set_layout(pos2);

      gw1.wait();
      gw1.restore_all_attributes();
    }

  } else {

    make_proof_panel(P,"These graphs are \\red not isomorphic",false);
    gw1.open_panel(P);

} }


void double_check_clique(
  const graph& G,
  const node_matrix<bool>& A,
  const set<node>& C)
{ 
  node v,w;
  forall(v,C)
    forall(w,C)
      if ( v != w && ( !A(v,w) || !A(w,v) ) )
        error_handler(1,"Wrong implementation of clique");
}

void double_check_maximal_clique(
  const graph& G,
  const node_matrix<bool>& A,
  const set<node>& C)
{ 
  double_check_clique(G,A,C);

  node v,w;
  set<node> REST;
  forall_nodes(v,G)
    if ( !C.member(v) ) REST.insert(v);
  bool adjacent_all;
  forall(v,REST) {
    adjacent_all = true;
    forall(w,C) {
      if ( !A(v,w) || !A(w,v) ) {
        adjacent_all = false;
        break;
    } }
    if ( adjacent_all )
      error_handler(1,
        "Wrong implementation of maximal clique");
} }

void all_cliques(
  const graph& G,
  const node_matrix<bool>& A,
  list<set<node> >& L)
{ 
  set<node> C;
  set<node> P;
  node v;
  forall_nodes(v,G)
    P.insert(v);
  next_clique(G,A,C,P,L);
}

void next_clique(
  const graph& G,
  const node_matrix<bool>& A,
  set<node>& C,
  set<node>& P,
  list<set<node> >& L)
{ 
  if ( C.size() > 2 ) { // ignore trivial cliques
    double_check_clique(G,A,C);
    L.append(C);
  }

  if ( !P.empty() ) {
    node v,w;
    forall(v,P) {
      P.del(v);
      set<node> PP;
      forall_adj_nodes(w,v)
        if ( P.member(w) ) PP.insert(w);
      C.insert(v);
      next_clique(G,A,C,PP,L);
      C.del(v);
} } }

void simple_all_maximal_cliques(
  const graph& G,
  const node_matrix<bool>& A,
  list<set<node> >& L)
{ 
  set<node> C;
  set<node> P;
  set<node> S;
  node v;
  forall_nodes(v,G)
    P.insert(v);
  simple_next_maximal_clique(G,A,C,P,S,L);
}

void simple_next_maximal_clique(
  const graph& G,
  const node_matrix<bool>& A,
  set<node>& C,
  set<node>& P,
  set<node>& S,
  list<set<node> >& L)
{ 
  if ( P.empty() && S.empty() ) {
    double_check_maximal_clique(G,A,C);
    L.append(C);
  } else {
    node v,w;
    forall(v,P) {
      P.del(v);
      set<node> PP;
      set<node> SS;
      forall_adj_nodes(w,v) {
        if ( P.member(w) ) PP.insert(w);
        if ( S.member(w) ) SS.insert(w);
      }
      C.insert(v);
      simple_next_maximal_clique(G,A,C,PP,SS,L);
      C.del(v);
      S.insert(v);
} } }

void all_maximal_cliques(
  const graph& G,
  const node_matrix<bool>& A,
  list<set<node> >& L)
{ 
  set<node> C;
  set<node> P;
  set<node> S;
  node v;
  forall_nodes(v,G)
    P.insert(v);
  next_maximal_clique(G,A,C,P,S,L);
}

void next_maximal_clique(
  const graph& G,
  const node_matrix<bool>& A,
  set<node>& C,
  set<node>& P,
  set<node>& S,
  list<set<node> >& L)
{ 
  if ( P.empty() ) {
    if ( S.empty() ) {
      double_check_maximal_clique(G,A,C);
      L.append(C);
  } } else {
    node u = P.choose();
    node v,w;
    forall(v,P) {
      if ( !A(u,v) || !A(v,u) ) {
        P.del(v);
        set<node> PP;
        set<node> SS;
        forall_adj_nodes(w,v) {
          if ( P.member(w) ) PP.insert(w);
          if ( S.member(w) ) SS.insert(w);
        }
        C.insert(v);
        next_maximal_clique(G,A,C,PP,SS,L);
        C.del(v);
        S.insert(v);
} } } }

void maximum_clique(
  graph& G,
  node_matrix<bool>& A,
  set<node>& MAX)
{ 
  set<node> C;
  set<node> P;
  set<node> S;
  node v;
  int max_deg = 0;
  forall_nodes(v,G) {
    P.insert(v);
    max_deg = leda_max(max_deg,G.outdeg(v));
  }
  MAX.clear();
  next_maximum_clique(G,A,C,P,S,max_deg,MAX);
}

void next_maximum_clique(
  const graph& G,
  const node_matrix<bool>& A,
  set<node>& C,
  set<node>& P,
  set<node>& S,
  const int max_deg,
  set<node>& MAX)
{ 
  if ( MAX.size() < C.size() )
    MAX = C; // new largest clique found

  if ( P.empty() ) {
    if ( S.empty() ) {
      double_check_maximal_clique(G,A,C);
  } } else {
    node u = P.choose();
    node v,w;
    forall(v,P) {
      if ( !A(u,v) || !A(v,u) ) {
        P.del(v);
        set<node> PP;
        set<node> SS;
        forall_adj_nodes(w,v) {
          if ( P.member(w) ) PP.insert(w);
          if ( S.member(w) ) SS.insert(w);
        }
        C.insert(v);
        if ( MAX.size() < C.size() + PP.size() &&
             MAX.size() < max_deg + 1 )
          next_maximum_clique(G,A,C,PP,SS,max_deg,MAX);
        C.del(v);
        S.insert(v);
} } } }

void double_check_tree_independent_set(
  const TREE<string,string>& T,
  const set<node>& I)
{ 
  node v,w;
  forall(v,I) {
    if ( !T.is_root(v) && I.member(T.parent(v)) ) {
      error_handler(1,
        "Wrong implementation of independent set");
  } }

  bool independent = false;
  forall_nodes(v,T) {
    if ( !I.member(v) ) {
      if ( !T.is_root(v) && !I.member(T.parent(v)) ) {
        independent = true; // presumed guilty
        forall_children(w,v) {
          if ( I.member(w) ) {
            independent = false; // found innocent
            break;
      } } }
      if ( independent ) { // condemned
        error_handler(1,
          "Wrong implementation of maximal independent set");
} } } }

void double_check_independent_set(
  const GRAPH<string,string>& G,
  const set<node>& I)
{ 
  node_matrix<bool> A(G,false);
  { edge e;
    forall_edges(e,G) {
      A(G.source(e),G.target(e)) = true;
  } }


  node v,w;
  forall(v,I)
    forall(w,I)
      if ( v != w && ( A(v,w) || A(w,v) ) )
        error_handler(1,
          "Wrong implementation of independent set");

  bool independent = false;
  forall_nodes(v,G) {
    if ( !I.member(v) ) {
      independent = true; // presumed guilty
      forall(w,I) {
        if ( A(v,w) || A(w,v) ) {
          independent = false; // found innocent
          break;
      } }
      if ( independent ) { // condemned
        error_handler(1,
          "Wrong implementation of maximal independent set");
} } } }

void maximum_independent_set(
  const TREE<string,string>& T,
  set<node>& I)
{ 
  I.clear();
  list<node> L;
  postorder_tree_list_traversal(T,L);
  node_array<bool> independent(T,true);
  node v,w;
  forall(v,L) {
    if ( !T.is_leaf(v) ) {
      forall_children(w,v) {
        if ( independent[w] ) {
          independent[v] = false;
          break;
    } } }
    if ( independent[v] ) I.insert(v);
  }
  double_check_tree_independent_set(T,I);
}

void graph_complement(
  const GRAPH<string,string>& G,
  GRAPH<string,string>& H,
  node_array<node>& M)
{ 
  H.clear(); // reset [[H]] to the empty graph
  node_array<node> N(G);
  node v,w;
  forall_nodes(v,G) {
    w = H.new_node();
    H[w] = G[v]; // vertex label
    M[w] = v;
    N[v] = w;
  }
  edge e;
  forall_edges(e,G)
    H.new_edge(N[G.source(e)],N[G.target(e)]);

  graph_complement(H);
}

void graph_complement(
  GRAPH<string,string>& G)
{ 
  Make_Bidirected(G);

  node_matrix<bool> A(G,false);
  { edge e;
    forall_edges(e,G) {
      A(G.source(e),G.target(e)) = true;
  } }


  G.del_all_edges();

  node v,w;
  forall_nodes(v,G) {
    forall_nodes(w,G) {
      if ( v != w && !A(v,w) ) {
        G.new_edge(v,w);
} } } }

void maximum_independent_set(
  const GRAPH<string,string>& G,
  set<node>& I)
{ 
  GRAPH<string,string> H;
  node_array<node> M(G);
  graph_complement(G,H,M);

  node_matrix<bool> A(H,false);
  { edge e;
    forall_edges(e,H) {
      A(H.source(e),H.target(e)) = true;
  } }
  set<node> C;
  maximum_clique(H,A,C);

  I.clear();
  node v;
  forall(v,C)
    I.insert(M[v]);
  double_check_independent_set(G,I);
}

void gw_all_cliques(
  GraphWin& gw)
{ 
  graph& G = gw.get_graph();
  Make_Bidirected(G);
  gw.update_graph();

  node_matrix<bool> A(G,false);
  { edge e;
    forall_edges(e,G) {
      A(G.source(e),G.target(e)) = true;
  } }


  list<set<node> > L;
  all_cliques(G,A,L);

  L.bucket_sort(clique_size);
  L.reverse();

  panel P;
  make_proof_panel(P,
    string("There are %i nontrivial cliques",L.length()),true);
  if ( gw.open_panel(P) ) { // proof button pressed

    node v;
    edge e;
    set<node> S;
    forall(S,L) {
      gw.save_all_attributes();
      forall(v,S) {
        gw.set_color(v,blue);
        forall_adj_edges(e,v) {
          if ( S.member(G.opposite(v,e)) ) {
            gw.set_color(G.opposite(v,e),blue);
            gw.set_color(e,blue);
            gw.set_width(e,2);
      } } }
      gw.redraw();
      panel Q;
      make_yes_no_panel(Q,"Continue",true);
      if ( gw.open_panel(Q) ) { // no button pressed
        gw.restore_all_attributes();
        break;
      }
      gw.restore_all_attributes();

} } }

inline int clique_size(
  const set<node>& C)
{ 
  return C.size();
}

void gw_all_maximal_cliques(
  GraphWin& gw)
{ 
  graph& G = gw.get_graph();
  Make_Bidirected(G);
  gw.update_graph();

  node_matrix<bool> A(G,false);
  { edge e;
    forall_edges(e,G) {
      A(G.source(e),G.target(e)) = true;
  } }


  list<set<node> > L;
  all_maximal_cliques(G,A,L);

  L.bucket_sort(clique_size);
  L.reverse();

  panel P;
  make_proof_panel(P,string("There are %i maximal cliques",L.length()),true);
  if ( gw.open_panel(P) ) { // proof button pressed

    node v;
    edge e;
    set<node> S;
    forall(S,L) {
      gw.save_all_attributes();
      forall(v,S) {
        gw.set_color(v,blue);
        forall_adj_edges(e,v) {
          if ( S.member(G.opposite(v,e)) ) {
            gw.set_color(G.opposite(v,e),blue);
            gw.set_color(e,blue);
            gw.set_width(e,2);
      } } }
      gw.redraw();
      panel Q;
      make_yes_no_panel(Q,"Continue",true);
      if ( gw.open_panel(Q) ) { // no button pressed
        gw.restore_all_attributes();
        break;
      }
      gw.restore_all_attributes();

} } }

void gw_maximum_clique(
    GraphWin& gw)
{ 
  graph& G = gw.get_graph();
  Make_Bidirected(G);
  gw.update_graph();

  node_matrix<bool> A(G,false);
  { edge e;
    forall_edges(e,G) {
      A(G.source(e),G.target(e)) = true;
  } }


  set<node> MAX;
  maximum_clique(G,A,MAX);

  gw.save_all_attributes();
  node v;
  edge e;
  forall(v,MAX) {
    gw.set_color(v,blue);
    forall_adj_edges(e,v) {
      if ( MAX.member(G.opposite(v,e)) ) {
        gw.set_color(G.opposite(v,e),blue);
        gw.set_color(e,blue);
        gw.set_width(e,2);
  } } }
  gw.redraw();
  gw.wait();
  gw.restore_all_attributes();
}

void gw_tree_maximum_independent_set(
  GraphWin& gw)
{ 
  GRAPH<string,string>& G = gw.get_graph();
  TREE<string,string> T(G);
  set<node> I;
  maximum_independent_set(T,I);

  gw.save_all_attributes();
  node v;
  forall(v,I)
    gw.set_color(v,blue);
  gw.wait();
  gw.restore_all_attributes();
}

void gw_maximum_independent_set(
  GraphWin& gw)
{ 
  GRAPH<string,string>& G = gw.get_graph();
  Make_Bidirected(G);
  gw.update_graph();

  set<node> I;
  maximum_independent_set(G,I);

  gw.save_all_attributes();
  node v;
  forall(v,I)
    gw.set_color(v,blue);
  gw.wait();
  gw.restore_all_attributes();
}

void gw_graph_complement(
  GraphWin& gw)
{ 
  GRAPH<string,string>& G = gw.get_graph();
  graph_complement(G);
  gw.update_graph();
}

void double_check_tree_vertex_cover(
  const TREE<string,string>& T,
  const set<node>& C)
{ 
  if ( T.number_of_nodes() == 1 ) return;

  node v,w;
  forall_nodes(v,T) {
    if ( !C.member(v) && !T.is_root(v) &&
         !C.member(T.parent(v)) ) {
      error_handler(1,
        "Wrong implementation of vertex cover");
  } }

  bool cover = false;
  forall(v,C) {
    if ( T.is_root(v) ||
         ( !T.is_root(v) && !C.member(T.parent(v)) ) ) {
      forall_children(w,v) {
        if ( C.member(w) ) {
          cover = true;
          break;
      } }
      if ( !cover ) {
        error_handler(1,
          "Wrong implementation of minimal vertex cover");
} } } }

void double_check_vertex_cover(
  const GRAPH<string,string>& G,
  const set<node>& C)
{ 
  if ( G.number_of_nodes() == 1 ) return;

  edge e;
  forall_edges(e,G) {
    if ( !C.member(G.source(e)) && !C.member(G.target(e)) ) {
      error_handler(1,"Wrong implementation of vertex cover");
  } }

  bool cover = false;
  node v,w;
  forall(v,C) {
    forall_adj_nodes(w,v) {
      if ( !C.member(w) ) {
        cover = true;
        break;
    } }
    if ( !cover ) {
      error_handler(1,
        "Wrong implementation of minimal vertex cover");
} } }

void minimum_vertex_cover(
  const TREE<string,string>& T,
  set<node>& C)
{ 
  C.clear();
  if ( T.number_of_nodes() == 1 ) {
    // C.insert(T.first_node());
  } else {
    set<node> I;
    maximum_independent_set(T,I);
    node v;
    forall_nodes(v,T) {
      if ( !I.member(v) ) C.insert(v);
  } }
  double_check_tree_vertex_cover(T,C);
}

void minimum_vertex_cover(
  const GRAPH<string,string>& G,
  set<node>& C)
{ 
  C.clear();
  if ( G.number_of_nodes() == 1 ) {
    // C.insert(G.first_node());
  } else {
    set<node> I;
    maximum_independent_set(G,I);
    node v;
    forall_nodes(v,G) {
      if ( !I.member(v) ) C.insert(v);
  } }
  double_check_vertex_cover(G,C);
}

void gw_tree_minimum_vertex_cover(
  GraphWin& gw)
{ 
  GRAPH<string,string>& G = gw.get_graph();
  TREE<string,string> T(G);
  set<node> C;
  minimum_vertex_cover(T,C);

  gw.save_all_attributes();
  node v;
  forall(v,C)
    gw.set_color(v,blue);
  gw.wait();
  gw.restore_all_attributes();
}

void gw_minimum_vertex_cover(
  GraphWin& gw)
{ 
  GRAPH<string,string>& G = gw.get_graph();
  Make_Bidirected(G);
  gw.update_graph();

  set<node> C;
  minimum_vertex_cover(G,C);

  gw.save_all_attributes();
  node v;
  forall(v,C)
    gw.set_color(v,blue);
  gw.wait();
  gw.restore_all_attributes();
}


void double_check_graph_isomorphism(
  const GRAPH<string,string>& G1,
  const GRAPH<string,string>& G2,
  const node_matrix<edge>& A1,
  const node_matrix<edge>& A2,
  const node_array<node>& M)
{ 
  if ( G1.number_of_nodes() != G2.number_of_nodes() ||
       G1.number_of_edges() != G2.number_of_edges() )
    error_handler(1,
      "Wrong implementation of graph isomorphism");

  node v,w;
  forall_nodes(v,G1)
    if ( M[v] == nil || G1[v] != G2[M[v]] )
      error_handler(1,
        "Wrong implementation of graph isomorphism");

  forall_nodes(v,G1)
    forall_nodes(w,G1)
      if ( ( v != w && ( M[v] == M[w] ||
                         A1(v,w) != nil && A2(M[v],M[w]) == nil ) ) ||
      ( A1(v,w) != nil && A2(M[v],M[w]) == nil &&
        G1[A1(v,w)] != G2[A2(M[v],M[w])] ) )
        error_handler(1,
          "Wrong implementation of graph isomorphism");
}

void double_check_subgraph_isomorphism(
  const GRAPH<string,string>& G1,
  const GRAPH<string,string>& G2,
  const node_matrix<edge>& A1,
  const node_matrix<edge>& A2,
  const node_array<node>& M)
{ 

  if ( G1.number_of_nodes() > G2.number_of_nodes() ||
       G1.number_of_edges() > G2.number_of_edges() )
    error_handler(1,"Wrong implementation of subgraph isomorphism");

  node v,w;
  forall_nodes(v,G1)
    if ( M[v] == nil || G1[v] != G2[M[v]] )
      error_handler(1,"Wrong implementation of subgraph isomorphism");

  forall_nodes(v,G1)
    forall_nodes(w,G1)
      if ( ( v != w && ( M[v] == M[w] ||
                         A1(v,w) != nil && A2(M[v],M[w]) == nil ) ) ||
      ( A1(v,w) != nil && A2(M[v],M[w]) == nil &&
        G1[A1(v,w)] != G2[A2(M[v],M[w])] ) )
        error_handler(1,"Wrong implementation of subgraph isomorphism");

}

void double_check_maximal_common_subgraph_isomorphism(
  const GRAPH<string,string>& G1,
  const GRAPH<string,string>& G2,
  const node_matrix<edge>& A1,
  const node_matrix<edge>& A2,
  const node_array<node>& M)
{ 
  node v,w,x;
  forall_nodes(v,G1)
    if ( M[v] != nil && G1[v] != G2[M[v]] )
      error_handler(1,
        "Wrong implementation of common subgraph isomorphism");

  forall_nodes(v,G1)
    forall_nodes(w,G1)
      if ( M[v] != nil && M[w] != nil &&
           ( v != w && M[v] == M[w] ||
             A1(v,w) != nil && A2(M[v],M[w]) == nil ||
             A1(v,w) == nil && A2(M[v],M[w]) != nil ||
             A1(v,w) != nil && A2(M[v],M[w]) != nil
             && G1[A1(v,w)] != G2[A2(M[v],M[w])] ) )
        error_handler(1,
          "Wrong implementation of common subgraph isomorphism");

  list<node> V1;
  list<node> R1;
  set<node> R2;
  forall_nodes(v,G1) {
    if ( M[v] != nil ) {
      V1.append(v);
    } else {
      R1.append(v);
  } }
  forall_nodes(v,G2) R2.insert(v);
  forall(v,V1) R2.del(M[v]);

  forall(v,R1) {
    forall(w,R2) {
      if ( G1[v] == G2[w] ) {
        bool isomorphic = true;
        forall(x,V1) {
          if ( A1(v,x) != nil && A2(w,M[x]) == nil ||
               A1(v,x) == nil && A2(w,M[x]) != nil ||
               A1(v,x) != nil && A2(w,M[x]) != nil
               && G1[A1(v,x)] != G2[A2(w,M[x])] ) {
            isomorphic = false;
            break;
          }
          if ( A1(x,v) != nil && A2(M[x],w) == nil ||
               A1(x,v) == nil && A2(M[x],w) != nil ||
               A1(x,v) != nil && A2(M[x],w) != nil
               && G1[A1(x,v)] != G2[A2(M[x],w)] ) {
            isomorphic = false;
            break;
        } }
        if ( isomorphic ) // can map [[v]] to [[w]]
          error_handler(1,
            "Wrong implementation of common subgraph isomorphism");
} } } }

void graph_isomorphism(
  const GRAPH<string,string>& G1,
  const GRAPH<string,string>& G2,
  node_array<node>& M,
  list<node_array<node> >& L)
{ 
  L.clear();
  if ( G1.number_of_nodes() != G2.number_of_nodes() ||
       G1.number_of_edges() != G2.number_of_edges() ) return;
    node_matrix<edge> A1(G1,nil);
    node_matrix<edge> A2(G2,nil);
  { edge e;
    forall_edges(e,G1)
      A1(G1.source(e),G1.target(e)) = e;
    forall_edges(e,G2)
      A2(G2.source(e),G2.target(e)) = e;
  }

  node v = G1.first_node();
  extend_graph_isomorphism(G1,G2,A1,A2,M,v,L);
}

bool extend_graph_isomorphism(
  const GRAPH<string,string>& G1,
  const GRAPH<string,string>& G2,
  const node_matrix<edge>& A1,
  const node_matrix<edge>& A2,
  node_array<node> M,
  node v,
  list<node_array<node> >& L)
{ 
  list<node> V2 = G2.all_nodes();
  node w;
  forall_nodes(w,G1)
    if ( M[w] != nil )
      V2.remove(M[w]); // already mapped vertex
  forall_nodes(w,G2)
    if ( G1[v] != G2[w] )
      V2.remove(w); // different vertex labels
  forall(w,V2) {
    if ( preserves_adjacencies(G1,G2,A1,A2,M,v,w) ) {
      M[v] = w;
      if ( v == G1.last_node() ) {
        double_check_graph_isomorphism(G1,G2,A1,A2,M);
        L.append(M);
      } else {
        extend_graph_isomorphism(G1,G2,A1,A2,M,G1.succ_node(v),L);
} } } }

bool preserves_adjacencies(
  const GRAPH<string,string>& G1,
  const GRAPH<string,string>& G2,
  const node_matrix<edge>& A1,
  const node_matrix<edge>& A2,
  const node_array<node>& M,
  node v,
  node w)
{ 
  if ( G1.indeg(v) != G2.indeg(w) || G1.outdeg(v) != G2.outdeg(w) )
    return false;
  node x;
  edge e;
  forall_in_edges(e,v) {
    x = G1.opposite(v,e);
    if ( M[x] != nil &&
         ( A2(M[x],w) == nil || G1[e] != G2[A2(M[x],w)] ) )
      return false;
  }
  forall_out_edges(e,v) {
    x = G1.opposite(v,e);
    if ( M[x] != nil && A2(w,M[x]) == nil ) return false;
  }
  return true;
}

void subgraph_isomorphism(
  const GRAPH<string,string>& G1,
  const GRAPH<string,string>& G2,
  node_array<node>& MAP,
  list<node_array<node> >& ALL)
{ 
  ALL.clear();
    node_array<set<node> > CAN(G1);
  { node v,w;
    forall_nodes(v,G1) {
      forall_nodes(w,G2) {
        if ( G1.indeg(v) <= G2.indeg(w) &&
             G1.outdeg(v) <= G2.outdeg(w) &&
             G1[v] == G2[w] ) {
          CAN[v].insert(w);
  } } } }

    node_matrix<edge> A1(G1,nil);
    node_matrix<edge> A2(G2,nil);
  { edge e;
    forall_edges(e,G1)
      A1(G1.source(e),G1.target(e)) = e;
    forall_edges(e,G2)
      A2(G2.source(e),G2.target(e)) = e;
  }

  node v = G1.first_node();
  extend_subgraph_isomorphism(G1,G2,A1,A2,CAN,v,MAP,ALL);
}

void extend_subgraph_isomorphism(
  const GRAPH<string,string>& G1,
  const GRAPH<string,string>& G2,
  const node_matrix<edge>& A1,
  const node_matrix<edge>& A2,
  node_array<set<node> >& CAN,
  node v,
  node_array<node>& MAP,
  list<node_array<node> >& L)
{ 
  node w,x,y;
  forall_nodes(w,G2) {
    if ( CAN[v].member(w) ) { // can map [[v]] to [[w]]

      node_array<set<node> > NEXT(G1);
      forall_nodes(x,G1) NEXT[x] = CAN[x];
      forall_nodes(x,G1)
        if ( x != v ) NEXT[x].del(w);
      forall_nodes(y,G2)
        if ( y != w ) NEXT[v].del(y);

      if ( refine_subgraph_isomorphism(G1,G2,A1,A2,NEXT,v,w) ) {
        if ( v == G1.last_node() ) {
          forall_nodes(x,G1) MAP[x] = NEXT[x].choose();
          double_check_subgraph_isomorphism(G1,G2,A1,A2,MAP);
          L.append(MAP);
        } else {
          extend_subgraph_isomorphism(G1,G2,A1,A2,
                                      NEXT,G1.succ_node(v),MAP,L);
} } } } }

bool refine_subgraph_isomorphism(
  const GRAPH<string,string>& G1,
  const GRAPH<string,string>& G2,
  const node_matrix<edge>& A1,
  const node_matrix<edge>& A2,
  node_array<set<node> >& CAN,
  node v,
  node w)
{ 

  if ( G1.indeg(v) > G2.indeg(w) || G1.outdeg(v) > G2.outdeg(w) )
    return false;

  node x,y;
  edge e;

  forall_in_edges(e,v) {
    x = G1.opposite(v,e);
    if ( index(x) > index(v) ) // future vertex
      forall_nodes(y,G2)
        if ( A2(y,w) == nil ||
             G1[e] != G2[A2(y,w)] ) CAN[x].del(y);
  }

  forall_out_edges(e,v) {
    x = G1.opposite(v,e);
    if ( index(x) > index(v) ) // future vertex
      forall_nodes(y,G2)
        if ( A2(w,y) == nil ||
             G1[e] != G2[A2(w,y)] ) CAN[x].del(y);
  }

  forall_nodes(x,G1)
    if ( index(x) > index(v) && CAN[x].empty() )
      return false;

  return true;

}

void graph_product(
  GRAPH<two_tuple<node,node>,edge>& G, // <edge> not used
  const GRAPH<string,string>& G1,
  const GRAPH<string,string>& G2)
{ 
  edge e;
  forall_edges(e,G1)
    if ( G1.source(e) == G1.target(e) )
      error_handler(1,"First graph cannot have self-loops");
  forall_edges(e,G2)
    if ( G2.source(e) == G2.target(e) )
      error_handler(1,"Second graph cannot have self-loops");

  G.clear();
  node v,w;
  forall_nodes(v,G1) {
    forall_nodes(w,G2) {
      if ( G1[v] == G2[w] ) {
        two_tuple<node,node> vw(v,w);
        G.new_node(vw);
  } } }
    node_matrix<edge> A1(G1,nil);
    node_matrix<edge> A2(G2,nil);
  { edge e;
    forall_edges(e,G1)
      A1(G1.source(e),G1.target(e)) = e;
    forall_edges(e,G2)
      A2(G2.source(e),G2.target(e)) = e;
  }

  forall_nodes(v,G) {
    forall_nodes(w,G) {
      if ( G[v].first() != G[w].first() &&
           G[v].second() != G[w].second() &&
           ( A1(G[v].first(),G[w].first()) != nil &&
             A2(G[v].second(),G[w].second()) != nil &&
             G1[A1(G[v].first(),G[w].first())]
             == G2[A2(G[v].second(),G[w].second())] ||
             A1(G[v].first(),G[w].first()) == nil &&
             A2(G[v].second(),G[w].second()) == nil ) )
        G.new_edge(v,w);
} } }

void maximal_common_subgraph_isomorphism(
  const GRAPH<string,string>& G1,
  const GRAPH<string,string>& G2,
  list<node_array<node> >& ALL)
{ 
  GRAPH<two_tuple<node,node>,edge> G; // <edge> not used
  graph_product(G,G1,G2);

  node_matrix<bool> A(G,false);
  { edge e;
    forall_edges(e,G) {
      A(G.source(e),G.target(e)) = true;
  } }


  list<set<node> > L;
  all_maximal_cliques(G,A,L);

    node_matrix<edge> A1(G1,nil);
    node_matrix<edge> A2(G2,nil);
  { edge e;
    forall_edges(e,G1)
      A1(G1.source(e),G1.target(e)) = e;
    forall_edges(e,G2)
      A2(G2.source(e),G2.target(e)) = e;
  }

  ALL.clear();
  node_array<node> M(G1);
  node v;
  set<node> C;
  forall(C,L) {
    forall_nodes(v,G1)
      M[v] = nil;
    forall(v,C)
      M[G[v].first()] = G[v].second();
    double_check_maximal_common_subgraph_isomorphism(G1,G2,A1,A2,M);
    ALL.append(M);
} }

void gw_graph_isomorphism(
  GraphWin& gw1)
{ 
  GRAPH<string,string>& G1 = gw1.get_graph();
  node_array<node> M(G1,nil);

  GRAPH<string,string> G2;
  GraphWin gw2(G2,500,500,"Graph Isomorphism");
  gw2.display();
  gw2.message("Enter second graph. Press done when finished");
  gw2.edit();
  gw2.del_message();

  list<node_array<node> > L;
  graph_isomorphism(G1,G2,M,L);
  panel P;
  if ( L.length() > 1 ) {

    make_proof_panel(P,
      string("There are %i graph isomorphisms",L.length()),true);
    if ( gw1.open_panel(P) ) { // proof button pressed
      gw1.save_all_attributes();
      node_array<point> pos1(G1);
      node_array<point> pos2(G2);
      forall(M,L) {
        gw1.get_position(pos1);
        gw1.set_layout(pos1); // remove edge bends
        node v;
        forall_nodes(v,G1)
          pos2[M[v]] = pos1[v];
        gw2.set_position(pos2);
        gw2.set_layout(pos2);

        panel Q;
        make_yes_no_panel(Q,"Continue",true);
        if ( gw1.open_panel(Q) ) // no button pressed
          break;
      }
      gw1.restore_all_attributes();
    }

  } else if ( L.length() == 1 ) {

    make_proof_panel(P,"These graphs are isomorphic",true);
    if ( gw1.open_panel(P) ) { // proof button pressed
      gw1.save_all_attributes();
      node_array<point> pos1(G1);
      node_array<point> pos2(G2);
      M = L.head();
      gw1.get_position(pos1);
      gw1.set_layout(pos1); // remove edge bends
      node v;
      forall_nodes(v,G1)
        pos2[M[v]] = pos1[v];
      gw2.set_position(pos2);
      gw2.set_layout(pos2);

      gw1.wait();
      gw1.restore_all_attributes();
    }

  } else {

    make_proof_panel(P,"These graphs are \\red not isomorphic",false);
    gw1.open_panel(P);

} }

void gw_graph_automorphism(
  GraphWin& gw)
{ 
  GRAPH<string,string>& G = gw.get_graph();
  node_array<node> M(G,nil);
  list<node_array<node> > L;
  graph_isomorphism(G,G,M,L);

  panel P;
  if ( L.length() > 1 ) {

    make_proof_panel(P,
      string("The automorphism group has order %i",L.length()),true);
    if ( gw.open_panel(P) ) { // proof button pressed
      node_array<point> pos1(G);
      node_array<point> pos2(G);
      node v;
      gw.save_all_attributes();
      forall(M,L) {
        gw.get_position(pos1);
        forall_nodes(v,G)
          pos2[M[v]] = pos1[v];
        gw.set_position(pos2);
        gw.set_layout(pos2);
        panel Q;
        make_yes_no_panel(Q,"Continue",true);
        if ( gw.open_panel(Q) ) // no button pressed
          break;
      }
      gw.restore_all_attributes();

  } } else {

    make_proof_panel(P,"The automorphism group has order 1",false);
    gw.open_panel(P);

} }

void gw_subgraph_isomorphism(
  GraphWin& gw1)
{ 
  GRAPH<string,string>& G1 = gw1.get_graph();
  node_array<node> M(G1,nil);

  GRAPH<string,string> G2;
  GraphWin gw2(G2,500,500,"Subgraph Isomorphism");
  gw2.display();
  gw2.message("Enter second graph. Press done when finished");
  gw2.edit();
  gw2.del_message();

  list<node_array<node> > L;
  subgraph_isomorphism(G1,G2,M,L);

  panel P;
  if ( L.length() > 1 ) {
    make_proof_panel(P,
      string("There are %i subgraph isomorphisms",L.length()),true);
    if ( gw1.open_panel(P) ) { // proof button pressed
        node_matrix<edge> A1(G1,nil);
        node_matrix<edge> A2(G2,nil);
      { edge e;
        forall_edges(e,G1)
          A1(G1.source(e),G1.target(e)) = e;
        forall_edges(e,G2)
          A2(G2.source(e),G2.target(e)) = e;
      }

      gw1.save_all_attributes();
      node_array<point> pos1(G1);
      node_array<point> pos2(G2);
      forall(M,L) {
        gw2.get_position(pos2);
        gw2.set_layout(pos2); // remove edge bends
        node v;
        edge e1,e2;
        forall_nodes(v,G1)
          pos1[v] = pos2[M[v]];
        gw1.set_position(pos1);
        gw1.set_layout(pos1);
        gw2.save_all_attributes();

        forall_nodes(v,G1)
          gw2.set_color(M[v],blue);
        forall_edges(e1,G1) {
          e2 = A2(M[G1.source(e1)],M[G1.target(e1)]);
          if ( e2 != nil ) {
            gw2.set_color(e2,blue);
            gw2.set_width(e2,2);
        } }

        panel Q;
        make_yes_no_panel(Q,"Continue",true);
        if ( gw1.open_panel(Q) ) { // no button pressed
          gw1.restore_all_attributes();
          gw2.restore_all_attributes();
          break;
        }
        gw2.restore_all_attributes();

      }
      gw1.restore_all_attributes();
    }

  } else if ( L.length() == 1 ) {

    make_proof_panel(P,"There is one subgraph isomorphism",true);
    if ( gw1.open_panel(P) ) { // proof button pressed
        node_matrix<edge> A1(G1,nil);
        node_matrix<edge> A2(G2,nil);
      { edge e;
        forall_edges(e,G1)
          A1(G1.source(e),G1.target(e)) = e;
        forall_edges(e,G2)
          A2(G2.source(e),G2.target(e)) = e;
      }

      gw1.save_all_attributes();
      node_array<point> pos1(G1);
      node_array<point> pos2(G2);
      M = L.head();
      gw2.get_position(pos2);
      gw2.set_layout(pos2); // remove edge bends
      node v;
      edge e1,e2;
      forall_nodes(v,G1)
        pos1[v] = pos2[M[v]];
      gw1.set_position(pos1);
      gw1.set_layout(pos1);
      gw2.save_all_attributes();

      forall_nodes(v,G1)
        gw2.set_color(M[v],blue);
      forall_edges(e1,G1) {
        e2 = A2(M[G1.source(e1)],M[G1.target(e1)]);
        if ( e2 != nil ) {
          gw2.set_color(e2,blue);
          gw2.set_width(e2,2);
      } }

      gw1.wait();
      gw1.restore_all_attributes();
    }

  } else {
      make_proof_panel(P,"There is \\red no subgraph isomorphism",false);
      gw1.open_panel(P);
} }

void gw_maximal_common_subgraph_isomorphism(
  GraphWin& gw1)
{ 
  GRAPH<string,string>& G1 = gw1.get_graph();

  GRAPH<string,string> G2;
  GraphWin gw2(G2,500,500,"Maximal Common Subgraph Isomorphism");
  gw2.display();
  gw2.message("Enter second graph. Press done when finished");
  gw2.edit();
  gw2.del_message();

  list<node_array<node> > L;
  maximal_common_subgraph_isomorphism(G1,G2,L);

  panel P;
  make_proof_maximum_panel(P,
    string("There are %i maximal common subgraph isomorphisms",
    L.length()),true);
  int button = gw1.open_panel(P);
  switch( button ) {
    case 0 : break; // ok button pressed
    case 1 : { // proof button pressed
      node_array<node> M;
      node v;
      edge e;
        node_matrix<edge> A1(G1,nil);
        node_matrix<edge> A2(G2,nil);
      { edge e;
        forall_edges(e,G1)
          A1(G1.source(e),G1.target(e)) = e;
        forall_edges(e,G2)
          A2(G2.source(e),G2.target(e)) = e;
      }


      forall(M,L) {
        gw1.save_all_attributes();
        gw2.save_all_attributes();
        node_array<point> pos1(G1);
        node_array<point> pos2(G2);
        gw1.get_position(pos1);
        gw1.set_layout(pos1); // remove edge bends
        gw2.get_position(pos2);
        gw2.set_layout(pos2); // remove edge bends
        forall_nodes(v,G1)
          if ( M[v] != nil ) pos1[v] = pos2[M[v]];
        gw1.set_position(pos1);
        gw1.set_layout(pos1);
        forall_nodes(v,G1) {
          if ( M[v] != nil ) {
            gw1.set_color(v,blue);
            gw2.set_color(M[v],blue);
        } }
        forall_edges(e,G1) {
          if ( gw1.get_color(G1.source(e)) == blue &&
               gw1.get_color(G1.target(e)) == blue ) {
            gw1.set_color(e,blue);
            gw1.set_width(e,2);
        } }
        forall_edges(e,G2) {
          if ( gw2.get_color(G2.source(e)) == blue &&
               gw2.get_color(G2.target(e)) == blue ) {
            gw2.set_color(e,blue);
            gw2.set_width(e,2);
        } }


        panel Q;
        make_yes_no_panel(Q,"Continue",true);
        if ( gw1.open_panel(Q) ) { // no button pressed
          gw1.restore_all_attributes();
          gw2.restore_all_attributes();
          break;
        }

        gw1.restore_all_attributes();
        gw2.restore_all_attributes();
      }
      break;
    }
    case 2 : { // maximum button pressed
      node_array<node> M;
      node v,w;
      edge e;
      int size;
      int maximum_size = 0;
      node_array<node> MAXIMUM;
        node_matrix<edge> A1(G1,nil);
        node_matrix<edge> A2(G2,nil);
      { edge e;
        forall_edges(e,G1)
          A1(G1.source(e),G1.target(e)) = e;
        forall_edges(e,G2)
          A2(G2.source(e),G2.target(e)) = e;
      }


      forall(M,L) {
        size = 0;
        forall_nodes(v,G1) {
          if ( M[v] != nil ) {
            size++;
            forall_nodes(w,G1)
              if ( M[w] != nil && A1(v,w) != nil ) size++;
        } }
        if ( size >  maximum_size ) {
          MAXIMUM = M;
          maximum_size = size;
      } }

    gw1.save_all_attributes();
    M = MAXIMUM;
    node_array<point> pos1(G1);
    node_array<point> pos2(G2);
    gw1.get_position(pos1);
    gw1.set_layout(pos1); // remove edge bends
    gw2.get_position(pos2);
    gw2.set_layout(pos2); // remove edge bends
    forall_nodes(v,G1)
      if ( M[v] != nil ) pos1[v] = pos2[M[v]];
    gw1.set_position(pos1);
    gw1.set_layout(pos1);
    forall_nodes(v,G1) {
      if ( M[v] != nil ) {
        gw1.set_color(v,blue);
        gw2.set_color(M[v],blue);
    } }
    forall_edges(e,G1) {
      if ( gw1.get_color(G1.source(e)) == blue &&
           gw1.get_color(G1.target(e)) == blue ) {
        gw1.set_color(e,blue);
        gw1.set_width(e,2);
    } }
    forall_edges(e,G2) {
      if ( gw2.get_color(G2.source(e)) == blue &&
           gw2.get_color(G2.target(e)) == blue ) {
        gw2.set_color(e,blue);
        gw2.set_width(e,2);
    } }

    gw1.wait();
    gw1.restore_all_attributes();
    break;
} } }


void about_book(
  GraphWin& gw)
{ 
  window& W = gw.get_window();
  panel P;
  P.text_item("\\bf Algorithms on Trees and Graphs");
  P.text_item("");
  P.text_item("");
  P.text_item("by Gabriel Valiente");
  P.text_item("");
  P.text_item("");
  P.text_item("(c) 2002 Springer-Verlag");
  P.text_item("");
  P.text_item("");
  P.text_item("LEDA (c) 1991-2002 Algorithmic Solutions GmbH");
  P.text_item("");
  P.button("ok");
  P.open(W);
}

int main() {
  GRAPH<string,string> G;
  GraphWin gw(G,500,500,"Algorithms on Trees and Graphs");

  int techniques_menu = gw.add_menu("Techniques");
  gw.add_simple_call(gw_tree_edit,
    "Tree Edit",techniques_menu);
  gw.add_simple_call(gw_backtracking_tree_edit,
    "Backtracking Tree Edit",techniques_menu);
  gw.add_simple_call(gw_branch_and_bound_tree_edit,
    "Branch-and-Bound Tree Edit",techniques_menu);
  gw.add_simple_call(gw_divide_and_conquer_tree_edit,
    "Divide-and-Conquer Tree Edit",techniques_menu);
  gw.add_simple_call(gw_top_down_dynamic_programming_tree_edit,
    "Top-Down Dynamic Programming Tree Edit",techniques_menu);
  gw.add_simple_call(gw_bottom_up_dynamic_programming_tree_edit,
    "Bottom-Up Dynamic Programming Tree Edit",techniques_menu);

  int traverse_menu = gw.add_menu("Traversal");
  gw.add_simple_call(gw_rec_preorder_tree_traversal,
    "Preorder Tree Traversal (Recursive)",traverse_menu);
  gw.add_simple_call(gw_preorder_tree_traversal,
    "Preorder Tree Traversal (Iterative)",traverse_menu);
  gw.add_simple_call(gw_rec_postorder_tree_traversal,
    "Postorder Tree Traversal (Recursive)",traverse_menu);
  gw.add_simple_call(gw_postorder_tree_traversal,
    "Postorder Tree Traversal (Iterative)",traverse_menu);
  gw.add_simple_call(gw_top_down_tree_traversal,
    "Top-Down Tree Traversal",traverse_menu);
  gw.add_simple_call(gw_bottom_up_tree_traversal,
    "Bottom-Up Tree Traversal",traverse_menu);

  gw.add_separator(traverse_menu);

  gw.add_simple_call(gw_preorder_tree_depth,
    "Tree Depth",traverse_menu);
  gw.add_simple_call(gw_bottom_up_tree_height,
    "Tree Height",traverse_menu);
  gw.add_simple_call(gw_layered_tree_layout,
    "Layered Tree Layout",traverse_menu);

  gw.add_separator(traverse_menu);

  gw.add_simple_call(gw_depth_first_traversal,
    "Depth-First Traversal",traverse_menu);
  gw.add_simple_call(gw_depth_first_spanning_subtree,
    "Depth-First Spanning Subtree",traverse_menu);
  gw.add_simple_call(gw_leftmost_depth_first_traversal,
    "Leftmost Depth-First Traversal",traverse_menu);
  gw.add_simple_call(gw_breadth_first_traversal,
    "Breadth-First Traversal",traverse_menu);
  gw.add_simple_call(gw_breadth_first_spanning_subtree,
    "Breadth-First Spanning Subtree",traverse_menu);

  gw.add_separator(traverse_menu);

  gw.add_simple_call(gw_ordered_graph_isomorphism,
    "Ordered Graph Isomorphism",traverse_menu);

  int clique_menu = gw.add_menu("Cliques");
  gw.add_simple_call(gw_all_cliques,
    "All Cliques",clique_menu);
  gw.add_simple_call(gw_all_maximal_cliques,
    "All Maximal Cliques",clique_menu);
  gw.add_simple_call(gw_maximum_clique,
    "Maximum Clique",clique_menu);

  gw.add_separator(clique_menu);

  gw.add_simple_call(gw_graph_complement,
    "Graph Complement",clique_menu);

  gw.add_separator(clique_menu);

  gw.add_simple_call(gw_tree_maximum_independent_set,
    "Maximum Independent Set in a Tree",clique_menu);
  gw.add_simple_call(gw_maximum_independent_set,
    "Maximum Independent Set",clique_menu);

  gw.add_separator(clique_menu);

  gw.add_simple_call(gw_tree_minimum_vertex_cover,
    "Minimum Vertex Cover in a Tree",clique_menu);
  gw.add_simple_call(gw_minimum_vertex_cover,
    "Minimum Vertex Cover",clique_menu);

  int isomorph_menu = gw.add_menu("Isomorphism");

  int tree_isomorph_menu =
    gw.add_menu("Tree Isomorphism",isomorph_menu);
  gw.add_simple_call(gw_ordered_tree_isomorphism,
    "Ordered",tree_isomorph_menu);
  gw.add_simple_call(gw_unordered_tree_isomorphism,
    "Unordered",tree_isomorph_menu);

  int subtree_isomorph_menu =
    gw.add_menu("Subtree Isomorphism",isomorph_menu);
  gw.add_simple_call(gw_top_down_ordered_subtree_isomorphism,
    "Top-Down Ordered",subtree_isomorph_menu);
  gw.add_simple_call(gw_top_down_unordered_subtree_isomorphism,
    "Top-Down Unordered",subtree_isomorph_menu);
  gw.add_simple_call(gw_bottom_up_ordered_subtree_isomorphism,
    "Bottom-Up Ordered",subtree_isomorph_menu);
  gw.add_simple_call(gw_bottom_up_unordered_subtree_isomorphism,
    "Bottom-Up Unordered",subtree_isomorph_menu);

  int max_common_subtree_isomorph_menu =
    gw.add_menu("Maximum Common Subtree Isomorphism",isomorph_menu);
  gw.add_simple_call(
    gw_top_down_ordered_maximum_common_subtree_isomorphism,
    "Top-Down Ordered",max_common_subtree_isomorph_menu);
  gw.add_simple_call(
    gw_top_down_unordered_maximum_common_subtree_isomorphism,
    "Top-Down Unordered",max_common_subtree_isomorph_menu);
  gw.add_simple_call(
    gw_bottom_up_ordered_maximum_common_subtree_isomorphism,
    "Bottom-Up Ordered",max_common_subtree_isomorph_menu);
  gw.add_simple_call(
    gw_bottom_up_unordered_maximum_common_subtree_isomorphism,
    "Bottom-Up Unordered",max_common_subtree_isomorph_menu);

  gw.add_separator(isomorph_menu);

  gw.add_simple_call(gw_graph_isomorphism,
    "Graph Isomorphism",isomorph_menu);
  gw.add_simple_call(gw_graph_automorphism,
    "Graph Automorphism",isomorph_menu);
  gw.add_simple_call(gw_subgraph_isomorphism,
    "Subgraph Isomorphism",isomorph_menu);
  gw.add_simple_call(gw_maximal_common_subgraph_isomorphism,
    "Maximal Common Subgraph Isomorphism",isomorph_menu);

  gw.display(window::min,window::min);

  int h_menu = gw.get_menu("Help");
  gw_add_simple_call(gw,about_book,"About the book",h_menu);

  gw.edit();
}

