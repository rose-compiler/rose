#ifndef DEF_USE_CHAIN_H
#define DEF_USE_CHAIN_H

#include <ReachingDefinition.h>
#include <StmtInfoCollect.h>

class DefUseChainNode : public GraphNode
{
  bool isdef;
  AstNodePtr ref, stmt;
 public:
  DefUseChainNode( GraphCreate* c, const AstNodePtr& _ref, 
		   const AstNodePtr& _stmt, bool def)
		// HK 7/31/2007 changed the order of initialization
    //: GraphNode(c), ref(_ref), stmt(_stmt), isdef(def) {}
    : GraphNode(c), isdef(def), ref(_ref), stmt(_stmt) {}
  bool is_definition() const { return isdef; }
  AstNodePtr get_ref() const { return ref; }
  AstNodePtr get_stmt() const { return stmt; }
  void Dump() const;
  std::string ToString() const;
};

  
template <class Node>
class DefUseChain 
  : public IDGraphCreateTemplate<Node, GraphEdge>
{
 public:
  typedef typename IDGraphCreateTemplate<Node, GraphEdge>::NodeIterator NodeIterator;
  typedef typename IDGraphCreateTemplate<Node, GraphEdge>::EdgeIterator EdgeIterator;
  
  DefUseChain( BaseGraphCreate* c) 
    : IDGraphCreateTemplate<Node, GraphEdge>(c) {}

  virtual Node* CreateNode( AstInterface& fa, const AstNodePtr& ref, 
			    const AstNodePtr& stmt, bool def)
    {
      Node* n = new Node(this, ref, stmt, def);
      CreateBaseNode(n);

      return n;
    }
  GraphEdge* CreateEdge( Node* n1, Node* n2)
    {
      GraphEdge* e = new GraphEdge(this);
      CreateBaseEdge( n1, n2, e);
      return e;
    }
  
  void build( AstInterface& fa,
	      ReachingDefinitionAnalysis& r, 
              AliasAnalysisInterface& alias,
	      FunctionSideEffectInterface* f = 0);
  void build(AstNodePtr root, AliasAnalysisInterface* alias = 0, 
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

#endif

