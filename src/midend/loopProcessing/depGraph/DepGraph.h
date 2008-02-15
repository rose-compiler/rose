
#ifndef DEP_GRAPH
#define DEP_GRAPH

#include <IDGraphCreate.h>
#include <DepInfoSet.h>
#include <IteratorCompound.h>

class DepInfoToString {
 public:
  static std::string ToString(const DepInfo& info) { return info.ToString(); }
  static std::string ToString(const DepInfoSet& info) { return info.ToString(); }
};

class DepInfoEdge : public GraphEdgeTemplate<DepInfo, const DepInfo&, DepInfoToString> 
{
  typedef  GraphEdgeTemplate<DepInfo, const DepInfo&, DepInfoToString> BaseClass;
 public:
  DepInfoEdge(GraphCreate* c, const DepInfo& d) : BaseClass(c, d) {} 
};

typedef GraphEdgeTemplate<DepInfoSet,const DepInfoSet&, DepInfoToString> DepInfoSetEdge;
typedef Iterator2Impl<GraphEdge*,DepInfoEdge*> DepInfoEdgeIteratorImpl;
typedef Iterator2Wrap<GraphEdge*,DepInfoEdge*> DepInfoEdgeIterator;
typedef Iterator2Impl<GraphEdge*,DepInfoSetEdge*> DepInfoSetEdgeIteratorImpl;
typedef Iterator2Wrap<GraphEdge*,DepInfoSetEdge*> DepInfoSetEdgeIterator;

class SelectDepType
{
  DepType sel;
 public:
  SelectDepType(DepType t) : sel(t) {}
//Boolean operator() ( const DepInfoEdge *e) const
  int operator() ( const DepInfoEdge *e) const
      { return (e->GetInfo().GetDepType() & sel) != 0; }
//Boolean operator() ( const DepInfo &info) const
  int operator() ( const DepInfo &info) const
     { return (info.GetDepType() & sel) != 0; }
};

class SelectDepLevel
{
  int level;
 public:
  SelectDepLevel(int t) : level(t) {}
//Boolean operator() ( const DepInfoEdge* e)  const
  int operator() ( const DepInfoEdge* e)  const
   { return operator()( e->GetInfo()); }
//Boolean operator() ( const DepInfo &d) const
  int operator() ( const DepInfo &d) const
   {
     int num = d.CommonLevel();
     if ( level <= num) {
        return level <= d.CarryLevel();
     }
     return false;
   }
};

template <class Node>
class DepInfoGraphCreate : public IDGraphCreateTemplate<Node,DepInfoEdge> 
{
 public:
  DepInfoGraphCreate(BaseGraphCreate *_impl = 0) 
     : IDGraphCreateTemplate<Node,DepInfoEdge>(_impl) {}
  DepInfoEdge* CreateEdge( Node *n1, Node *n2, const DepInfo &info)
       { assert(!info.IsTop());
         DepInfoEdge *e = new DepInfoEdge(this, info);
         CreateBaseEdge(n1, n2, e);
         return e;
       }
  void DeleteEdge(DepInfoEdge *e) 
      { IDGraphCreateTemplate<Node,DepInfoEdge>::DeleteEdge(e); }
};

template <class Node>
class DepInfoSetGraphCreate : public IDGraphCreateTemplate<Node,DepInfoSetEdge>
{
 public:
  DepInfoSetGraphCreate(BaseGraphCreate *_impl = 0)
     : IDGraphCreateTemplate<Node,DepInfoSetEdge>(_impl) {}

  DepInfoSetEdge* CreateEdge( Node *n1, Node *n2, const DepInfo &info)
       { assert(!info.IsTop());
          DepInfoSetEdge *e = CreateEdge(n1,n2, DepInfoSet());
         e->GetInfo().AddDep(info);
         return e;
       }
  DepInfoSetEdge* CreateEdge(Node *n1, Node* n2, const DepInfoSet& tmp)
      { DepInfoSetEdge *e = GraphGetCrossEdge<DepInfoSetGraphCreate<Node> >()(this, n1,n2);
         if (e == 0) {
           e = new DepInfoSetEdge(this, tmp);
           CreateBaseEdge(n1,n2, e);
         }
         else
           e->GetInfo() |= tmp;
         return e;
      }
  void DeleteEdge(DepInfoSetEdge *e) 
      { IDGraphCreateTemplate<Node,DepInfoSetEdge>::DeleteEdge(e); }
};

class DepEdgeGetUpdateInfoIterator
{
 public:
  DepInfoUpdateIterator operator() (DepInfoEdge *e) const
    { return new IteratorImplTemplate<DepInfo&, SingleIterator<DepInfo&> >
                 (SingleIterator<DepInfo&>(e->GetInfo())); }
  DepInfoUpdateIterator operator() (DepInfoSetEdge *e) const
     { return e->GetInfo().GetUpdateIterator(); }
};

class DepEdgeGetConstInfoIterator
{
 public:
  DepInfoConstIterator operator()(const DepInfoEdge *e) const
     { return new IteratorImplTemplate<DepInfo, SingleIterator<DepInfo> >
                  (SingleIterator<DepInfo>(e->GetInfo()) ); }
  DepInfoConstIterator operator()(const DepInfoSetEdge *e)  const
     { return e->GetInfo().GetConstIterator(); }
};

class DepInfoEdgeGetConstInfo
{ public:
   DepInfo operator()( const DepInfoEdge *e) const { return e->GetInfo(); }
};
class DepInfoEdgeGetUpdateInfo
{ public:
   DepInfo& operator()( DepInfoEdge* e) const { return e->GetInfo(); }
};
typedef SingleCrossIterator<DepInfoEdge*, DepInfo, DepInfoEdgeIterator,
                            DepInfoEdgeGetConstInfo>
        DepInfoEdgeConstInfoIterator;
typedef SingleCrossIterator<DepInfoEdge*, DepInfo&, DepInfoEdgeIterator,
                            DepInfoEdgeGetUpdateInfo>
        DepInfoEdgeUpdateInfoIterator;

typedef MultiCrossIterator<DepInfo,DepInfoSetEdgeIterator,DepInfoConstIterator,
                           DepEdgeGetConstInfoIterator>
        DepInfoSetEdgeConstInfoIterator;
typedef MultiCrossIterator<DepInfo&,DepInfoSetEdgeIterator,
                           DepInfoUpdateIterator, DepEdgeGetUpdateInfoIterator>
        DepInfoSetEdgeUpdateInfoIterator;

class DepEdgeSetGetConstInfoIterator
{
 public:
  DepInfoConstIterator operator()(const DepInfoEdgeIterator iter) const
   { return new IteratorImplTemplate<DepInfo, DepInfoEdgeConstInfoIterator>(iter); }
  DepInfoConstIterator operator()(const DepInfoSetEdgeIterator iter)  const
  { return new IteratorImplTemplate<DepInfo,DepInfoSetEdgeConstInfoIterator>(iter);}
};

#endif
