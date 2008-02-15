
#ifndef ID_GRAPH_CREATE_C
#define ID_GRAPH_CREATE_C

#include <general.h>

#if 1
// pmp 08JUN05
//   reorderd include files
//   cmp comment in ./IDGraphCreate.h
  #include <IDGraphCreate.h>
  #include <SinglyLinkedList.h>
  #include <DGBaseGraphImpl.h>
#else
  #include <SinglyLinkedList.h>
  #include <DGBaseGraphImpl.h>
  #include <IDGraphCreate.h>
#endif

template<class NodeImpl, class EdgeImpl>
NodeImpl* IDGraphCreateTemplate<NodeImpl,EdgeImpl>:: 
GetIDNode( const BaseGraphCreate::Node *baseNode) const
{ return static_cast<NodeImpl*> (baseNode->GetInfo()); }

template<class NodeImpl, class EdgeImpl>
BaseGraphCreate::Node* IDGraphCreateTemplate<NodeImpl,EdgeImpl> ::
GetBaseNode( const NodeImpl *id) const
{ return nodeMap.Map(const_cast<NodeImpl*>(id)); }

template<class NodeImpl, class EdgeImpl>
EdgeImpl* IDGraphCreateTemplate<NodeImpl,EdgeImpl> :: 
GetIDEdge( const BaseGraphCreate::Edge *baseEdge) const
{ return static_cast<Edge*>(baseEdge->GetInfo()); }

template<class NodeImpl, class EdgeImpl>
BaseGraphCreate::Edge* IDGraphCreateTemplate<NodeImpl,EdgeImpl> :: 
GetBaseEdge( const EdgeImpl *id) const
{
  return edgeMap.Map( const_cast<EdgeImpl*>(id));
}

template<class NodeImpl, class EdgeImpl>
//Boolean IDGraphCreateTemplate<NodeImpl,EdgeImpl> :: DeleteNode(NodeImpl* n)
int IDGraphCreateTemplate<NodeImpl,EdgeImpl> :: DeleteNode(NodeImpl* n)
{ 
     for (EdgeIterator edgeIter1 = GetNodeEdgeIterator( n, GraphAccess::EdgeOut); 
          !edgeIter1.ReachEnd(); edgeIter1++) {
        Edge *e = edgeIter1.Current(); 
        if (GetEdgeEndPoint(e, GraphAccess::EdgeIn) != n) {
            edgeMap.RemoveMapping(e);
            UnlinkElem(e);
        }
     }
     for (EdgeIterator edgeIter2 = GetNodeEdgeIterator( n, GraphAccess::EdgeIn);
          !edgeIter2.ReachEnd(); edgeIter2++) {
        Edge *e = edgeIter2.Current();
        edgeMap.RemoveMapping(e);
        UnlinkElem(e);
     }

  impl->DeleteNode( GetBaseNode(n) ); 
  nodeMap.RemoveMapping(n);
  return UnlinkElem(n);
}

template<class NodeImpl, class EdgeImpl>
//Boolean IDGraphCreateTemplate<NodeImpl,EdgeImpl> :: DeleteEdge(EdgeImpl* e)
int IDGraphCreateTemplate<NodeImpl,EdgeImpl> :: DeleteEdge(EdgeImpl* e)
{ 
  impl->DeleteEdge( GetBaseEdge(e) ); 
  edgeMap.RemoveMapping(e);
  return UnlinkElem(e);
}

template<class NodeImpl, class EdgeImpl>
BaseGraphCreate::Node* IDGraphCreateTemplate<NodeImpl,EdgeImpl> :: CreateBaseNode(Node *idNode)
{
  BaseGraphCreate::Node *node = nodeMap.Map(idNode);
  if (node == 0) {
     LinkElem(idNode);
     node = impl->CreateNode(idNode);
     nodeMap.InsertMapping(idNode, node);
  } 
  return node;
}

template<class NodeImpl, class EdgeImpl>
BaseGraphCreate::Edge* IDGraphCreateTemplate<NodeImpl,EdgeImpl> ::
CreateBaseEdge(NodeImpl *idSrc, NodeImpl *idSnk, EdgeImpl *idEdge)
{
 BaseGraphCreate::Edge* edge = edgeMap.Map(idEdge);
 if (edge == 0) {
    LinkElem(idEdge);
    edge = impl->CreateEdge(GetBaseNode(idSrc),GetBaseNode(idSnk),idEdge);
    edgeMap.InsertMapping(idEdge, edge);
 }
 return edge;
}

template<class NodeImpl, class EdgeImpl>
NodeImpl * IDGraphCreateTemplate<NodeImpl,EdgeImpl> :: 
GetEdgeEndPoint( const EdgeImpl *e, EdgeDirection dir) const
{
  BaseGraphCreate::Node *n = impl->GetEdgeEndPoint(GetBaseEdge(e), dir);
  return GetIDNode(n);
}

template<class NodeImpl, class EdgeImpl>
IDGraphCreateTemplate<NodeImpl,EdgeImpl> :: ~IDGraphCreateTemplate()
{
 for (NodeIterator nodes = GetNodeIterator();
      !nodes.ReachEnd(); ++nodes) {
   Node* n = nodes.Current();
   for (EdgeIterator edges = 
            GetNodeEdgeIterator( n, GraphAccess::EdgeOut);
        !edges.ReachEnd(); ++edges) {
      Edge *e = edges.Current();
      UnlinkElem(e);
   }
   UnlinkElem(n);
 }
 delete impl;
}

template<class NodeImpl, class EdgeImpl>
IDGraphCreateTemplate<NodeImpl,EdgeImpl> :: IDGraphCreateTemplate(BaseGraphCreate *_impl)
{
  impl = _impl;
  if (impl == 0)
    impl = new DGBaseGraphImpl();
}

template<class NodeImpl, class EdgeImpl>
void IDGraphCreateTemplate<NodeImpl,EdgeImpl> :: 
MoveEdgeEndPoint(EdgeImpl *e, EdgeDirection dir, NodeImpl *n)
{
  impl->MoveEdgeEndPoint(GetBaseEdge(e), GetBaseNode(n),dir);
}

template<class NodeImpl, class EdgeImpl>
void IDGraphCreateTemplate<NodeImpl,EdgeImpl> :: 
CloneNodeSet( IDGraphCreateTemplate<NodeImpl,EdgeImpl>::NodeIterator nodeIter)
{
 while (!nodeIter.ReachEnd()) {
    CreateBaseNode(nodeIter.Current());
    ++nodeIter; 
 }
}

template<class NodeImpl, class EdgeImpl>
void IDGraphCreateTemplate<NodeImpl,EdgeImpl> :: 
CloneGraph( const GraphAccessTemplate<NodeImpl,EdgeImpl> *dg)
{
  CloneNodeSet(dg->GetNodeIterator());
  for (EdgeIterator edgeIter = GraphGetEdgeIterator<GraphAccessTemplate<NodeImpl,EdgeImpl> >()(dg);
       !edgeIter.ReachEnd(); ++edgeIter) {
    EdgeImpl *e = edgeIter.Current();
    NodeImpl *n1 = dg->GetEdgeEndPoint(e, GraphAccess::EdgeOut);
    NodeImpl *n2 = dg->GetEdgeEndPoint(e, GraphAccess::EdgeIn);
    CreateBaseEdge( n1,n2, e);
  }
}

#ifndef IDGRAPH_TEMPLATE_ONLY
template class IDGraphCreateTemplate<GraphNode, GraphEdge>;
#endif

// endif for ID_GRAPH_CREATE_C
#endif
