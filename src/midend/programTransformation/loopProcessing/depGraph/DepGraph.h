
#ifndef DEP_GRAPH
#define DEP_GRAPH

#include <VirtualGraphCreate.h>
#include <DepInfoSet.h>

class DepInfoEdge : public MultiGraphElemTemplate<DepInfo> 
{
 public:
  DepInfoEdge(MultiGraphCreate* c, const DepInfo& d) 
           : MultiGraphElemTemplate<DepInfo>(c, d) {} 
  std::string toString() const { return GetInfo().toString(); }
  DepInfoConstIterator get_depIterator() const
    { return new SingleIterator<DepInfo>(GetInfo()); }
};

class DepInfoSetEdge : public MultiGraphElemTemplate<DepInfoSet> 
{
 public:
  DepInfoSetEdge(MultiGraphCreate* c, const DepInfoSet& d) 
           : MultiGraphElemTemplate<DepInfoSet>(c, d) {} 
  std::string toString() const { return GetInfo().toString(); }
  DepInfoConstIterator get_depIterator() const
    { return GetInfo().GetConstIterator(); }
};

typedef IteratorImpl<DepInfoEdge*> DepInfoEdgeIteratorImpl;
typedef IteratorWrap<DepInfoEdge*, DepInfoEdgeIteratorImpl> DepInfoEdgeIterator;
template <class EdgeIterator>
class DepInfoEdgeConstInfoIterator : public IteratorImpl<DepInfo>
{
   EdgeIterator impl;
 public:
   DepInfoEdgeConstInfoIterator( const EdgeIterator& p): impl(p) {}
   DepInfo Current() const { return impl.Current()->GetInfo(); }
   void Reset() { impl.Reset(); }
   void Advance() { impl.Advance(); } 
   bool ReachEnd() const { return impl.ReachEnd(); }
   IteratorImpl<DepInfo>* Clone() const
        { return new DepInfoEdgeConstInfoIterator(*this); }
};

typedef IteratorImpl<DepInfoSetEdge*> DepInfoSetEdgeIteratorImpl;
typedef IteratorWrap<DepInfoSetEdge*, DepInfoSetEdgeIteratorImpl> 
DepInfoSetEdgeIterator;

inline bool SelectDepType( const DepInfo &info, const DepType& sel) 
     { return (info.GetDepType() & sel) != 0; }

inline bool SelectDepLevel( const DepInfo &d, int level) 
   {  
     int num1 = 0, num2 = 0;
     d.CarryLevels(num1,num2);
     return (level <= num2);
   }

template <class Node>
class DepInfoGraphCreate : public VirtualGraphCreateTemplate<Node,DepInfoEdge> 
{
 public:
  DepInfoGraphCreate(BaseGraphCreate *_impl = 0) 
     : VirtualGraphCreateTemplate<Node,DepInfoEdge>(_impl) {}
  DepInfoEdge* CreateEdge( Node *n1, Node *n2, const DepInfo &info)
       { assert(!info.IsTop());
         DepInfoEdge *e = new DepInfoEdge(this, info);
         VirtualGraphCreateTemplate<Node,DepInfoEdge>::AddEdge(n1, n2, e);
         return e;
       }
  void DeleteEdge(DepInfoEdge *e) 
      { VirtualGraphCreateTemplate<Node,DepInfoEdge>::DeleteEdge(e); }
};

template <class Node>
class DepInfoSetGraphCreate : public VirtualGraphCreateTemplate<Node,DepInfoSetEdge>
{
 public:
  DepInfoSetGraphCreate(BaseGraphCreate *_impl = 0)
     : VirtualGraphCreateTemplate<Node,DepInfoSetEdge>(_impl) {}

  DepInfoSetEdge* CreateEdge( Node *n1, Node *n2, const DepInfo &info)
       { assert(!info.IsTop());
          DepInfoSetEdge *e = CreateEdge(n1,n2, DepInfoSet());
         e->GetInfo().AddDep(info);
         return e;
       }
  DepInfoSetEdge* CreateEdge(Node *n1, Node* n2, const DepInfoSet& tmp)
      { 
         GraphCrossEdgeIterator<DepInfoSetGraphCreate> p(this, n1, n2);
         DepInfoSetEdge *e = p.ReachEnd()? 0 : p.Current();
         if (e == 0) {
           e = new DepInfoSetEdge(this, tmp);
           AddEdge(n1,n2, e);
         }
         else
           e->GetInfo() |= tmp;
         return e;
      }
  void DeleteEdge(DepInfoSetEdge *e) 
      { VirtualGraphCreateTemplate<Node,DepInfoSetEdge>::DeleteEdge(e); }
};

#endif
