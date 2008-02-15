#ifndef TRANSITIVE_ANALYSIS
#define TRANSITIVE_ANALYSIS

#include <GraphInterface.h>

/*
struct T
{
   T& operator = (const T &that) { return *this; }
 private:
   T() {}
};
*/

template <class T>
class TransInfoOP
{
 public:
   virtual T  GetBottomInfo( const GraphNode *n1, const GraphNode *n2)=0 ;
   virtual T  GetTopInfo(const GraphNode *n1, const GraphNode *n2)=0;
   virtual T  GetIDTransInfo( const GraphNode *n) =0;
   virtual T  GetTransInfo( const GraphEdge *e) =0;

   virtual void  UnionWith(T& info1, T info2)=0;
   virtual T Composite(T info1, T info2)=0;
   virtual T Closure(T info)=0;
};

template <class T>
class TransInfoGraph
{
 public:
//virtual Boolean TransInfoComputed(const GraphNode *src, 
  virtual int TransInfoComputed(const GraphNode *src, 
                                    const GraphNode *snk)=0;
  virtual T GetTransInfo( const GraphNode *src, const GraphNode *snk)=0;
  virtual void SetTransInfo( GraphNode *src, GraphNode *snk, T e) =0;
  virtual void Dump() const {}
};

template <class T> class TransAnalSCCCreate;
template <class T>
class GraphTransAnalysis 
{
  TransAnalSCCCreate<T> *impl; 
 public:
  GraphTransAnalysis( GraphAccess *graph, TransInfoOP<T> *op, 
                        int limitSplitNode);
  ~GraphTransAnalysis();
//Boolean ComputeTransInfo(TransInfoGraph<T>* tg, GraphNode *n1, GraphNode *n2);
  int ComputeTransInfo(TransInfoGraph<T>* tg, GraphNode *n1, GraphNode *n2);
};
 

#endif
