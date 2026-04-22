#ifndef ROSE_FlowGraphInterface_H
#define ROSE_FlowGraphInterface_H

#include <RoseFirst.h>

#include <vector>
#include <functional>
#include <boost/range/iterator_range.hpp>

namespace Rose{
namespace FlowGraphInterface {

enum MutationSupportOption { None, NodeInsertion, EdgeInsertion, NodeDeletion, EdgeDeletion};

class TraversalSupportOption {
  int t_;
 public:
  bool operator == (const TraversalSupportOption& t1) const { return t_ == t1.t_; }
  // Below are possible options of node traversals supported by a graph.
  static constexpr auto ENTRY_NODES_TRAVERSAL = 1;
  static constexpr auto EXIT_NODES_TRAVERSAL = 2;
  static constexpr auto INTERNAL_NODES_TRAVERSAL = 4;
  static constexpr auto OUT_EDGES_TRAVERSAL = 8;
  static constexpr auto IN_EDGES_TRAVERSAL = 16;
  static constexpr auto ALL_NODES_OUT_EDGES_TRAVERSAL = ENTRY_NODES_TRAVERSAL + EXIT_NODES_TRAVERSAL + INTERNAL_NODES_TRAVERSAL + OUT_EDGES_TRAVERSAL;
  static constexpr auto ALL_NODES_IN_EDGES_TRAVERSAL = ENTRY_NODES_TRAVERSAL + EXIT_NODES_TRAVERSAL + INTERNAL_NODES_TRAVERSAL + IN_EDGES_TRAVERSAL;
  // Below are examples of definition different combinations of traversals supported.
  // In essence, combine all supported traversal options via the | or + operator.
  static constexpr auto BOUNDARY_NODES_TRAVERSAL=ENTRY_NODES_TRAVERSAL | EXIT_NODES_TRAVERSAL;

  TraversalSupportOption(int t = ALL_NODES_OUT_EDGES_TRAVERSAL) : t_(t) {}
  bool isIncludedTraversal(int t) const { return t_ & t; }
  bool isTraversal(int t) const { return t_ == t; }

  template <class Iterator>
  auto traversalSupportFilter() const // optional: -> std::function<bool(EdgeIterator)>
  {
    return [this](Iterator it) -> bool
           {
             return isIncludedTraversal(it->traversal_kind());
           };
  }
};

  template<class T>
  auto
  noFilter() // optional: -> std::function<bool(T)>
  {
    return [](T) -> bool { return true; };
  }



// Provide an interface for traversing nodes in a directed graph.
// Node: content of the nodes; NodeIterator: key to access nodes of the graph.
// Edge traversal is not supported directly here and is provided in FlowGraphNode
// DONE: make the names more consistent (with vs without Type at the end): renamed types to options, support,etc.
template <class NodeIterator, class EdgeIterator,
          class NodeCollection=boost::iterator_range<NodeIterator>,
          class EdgeCollection=boost::iterator_range<EdgeIterator>>
class FlowGraphAccessInterface{
 public:
  using NodeCollectionType = NodeCollection;
  using EdgeCollectionType = EdgeCollection;

  using NodePredicate = std::function<bool(NodeIterator)>;
  using EdgePredicate = std::function<bool(EdgeIterator)>;

  // Return all the nodes that meet the given predicate.
  // If the requested traversal is not supported, return a pair of empty iterators.
  virtual NodeCollection getNodes(NodePredicate = noFilter<NodeIterator>() ) const = 0;

  // Return all the edges that meet the given predicate.
  // If the requested traversal is not supported, return a pair of empty iterators.
  virtual EdgeCollection getEdges(EdgePredicate = noFilter<EdgeIterator>()) const = 0;

  // Assume each edge is directed and has a single source and a single target.
  virtual NodeIterator edgeSource(const EdgeIterator& p) const  = 0;
  virtual NodeIterator edgeTarget(const EdgeIterator& p) const  = 0;

   // Returns edges connected to the given node filtered by the given traversal support.
  // If the requested traversal is not supported, return a pair of empty iterators.
  virtual EdgeCollection getEdgesFrom(const NodeIterator&, EdgePredicate = noFilter<EdgeIterator>()) const { assert("Error: outgoing edge traversal not supported!"); return EdgeCollection(); }
  virtual EdgeCollection getEdgesTo(const NodeIterator&, EdgePredicate = noFilter<EdgeIterator>()) const { assert("Error: incomng edge traversal not supported!"); return EdgeCollection(); }

  virtual enum MutationSupportOption allowMutationDuringTraversal() const { return MutationSupportOption::None; }
  virtual TraversalSupportOption getTraversalSupport() const { return TraversalSupportOption(); }
};

// Maybe add a template parameter to represent what type of iterator invalidation. Instantiate EdgeIterator with void if not need it.
template <class NodeInfo, class EdgeInfo, class NodeIterator, class EdgeIterator>
class FlowGraphCreateInterface{
  public:
      // For correctness, the underlying implementation must ensure the returned NodeIterator object
      // is not invalidated after more nodes are added into the graph via additional calls of addNode.
      // Returning a NodeIterator value instead of const NodeIterator&
     virtual NodeIterator addNode(const NodeInfo& node) = 0;

     // It may happen The Edgeterator stays valid only if no additional node or edge is added.
     virtual EdgeIterator addEdge(const NodeIterator& p1, const NodeIterator& p2, const EdgeInfo& t) = 0;

     // TODO: What about iterators being invalidated while creating. Documentation. Add API to specify whether an iterator is still valid?
};


class SgNode;

// Control flows can be traversed forward (outgoing) or backward (incoming), with each edge conditioned with the boolean value of the conditional branch.
template <class NodeIterator, class EdgeIterator,
          class NodeInfo=std::vector<SgNode*>, class EdgeInfo=TraversalSupportOption,
          class NodeCollection=boost::iterator_range<NodeIterator>,
          class EdgeCollection=boost::iterator_range<EdgeIterator>>
class CtrlflowGraphInterface :
     public FlowGraphAccessInterface<NodeIterator,EdgeIterator,NodeCollection,EdgeCollection>,
     public FlowGraphCreateInterface<NodeInfo,EdgeInfo,NodeIterator,EdgeIterator>  {
 public:
  using NodeCollectionType = NodeCollection;
  using EdgeCollectionType = EdgeCollection; 
  virtual const NodeInfo& dereferenceNode(const NodeIterator& p) const = 0;
  virtual const EdgeInfo& dereferenceEdge(const EdgeIterator& p) const = 0;
};


// Data flows can be speculative (maybe_flow, which depends on runtime values), or non-speculative (must_flow, which does not depend on runtime info). It can flow from a definition to a use of a memory store (DEF_USE_FLOW), from a use to a definition (USE_DEF_FLOW, which is the reverse of DEF_USE_FLOW), or from a use to a memory whose value is computed by combining all the incoming used values (USE_JOIN_FLOW).
template <class NodeIterator, class EdgeIterator,
          class NodeInfo=SgNode*, class EdgeInfo=TraversalSupportOption,
          class NodeCollection=boost::iterator_range<NodeIterator>,
          class EdgeCollection=boost::iterator_range<EdgeIterator>>
class DefUseGraphInterface :
  public FlowGraphAccessInterface<NodeIterator, EdgeIterator, NodeCollection, EdgeCollection>,
  public FlowGraphCreateInterface<NodeInfo, EdgeInfo, NodeIterator, EdgeIterator> {
 public:
  using NodeCollectionType = NodeCollection;
  using EdgeCollectionType = EdgeCollection; 
  virtual const NodeInfo& dereferenceNode(const NodeIterator& p) const = 0;
  virtual const EdgeInfo& dereferenceEdge(const EdgeIterator& p) const = 0;
};

}; // namespace FlowGraphInterface
}; // namespace Rose

#endif
