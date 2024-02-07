#ifndef MULTI_GRAPH_OBSERVE_H
#define MULTI_GRAPH_OBSERVE_H

#include <MultiGraphCreate.h>

class MultiGraphObserver
{
 public:
  virtual void UpdateDeleteNode( const MultiGraphElem*) {}
  virtual void UpdateDeleteEdge( const MultiGraphElem*) {}
  virtual ~MultiGraphObserver() {}
};

class MultiGraphDeleteNodeInfo : public ObserveInfo<MultiGraphObserver>
{
   MultiGraphElem* n;
 public:
   MultiGraphDeleteNodeInfo( MultiGraphElem *_n) : n(_n) {}
   void UpdateObserver( MultiGraphObserver& o) const
     { o.UpdateDeleteNode( n ); }
};

class MultiGraphDeleteEdgeInfo : public ObserveInfo<MultiGraphObserver>
{
   MultiGraphElem* n;
 public:
   MultiGraphDeleteEdgeInfo( MultiGraphElem *_n) : n(_n) {}
   void UpdateObserver( MultiGraphObserver& o) const
     { o.UpdateDeleteEdge( n ); }
};

template <class NodeInfo>
class MultiGraphObserveNodeTemplate 
  : public MultiGraphElemTemplate<NodeInfo>, 
    public ObserveObject<MultiGraphObserver>
{
 protected:
  virtual ~MultiGraphObserveNodeTemplate() 
     { 
          MultiGraphDeleteNodeInfo _info(this);
          ObserveObject<MultiGraphObserver>::Notify(_info);
      }
 public:
  MultiGraphObserveNodeTemplate(MultiGraphCreate *c, const NodeInfo&  _info)
    : MultiGraphElemTemplate<NodeInfo>(c,_info) {}
};

template <class EdgeInfo>
class MultiGraphObserveEdgeTemplate 
   : public MultiGraphElemTemplate<EdgeInfo>, 
     public ObserveObject<MultiGraphObserver>
{
 protected:
  virtual ~MultiGraphObserveEdgeTemplate() 
    {
       MultiGraphDeleteEdgeInfo info(this);
       ObserveObject<MultiGraphObserver>::Notify(info);
    }
 public:
  MultiGraphObserveEdgeTemplate(MultiGraphCreate *c, const EdgeInfo& _info)
    : MultiGraphElemTemplate<EdgeInfo>(c, _info) {}
};

#endif
