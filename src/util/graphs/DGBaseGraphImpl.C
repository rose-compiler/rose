#include <DGBaseGraphImpl.h>
#include <DirectedGraph.C>
#include <DAG.C>

template class DirectedGraphEdge<DGBaseNodeImpl,DGBaseEdgeImpl>;
template class DirectedGraphEdge<DAGBaseNodeImpl, DAGBaseEdgeImpl>;
template class DirectedGraphNode<DGBaseNodeImpl,DGBaseEdgeImpl>;
template class DirectedGraphNode<DAGBaseNodeImpl,DAGBaseEdgeImpl>;
template class DAGEdge<DAGBaseNodeImpl,DAGBaseEdgeImpl>;
template class DAGNode<DAGBaseNodeImpl,DAGBaseEdgeImpl>;

