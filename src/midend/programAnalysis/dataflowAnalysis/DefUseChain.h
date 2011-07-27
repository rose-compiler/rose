#ifndef DEF_USE_CHAIN_H
#define DEF_USE_CHAIN_H

#include "ReachingDefinition.h"
#include "StmtInfoCollect.h"

class DefUseChainNode : public MultiGraphElem
{
  bool isdef;
  AstNodePtr ref, stmt;
 public:
  DefUseChainNode( MultiGraphCreate* c, const AstNodePtr& _ref, 
                   const AstNodePtr& _stmt, bool def)
    : MultiGraphElem(c), isdef(def), ref(_ref), stmt(_stmt) {}
  bool is_definition() const { return isdef; }
  AstNodePtr get_ref() const { return ref; }
  AstNodePtr get_stmt() const { return stmt; }
  void Dump() const;
  std::string toString() const;
};

  
template <class Node>
class DefUseChain 
  : public VirtualGraphCreateTemplate<Node, MultiGraphElem>
{
 public:
  typedef MultiGraphElem Edge;
  typedef typename VirtualGraphCreateTemplate<Node, Edge>::NodeIterator NodeIterator;
  typedef typename VirtualGraphCreateTemplate<Node, Edge>::EdgeIterator EdgeIterator;
  
  DefUseChain( BaseGraphCreate* c) 
    : VirtualGraphCreateTemplate<Node, Edge>(c) {}

  virtual Node* CreateNode( AstInterface& fa, const AstNodePtr& ref, 
                            const AstNodePtr& stmt, bool def)
    {
      Node* n = new Node(this, ref, stmt, def);
      VirtualGraphCreateTemplate<Node, Edge>::AddNode(n);

      return n;
    }
  Edge* CreateEdge( Node* n1, Node* n2)
    {
      Edge* e = new Edge(this);
      AddEdge( n1, n2, e);
      return e;
    }
  
  void build( AstInterface& fa,
              ReachingDefinitionAnalysis& r, 
              AliasAnalysisInterface& alias,
              FunctionSideEffectInterface* f = 0);
  void build(AstInterface& fa, AstNodePtr root,  
             AliasAnalysisInterface* alias = 0, 
             FunctionSideEffectInterface* f = 0);
};

template<class Node>
class UpdateDefUseChainNode {
 public:
  virtual void init(CollectObject<Node*>& newnodes ) = 0;
  virtual bool update_def_node( Node* def, const Node* use, 
                                CollectObject<Node*>& newnodes) = 0;
  virtual bool update_use_node( Node* use, const Node* def,
                                CollectObject<Node*>& newnodes) = 0;
  
  virtual ~UpdateDefUseChainNode() {}
};

class DefaultDUchain : public DefUseChain<DefUseChainNode>
{
 public:
   DefaultDUchain( BaseGraphCreate* c = 0)
    : DefUseChain<DefUseChainNode>(c) {}
};

template<class Node>
void PropagateDefUseChainUpdate( DefUseChain<Node> *graph, 
                                 UpdateDefUseChainNode<Node>& update);

#define TEMPLATE_ONLY
#include <DefUseChain.C>
#undef TEMPLATE_ONLY

#endif

