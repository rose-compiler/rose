#ifndef TRANSFORM_ANALYSIS
#define TRANSFORM_ANALYSIS

#include <TransDepGraph.h>

template <class Node> 
struct LoopAnalInfo
{
 Node *n;
 int index;
 LoopAnalInfo( Node* _n, int _level) { n = _n; index = _level; }
};


struct LoopAlignInfo {
     int mina, maxa;
     LoopAlignInfo() { mina = 1; maxa = -1; }
     LoopAlignInfo(int _mina, int _maxa) : mina(_mina), maxa(_maxa) {}
     operator bool() { return mina <= maxa; }
     void operator &= ( const LoopAlignInfo &that)
       { if (mina < that.mina) mina = that.mina; 
         if (maxa > that.maxa) maxa = that.maxa; }
     void Set(int m1, int m2) { mina  = m1; maxa = m2; }
  };

inline LoopAlignInfo operator +(const LoopAlignInfo& info1, const LoopAlignInfo& info2)
     { return LoopAlignInfo(info1.mina + info2.mina, info1.maxa + info2.maxa); }
inline LoopAlignInfo operator -(const LoopAlignInfo& info1, const LoopAlignInfo& info2)
     { return LoopAlignInfo(info1.mina - info2.maxa, info1.maxa - info2.mina); }

template <class Node> 
class TransLoopFusible
{public:
  LoopAlignInfo operator()( TransDepGraphCreate<Node> *graph, 
                             const LoopAnalInfo<Node> &l1, 
                             const LoopAnalInfo<Node> &l2);
};

template <class Node> 
class TransLoopSlicable
{ public: bool operator() ( TransDepGraphCreate<Node> *graph, 
                          const LoopAnalInfo<Node> &l);
};

template <class Edge, class GraphCreate>
class PerfectLoopSlicable
{ public: bool operator() (GraphCreate *g, int level); };

template <class Node> class TransLoopDistributable
{ public: bool operator() ( TransDepGraphCreate<Node> *graph,
                       const LoopAnalInfo<Node> &l1,
                      const LoopAnalInfo<Node> &l2);
};

template <class Node> 
class TransLoopReversible
{ public: bool operator()( TransDepGraphCreate<Node> *graph,
                             const LoopAnalInfo<Node> &l);
};

template <class Edge, class GraphCreate> 
class PerfectLoopReversible
{ public: bool operator()( GraphCreate *g, int  level);
};

#endif
