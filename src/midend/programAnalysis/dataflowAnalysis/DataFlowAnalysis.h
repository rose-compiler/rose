#ifndef DATA_FLOW_ANALYSIS
#define DATA_FLOW_ANALYSIS

#include <CFGImpl.h>

template <class Data>
class DataFlowNode : public CFGNodeImpl
{
 protected:
  DataFlowNode( GraphCreate* c) 
    : CFGNodeImpl(c) {}
 public:
  virtual Data get_entry_data() const = 0;
  virtual void set_entry_data( const Data& d) = 0;
  virtual Data get_exit_data() const = 0;
  virtual void apply_transfer_function() = 0;
};

template<class Node,class Data>
class DataFlowAnalysis  : public CFGImplTemplate<Node, CFGEdgeImpl>
{
  virtual Data meet_data( const Data& d1, const Data& d2) = 0;
  virtual Data get_empty_data() const = 0;
  virtual void FinalizeCFG( AstInterface& fa) = 0; 
  CFGImplTemplate<Node, CFGEdgeImpl>::GetBaseGraph;
 public:
  typedef typename CFGImplTemplate<Node, CFGEdgeImpl>::NodeIterator NodeIterator;
  typedef typename CFGImplTemplate<Node, CFGEdgeImpl>::EdgeIterator EdgeIterator;
  CFGImplTemplate<Node, CFGEdgeImpl>::GetNodeIterator;

  DataFlowAnalysis();
  void operator()( AstInterface& fa, const AstNodePtr& head);
};

#endif
