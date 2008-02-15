
#ifndef GRAPH_TEMPLATE
#define GRAPH_TEMPLATE

#include <GraphInterface.h>

template <class NodeInfo, class Init, class tostring>
class GraphNodeTemplate : public GraphNode
{
  NodeInfo nodeInfo;
 protected:
  virtual ~GraphNodeTemplate() {}
 public:
  GraphNodeTemplate(GraphCreate *c, Init  _info)
    : GraphNode(c), nodeInfo(_info) {}
  NodeInfo&  GetInfo() { return nodeInfo; }
  NodeInfo GetInfo() const { return nodeInfo; }
  std::string ToString() const { return tostring::ToString(nodeInfo); }
};

template <class EdgeInfo,class Init, class tostring>
class GraphEdgeTemplate : public GraphEdge
{
  EdgeInfo edgeInfo;
 protected:
  virtual ~GraphEdgeTemplate() {}
 public:
  GraphEdgeTemplate(GraphCreate *c, Init _info)
    : GraphEdge(c), edgeInfo(_info) {}
  EdgeInfo&  GetInfo() { return edgeInfo; }
  EdgeInfo GetInfo() const { return edgeInfo; }
  std::string ToString() const { return tostring::ToString(edgeInfo); }
};

template <class NodeInfo, class Init, class tostring>
class GraphObserveNodeTemplate 
  : public GraphNodeTemplate<NodeInfo, Init, tostring>, public ObserveObject<GraphObserver>
{
 protected:
  virtual ~GraphObserveNodeTemplate() 
     { 
          GraphDeleteNodeInfo info(this);
          Notify(info);
      }
 public:
  GraphObserveNodeTemplate(GraphCreate *c, Init  _info)
    : GraphNodeTemplate<NodeInfo,Init,tostring>(c,_info) {}
};

template <class EdgeInfo,class Init, class Observer, class tostring>
class GraphObserveEdgeTemplate : GraphEdgeTemplate<EdgeInfo, Init, tostring>, public ObserveObject<Observer>
{
 protected:
  ObserveObject<Observer>::Notify;
  virtual ~GraphObserveEdgeTemplate() 
    {
       GraphDeleteEdgeInfo info(this);
       Notify(info);
    }
 public:
  GraphObserveEdgeTemplate(GraphCreate *c, Init _info)
    : GraphEdgeTemplate<EdgeInfo,Init, tostring>(c, _info) {}
};



// DQ (3/21/2006): Contribution for graph filtering from Andreas (using Functor interface)
/*************************************************************************************************
 * The function
 *   void filterGraph(_GraphStructure& graph, _NodePredicate _nodePred)
 * will filter out all nodes which satisfy the criteria which makes
_NodePredicate return 'true'.
 * _NodePredicate is any function object that takes a
_GraphStructure::Node* as a paramater and
 * returns a bool. In order for this to work a function
_GraphStructure::DeleteNode must be implemented
 * in order to Delete the nodes. This exact function is implemented as
protected in IDGraphCreateTemplate
 * so it should be easy.
 *************************************************************************************************/
template<typename _GraphStructure, typename _NodePredicate>
void filterGraph(_GraphStructure& graph, _NodePredicate _nodePred){
     typename _GraphStructure::NodeIterator it1 =  graph. GetNodeIterator();
     while(!it1.ReachEnd()){
      typename _GraphStructure::Node* x1 = *it1;
      ROSE_ASSERT(x1!=NULL);
      _nodePred(x1);

      it1++;

      if(_nodePred(x1)==true)
         graph.DeleteNode(x1);

     }
};


#endif
