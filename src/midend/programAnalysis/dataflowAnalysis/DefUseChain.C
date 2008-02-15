
#include <general.h>
#include <DefUseChain.h>
#include <StmtInfoCollect.h>
#include <SinglyLinkedList.h>
#include <CommandOptions.h>
#include <GraphIO.h>

#include <vector>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a heade file it is OK here!
#define Boolean int

bool DebugDefUseChain();

template <class Node>
class BuildDefUseChain 
{
 protected:
  DefUseChain<Node>* graph;
  vector<Node*>& defvec;

  const ReachingDefinitionGenerator *g;
  AliasAnalysisInterface& alias;
  AstInterface& fa;

 public:
  void CreateEdges( Node* cur, const AstNodePtr& ref, const ReachingDefinitions& in)
  {
    ReachingDefinitions known = g->get_empty_set();
    ReachingDefinitions unknown = in;
    string varname;
    AstNodePtr scope;
    if (fa.IsVarRef(ref, 0, &varname, &scope)) {
      unknown = known = g->get_def_set(varname, scope);
      unknown.complement();
      known &= in;
      unknown &= in;
    }
    for (unsigned int i = 0; i < defvec.size(); ++i) {
        Node* def = defvec[i];
        assert (def != 0);
        if (known.has_member(i) ||
            (unknown.has_member(i) && alias.may_alias( fa, ref, def->get_ref()) )) {
          if (DebugDefUseChain()) 
             cerr << " creating edge from " << def->ToString() << endl;
          graph->CreateEdge(def, cur);
        }
        else if (DebugDefUseChain())  {
          if (!unknown.has_member(i)) 
             cerr << "not in reaching definition: " << def->ToString();
          else if (!alias.may_alias( fa, ref, def->get_ref())) 
             cerr << "not aliased: " << def->ToString(); 
        }
    }
  }
  BuildDefUseChain(  DefUseChain<Node>* _graph, vector<Node*>& _defvec,
                     const ReachingDefinitionGenerator *_g,
                     AliasAnalysisInterface& _alias,
                     AstInterface& _fa)
    : graph(_graph), defvec(_defvec), g(_g), alias(_alias), fa(_fa) {}
};

template <class Node>
class ProcessGenInfo 
  : public CollectObject<pair<AstNodePtr, AstNodePtr> >,
    public BuildDefUseChain<Node>
{
  // pmp 09JUN05
  //   add using decls to non-template dependent base-class members (gcc 3.4 issue)
  //   was: // nothing
  using BuildDefUseChain<Node>::fa;
  using BuildDefUseChain<Node>::defvec;
  using BuildDefUseChain<Node>::g;
  using BuildDefUseChain<Node>::graph;
  
  ReachingDefinitions& in;
  map<AstNodePtr, Node*>& defmap;

// DQ (1/13/2006): Andreas points out that this does not compile with ROSE (fails in EDG frontend) or Intel icc compliler
// BuildDefUseChain<Node>::fa;
// BuildDefUseChain<Node>::defvec;
// BuildDefUseChain<Node>::g;

  Boolean operator()( const pair<AstNodePtr, AstNodePtr>& mod)
  {
    string varname;
    AstNodePtr scope;
    if (DebugDefUseChain()) {
      cerr << "processing gen mod info : " << fa.AstToString(mod.first) << " : " << fa.AstToString(mod.second) << endl;
      DumpDefSet(defvec,in);
    }

    typename map<AstNodePtr,Node*>::const_iterator p = defmap.find( mod.first);
    assert( p != defmap.end());
    Node* cur = (*p).second;
    CreateEdges( cur, mod.first, in);
    if (fa.IsVarRef(mod.first, 0, &varname, &scope)) {
      g->add_def( in, varname, scope, mod);
    }
    else {
      g->add_unknown_def( in, mod);
    }
    if (DebugDefUseChain()) {
      cerr << "finish processing gen mod info : " << fa.AstToString(mod.first) << " : " << fa.AstToString(mod.second) << endl;
      DumpDefSet(defvec,in);
    }
    return true;
  }
public:
  ProcessGenInfo( DefUseChain<Node>* _graph, vector<Node*>& _defvec,
                     const ReachingDefinitionGenerator *_g,
                     AliasAnalysisInterface& _alias,
                     AstInterface& _fa, map<AstNodePtr, Node*>& dm, ReachingDefinitions& _in)
    : BuildDefUseChain<Node>(_graph, _defvec, _g, _alias, _fa), in(_in), defmap(dm) {}
};

template <class Node>
class ProcessKillInfo
  : public CollectObject<pair<AstNodePtr, AstNodePtr> >,
    public BuildDefUseChain<Node>
{
  // pmp 09JUN05
  //   cmp previous comment  
  using BuildDefUseChain<Node>::fa;
  using BuildDefUseChain<Node>::defvec;
  using BuildDefUseChain<Node>::g;
  using BuildDefUseChain<Node>::graph;

  ReachingDefinitions& in;
  map<AstNodePtr, Node*>& defmap;

// DQ (1/13/2006): Andreas points out that this does not compile with ROSE (fails in EDG frontend) or Intel icc compliler
// BuildDefUseChain<Node>::g;
// BuildDefUseChain<Node>::fa;
// BuildDefUseChain<Node>::defvec;

  Boolean operator()( const pair<AstNodePtr, AstNodePtr>& mod)
  {
    string varname;
    AstNodePtr scope;
    if (DebugDefUseChain()) {
      cerr << "processing kill mod info : " << fa.AstToString(mod.first) << " : " << fa.AstToString(mod.second) << endl;
      DumpDefSet(defvec,in);
    }
    if (fa.IsVarRef(mod.first, 0, &varname, &scope)) {
      ReachingDefinitions kill = g->get_def_set(varname, scope);
      kill.complement();
      in &= kill;
    }
    if (DebugDefUseChain()) {
      cerr << "finish processing kill mod info : " << fa.AstToString(mod.first) << " : " << fa.AstToString(mod.second) << endl;
      DumpDefSet(defvec,in);
    }
    return true;
  }
public:
  ProcessKillInfo( DefUseChain<Node>* _graph, vector<Node*>& _defvec,
                     const ReachingDefinitionGenerator *_g,
                     AliasAnalysisInterface& _alias,
                     AstInterface& _fa, map<AstNodePtr, Node*>& dm, ReachingDefinitions& _in)
    : BuildDefUseChain<Node>(_graph, _defvec, _g, _alias, _fa), in(_in), defmap(dm) {}
};



template <class Node> 
class ProcessUseInfo
  : public CollectObject< pair<AstNodePtr, AstNodePtr> >,
    public BuildDefUseChain<Node>
{
  // pmp 09JUN05
  //   cmp previous comment
   using BuildDefUseChain<Node>::fa;
   using BuildDefUseChain<Node>::defvec;
// DQ (1/13/2006): We don't appear to need this
// using BuildDefUseChain<Node>::g;
   using BuildDefUseChain<Node>::graph;

  ReachingDefinitions& in;

// DQ (1/13/2006): Andreas points out that this does not compile with ROSE (fails in EDG frontend) or Intel icc compliler
// BuildDefUseChain<Node>::graph;
// BuildDefUseChain<Node>::fa;
// BuildDefUseChain<Node>::defvec;

  Boolean operator()( const pair<AstNodePtr, AstNodePtr>& read)
  {
    if (DebugDefUseChain())  {
       cerr << "processind read info : " << fa.AstToString(read.first) << " : " << fa.AstToString(read.second) << endl;
         DumpDefSet(defvec,in);
    }
    Node* cur = graph->CreateNode( fa, read.first, read.second, false);
    if (cur == 0) {
       if (DebugDefUseChain()) 
           cerr << "do not create node in def-use chain \n";
        return false;
    }
    CreateEdges( cur, read.first, in);
    return true;
  }
public:
  ProcessUseInfo( DefUseChain<Node>* _graph, vector<Node*>& _defvec,
		     const ReachingDefinitionGenerator *_g,
		     AliasAnalysisInterface& _alias,
		     AstInterface& _fa, ReachingDefinitions& _in)
    :  BuildDefUseChain<Node>(_graph, _defvec, _g, _alias, _fa), in(_in)  {}
};

template<class Node>
void DumpDefSet( const vector<Node*>& defvec, const ReachingDefinitions& in)
{
        for (unsigned int i = 0; i < defvec.size(); ++i) {
          if (in.has_member(i)) {
             Node* def = defvec[i];
             assert (def != 0);
             cerr << def->ToString();
           }
        }
}
template <class Node>
void DefUseChain<Node>::
build( AstNodePtr root, AliasAnalysisInterface* alias, 
       FunctionSideEffectInterface *f)
{
   AstInterface fa(root);
   StmtVarAliasCollect defaultAlias;

   ReachingDefinitionAnalysis reachingDef;
   reachingDef(fa, root);
   if (alias == 0) {
      alias = &defaultAlias;
      AstNodePtr defn = root; 
      while (defn != 0 && !fa.IsFunctionDefinition(defn))
           defn = fa.GetParent(defn);
      assert(defn != 0);    
      defaultAlias(fa, defn);
   }
   build(fa, reachingDef,*alias, f);
}

template <class Node>
void DefUseChain<Node>::
build( AstInterface& fa, ReachingDefinitionAnalysis& r, 
       AliasAnalysisInterface& alias, FunctionSideEffectInterface* f)
{
  vector <Node*> defvec;
  const ReachingDefinitionGenerator* g = r.get_generator();
  StmtSideEffectCollect collect(f);

  map<AstNodePtr, Node*> defmap;
  const ReachingDefinitionBase& base = g->get_base();
  for (ReachingDefinitionBase::iterator p = base.begin(); p != base.end(); ++p) {
    pair<AstNodePtr,AstNodePtr> cur = base.get_ref(p);
    if (DebugDefUseChain()) 
       cerr << "creating def node : " << fa.AstToString(cur.first) << " : " << fa.AstToString(cur.second) << endl;
    Node* n = CreateNode( fa, cur.first, cur.second, true);
    assert(n != 0);
    defvec.push_back(n);
    defmap[cur.first] = n;
  }
  for (ReachingDefinitionAnalysis::NodeIterator p = r.GetNodeIterator(); 
       !p.ReachEnd(); ++p) {
      ReachingDefNode* cur = *p;
      if (DebugDefUseChain())  {
         cerr << "processing CFG node : ";
         cur->write(cerr);
      }
      ReachingDefinitions in = cur->get_entry_defs();
      if (DebugDefUseChain()) { 
         cerr << "Reaching definitions: \n";
         DumpDefSet(defvec,in);
         cerr << endl;
      }

      ProcessUseInfo<Node> opread( this, defvec, g, alias,fa, in);
      ProcessGenInfo<Node> opgen( this, defvec, g, alias, fa, defmap, in);
      ProcessKillInfo<Node> opkill( this, defvec, g, alias, fa, defmap, in);
      list <AstNodePtr>& stmts = cur->GetStmts();
      for (list<AstNodePtr>::iterator p = stmts.begin(); p != stmts.end();
	   ++p) {
	AstNodePtr cur = *p;
        if (DebugDefUseChain())  
            cerr << "processing stmt : " << fa.AstToString(cur) << endl;
	collect(fa, cur, &opgen, &opread, &opkill); 
      }
  }

  if (DebugDefUseChain()) {
     cerr << "\nfinished building def-use chain:\n";
     cerr << GraphToString(*this);
  }
}

template <class Node>
class AppendWorkListWrap : public CollectObject<Node*>
{
  set<Node*>& worklist;
 public:
  AppendWorkListWrap( set<Node*> &w) : worklist(w) {}
  Boolean operator() (Node* const& cur)
   {
     if (worklist.find(cur) == worklist.end())  {
        worklist.insert(cur);
         return true;
     }
     return false;
   }
};

template<class Node>
void PropagateDefUseChainUpdate( DefUseChain<Node> *graph, 
				 UpdateDefUseChainNode<Node>& update)
{
  set<Node*> worklist;
  AppendWorkListWrap<Node> append(worklist);
  update.init(append);
  while (worklist.size()) {
    Node* cur = *worklist.begin();
    worklist.erase(worklist.begin());
    if (cur->is_definition()) {
      for (typename DefUseChain<Node>::NodeIterator usep = 
                       GraphGetNodeSuccessors<DefUseChain<Node> >()(graph,cur);
	   !usep.ReachEnd(); ++usep) {
	Node* use = *usep;
        if (use->is_definition())
           continue;
	typename DefUseChain<Node>::NodeIterator defp = 
                 GraphGetNodePredecessors<DefUseChain<Node> >()(graph,use);
        Node *tmp = *defp;
	++defp;
	if (defp.ReachEnd()) {
	  assert( tmp == cur);
	  update.update_use_node(use, cur, append);
	}
      }
    }
    else {
      typename DefUseChain<Node>::NodeIterator defp = 
             GraphGetNodePredecessors<DefUseChain<Node> >()(graph,cur);
      if (defp.ReachEnd()) {
          if (DebugDefUseChain())  {
             cerr << "Error: use of reference with no definition: ";
             cur->Dump();
             cerr << endl;
          }
      }
      else {
        // Node* def = *defp;
        if (!defp.ReachEnd()) {
           Node* def = *defp;
           ++defp;
           if (defp.ReachEnd()) {
	      if (update.update_def_node(def, cur, append)) 
                       append(def);
            }
         }
      }
    }
  }
}

#ifndef DEFUSECHAIN_TEMPLATE_ONLY
void DefUseChainNode::Dump() const
{
  if (isdef) 
    cerr << "definition: ";
  else
    cerr << "use: ";
  cerr << AstInterface::AstToString(ref) << " : " << AstInterface::AstToString(stmt) << endl;
}

string DefUseChainNode::ToString() const
{
  string result;
  if (isdef)
     result = "definition:";
  else
    result = "use:";
  result = result + AstInterface::AstToString(ref) + ":" + AstInterface::AstToString(stmt) + " ";
  return result;
}


bool DebugDefUseChain()
{
  static int r =  0;
  if (r == 0) {
     if (CmdOptions::GetInstance()->HasOption("-debugdefusechain"))
         r = 1;
     else
         r = -1;
   }
  return r == 1;
}

#define TEMPLATE_ONLY
#include <IDGraphCreate.C>
template class  IDGraphCreateTemplate<DefUseChainNode, GraphEdge>;

template void DefUseChain<DefUseChainNode>::build(AstInterface&, ReachingDefinitionAnalysis&, AliasAnalysisInterface&, FunctionSideEffectInterface*);
template void DefUseChain<DefUseChainNode>::build(AstNodePtr, AliasAnalysisInterface*, FunctionSideEffectInterface*);

#endif

