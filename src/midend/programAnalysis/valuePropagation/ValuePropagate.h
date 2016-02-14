#ifndef VALUE_PROPAGATE_H
#define VALUE_PROPAGATE_H

#include <DefUseChain.h>
#include <ValueAnnot.h>
#include <FunctionObject.h>

class ValuePropagateNode : public DefUseChainNode
{
  HasValueDescriptor desc;
 public:
  ValuePropagateNode( MultiGraphCreate *c, const AstNodePtr& ref, 
                      const AstNodePtr& _stmt, bool def)
    : DefUseChainNode( c, ref, _stmt, def) {}

  const HasValueDescriptor& get_desc() const { return desc; }
  HasValueDescriptor& get_desc() { return desc; }
  virtual std::string toString() const;
};

class HasValueMap : public AstObserver
{
  std::map<AstNodePtr, HasValueDescriptor> valmap;

  void ObserveCopyAst(AstInterfaceImpl& fa, const AstNodePtr& orig, const AstNodePtr& copy);
 public:
  bool has_value( const AstNodePtr& ast, HasValueDescriptor* r = 0) const;
  void set_val( const AstNodePtr& ast, const HasValueDescriptor& val);

  void copy_value( AstInterfaceImpl& fa, const AstNodePtr& orig, const AstNodePtr& copy);
 friend class ValuePropagate;
};

class HasValueMapReplace 
  : public MapObject<SymbolicVal, SymbolicVal>, public SymbolicVisitor
{
  HasValueMap& valmap;
  AstInterface& fa;

  bool usedefault;
  SymbolicVal repl;

  void VisitFunction (const SymbolicFunction& u);
  SymbolicVal operator() ( const SymbolicVal& v);
 public:
  HasValueMapReplace ( AstInterface& _fa, HasValueMap& _m, bool _usedefault) 
     : valmap(_m), fa(_fa), usedefault(_usedefault) {}
};

class HasValueCodeGen : public Map2Object<AstInterface*, AstNodePtr, AstNodePtr>
{
  std::map<AstNodePtr, AstNodePtr> astmap;
 public:
  AstNodePtr operator() (AstInterface* const& fa, const AstNodePtr& orig);
};

class ValuePropagate 
: public DefUseChain<ValuePropagateNode>
{
  std::map<AstNodePtr, ValuePropagateNode*> nodemap;
  HasValueMap valmap;
  HasValueCodeGen astmap;

  virtual ValuePropagateNode* 
    CreateNode( AstInterface& fa, const AstNodePtr& ref, 
                const AstNodePtr& stmt, bool def);

 public:
  ValuePropagate( BaseGraphCreate* c = 0) 
    : DefUseChain<ValuePropagateNode>(c) {}
  void build( AstInterface& fa, const AstNodePtr& head,
                   ReachingDefinitionAnalysis& r, 
                   AliasAnalysisInterface& alias,
                   FunctionSideEffectInterface* f = 0);
  void build (AstInterface& fa, const AstNodePtr& head, 
                   AliasAnalysisInterface& alias,
                   FunctionSideEffectInterface* f = 0);

  bool known_value( const AstNodePtr& exp, 
                  HasValueDescriptor* result = 0, bool *change = 0);

  HasValueMap& get_value_map() { return valmap; }
};

#endif
