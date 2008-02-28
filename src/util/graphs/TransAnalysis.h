/*
  Qing Yi: this file defines the interface for an transitive path analysis
  algorithm published by Yi, Adve and Kennedy in PLDI00
*/
#ifndef TRANSITIVE_ANALYSIS
#define TRANSITIVE_ANALYSIS

#include <GraphAccess.h>

/*
struct T
{
   T& operator = (const T &that) { return *this; }
 private:
   T() {}
};
*/

/* TransInfoOP class defines the required operations that need to be 
  implemented for the path representation of graphs during transitive
  path analysis.
  T is the result type of transitive path analysis and represents 
  a collection (eg. set) of paths.
*/
template <class T>
class TransInfoOP
{
 public:
   // the set union of two collection of paths. More specifically,
   // if info1 and info2 are sets of paths from n1 to n2, 
   // UnionWith(info1,info2) adds all paths in info2 into info1 as well.
   virtual void  UnionWith(T& info1, T info2)=0;

   // The special element of T that represents the most conservative 
   // approximation of path information. Specifically, it represents
   // all paths from n1 to n2 into the original graph. 
   // If b = GetBottomInfo(n1,n2), then for every collection t of paths
   // from n1 to n2, b union t = b; (t is a subset of b).
   virtual T  GetBottomInfo( const GraphAccessInterface::Node *n1, const GraphAccessInterface::Node *n2)=0 ;

   // The empty collection (set) of paths from n1 to n2. 
   // If b = GetTopInfo(n1,n2), then for every collection t of paths
   // from n1 to n2, b union t = t; (b is a subset of t).
   virtual T  GetTopInfo(const GraphAccessInterface::Node *n1, const GraphAccessInterface::Node *n2)=0;

   // return whether a TransInfo is empty
   virtual bool IsTop(T t) = 0;

   //If info1 is a set of paths from n1 to n2, and info2 is a set of paths
   // from n2 to n3, Composite(info1,info2) returns the set of all paths
   // from n1 to n3 by concatenating paths in info1 and info2 respectively. 
   virtual T Composite(T info1, T info2)=0;

   // The empty path representation from n to itself. 
   // Suppose t = GetIDTransInfo(n). If t1 is a set of paths from n to n1,
   // then Composite(t, t1) = t1. Similarly, if t2 is a set of paths from n2
   // to n, then Composite(t2, t) = t.
   virtual T  GetIDTransInfo( const GraphAccessInterface::Node *n) =0;

   // Composite a cycle with itself infinite times. 
   // Specifically, if info is a cycle from node n to itself, 
   // Closure(info) = info union Composite(info,info) union Composite(info,info,info)...
   virtual T Closure(T info)=0;

   // The transitive path representation associated with edge e.
   virtual T  GetTransInfo( const GraphAccessInterface::Edge *e) =0;

   virtual ~TransInfoOP() {}
};

/****************************************************
 This is the graph to store the result of transitive analysis.
 An object of this class is passed as argument to construct a
 GraphTransAnalysis operator, and as we invoke 
 GraphTransAnalysis::ComputeTransInfo to compute transitive info
 between nodes of the input graph, the transitive info is stored
 into TransInfoGraph by calling TransInfoGraph::SetTransInfo.
 Users of GraphTransAnalysis must implement all three virtual functions
 defined TransInfoGraph, to record and return transitive information 
 already computed. 
 The template paramter T is the type of transitive information (the result
 of transitive analysis). It models information regarding transitive paths 
 in the input graph
*/
template <class T>
class TransInfoGraph
{
 public:
 // return whether transitive info has been computed from node src to snk
  virtual bool TransInfoComputed(const GraphAccessInterface::Node *src, 
                                    const GraphAccessInterface::Node *snk)=0;

  // return the transitive information computed from src to snk (needs to be
  // already computed)
  virtual T GetTransInfo( const GraphAccessInterface::Node *src, const GraphAccessInterface::Node *snk)=0;

  // record the transitive information e computed from src to snk
  virtual void SetTransInfo( GraphAccessInterface::Node *src, GraphAccessInterface::Node *snk, T e) =0;

  // for debugging purpose only
  virtual void Dump() const {}

  virtual ~TransInfoGraph() {}
};

// class used in the implementation of transitive analysis algorithm
template <class T> class TransAnalSCCCreate;

/* This class implements the transitive path analysis algorithm.
  T is the specific implementation of the transitive path information (result
  of transitive analysis).
*/
template <class T>
class GraphTransAnalysis 
{
  TransAnalSCCCreate<T> *impl; 
 public:
  // constructor of GraphTransAnalysis operator. The GraphAccessInterface object is
  // the input graph to perform transitive analysis on. The TransInfoOP
  // object defines the required operators (union, composition, closure) on
  // the transitive information. 
  // limitSplitNode reduces the cost of performing transitive analysis so
  // that when the algorithm can conservatively make approximations when the
  // result transitive info is likely to be BOTTOM
  GraphTransAnalysis( GraphAccessInterface *graph, TransInfoOP<T> *op, 
                        int limitSplitNode);

  ~GraphTransAnalysis();

  // compute the transitive info between node n1 and n2. Record transitive
  // analysis results into tg
  bool ComputeTransInfo(TransInfoGraph<T>* tg, GraphAccessInterface::Node *n1, GraphAccessInterface::Node *n2);
};
 

#endif
