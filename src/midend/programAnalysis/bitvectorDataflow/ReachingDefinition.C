#include "ReachingDefinition.h"
#include "StmtInfoCollect.h"
#include "CommandOptions.h"
#include "GraphUtils.h"
#include "GraphIO.h"
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

bool DebugReachingDef()
{
  static int r = 0;
  if (r == 0) {
     if (CmdOptions::GetInstance()->HasOption("-debugreachingdef"))
         r = 1;
     else
         r = -1;
  }
  return r == 1;
}



class ConstructReachingDefinitionBase
  : public  CollectObject< std::pair<AstNodePtr, AstNodePtr> >
{
  ReachingDefinitionBase& base;
  AstInterface& fa;
  bool operator()( const std::pair<AstNodePtr, AstNodePtr>& mod)
  {
    std::string varname;
    AstNodePtr scope;
    if (fa.IsVarRef(mod.first, 0, &varname, &scope)) {
      base.add_ref( varname, scope, mod);
      if (DebugReachingDef())
         std::cerr << "collecting var ref: " << varname << ":" << AstInterface::AstToString(mod.second) << std::endl;
    }
    else {
      base.add_unknown_def( mod);
      if (DebugReachingDef())
         std::cerr << "collecting unknown ref: " << AstInterface::AstToString(mod.first) << ":" << AstInterface::AstToString(mod.second) << std::endl;
    }
    return true;
  }
public:
  ConstructReachingDefinitionBase( AstInterface& _fa, ReachingDefinitionBase& b)
    : base(b), fa(_fa) {}
};

void ReachingDefinitionBase::
add_ref( const std::string& varname, const AstNodePtr& scope, const std::pair<AstNodePtr,AstNodePtr>& def)
{
   std::string scopename = Ast2StringMap::inst()->get_string(scope);
   std::string name = varname + scopename;
   if (DebugReachingDef())
       std:: cerr << "adding variable name: " << name << "\n";
   add_data(name, def);
}

void ReachingDefinitionBase::
collect_refs ( AstInterface& fa, const AstNodePtr& h, FunctionSideEffectInterface* a,
               AstInterface::AstNodeList* in)
{

  for (AstInterface::AstNodeList::iterator p = in->begin();
       p != in->end(); ++p) {
     AstNodePtr cur = *p;
     std::string varname;
     AstNodePtr scope;
     if (fa.IsVarRef( cur, 0, &varname, &scope))
        add_ref(varname, scope, std::pair<AstNodePtr, AstNodePtr>(cur, AST_NULL) );
  }
  ConstructReachingDefinitionBase collect(fa, *this);
  StmtSideEffectCollect op(a);
  op(fa, h, &collect);
}

void ReachingDefinitionGenerator::
add_def( ReachingDefinitions& repr, const std::string& varname, const AstNodePtr& scope,
                const std::pair<AstNodePtr,AstNodePtr>& def) const
{
   std::string scopename = Ast2StringMap::inst()->get_string(scope);
   std::string name = varname + scopename;
   add_member(repr, name, def);
}

ReachingDefinitions ReachingDefinitionGenerator::
get_def_set( const std::string& varname, const AstNodePtr& scope) const
{
   std::string scopename = Ast2StringMap::inst()->lookup_string(scope);
   if (scopename != "") {
      std::string name = varname + scopename;
      return get_data_set(name);
   }
   /*QY: if scope is not mapped, the variable hasn't been defined*/
   return get_empty_set();
}

class CollectLocalDefinitions : public CollectObject< std::pair<AstNodePtr, AstNodePtr> >
{
  AstInterface& fa;
  std::map<std::string, std::pair<AstNodePtr, std::pair<AstNodePtr,AstNodePtr> > > defvars;
  ReachingDefinitions gen;
  const ReachingDefinitionGenerator& g;

  bool operator()( const std::pair<AstNodePtr,AstNodePtr>& mod)
  {
    std::string varname;
    AstNodePtr scope;
    if (fa.IsVarRef(mod.first, 0, &varname, &scope)) {
      assert(mod.second != AST_NULL);
      defvars[varname] = std::pair<AstNodePtr, std::pair<AstNodePtr,AstNodePtr> >(scope, mod);
    }
    else {
      g.add_unknown_def( gen, mod);
    }
    return true;
  }
public:
  CollectLocalDefinitions( AstInterface& _fa, const ReachingDefinitionGenerator& _g)
      : fa(_fa), g(_g)
   {
     init();
   }
  ReachingDefinitions get_gen()
    {
      for (std::map<std::string, std::pair<AstNodePtr, std::pair<AstNodePtr,AstNodePtr> > >::const_iterator p = defvars.begin();
           p != defvars.end(); ++p) {
         std::pair <std::string, std::pair<AstNodePtr,std::pair<AstNodePtr,AstNodePtr> > > cur = *p;
         g.add_def( gen, cur.first, cur.second.first, cur.second.second);
      }
      defvars.clear();
      return gen;
    }
  void init()
  {
    gen = g.get_empty_set();
    defvars.clear();
  }
};

class CollectKillDefinitions : public CollectObject< std::pair<AstNodePtr, AstNodePtr> >
{
  AstInterface& fa;
  ReachingDefinitions kill;
  const ReachingDefinitionGenerator& g;

  bool operator()( const std::pair<AstNodePtr,AstNodePtr>& mod)
  {
    std::string varname;
    AstNodePtr scope;
    if (fa.IsVarRef(mod.first, 0, &varname, &scope)) {
      kill |= g.get_def_set(varname, scope);
    }
    return true;
  }
public:
  CollectKillDefinitions( AstInterface& _fa, const ReachingDefinitionGenerator& _g)
      : fa(_fa), g(_g)
   {
     init();
   }
  ReachingDefinitions get_kill() const { return kill; }
  void init()
  { kill = g.get_empty_set(); }
};

void ReachingDefNode::
finalize( AstInterface& fa, const ReachingDefinitionGenerator& g,
          FunctionSideEffectInterface* a, const ReachingDefinitions* _in)
{
  CollectLocalDefinitions collectgen(fa, g);
  CollectKillDefinitions collectkill(fa, g);
  StmtSideEffectCollect op(a);
  std::list <AstNodePtr>& stmts = GetStmts();
  for (std::list<AstNodePtr>::iterator p = stmts.begin(); p != stmts.end();
       ++p) {
    op( fa, *p, &collectgen, 0, &collectkill);
  }
  gen = collectgen.get_gen();
  notkill = collectkill.get_kill();
  notkill.complement();
  if (_in != 0) {
      in = *_in;
     apply_transfer_function();
  }
  else {
     in = g.get_empty_set();
     out = gen;
  }
}

void ReachingDefNode::Dump() const
{
  DataFlowNode<ReachingDefinitions>::Dump();
  std::cerr << "Entry ReachingDefinitions:" << in.toString();
  std::cerr << "Exit  ReachingDefinitions:" << out.toString();
  std::cerr << std::endl;
}


void ReachingDefinitionAnalysis:: FinalizeCFG( AstInterface& fa)
{
  ReachingDefinitions in = g->get_empty_set();
  for (AstInterface::AstNodeList::iterator p = pars.begin();
       p != pars.end(); ++p) {
      AstNodePtr cur = *p;
      std::string name;
      AstNodePtr scope;
      if (fa.IsVarRef(cur, 0, &name, &scope))
         g->add_def( in, name, scope, std::pair<AstNodePtr,AstNodePtr>(cur,AST_NULL));
  }
  NodeIterator p = GetNodeIterator();
  (*p)->finalize( fa, *g, a, &in);
  for ( ++p;!p.ReachEnd(); ++p) {
    (*p)->finalize(fa, *g, a);
  }
}

void ReachingDefinitionAnalysis::
operator()( AstInterface& fa, const AstNodePtr& h,  FunctionSideEffectInterface* anal)
{
  assert( g == 0 && pars.size() == 0);

  AstNodePtr body = h;
  if (!fa.IsFunctionDefinition( h, 0, &pars, 0, &body))
     ;//assert(false);

  ReachingDefinitionBase base;
  base.collect_refs( fa, body, anal, &pars);
  base.finalize();
  g = new ReachingDefinitionGenerator( base);

  a = anal;

  if (DebugReachingDef())
     std::cerr << "start building reaching definitions \n";
  DataFlowAnalysis<ReachingDefNode, ReachingDefinitions>::operator()( fa, h);
  if (DebugReachingDef())
     std::cerr << "finished building reaching definitions \n" << GraphToString(*this);
}

void ReachingDefinitionAnalysis::
collect_ast( const ReachingDefinitions& repr,
             CollectObject< std::pair<AstNodePtr, AstNodePtr> >& collect)
{
  assert(g != 0);
  g->collect_member( repr, collect);

}

#define TEMPLATE_ONLY
#include <DataFlowAnalysis.C>
template class DataFlowAnalysis<ReachingDefNode, ReachingDefinitions>;
