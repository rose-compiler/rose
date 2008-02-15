#include <general.h>
#include <ReachingDefinition.h>
#include <StmtInfoCollect.h>
#include <CommandOptions.h>
#include <GraphIO.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a heade file it is OK here!
#define Boolean int

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
  : public  CollectObject< pair<AstNodePtr, AstNodePtr> >
{
  ReachingDefinitionBase& base;
  AstInterface& fa;
  Boolean operator()( const pair<AstNodePtr, AstNodePtr>& mod)
  {
    string varname;
    AstNodePtr scope;
    if (fa.IsVarRef(mod.first, 0, &varname, &scope)) {
      base.add_ref( varname, scope, mod);
      if (DebugReachingDef()) 
         cerr << "collecting var ref: " << varname << ":" << fa.AstToString(mod.second) << endl;
    }
    else {
      base.add_unknown_def( mod);
      if (DebugReachingDef()) 
         cerr << "collecting unknown ref: " << fa.AstToString(mod.first) << ":" << fa.AstToString(mod.second) << endl;
    }
    return true;
  }
public:
  ConstructReachingDefinitionBase( AstInterface& _fa, ReachingDefinitionBase& b)
    : base(b), fa(_fa) {}
};

void ReachingDefinitionBase:: 
add_ref( const string& varname, const AstNodePtr& scope, const pair<AstNodePtr,AstNodePtr>& def)
{ 
   string scopename = scopemap.get_scope_string(scope);
   string name = varname + scopename;
   add_data(name, def); 
}
  
void ReachingDefinitionBase::
collect_refs ( AstInterface& fa, const AstNodePtr& h, FunctionSideEffectInterface* a,
               AstInterface::AstNodeList* in)
{ 

  for (AstInterface::AstNodeListIterator p = fa.GetAstNodeListIterator(*in);
       !p.ReachEnd(); ++p) {
     AstNodePtr cur = *p;
     string varname;
     AstNodePtr scope;
     if (fa.IsVarRef( cur, 0, &varname, &scope))
        add_ref(varname, scope, pair<AstNodePtr, AstNodePtr>(cur, 0) ); 
  }
  ConstructReachingDefinitionBase collect(fa, *this);
  StmtSideEffectCollect op(a);
  op(fa, h, &collect);
}

void ReachingDefinitionGenerator::
add_def( ReachingDefinitions& repr, const string& varname, const AstNodePtr& scope,
                const pair<AstNodePtr,AstNodePtr>& def) const
{
   string scopename = scopemap.get_scope_string(scope);
   string name = varname + scopename;
   add_member(repr, name, def);
}

ReachingDefinitions ReachingDefinitionGenerator::
get_def_set( const string& varname, const AstNodePtr& scope) const
{
   string scopename = scopemap.get_scope_string(scope);
   string name = varname + scopename;
   return get_data_set(name);
}

class CollectLocalDefinitions : public CollectObject< pair<AstNodePtr, AstNodePtr> >
{
  AstInterface& fa;
  map<string, pair<AstNodePtr, pair<AstNodePtr,AstNodePtr> > > defvars;
  ReachingDefinitions gen;
  const ReachingDefinitionGenerator& g;
  
  Boolean operator()( const pair<AstNodePtr,AstNodePtr>& mod)
  {
    string varname;
    AstNodePtr scope;
    if (fa.IsVarRef(mod.first, 0, &varname, &scope)) {
      defvars[varname] = pair<AstNodePtr, pair<AstNodePtr,AstNodePtr> >(scope, mod);
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
      for (map<string, pair<AstNodePtr, pair<AstNodePtr,AstNodePtr> > >::const_iterator p = defvars.begin();
           p != defvars.end(); ++p) {
         pair <string, pair<AstNodePtr,pair<AstNodePtr,AstNodePtr> > > cur = *p;
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

class CollectKillDefinitions : public CollectObject< pair<AstNodePtr, AstNodePtr> >
{
  AstInterface& fa;
  ReachingDefinitions kill;
  const ReachingDefinitionGenerator& g;

  Boolean operator()( const pair<AstNodePtr,AstNodePtr>& mod)
  {
    string varname;
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
  list <AstNodePtr>& stmts = GetStmts();
  for (list<AstNodePtr>::iterator p = stmts.begin(); p != stmts.end();
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
  cerr << "Entry ReachingDefinitions:";
  in.Dump();
  cerr << "Exit  ReachingDefinitions:";
  out.Dump();
  cerr << endl;
}


void ReachingDefinitionAnalysis:: FinalizeCFG( AstInterface& fa)
{
  ReachingDefinitions in = g->get_empty_set();
  for (AstInterface::AstNodeListIterator p = fa.GetAstNodeListIterator(pars);
       !p.ReachEnd(); ++p) {
      AstNodePtr cur = *p;
      string name;
      AstNodePtr scope;
      if (fa.IsVarRef(cur, 0, &name, &scope))
         g->add_def( in, name, scope, pair<AstNodePtr,AstNodePtr>(cur,0));
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

  AstNodePtr decl, body = h;
  if (!fa.IsFunctionDefinition( h, &decl, &body)) {
     //assert(false);
  }
  if (decl != 0 && !fa.IsFunctionDecl( decl, 0, 0, 0, &pars)) {
     //assert(false);
  }

  ReachingDefinitionBase base;
  base.collect_refs( fa, body, anal, &pars);
  base.finalize();
  g = new ReachingDefinitionGenerator( base);
 
  a = anal;

  if (DebugReachingDef())
     cerr << "start building reaching definitions \n";
  DataFlowAnalysis<ReachingDefNode, ReachingDefinitions>::operator()( fa, h);
  if (DebugReachingDef()) 
     cerr << "finished building reaching definitions \n" << GraphToString(*this);
}

void ReachingDefinitionAnalysis:: 
collect_ast( const ReachingDefinitions& repr, 
             CollectObject< pair<AstNodePtr, AstNodePtr> >& collect)
{
  assert(g != 0);
  g->collect_member( repr, collect);

}

#define TEMPLATE_ONLY
#include <IDGraphCreate.C>
#include <DataFlowAnalysis.C>
#include <CFG_ROSE.C>
template class DataFlowAnalysis<ReachingDefNode, ReachingDefinitions>;
