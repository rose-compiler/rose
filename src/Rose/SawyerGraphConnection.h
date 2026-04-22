#include "RoseFirst.h"
#include "FlowGraphInterface.h"
#include <Sawyer/Graph.h>

#ifndef ROSE_SawyerGraphConnection_H
#define ROSE_SawyerGraphConnection_H

namespace Rose {
namespace FlowGraphInterface {

template <class NodeInfo, class EdgeInfo>
class SawyerCtrlflowGraph
   : public Sawyer::Container::Graph<NodeInfo, EdgeInfo>,
     public CtrlflowGraphInterface<
                 typename Sawyer::Container::Graph<NodeInfo,EdgeInfo>::ConstVertexIterator, 
                 typename Sawyer::Container::Graph<NodeInfo,EdgeInfo>::ConstEdgeIterator, NodeInfo, EdgeInfo>  {
   public: 
     typedef typename Sawyer::Container::Graph<NodeInfo,EdgeInfo>::ConstVertexIterator NodeIterator;
     typedef typename Sawyer::Container::Graph<NodeInfo,EdgeInfo>::ConstEdgeIterator EdgeIterator;
     typedef typename Sawyer::Container::Graph<NodeInfo, EdgeInfo> SawyerBase;
     typedef typename boost::iterator_range<EdgeIterator> EdgeCollectionType;
     typedef typename boost::iterator_range<NodeIterator> NodeCollectionType;
     typedef typename Rose::FlowGraphInterface::CtrlflowGraphInterface<
                 typename Sawyer::Container::Graph<NodeInfo,EdgeInfo>::ConstVertexIterator,
                 typename Sawyer::Container::Graph<NodeInfo,EdgeInfo>::ConstEdgeIterator, NodeInfo, EdgeInfo> FlowGraphInterfaceType;
     using typename FlowGraphInterfaceType::NodePredicate;
     using typename FlowGraphInterfaceType::EdgePredicate;

     virtual NodeCollectionType getNodes(NodePredicate = noFilter<NodeIterator>() ) const override
        { return Sawyer::Container::Graph<NodeInfo, EdgeInfo>::vertices(); }
     virtual EdgeCollectionType getEdges(EdgePredicate = noFilter<EdgeIterator>()) const override 
        { return Sawyer::Container::Graph<NodeInfo, EdgeInfo>::edges(); }

     virtual const NodeInfo& dereferenceNode(const NodeIterator& p) const { return (*p).value(); }
     virtual const EdgeInfo& dereferenceEdge(const EdgeIterator& p) const { return (*p).value(); }

     virtual NodeIterator edgeSource(const EdgeIterator& p) const override 
             { return (*p).source(); }
     virtual NodeIterator edgeTarget(const EdgeIterator& p) const override 
             { return (*p).target(); }

     virtual EdgeCollectionType getEdgesFrom(const NodeIterator& p, EdgePredicate = noFilter<EdgeIterator>()) const override {
              return (*p).outEdges(); }
     virtual EdgeCollectionType getEdgesTo(const NodeIterator& p, EdgePredicate = noFilter<EdgeIterator>()) const override {
                   return (*p).inEdges(); }

     virtual NodeIterator addNode(const NodeInfo& node) override 
            { return SawyerBase::insertVertex(node); }
     virtual EdgeIterator addEdge(const NodeIterator& p1, const NodeIterator& p2, 
                                       const EdgeInfo& t) override 
            { return SawyerBase::insertEdge(p1, p2, t); }
   }; 
}; // ROSE_FlowGraphInterface
}; // Rose

#endif
