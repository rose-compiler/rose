
#include <ValuePropagate.h>
#include <SinglyLinkedList.h>
#include <CommandOptions.h>
#include <GraphIO.h>
#include <GraphUtils.h>

bool DebugValuePropogate()
{
  static int r = 0;
  if (r == 0 ) {
     if (CmdOptions::GetInstance()->HasOption("-debugvalueprop"))
        r = 1;
     else 
        r = -1;
  }
  return r == 1;
}

std::string ValuePropagateNode:: toString() const
{
  std::stringstream out;
  out << DefUseChainNode::toString();
  out << "ref address: " << get_ref().get_ptr(); 
  out << "has_value: ";
  out << desc.ToString();
  return out.str();
}

bool HasValueMap :: has_value( const AstNodePtr& ast, HasValueDescriptor* r) const
   {
     std::map<AstNodePtr,HasValueDescriptor>::const_iterator p = valmap.find(ast);
     if (p == valmap.end())
         return false;
     if (r != 0)
       *r = (*p).second;
     return true;
   }
void HasValueMap:: set_val( const AstNodePtr& ast, const HasValueDescriptor& val)
  { 
    std::map<AstNodePtr,HasValueDescriptor>::iterator p = valmap.find(ast);
    if (p == valmap.end())
      valmap[ast] = val;
    else
      (*p).second.merge(val);
    if (DebugValuePropogate())  {
       std::cerr << "set value for " << ast.get_ptr() << ":" << AstToString(ast) << " : ";
       valmap[ast].Dump();
       std::cerr << std::endl;
    }
  }

AstNodePtr 
HasValueCodeGen :: operator() (AstInterface* const& fa, const AstNodePtr& orig)
{
  if (fa->IsConstant(orig))
      return fa->CopyAstTree(orig);
  std::map<AstNodePtr, AstNodePtr>::const_iterator p = astmap.find(orig);
  if (p != astmap.end()) { 
      AstNodePtr r = (*p).second;
      return fa->CopyAstTree(r); 
  }
  AstNodeType valtype;
  if (fa->IsExpression( orig, &valtype) == AST_NULL)
     assert(false);
  std::string varname = fa->NewVar( valtype);
  AstNodePtr var = fa->CreateVarRef (varname);
  astmap[orig] = var;
  AstNodePtr assign = fa->CreateAssignment( fa->CopyAstTree(var), fa->CopyAstTree(orig)); 
  fa->ReplaceAst( orig, assign); 
  return var;
}

SymbolicVal HasValueMapReplace :: operator() ( const SymbolicVal& v)
   {
      repl = SymbolicVal();
      v.Visit(this);
      return repl;
   }

void HasValueMapReplace :: VisitFunction (const SymbolicFunction& u) 
   {
    const SymbolicDotExp *dot = dynamic_cast<const SymbolicDotExp*>(&u);
    if (dot != 0 && dot->first_arg().GetValType() == VAL_AST) {
       AstNodePtr curast;
       if (!dot->first_arg().isAstWrap(curast)) assert(false);
       std::string field = dot->last_arg().toString();
       HasValueDescriptor curval;
       SymbolicValDescriptor replval;
       bool hasval = valmap.has_value( curast, &curval);
       if ( hasval && curval.has_value( field, &replval ) 
                 &&  !replval.is_top() &&  !replval.is_bottom())  {
               repl = replval;
       }
       else if (usedefault && ValueAnnotation::get_inst()->known_type( fa, curast, &curval)
             && curval.has_value( field, &replval ) ) {
               repl = replval; 
       }
    }
   }

class AppendValueNode : public Collect2Object< AstNodePtr, HasValueDescriptor >
{
  const std::map<AstNodePtr, ValuePropagateNode*>& nodemap;
  HasValueMap& valmap;
  CollectObject< ValuePropagateNode*>* nodeCollect;
 public:
  AppendValueNode( const std::map<AstNodePtr, ValuePropagateNode*>& m,
                   HasValueMap& v)
   : nodemap(m), valmap(v), nodeCollect(0) {} 

  HasValueMap& get_val_map() { return valmap; }
  void set_node_collect( CollectObject< ValuePropagateNode*>& n) 
    { nodeCollect = &n; }
  bool has_value( const AstNodePtr& ast, HasValueDescriptor* r = 0)
   {
     return valmap.has_value(ast, r);
   }

  bool operator() ( const AstNodePtr& curast, const HasValueDescriptor& curval)
  {
    valmap.set_val( curast, curval);
    if (nodeCollect != 0) {
      std::map<AstNodePtr,ValuePropagateNode*>::const_iterator p = nodemap.find(curast); 
      if (p != nodemap.end()) {
         if ((*p).second->get_desc().merge( curval) ) {
             (*nodeCollect)( (*p).second);
         }
         if (DebugValuePropogate()) {
            std::cerr << "found node for ref: " << AstToString(curast) << std::endl;
            (*p).second->Dump();
         } 
      }
    }
    return true;
  }
};

class CollectKnownValue 
  : public ProcessAstNode
{
  HasValueMap& valmap;
  HasValueCodeGen& astcodegen;
  AppendValueNode& append;
  SymbolicVal repl;
 public:
  CollectKnownValue(HasValueMap& m, HasValueCodeGen& cg, AppendValueNode& p) 
     : valmap(m), astcodegen(cg), append(p)  {}
  virtual bool Traverse( AstInterface &fa, const AstNodePtr& s,
                               AstInterface::TraversalVisitType t)
   {
       HasValueMapReplace valrepl( fa, valmap, false);
       if (ValueAnnotation::get_inst()->is_value_restrict_op( fa, s, &append, &valrepl, &astcodegen)) 
             return true;
       AstNodePtr lhs, rhs;
       AstInterface::AstNodeList vars, args;
       HasValueDescriptor desc;
       if (fa.IsAssignment(s, &lhs, &rhs)) {
          if (append.has_value( rhs, &desc) ) {
             append( lhs, desc);
          }
       }
       else if (fa.IsVariableDecl( s, &vars, &args)) {
          AstInterface::AstNodeList::iterator pv = vars.begin();
          AstInterface::AstNodeList::iterator pa = args.begin();
          while (pv != vars.end()) {
            lhs = *pv;
            rhs = *pa;
            if (append.has_value( rhs, &desc) ) {
                append( lhs, desc );
            }
            ++pv;
            ++pa;
          }
       }

       return true;
   } 

  void collect( AstInterface& fa, const AstNodePtr& h) 
   {
      ReadAstTraverse( fa, h, *this, AstInterface::PostOrder); 
   }
};


class UpdateValuePropagateNode 
  : public UpdateDefUseChainNode<ValuePropagateNode>
{
  HasValueMap& valmap;
  HasValueCodeGen& astcodegen;
  AppendValueNode valappend;
  AstInterface& fa;
  AstNodePtr head;
public:
  UpdateValuePropagateNode( AstInterface& _fa, const AstNodePtr& h,
                            HasValueMap& vm, HasValueCodeGen& cg, 
                            const std::map<AstNodePtr, ValuePropagateNode*>& m) 
    : valmap(vm), astcodegen(cg), valappend(m, valmap), fa(_fa), head(h) {}
  void init(CollectObject<ValuePropagateNode*>& append ) 
  {
     valappend.set_node_collect(append);
     CollectKnownValue op( valmap, astcodegen, valappend);
     op.collect( fa, head);  
  }
  
  bool update_def_node( ValuePropagateNode* def, const ValuePropagateNode* use,
                        CollectObject<ValuePropagateNode*>& append)
  {
    if (def->get_desc().merge(use->get_desc())) {
       valmap.set_val( def->get_ref(), def->get_desc());
       valappend.set_node_collect(append);
       CollectKnownValue op( valmap, astcodegen, valappend);
       op.collect( fa, def->get_stmt());
       return true;
    }
    return false; 
  }
  bool update_use_node( ValuePropagateNode* use, const ValuePropagateNode* def,
                        CollectObject<ValuePropagateNode*>& append)
  {
    if (use->get_desc().merge(def->get_desc())) {
       valmap.set_val( use->get_ref(), use->get_desc());
       valappend.set_node_collect(append);
       CollectKnownValue op( valmap, astcodegen, valappend);
       op.collect( fa, use->get_stmt());
       return true;
    }
    return false;
  }
};

ValuePropagateNode* ValuePropagate::
    CreateNode( AstInterface& fa, const AstNodePtr& ref,
                const AstNodePtr& stmt, bool def)
    {
      if (!ValueAnnotation::get_inst()->known_type( fa, ref) && !def)
          return 0;

      ValuePropagateNode* n = new ValuePropagateNode(this, ref, stmt, def);
      nodemap[ref] = n;
      AddNode(n);
      return n;
    }

void ValuePropagate::
build(AstInterface& fa, const AstNodePtr& h, ReachingDefinitionAnalysis& r, 
           AliasAnalysisInterface& alias, FunctionSideEffectInterface* f) 
{
  if (DebugValuePropogate())
     std::cerr << "building def-use chain\n";

  DefUseChain<ValuePropagateNode>::build(fa, r, alias, f);
  if (DebugValuePropogate())
      write_graph(*this, std::cout, "def-use");

  if (DebugValuePropogate()) {
    std::cerr << "finshed building def-use chain\n";
    std::cerr << "propagating values on def-use chain\n";
  }
  UpdateValuePropagateNode  update(fa, h, valmap, astmap, nodemap);
  PropagateDefUseChainUpdate( this, update); 
  if (DebugValuePropogate()) 
     std::cerr << "\nfinished propagating values on def-use chain\n" << GraphToString(*this) << std::endl;
}

void ValuePropagate::
build(AstInterface& fa, const AstNodePtr& head,
           AliasAnalysisInterface& alias, FunctionSideEffectInterface* f) 
{
  ReachingDefinitionAnalysis r;
  if (DebugValuePropogate())
     std::cerr << "constructing reaching definitions\n";
  r(fa, head, f);
  if (DebugValuePropogate()) 
     std::cerr << "finished reaching definition\n" << GraphToString(*this) << std::endl;
  build(fa, head, r, alias, f);
}

void HasValueMap::
ObserveCopyAst( AstInterfaceImpl& fa, const AstNodePtr& orig, const AstNodePtr& copy)
{
  copy_value( fa, orig, copy);
}

void HasValueMap::
copy_value( AstInterfaceImpl& fa, const AstNodePtr& orig, const AstNodePtr& copy)
{
  HasValueDescriptor desc;
  if (has_value( orig, &desc)) {
     set_val(copy, desc); 
     if (DebugValuePropogate()) {
        std::cerr << "copying ast: " << AstToString(copy) << copy.get_ptr() 
                 << " to have value " << desc.toString() << std::endl;
     }
  }
}

bool ValuePropagate:: 
known_value( const AstNodePtr& exp, HasValueDescriptor* result,
             bool *change)
{
  std::map<AstNodePtr, ValuePropagateNode*>::const_iterator p = nodemap.find(exp);
  if (p == nodemap.end())  {
     bool r = valmap.has_value( exp, result);
     if (change != 0)
       *change = true;
     return r;
  }
  ValuePropagateNode* node = (*p).second;
  if (result != 0)
    *result = node->get_desc();
  if (change != 0) {
    if (!node->is_definition()) 
         *change = false;
    else {
       *change = false;
       for (GraphNodePredecessorIterator<ValuePropagate> preds(this,node); 
              !preds.ReachEnd(); ++preds) {
          ValuePropagateNode* cur = *preds; 
          HasValueDescriptor tmp = cur->get_desc();
          if (tmp.merge(node->get_desc())) {
              *change = true;
              if (DebugValuePropogate()) {
                  std::cerr << "HasValue descriptors differ : " << cur->toString() << " : " 
                          << node->toString() << std::endl;
              }
              break;
          } 
       } 
    }
  }
  return true;
}

